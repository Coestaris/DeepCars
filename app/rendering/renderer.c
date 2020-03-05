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
#include "vfx.h"
#include "text_rendering.h"

typedef struct _geometry_shader_data {
   camera_t* camera;
   mat4 buffmat;

} geometry_shader_data_t;

render_chain_t* default_rc;
render_chain_t* editor_rc;

//ssao
texture_t* noise_texure;
vec4* ssao_kernel;
texture_t* ssao_texture;
texture_t* ssao_dummy_texture;

//stages
texture_t* texture_to_draw;
int ssao_state = 0;
int state = -1;
int fxaa_state = 1;
int wireframe = 0;
int fxaa_edges = 0;

mat4 view;

list_t* blurred_regions;
sprite_renderer_t* default_sprite_renderer;
primitive_renderer_t* default_primitive_renderer;
shader_t* br_shader;

void switch_wireframe(void)
{
   wireframe = !wireframe;
   APP_LOG("WIREFRAME MODE: %s", wireframe ? "ON" : "OFF");
}

void switch_fxaa(void)
{
   fxaa_state = !fxaa_state;
   APP_LOG("FXAA: %s", fxaa_state ? "ON" : "OFF");
}

void switch_fxaa_edges(void)
{
   fxaa_edges = !fxaa_edges;
   APP_LOG("FXAA EDGES: %s", fxaa_edges ? "ON" : "OFF");
}

char _buff[140];
char* get_ssao_stage_string()
{
   if(state == 0)
      snprintf(_buff, sizeof(_buff), "Default view, SSAO %s, FXAA: %s",
            ssao_state ? "on" : "off",
            fxaa_state ? "on" : "off");
   else
   {
      snprintf(_buff, sizeof(_buff),
            state == 1 ? "Local positions view" :
            state == 2 ? "Local normals view" :
            state == 3 ? "Colors view" :
            state == 4 ? "World position view" :
            state == 5 ? "SSAO view" :
            state == 6 ? "Blurred SSAO view" :
            "ShadowMap view");
   }
   return _buff;
}

inline void update_shadow_light(void)
{
   render_stage_t* shadow_map_stage = default_rc->stages->collection[STAGE_SHADOWMAP];
   render_stage_t* shading_stage = default_rc->stages->collection[STAGE_SHADING];
   shadow_map_stage->skip = false;

   scene_t* scene = scm_get_current();
   vec4_cpy(scene->shadow_light->light_camera->position, scene->shadow_light->position);

   c_to_mat(scene->shadow_light->light_view, scene->shadow_light->light_camera);

   vec4_cpy(scene->shadow_light->light_camera->direction, scene->shadow_light->light_camera->target);
   vec4_subv(scene->shadow_light->light_camera->direction, scene->shadow_light->position);
   vec4_norm(scene->shadow_light->light_camera->direction);

   mat4_cpy(scene->shadow_light->light_space, scene->shadow_light->light_proj);
   mat4_mulm(scene->shadow_light->light_space, scene->shadow_light->light_view);

   shader_t* shading_shader = shading_stage->shader;
   sh_use(shading_shader);
   sh_set_vec3(UNIFORM_SHADING.shadow_light_position, scene->shadow_light->position);
   sh_set_vec3(UNIFORM_SHADING.shadow_light_direction, scene->shadow_light->light_camera->direction);
   sh_set_mat4(UNIFORM_SHADING.shadow_light_lightspace, scene->shadow_light->light_space);
   sh_set_float(UNIFORM_SHADING.shadow_light_bright,  scene->shadow_light->brightness);
   sh_use(NULL);
}

inline void update_lights(void)
{
   render_stage_t* shading_stage = default_rc->stages->collection[STAGE_SHADING];
   list_t* lights = scm_get_current()->lights;

   shader_t* shading_shader = shading_stage->shader;
   sh_use(shading_shader);

   sh_set_int(UNIFORM_SHADING.lights_count, lights->count);

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
   render_stage_t* ssao_blur_stage = default_rc->stages->collection[STAGE_SSAO_BLUR];
   if(ssao_state)
      ssao_texture = ssao_dummy_texture;
   else
      ssao_texture = ssao_blur_stage->color0_tex;

   ssao_state = !ssao_state;
   APP_LOG("SSAO: %s", ssao_state ? "ON" : "OFF");
}

inline void switch_stages(void)
{
   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];
   render_stage_t* ssao_stage = default_rc->stages->collection[STAGE_SSAO];
   render_stage_t* ssao_blur_stage = default_rc->stages->collection[STAGE_SSAO_BLUR];
   render_stage_t* skybox_stage = default_rc->stages->collection[STAGE_SKYBOX];
   render_stage_t* shadowmap_stage = default_rc->stages->collection[STAGE_SHADOWMAP];
   render_stage_t* shading_stage = default_rc->stages->collection[STAGE_SHADING];
   render_stage_t* fxaa_stage = default_rc->stages->collection[STAGE_FXAA];

   state = (state + 1) % 8;
   switch(state)
   {
      default:
      case 0:
         texture_to_draw = fxaa_stage->color0_tex; // result
         break;
      case 1:
         texture_to_draw = g_buffer_stage->color0_tex; //positions
         break;
      case 2:
         texture_to_draw = g_buffer_stage->color1_tex; //normals
         break;
      case 3:
         texture_to_draw = g_buffer_stage->color2_tex; //albedo_spec
         break;
      case 4:
         texture_to_draw = g_buffer_stage->color3_tex; //positions
         break;
      case 5:
         texture_to_draw = ssao_stage->color0_tex; //ssao
         break;
      case 6:
         texture_to_draw = ssao_blur_stage->color0_tex; //ssao blurred
         break;
      case 7:
         texture_to_draw = shadowmap_stage->depth_tex; //shadowmap
         break;
   }
}

// G BUFFER ROUTINES
void bind_g_buffer(render_stage_t* stage)
{
   GL_PCALL(glClearColor(0, 0, 0, 0));
   GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

   if(wireframe)
   GL_PCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   c_to_mat(view, data->camera);
   sh_set_mat4(UNIFORM_GBUFF.view, view);
}

void unbind_g_buffer(render_stage_t* stage)
{
   if(wireframe)
   GL_PCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
}

void setup_object_g_buffer(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   t_bind(object->draw_info->material->map_diffuse, UNIFORM_GBUFF.diffuse_tex);
   t_bind(object->draw_info->material->map_specular, UNIFORM_GBUFF.spec_tex);
   sh_set_mat4(UNIFORM_GBUFF.model, model_mat);
}

// NORMAL ROUTINES
void bind_normal(render_stage_t* stage)
{
   sh_set_mat4(UNIFORM_NORMAL.view, view);
}

void unbind_normal(render_stage_t* stage) { }

void draw_normal(render_stage_t* stage)
{
   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_stage->fbo));
   list_t* objects = u_get_objects();

   //GL_PCALL(glEnable(GL_DEPTH_TEST));
   for(size_t i = 0; i < objects->count; i++)
   {
      object_t* object = objects->collection[i];
      if(object->draw_info->draw_normals)
      {
         gr_transform(object->position, object->scale, object->rotation);
         sh_set_mat4(UNIFORM_NORMAL.model, model_mat);

         GL_PCALL(glBindVertexArray(object->draw_info->normal_vao));

         GL_PCALL(glDrawArrays(GL_LINES, 0, object->draw_info->normal_buffer_len));

         GL_PCALL(glBindVertexArray(0));
      }
   }
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// SSAO ROUTINES
void bind_ssao(render_stage_t* stage)
{
   //GL_PCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];

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
   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];
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

   sh_set_mat4(UNIFORM_SHADOWMAP.light_space, shadow_light->light_space);

   GL_PCALL(glCullFace(GL_FRONT));

   //stage->skip = true;
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
   render_stage_t* g_buffer_stage = default_rc->stages->collection[STAGE_G_BUFFER];
   render_stage_t* shadowmap_stage = default_rc->stages->collection[STAGE_SHADOWMAP];

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

// FXAA ROUTINES
void bind_fxaa(render_stage_t* stage)
{
   render_stage_t* shading_stage = default_rc->stages->collection[STAGE_SHADING];
   t_bind(shading_stage->color0_tex, UNIFORM_FXAA.tex);

   sh_set_int(UNIFORM_FXAA.show_edges, fxaa_edges);
   sh_set_int(UNIFORM_FXAA.on, fxaa_state);
}

void unbind_fxaa(render_stage_t* stage) { }

// GAMMA / BYPASS ROUTINES
void bind_bypass(render_stage_t* stage)
{
   t_bind(texture_to_draw, UNIFORM_GAMMA.tex);
}

void unbind_bypass(render_stage_t* stage) { }

//FONT ROUTINES
void bind_primitive(render_stage_t* stage)
{
   glEnable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void unbind_primitive(render_stage_t* stage)
{
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
}

void draw_primitives(render_stage_t* stage)
{
   render_stage_t* rs = default_rc->stages->collection[STAGE_SHADING];

   sh_use(br_shader);

   for(size_t i = 0; i < blurred_regions->count; i++)
   {
      blurred_region_t* br = blurred_regions->collection[i];
      if(!br->visible)
         continue;

      GL_PCALL(glBindVertexArray(br->vao));
      if(br->gray_color)
         sh_set_vec3(UNIFORM_BR.gray_color, br->gray_color);
      sh_set_float(UNIFORM_BR.transparency, br->transparency);
      t_bind(br->back_tex, UNIFORM_BR.back_tex);
      t_bind(br->tex, UNIFORM_BR.tex);

      GL_PCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
   }
   GL_PCALL(glBindVertexArray(0));
   sh_use(NULL);

   gr_pq_flush();
}

void bind_sprite_renderer(sprite_renderer_t* this, mat4 transform, void* data)
{
   float trans = *((float*)data);
   sh_set_mat4(UNIFORM_SPRITE.model, transform);
   sh_set_float(UNIFORM_SPRITE.transparency, trans);
}

void bind_line_renderer(struct _primitive_renderer* this, float width, vec4 color, void* data)
{
   sh_set_vec3(UNIFORM_PRIMITIVE.color, color);
}

render_chain_t* get_chain(win_info_t* info, camera_t* camera, mat4 proj)
{
   blurred_regions = list_create();

   primitive_proj = cmat4();
   mat4_ortho(primitive_proj, -1, 1, info->w, info->h);

   noise_texure = generate_noise(4);
   ssao_kernel = generate_kernel(KERNEL_SIZE);
   ssao_dummy_texture = mt_create_colored_tex(COLOR_WHITE);
   view = cmat4();

   shader_t* g_buffer_shader = setup_g_buffer(proj);
   shader_t* normal_shader = setup_normal(proj);
   shader_t* ssao_shader = setup_ssao(ssao_kernel, proj);
   shader_t* ssao_blur_shader = setup_ssao_blur();
   shader_t* skybox_shader = setup_skybox(proj);
   shader_t* shadowmap_shader = setup_shadowmap();
   shader_t* shading_shader = setup_shading();
   shader_t* fxaa_shader = setup_fxaa(0.3f, 8, 128, 8, info);
   shader_t* gamma_shader = setup_gamma();
   br_shader = setup_br(primitive_proj, 27, 7);
   shader_t* sprite_shader = setup_sprite(primitive_proj);
   shader_t* primitive_shader = setup_primitive(primitive_proj);

   default_sprite_renderer = gr_create_sprite_renderer(sprite_shader);
   default_sprite_renderer->bind_func = bind_sprite_renderer;

   default_primitive_renderer = gr_create_primitive_renderer(primitive_shader);
   default_primitive_renderer->bind_line_func = bind_line_renderer;

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
   g_buffer->color2_format.tex_int_format = GL_RGBA16F;
   g_buffer->color2_format.tex_mag_filter = GL_NEAREST;
   g_buffer->color2_format.tex_min_filter = GL_NEAREST;
   g_buffer->color2_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   g_buffer->color2_format.tex_wrapping_s = GL_CLAMP_TO_EDGE;
   g_buffer->color2_format.tex_target = GL_TEXTURE_2D;
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
   geometry_shader_data_t* g_buffer_data = (g_buffer->data = DEEPCARS_MALLOC(sizeof(geometry_shader_data_t)));
   g_buffer_data->camera = camera;
   g_buffer_data->buffmat = cmat4();

   render_stage_t* normal = rs_create("normal", RM_CUSTOM, normal_shader);
   normal->width = (float)info->w;
   normal->height = (float)info->h;
   normal->bind_func = bind_normal;
   normal->unbind_func = unbind_normal;
   normal->custom_draw_func = draw_normal;

   render_stage_t* ssao = rs_create("ssao", RM_FRAMEBUFFER, ssao_shader);
   ssao->attachments = TF_COLOR0;
   //color
   ssao->color0_format.tex_width = (float)info->w;// / 2.0f;
   ssao->color0_format.tex_height = (float)info->h;// / 2.0f;
   ssao->color0_format.tex_format = GL_RGB;
   ssao->color0_format.tex_int_format = GL_RED;
   ssao->color0_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   ssao->color0_format.tex_wrapping_s = GL_CLAMP_TO_EDGE;
   //ssao->color0_format.tex_min_filter = GL_LINEAR;
   //ssao->color0_format.tex_mag_filter = GL_LINEAR;
   ssao->bind_func = bind_ssao;
   ssao->unbind_func = unbind_ssao;
   ssao->vao = rc_get_quad_vao();
   mat4_cpy(ssao->proj, proj);

   render_stage_t* ssao_blur = rs_create("ssao_blur", RM_FRAMEBUFFER, ssao_blur_shader);
   ssao_blur->attachments = TF_COLOR0;
   ssao_blur->color0_format.tex_width = (float)info->w;// / 2.0f;
   ssao_blur->color0_format.tex_height = (float)info->h;// / 2.0f;
   ssao_blur->color0_format.tex_format = GL_RGB;
   ssao_blur->color0_format.tex_int_format = GL_RED;
   //ssao_blur->color0_format.tex_min_filter = GL_LINEAR;
   //ssao_blur->color0_format.tex_mag_filter = GL_LINEAR;
   ssao_blur->bind_func = bind_ssao_blur;
   ssao_blur->unbind_func = unbind_ssao_blur;
   ssao_blur->vao = rc_get_quad_vao();

   render_stage_t* skybox = rs_create("skybox", RM_CUSTOM, skybox_shader);
   skybox->width = (float)info->w;
   skybox->height = (float)info->h;
   skybox->bind_func = bind_skybox;
   skybox->unbind_func = unbind_skybox;
   skybox->custom_draw_func = draw_skybox;
   mat4_cpy(skybox->proj, proj);
   mat4_identity(skybox->view);
   skybox->vao = rc_get_cube_vao();
   geometry_shader_data_t* skybox_data = (skybox->data = DEEPCARS_MALLOC(sizeof(geometry_shader_data_t)));
   skybox_data->buffmat = cmat4();
   skybox_data->camera = camera;

   render_stage_t* shadowmap = rs_create("shadow_map", RM_GEOMETRY, shadowmap_shader);
   shadowmap->attachments = TF_DEPTH;
   //depth
   shadowmap->depth_format.tex_width = 1024;
   shadowmap->depth_format.tex_height = 1024;
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
   geometry_shader_data_t* shading_data = (shading->data = DEEPCARS_MALLOC(sizeof(geometry_shader_data_t)));
   shading_data->camera = camera;
   shading_data->buffmat = cmat4();

   render_stage_t* fxaa = rs_create("fxaa", RM_FRAMEBUFFER, fxaa_shader);
   fxaa->attachments = TF_COLOR0;
   fxaa->color0_format.tex_width = info->w;
   fxaa->color0_format.tex_height = info->h;
   fxaa->color0_format.tex_format = GL_RGB;
   fxaa->color0_format.tex_int_format = GL_RGB;
   fxaa->bind_func = bind_fxaa;
   fxaa->unbind_func = unbind_fxaa;
   fxaa->vao = rc_get_quad_vao();

   render_stage_t* bypass = rs_create("bypass", RM_BYPASS, gamma_shader);
   bypass->width = info->w;
   bypass->height = info->h;
   bypass->bind_func = bind_bypass;
   bypass->unbind_func = unbind_bypass;
   bypass->vao = rc_get_quad_vao();

   render_stage_t* primitive = rs_create("primitive", RM_CUSTOM, NULL);
   primitive->width = info->w;
   primitive->height = info->h;
   primitive->bind_func = bind_primitive;
   primitive->unbind_func = unbind_primitive;
   primitive->custom_draw_func = draw_primitives;

   default_rc = rc_create();
   list_push(default_rc->stages, g_buffer);
   list_push(default_rc->stages, normal);
   list_push(default_rc->stages, ssao);
   list_push(default_rc->stages, ssao_blur);
   list_push(default_rc->stages, skybox);
   list_push(default_rc->stages, shadowmap);
   list_push(default_rc->stages, shading);
   list_push(default_rc->stages, fxaa);
   list_push(default_rc->stages, bypass);
   list_push(default_rc->stages, primitive);

   editor_rc = rc_create();
   list_push(editor_rc->stages, primitive);

   rc_build(default_rc);
   rc_build(editor_rc);
   GL_PCALL(glEnable(GL_DEPTH_TEST));
   return default_rc;
}

void free_geometry_shader_data(render_chain_t* render_chain, size_t index)
{
   geometry_shader_data_t* gsd = ((render_stage_t*)render_chain->stages->collection[index])->data;
   mat4_free(gsd->buffmat);
}

void free_stages(void)
{
   DEEPCARS_FREE(default_primitive_renderer);
   DEEPCARS_FREE(default_sprite_renderer);

   free_geometry_shader_data(default_rc, STAGE_G_BUFFER);
   rs_free(default_rc->stages->collection[STAGE_G_BUFFER]);
   rs_free(default_rc->stages->collection[STAGE_NORMAL]);

   rs_free(default_rc->stages->collection[STAGE_SSAO]);
   rs_free(default_rc->stages->collection[STAGE_SSAO_BLUR]);

   free_geometry_shader_data(default_rc, STAGE_SKYBOX);
   rs_free(default_rc->stages->collection[STAGE_SKYBOX]);

   rs_free(default_rc->stages->collection[STAGE_SHADOWMAP]);

   free_geometry_shader_data(default_rc, STAGE_SHADING);
   rs_free(default_rc->stages->collection[STAGE_SHADING]);

   rs_free(default_rc->stages->collection[STAGE_FXAA]);
   rs_free(default_rc->stages->collection[STAGE_BYPASS]);
   rs_free(default_rc->stages->collection[STAGE_PRIMITIVE]);

   rc_free(default_rc, false);
   rc_free(editor_rc, false);

   mat4_free(view);

   for(size_t i = 0; i < KERNEL_SIZE; i++)
      vec4_free(ssao_kernel[i]);
   DEEPCARS_FREE(ssao_kernel);
}
