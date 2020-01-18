//
// Created by maxim on 12/30/19.
//

#ifdef __GNUC__
#pragma implementation "renderer.h"
#endif
#include "renderer.h"

#include "../win_defaults.h"
#include "../../lib/resources/rmanager.h"
#include "../../lib/scene.h"
#include "../../lib/scm.h"
#include "ssao.h"
#include "shader_setup.h"
#include "../../lib/resources/font.h"

typedef struct _geometry_shader_data {
   camera_t* camera;
   mat4 buffmat;

} geometry_shader_data_t;

#define STAGE_G_BUFFER 0
#define STAGE_SSAO 1
#define STAGE_SSAO_BLUR 2
#define STAGE_SKYBOX 3
#define STAGE_SHADOWMAP 4
#define STAGE_SHADING 5
#define STAGE_BYPASS 6

render_chain_t* rc;

//ssao
texture_t* noise_texure;
vec4* ssao_kernel;
texture_t* ssao_texture;
texture_t* ssao_dummy_texture;

//stages
texture_t* texture_to_draw;
int ssao_state = 0;
int state = -1;

mat4 view;
mat4 font_proj;

inline void update_shadow_light(void)
{
   render_stage_t* shadow_map_stage = rc->stages->collection[STAGE_SHADOWMAP];
   render_stage_t* shading_stage = rc->stages->collection[STAGE_SHADING];
   shadow_map_stage->skip = false;

   scene_t* scene = scm_get_current();

   vec4_cpy(scene->shadow_light->light_camera->direction, scene->shadow_light->light_camera->target);
   vec4_subv(scene->shadow_light->light_camera->direction, scene->shadow_light->position);

   shader_t* shading_shader = shading_stage->shader;
   sh_use(shading_shader);
   sh_set_vec3(UNIFORM_SHADING.shadow_light_position, scene->shadow_light->position);
   sh_set_vec3(UNIFORM_SHADING.shadow_light_direction, scene->shadow_light->light_camera->direction);
   sh_set_mat4(UNIFORM_SHADING.shadow_light_lightspace, scene->shadow_light->light_space);
   sh_set_float(UNIFORM_SHADING.shadow_light_bright, .7f);
   sh_use(NULL);
}

inline void update_lights(void)
{
   render_stage_t* shading_stage = rc->stages->collection[STAGE_SHADING];
   list_t* lights = scm_get_current()->lights;

   shader_t* shading_shader = shading_stage->shader;
   sh_use(shading_shader);

   for (size_t i = 0; i < lights->count; i++)
   {
      light_t* lt = lights->collection[i];
      sh_set_vec3(UNIFORM_SHADING.lights_position[i], lt->position);
      sh_set_vec3(UNIFORM_SHADING.lights_color[i], lt->color);
      sh_set_float(UNIFORM_SHADING.lights_linear[i], lt->linear);
      sh_set_float(UNIFORM_SHADING.lights_quadratic[i], lt->quadratic);
      sh_set_float(UNIFORM_SHADING.lights_radius[i], lt->radius);
   }

   sh_use(NULL);
}

inline void switch_ssao(void)
{
   render_stage_t* ssao_blur_stage = rc->stages->collection[STAGE_SSAO_BLUR];
   if(ssao_state)
   {
      ssao_texture = ssao_dummy_texture;
      APP_LOG("SSAO off",0);
   }
   else
   {
      ssao_texture = ssao_blur_stage->color0_tex;
      APP_LOG("SSAO on",0);
   }

   ssao_state = !ssao_state;
}

inline void switch_stages(void)
{
   render_stage_t* g_buffer_stage = rc->stages->collection[STAGE_G_BUFFER];
   render_stage_t* ssao_stage = rc->stages->collection[STAGE_SSAO];
   render_stage_t* ssao_blur_stage = rc->stages->collection[STAGE_SSAO_BLUR];
   render_stage_t* skybox_stage = rc->stages->collection[STAGE_SKYBOX];
   render_stage_t* shadowmap_stage = rc->stages->collection[STAGE_SHADOWMAP];
   render_stage_t* shading_stage = rc->stages->collection[STAGE_SHADING];

   state = (state + 1) % 8;
   switch(state)
   {
      default:
      case 0:
         texture_to_draw = shading_stage->color0_tex; // result
         APP_LOG("0. Result image",0);
         break;
      case 1:
         texture_to_draw = g_buffer_stage->color0_tex; //positions
         APP_LOG("1. View Positions",0);
         break;
      case 2:
         texture_to_draw = g_buffer_stage->color1_tex; //normals
         APP_LOG("2. Normals",0);
         break;
      case 3:
         texture_to_draw = g_buffer_stage->color2_tex; //albedo_spec
         APP_LOG("3. Albedo spec",0);
         break;
      case 4:
         texture_to_draw = g_buffer_stage->color3_tex; //positions
         APP_LOG("3. Positions",0);
         break;
      case 5:
         texture_to_draw = ssao_stage->color0_tex; //ssao
         APP_LOG("5. SSAO",0);
         break;
      case 6:
         texture_to_draw = ssao_blur_stage->color0_tex; //ssao blurred
         APP_LOG("6. Blurred SSAO",0);
         break;
      case 7:
         texture_to_draw = shadowmap_stage->depth_tex; //shadowmap
         APP_LOG("7. Shadowmap",0);
         break;
   }
}

// G BUFFER ROUTINES
void bind_g_buffer(render_stage_t* stage)
{
   GL_PCALL(glClearColor(0,0,0,0));
   GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   c_to_mat(view, data->camera);
   sh_set_mat4(UNIFORM_GBUFF.view, view);
}

void unbind_g_buffer(render_stage_t* stage)
{
}

void setup_object_g_buffer(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   t_bind(object->draw_info->material->map_diffuse, UNIFORM_GBUFF.diffuse_tex);
   t_bind(object->draw_info->material->map_specular, UNIFORM_GBUFF.spec_tex);
   sh_set_mat4(UNIFORM_GBUFF.model, model_mat);
}

// SSAO ROUTINES
void bind_ssao(render_stage_t* stage)
{
   GL_PCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

   render_stage_t* g_buffer_stage = rc->stages->collection[STAGE_G_BUFFER];
   t_bind(g_buffer_stage->color0_tex, UNIFORM_SSAO.pos_tex);
   t_bind(g_buffer_stage->color1_tex, UNIFORM_SSAO.norm_tex);
   t_bind(noise_texure, UNIFORM_SSAO.noise_tex);
}

void unbind_ssao(render_stage_t* stage) { }

// SSAO BLUR ROUTINES
void bind_ssao_blur(render_stage_t* stage)
{
   t_bind(stage->prev_stage->color0_tex, UNIFORM_SSAO_BLUR.tex);
}

void unbind_ssao_blur(render_stage_t* stage) { }

// SKYBOX ROUTINES
void bind_skybox(render_stage_t* stage)
{
   scene_t* scene = scm_get_current();
   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   mat4_cpy(data->buffmat, view);

   data->buffmat[3] = 0;
   data->buffmat[7] = 0;
   data->buffmat[11] = 0;
   data->buffmat[15] = 0;

   sh_set_mat4(UNIFORM_SKYBOX.view, data->buffmat);
   t_bind(scene->skybox, UNIFORM_SKYBOX.skybox_tex);
}

void unbind_skybox(render_stage_t* stage)
{
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void draw_skybox(render_stage_t* stage)
{
   render_stage_t* g_buffer_stage = rc->stages->collection[STAGE_G_BUFFER];
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_stage->fbo));
   {
      GL_PCALL(glDepthFunc(GL_LEQUAL));
      GL_PCALL(glBindVertexArray(stage->vao));
      GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, 36));
      GL_PCALL(glBindVertexArray(0));
      GL_PCALL(glDepthFunc(GL_LESS));
   }
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// SHADOWMAP ROUTINES
void bind_shadowmap(render_stage_t* stage)
{
   scene_t* scene = scm_get_current();
   shadow_light_t* shadow_light = scene->shadow_light;

   GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

   vec4_cpy(shadow_light->light_camera->position, shadow_light->position);
   c_to_mat(shadow_light->light_view, shadow_light->light_camera);
   mat4_cpy(shadow_light->light_space, shadow_light->light_proj);
   mat4_mulm(shadow_light->light_space, shadow_light->light_view);

   sh_set_mat4(UNIFORM_SHADOWMAP.light_space, shadow_light->light_space);
   GL_PCALL(glCullFace(GL_FRONT));

   stage->skip = true;
}

void unbind_shadowmap(render_stage_t* stage)
{
   GL_PCALL(glCullFace(GL_BACK));
}

void setup_object_shadowmap(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   sh_set_mat4(UNIFORM_SHADOWMAP.model, model_mat);
}

// SHADING ROUTINES
void bind_shading(render_stage_t* stage)
{
   render_stage_t* g_buffer_stage = rc->stages->collection[STAGE_G_BUFFER];
   render_stage_t* shadowmap_stage = rc->stages->collection[STAGE_SHADOWMAP];

   scene_t* scene = scm_get_current();
   geometry_shader_data_t* data = stage->data;

   sh_set_vec3(UNIFORM_SHADING.view_pos, data->camera->position);
   sh_set_mat4(UNIFORM_SHADING.view, view);

   //t_bind(g_buffer_stage->color0_tex, uniform);
   t_bind(g_buffer_stage->color1_tex, UNIFORM_SHADING.norm_tex);
   t_bind(g_buffer_stage->color2_tex, UNIFORM_SHADING.albedoSpec_tex);
   t_bind(ssao_texture, UNIFORM_SHADING.ssao_tex);
   t_bind(g_buffer_stage->color3_tex, UNIFORM_SHADING.pos_tex);
   t_bind(shadowmap_stage->depth_tex, UNIFORM_SHADING.shadow_light_shadowmap_tex);
}

void unbind_shading(render_stage_t* stage) { }

// GAMMA / BYPASS ROUTINES
void bind_bypass(render_stage_t* stage)
{
   t_bind(texture_to_draw, UNIFORM_GAMMA.tex);
}

void unbind_bypass(render_stage_t* stage) { }

//FONT ROUTINES
void bind_font(render_stage_t* stage)
{
   glEnable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void unbind_font(render_stage_t* stage)
{
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
}

GLuint vao, vbo, ebo;
void draw_font(render_stage_t* stage)
{
   font_t* f = rm_getn(FONT, "default");
   const char* string =
         "Lorem ipsum dolor sit amet, consectetur adipiscing elit, \n"
         "sed do eiusmod tempor incididunt ut labore et dolore magna\n"
         "aliqua. Ut enim ad minim veniam, quis nostrud exercitation\n"
         "ullamco laboris nisi ut aliquip ex ea commodo consequat. \n"
         "Duis aute irure dolor in reprehenderit in voluptate velit\n"
         "esse cillum dolore eu fugiat nulla pariatur Excepteur sint\n"
         "occaecat cupidatat non proident, sunt in culpa qui officia\n"
         "deserunt mollit anim id est laborum.";

   float scale = 0.95f;
   vec2f_t cursor = vec2f(10, 10);
   float start_x = cursor.x;
   //vec4 border_color = COLOR_BLUE;

   sh_set_vec4(UNIFORM_FONT.color, COLOR_WHITE);
   //sh_set_vec3(UNIFORM_FONT.borderColor, border_color);
   sh_set_vec2v(UNIFORM_FONT.params, 0.5f, 9.0f);
   t_bind(f->texture, UNIFORM_FONT.tex);

   GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
   GL_PCALL(glBindVertexArray(vao));


   for(size_t i = 0; i < strlen(string); i++)
   {
      charinfo_t* ci = &f->infos[string[i]];
      if(string[i] == '\n')
      {
         cursor.y += f->base * scale;
         cursor.x = start_x;
         continue;
      }

      assert(ci->id != -1);

      float x1 = scale * (float)(cursor.x + ci->xoffset);
      float y1 = scale * (float)(cursor.y + ci->yoffset);
      float x2 = scale * (float)(cursor.x + ci->xoffset + ci->width);
      float y2 = scale * (float)(cursor.y + ci->yoffset + ci->height);
/*     float x1 = 0;
     float x2 = 100;
     float y1 = 0;
     float y2 = 100;*/

/*
 *          1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
           1.0f, -1.0f, 0.0f,    1.0f, 0.0f,
           -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
           -1.0f,  1.0f, 0.0f,   0.0f, 1.0f
 */
      float values[] = {
            x2 - (float)win->w / 2.0f, (float)win->h / 2.0f - y2, 0, ci->tex_coord[0], ci->tex_coord[1],
            x2 - (float)win->w / 2.0f, (float)win->h / 2.0f - y1, 0, ci->tex_coord[2], ci->tex_coord[3],
            x1 - (float)win->w / 2.0f, (float)win->h / 2.0f - y1, 0, ci->tex_coord[4], ci->tex_coord[5],
            x1 - (float)win->w / 2.0f, (float)win->h / 2.0f - y2, 0, ci->tex_coord[6], ci->tex_coord[7],
      };

      GL_PCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(values), values, GL_DYNAMIC_DRAW));
      GL_PCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

      cursor.x += scale * ci->xadvance;
   }

   GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
   GL_PCALL(glBindVertexArray(0));

   //sh_use(NULL);
}

render_chain_t* get_chain(win_info_t* info, camera_t* camera, mat4 proj)
{

   noise_texure = generate_noise(4);
   ssao_kernel = generate_kernel(KERNEL_SIZE);
   ssao_dummy_texture = mt_create_colored_tex(COLOR_WHITE);
   view = cmat4();

   font_proj = cmat4();
   mat4_ortho(font_proj, -1, 1, info->w, info->h);

   GLuint indices[] = {
         0, 1, 3,
         1, 2, 3
   };

   GL_CALL(glGenVertexArrays(1, &vao));
   GL_CALL(glGenBuffers(1, &vbo));
   GL_CALL(glGenBuffers(1, &ebo));
   GL_CALL(glBindVertexArray(vao));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 5, NULL, GL_STATIC_DRAW));

   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
   GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

   GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
   GL_CALL(glEnableVertexAttribArray(0));

   GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
   GL_CALL(glEnableVertexAttribArray(1));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

   GL_CALL(glBindVertexArray(0));
   GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

   shader_t* g_buffer_shader = setup_g_buffer(proj);
   shader_t* ssao_shader = setup_ssao(ssao_kernel, proj);
   shader_t* ssao_blur_shader = setup_ssao_blur();
   shader_t* skybox_shader = setup_skybox(proj);
   shader_t* shadowmap_shader = setup_shadowmap();
   shader_t* shading_shader = setup_shading();
   shader_t* font_shader = setup_font(font_proj);
   shader_t* gamma_shader = setup_gamma();

   render_stage_t* g_buffer = rs_create("gbuffer", RM_GEOMETRY, g_buffer_shader);
   g_buffer->attachments = TF_COLOR0 | TF_COLOR1 | TF_COLOR2 | TF_COLOR3 | TF_DEPTH;
   // View Position buffer
   g_buffer->color0_format.tex_width = win->w;
   g_buffer->color0_format.tex_height = win->h;
   g_buffer->color0_format.tex_format = GL_RGB;
   g_buffer->color0_format.tex_int_format = GL_RGB16F;
   g_buffer->color0_format.tex_mag_filter = GL_NEAREST;
   g_buffer->color0_format.tex_min_filter = GL_NEAREST;
   g_buffer->color0_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   g_buffer->color0_format.tex_wrapping_s = GL_CLAMP_TO_EDGE;
   // Normals buffer
   g_buffer->color1_format.tex_width = win->w;
   g_buffer->color1_format.tex_height = win->h;
   g_buffer->color1_format.tex_format = GL_RGB;
   g_buffer->color1_format.tex_int_format = GL_RGB16F;
   g_buffer->color1_format.tex_mag_filter = GL_NEAREST;
   g_buffer->color1_format.tex_min_filter = GL_NEAREST;
   g_buffer->color1_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   g_buffer->color1_format.tex_wrapping_s = GL_CLAMP_TO_EDGE;
   // Color + Specular buffer
   g_buffer->color2_format.tex_width = win->w;
   g_buffer->color2_format.tex_height = win->h;
   g_buffer->color2_format.tex_format = GL_RGBA;
   g_buffer->color2_format.tex_int_format = GL_RGBA;
   g_buffer->color2_format.tex_mag_filter = GL_NEAREST;
   g_buffer->color2_format.tex_min_filter = GL_NEAREST;
   g_buffer->color2_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   g_buffer->color2_format.tex_wrapping_s = GL_CLAMP_TO_EDGE;
   // Position
   g_buffer->color3_format.tex_width = win->w;
   g_buffer->color3_format.tex_height = win->h;
   g_buffer->color3_format.tex_format = GL_RGB;
   g_buffer->color3_format.tex_int_format = GL_RGB16F;
   g_buffer->color3_format.tex_mag_filter = GL_NEAREST;
   g_buffer->color3_format.tex_min_filter = GL_NEAREST;
   g_buffer->color3_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   g_buffer->color3_format.tex_wrapping_s = GL_CLAMP_TO_EDGE;
   // Depth component
   g_buffer->depth_format.tex_width = win->w;
   g_buffer->depth_format.tex_height = win->h;
   g_buffer->depth_format.tex_format = GL_DEPTH_COMPONENT;
   g_buffer->depth_format.tex_int_format = GL_DEPTH_COMPONENT;
   g_buffer->depth_format.tex_mag_filter = GL_NEAREST;
   g_buffer->depth_format.tex_min_filter = GL_NEAREST;
   g_buffer->depth_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   g_buffer->depth_format.tex_wrapping_s = GL_CLAMP_TO_EDGE;

   mat4_cpy(g_buffer->proj, proj);
   mat4_identity(g_buffer->view);
   g_buffer->bind_func = bind_g_buffer;
   g_buffer->unbind_func = unbind_g_buffer;
   g_buffer->setup_obj_func = setup_object_g_buffer;
   geometry_shader_data_t* g_buffer_data = (g_buffer->data = malloc(sizeof(geometry_shader_data_t)));
   g_buffer_data->camera = camera;
   g_buffer_data->buffmat = cmat4();

   render_stage_t* ssao = rs_create("ssao", RM_FRAMEBUFFER, ssao_shader);
   ssao->attachments = TF_COLOR0;
   //color
   ssao->color0_format.tex_width = win->w;
   ssao->color0_format.tex_height = win->h;
   ssao->color0_format.tex_format = GL_RGB;
   ssao->color0_format.tex_int_format = GL_RED;
   ssao->color0_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   ssao->color0_format.tex_wrapping_s = GL_CLAMP_TO_EDGE;
   ssao->bind_func = bind_ssao;
   ssao->unbind_func = unbind_ssao;
   ssao->vao = rc_get_quad_vao();
   mat4_cpy(ssao->proj, proj);

   render_stage_t* ssao_blur = rs_create("ssao_blur", RM_FRAMEBUFFER, ssao_blur_shader);
   ssao_blur->attachments = TF_COLOR0;
   ssao_blur->color0_format.tex_width = info->w;
   ssao_blur->color0_format.tex_height = info->h;
   ssao_blur->color0_format.tex_format = GL_RGB;
   ssao_blur->color0_format.tex_int_format = GL_RED;

   ssao_blur->bind_func = bind_ssao_blur;
   ssao_blur->unbind_func = unbind_ssao_blur;
   ssao_blur->vao = rc_get_quad_vao();

   render_stage_t* skybox = rs_create("skybox", RM_CUSTOM, skybox_shader);
   skybox->width = info->w;
   skybox->height = info->h;
   skybox->bind_func = bind_skybox;
   skybox->unbind_func = unbind_skybox;
   skybox->custom_draw_func = draw_skybox;
   mat4_cpy(skybox->proj, proj);
   mat4_identity(skybox->view);
   skybox->vao = rc_get_cube_vao();
   geometry_shader_data_t* skybox_data = (skybox->data = malloc(sizeof(geometry_shader_data_t)));
   skybox_data->buffmat = cmat4();
   skybox_data->camera = camera;


   render_stage_t* shadowmap = rs_create("shadow_map", RM_GEOMETRY, shadowmap_shader);
   shadowmap->attachments = TF_DEPTH;
   //depth
   shadowmap->depth_format.tex_height = 2048;
   shadowmap->depth_format.tex_width = 2048;
   shadowmap->depth_format.tex_wrapping_t = GL_CLAMP_TO_BORDER;
   shadowmap->depth_format.tex_wrapping_s = GL_CLAMP_TO_BORDER;
   shadowmap->depth_format.tex_border_color[0] = 1;
   shadowmap->depth_format.tex_border_color[1] = 1;
   shadowmap->depth_format.tex_border_color[2] = 1;

   shadowmap->bind_func = bind_shadowmap;
   shadowmap->setup_obj_func = setup_object_shadowmap;
   shadowmap->unbind_func = unbind_shadowmap;


   render_stage_t* shading = rs_create("shading", RM_FRAMEBUFFER, shading_shader);
   shading->attachments = TF_COLOR0;
   shading->color0_format.tex_width = info->w;
   shading->color0_format.tex_height = info->h;
   shading->color0_format.tex_format = GL_RGB;
   shading->color0_format.tex_int_format = GL_RGB16F;
   shading->bind_func = bind_shading;
   shading->unbind_func = unbind_shading;
   shading->vao = rc_get_quad_vao();
   mat4_cpy(shading->proj, proj);
   geometry_shader_data_t* shading_data = (shading->data = malloc(sizeof(geometry_shader_data_t)));
   shading_data->camera = camera;
   shading_data->buffmat = cmat4();


   render_stage_t* bypass = rs_create("bypass", RM_BYPASS, gamma_shader);
   bypass->width = info->w;
   bypass->height = info->h;
   bypass->bind_func = bind_bypass;
   bypass->unbind_func = unbind_bypass;
   bypass->vao = rc_get_quad_vao();

   render_stage_t* font = rs_create("font", RM_CUSTOM, font_shader);
   font->width = info->w;
   font->height = info->h;
   font->bind_func = bind_font;
   font->unbind_func = unbind_font;
   font->custom_draw_func = draw_font;

   rc = rc_create();
   list_push(rc->stages, g_buffer);
   list_push(rc->stages, ssao);
   list_push(rc->stages, ssao_blur);
   list_push(rc->stages, skybox);
   list_push(rc->stages, shadowmap);
   list_push(rc->stages, shading);
   list_push(rc->stages, bypass);
   list_push(rc->stages, font);

   rc_build(rc);
   GL_PCALL(glEnable(GL_DEPTH_TEST));
   return rc;
}

void free_geometry_shader_data(render_chain_t* render_chain, size_t index)
{
   geometry_shader_data_t* gsd = ((render_stage_t*)render_chain->stages->collection[index])->data;
   mat4_free(gsd->buffmat);
}

void free_stages(void)
{
   free_geometry_shader_data(rc, STAGE_G_BUFFER);
   rs_free(rc->stages->collection[STAGE_G_BUFFER]);

   rs_free(rc->stages->collection[STAGE_SSAO]);
   rs_free(rc->stages->collection[STAGE_SSAO_BLUR]);

   free_geometry_shader_data(rc, STAGE_SKYBOX);
   rs_free(rc->stages->collection[STAGE_SKYBOX]);

   rs_free(rc->stages->collection[STAGE_SHADOWMAP]);

   free_geometry_shader_data(rc, STAGE_SHADING);
   rs_free(rc->stages->collection[STAGE_SHADING]);

   rs_free(rc->stages->collection[STAGE_BYPASS]);

   rc_free(rc, false);

   mat4_free(view);
   mat4_free(font_proj);

   for(size_t i = 0; i < KERNEL_SIZE; i++)
      vec4_free(ssao_kernel[i]);
   free(ssao_kernel);
}