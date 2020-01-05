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

render_chain_t* rc1;
render_chain_t* rc2;

typedef struct _geometry_shader_data {
   camera_t* camera;
   mat4 buffmat;

} geometry_shader_data_t;

texture_t* noise_texure;
vec4* ssao_kernel;
texture_t* ssao_texture;
int ssao_state = 0;

void switch_ssao(void)
{
   render_stage_t* ssao_blur_stage = rc1->stages->collection[2];

   if(ssao_state) ssao_texture = rm_getn(TEXTURE, "__generated_mt_default_amb_1.0_1.0_1.0");
   else ssao_texture = ssao_blur_stage->color0_tex;
   ssao_state = !ssao_state;
}

texture_t* texture_to_draw;
int state = -1;
void switch_stages()
{
   render_stage_t* g_buffer_stage = rc1->stages->collection[0];
   render_stage_t* ssao_stage = rc1->stages->collection[1];
   render_stage_t* ssao_blur_stage = rc1->stages->collection[2];
 //render_stage_t* skybox_stage = rc1->stages->collection[3];
   render_stage_t* shading_stage = rc1->stages->collection[4];

   state = (state + 1) % 6;
   switch(state)
   {
      default:
      case 0:
         texture_to_draw = shading_stage->color0_tex; // result
         puts("0. Result image");
         break;
      case 1:
         texture_to_draw = g_buffer_stage->color0_tex; //positions
         puts("1. Positions");
         break;
      case 2:
         texture_to_draw = g_buffer_stage->color1_tex; //normals
         puts("2. Normals");
         break;
      case 3:
         texture_to_draw = g_buffer_stage->color2_tex; //albedo_spec
         puts("3. Albedo spec");
         break;
      case 4:
         texture_to_draw = ssao_stage->color0_tex; //ssao
         puts("4. SSAO");
         break;
      case 5:
         texture_to_draw = ssao_blur_stage->color0_tex; //ssao blurred
         puts("5. Blurred SSAO");
         break;
   }
}

// G BUFFER ROUTINES
void bind_g_buffer(render_stage_t* stage)
{
   GL_PCALL(glClearColor(0,0,0,0));
   GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   c_to_mat(data->buffmat, data->camera);

   //scene_t* scene = scm_get_current();
   //mat4_cpy(scene->light->light_space, scene->light->light_proj);
   //mat4_mulm(scene->light->light_space, scene->light->light_view);

   sh_nset_mat4(stage->shader, "projection", stage->proj);
   sh_nset_mat4(stage->shader, "view", data->buffmat);

   sh_nset_int(stage->shader, "texture_diffuse", 0);
   sh_nset_int(stage->shader, "texture_specular", 1);
}

void unbind_g_buffer(render_stage_t* stage)
{
}

void setup_object_g_buffer(render_stage_t* stage, object_t* object, mat4 model_mat)
{
   t_bind(object->draw_info->material->map_diffuse, 0);
   t_bind(object->draw_info->material->map_specular, 1);
   //t_bind(object->draw_info->material->map_ambient, 3);
   //sh_nset_float(stage->shader, "shininess", object->draw_info->material->shininess);

   sh_nset_mat4(stage->shader, "model", model_mat);
}

// SSAO ROUTINES
void bind_ssao(render_stage_t* stage)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   render_stage_t* g_buffer_stage = rc1->stages->collection[0];
   sh_nset_int(stage->shader, "gPosition", 0);
   sh_nset_int(stage->shader, "gNormal", 1);
   sh_nset_int(stage->shader, "texNoise", 2);

   t_bind(g_buffer_stage->color0_tex, 0);
   t_bind(g_buffer_stage->color1_tex, 1);
   t_bind(noise_texure, 2);

   sh_nset_mat4(stage->shader, "projection", stage->proj);
   for (size_t i = 0; i < KERNEL_SIZE; i++)
   {
      char buffer[50];
      snprintf(buffer, 50, "samples[%li]", i);
      sh_nset_vec3(stage->shader, buffer, ssao_kernel[i]);
   }
}

void unbind_ssao(render_stage_t* stage) { }

// SSAO BLUR ROUTINES
void bind_ssao_blur(render_stage_t* stage)
{
   sh_nset_int(stage->shader, "ssaoInput", 0);
   t_bind(stage->prev_stage->color0_tex, 0);
}

void unbind_ssao_blur(render_stage_t* stage) { }

// SKYBOX ROUTINES
void bind_skybox(render_stage_t* stage)
{
   geometry_shader_data_t* data = (geometry_shader_data_t*)stage->data;
   c_to_mat(data->buffmat, data->camera);

   scene_t* scene = scm_get_current();

   data->buffmat[3] = 0;
   data->buffmat[7] = 0;
   data->buffmat[11] = 0;
   data->buffmat[15] = 0;

   sh_nset_mat4(stage->shader, "view", data->buffmat);
   sh_nset_mat4(stage->shader, "projection", stage->proj);
   sh_nset_int(stage->shader, "skybox", 0);

   t_bind(scene->skybox, 0);
}

void unbind_skybox(render_stage_t* stage)
{
   GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void draw_skybox(render_stage_t* stage)
{
   render_stage_t* g_buffer_stage = rc1->stages->collection[0];
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

// SHADING ROUTINES
void bind_shading(render_stage_t* stage)
{
   render_stage_t* g_buffer_stage = rc1->stages->collection[0];
   render_stage_t* ssao_stage = rc1->stages->collection[1];

   geometry_shader_data_t* data = stage->data;
   list_t* lights = scm_get_current()->lights;
   c_to_mat(data->buffmat, data->camera);

   sh_nset_int(stage->shader, "gPosition", 0);
   sh_nset_int(stage->shader, "gNormal", 1);
   sh_nset_int(stage->shader, "gAlbedoSpec", 2);
   sh_nset_int(stage->shader, "ssao", 3);

   sh_nset_vec3(stage->shader, "viewPos", data->camera->position);
   //mat4_print(data->buffmat);
   sh_nset_mat4(stage->shader, "view", data->buffmat);

   t_bind(g_buffer_stage->color0_tex, 0);
   t_bind(g_buffer_stage->color1_tex, 1);
   t_bind(g_buffer_stage->color2_tex, 2);
   t_bind(ssao_texture, 3);

   for (size_t i = 0; i < lights->count; i++)
   {
      light_t* lt = lights->collection[i];

      char buffer[255];
      snprintf(buffer, 255, "lights[%li].Position", i);
      sh_nset_vec3(stage->shader, buffer, lt->position);

      snprintf(buffer, 255, "lights[%li].Color", i);
      sh_nset_vec3(stage->shader, buffer, lt->color);

      const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
      const float linear = 0.045f;
      const float quadratic = 0.0075f;

      snprintf(buffer, 255, "lights[%li].Linear", i);
      sh_nset_float(stage->shader, buffer, linear);

      snprintf(buffer, 255, "lights[%li].Quadratic", i);
      sh_nset_float(stage->shader, buffer, quadratic);

      const float max_brightness = fmaxf(fmaxf(lt->color[0], lt->color[1]), lt->color[2]);
      float radius = (-linear + sqrtf(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * max_brightness))) / (2.0f * quadratic);

      snprintf(buffer, 255, "lights[%li].Radius", i);
      sh_nset_float(stage->shader, buffer, radius);
   }
}

void unbind_shading(render_stage_t* stage)
{
}

// GAMMA / BYPASS ROUTINES
void bind_bypass(render_stage_t* stage)
{
   sh_nset_int(stage->shader, "tex", 0);
   t_bind(texture_to_draw, 0);
}

void unbind_bypass(render_stage_t* stage)
{
}

render_chain_t* get_chain(win_info_t* info, camera_t* camera, mat4 proj)
{
   noise_texure = generate_noise(4);
   ssao_kernel = generate_kernel(KERNEL_SIZE);

   shader_t* g_buffer_shader = s_getn_shader("g_buffer");
   shader_t* ssao_shader = s_getn_shader("ssao");
   shader_t* ssao_blur_shader = s_getn_shader("ssao_blur");
   shader_t* skybox_shader = s_getn_shader("skybox");
   shader_t* shading_shader = s_getn_shader("shading");
   shader_t* gamma_shader = s_getn_shader("gamma");

   render_stage_t* g_buffer = rs_create(RM_GEOMETRY, g_buffer_shader);
   g_buffer->attachments = TF_COLOR0 | TF_COLOR1 | TF_COLOR2 | TF_DEPTH;
   // Position buffer
   g_buffer->color0_format.tex_width = win->w;
   g_buffer->color0_format.tex_height = win->h;
   g_buffer->color0_format.tex_format = GL_RGB;
   g_buffer->color0_format.tex_int_format = GL_RGB16F;
   g_buffer->color0_format.tex_mag_filter = GL_NEAREST;
   g_buffer->color0_format.tex_min_filter = GL_NEAREST;
   g_buffer->color0_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   g_buffer->color0_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   // Normals buffer
   g_buffer->color1_format.tex_width = win->w;
   g_buffer->color1_format.tex_height = win->h;
   g_buffer->color1_format.tex_format = GL_RGB;
   g_buffer->color1_format.tex_int_format = GL_RGB16F;
   g_buffer->color1_format.tex_mag_filter = GL_NEAREST;
   g_buffer->color1_format.tex_min_filter = GL_NEAREST;
   g_buffer->color1_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   g_buffer->color1_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   // Color + Specular buffer
   g_buffer->color2_format.tex_width = win->w;
   g_buffer->color2_format.tex_height = win->h;
   g_buffer->color2_format.tex_format = GL_RGBA;
   g_buffer->color2_format.tex_int_format = GL_RGBA;
   g_buffer->color2_format.tex_mag_filter = GL_NEAREST;
   g_buffer->color2_format.tex_min_filter = GL_NEAREST;
   g_buffer->color2_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   g_buffer->color2_format.tex_wrapping_t = GL_CLAMP_TO_EDGE;
   // Depth component
   g_buffer->depth_format.tex_width = win->w;
   g_buffer->depth_format.tex_height = win->h;
   g_buffer->depth_format.tex_format = GL_DEPTH_COMPONENT;
   g_buffer->depth_format.tex_int_format = GL_DEPTH_COMPONENT;
   g_buffer->depth_format.tex_mag_filter = GL_NEAREST;
   g_buffer->depth_format.tex_min_filter = GL_NEAREST;

   mat4_cpy(g_buffer->proj, proj);
   mat4_identity(g_buffer->view);
   g_buffer->bind_func = bind_g_buffer;
   g_buffer->unbind_func = unbind_g_buffer;
   g_buffer->setup_obj_func = setup_object_g_buffer;
   geometry_shader_data_t* g_buffer_data = (g_buffer->data = malloc(sizeof(geometry_shader_data_t)));
   g_buffer_data->camera = camera;
   g_buffer_data->buffmat = cmat4();

   render_stage_t* ssao = rs_create(RM_FRAMEBUFFER, ssao_shader);
   ssao->attachments = TF_COLOR0 | TF_DEPTH;
   //color
   ssao->color0_format.tex_width = win->w;
   ssao->color0_format.tex_height = win->h;
   ssao->color0_format.tex_format = GL_RGB;
   ssao->color0_format.tex_int_format = GL_RED;
   //depth
   ssao->depth_format.tex_width = win->w;
   ssao->depth_format.tex_height = win->h;
   ssao->depth_format.tex_format = GL_DEPTH_COMPONENT;
   ssao->depth_format.tex_int_format = GL_DEPTH_COMPONENT;
   ssao->depth_format.tex_mag_filter = GL_NEAREST;
   ssao->depth_format.tex_min_filter = GL_NEAREST;

   ssao->bind_func = bind_ssao;
   ssao->unbind_func = unbind_ssao;
   ssao->vao = rc_get_quad_vao();
   mat4_cpy(ssao->proj, proj);

   render_stage_t* ssao_blur = rs_create(RM_FRAMEBUFFER, ssao_blur_shader);
   ssao_blur->attachments = TF_COLOR0;
   ssao_blur->color0_format.tex_width = info->w;
   ssao_blur->color0_format.tex_height = info->h;
   ssao_blur->color0_format.tex_format = GL_RGB;
   ssao_blur->color0_format.tex_int_format = GL_RED;
   ssao_blur->bind_func = bind_ssao_blur;
   ssao_blur->unbind_func = unbind_ssao_blur;
   ssao_blur->vao = rc_get_quad_vao();

   render_stage_t* skybox = rs_create(RM_CUSTOM, skybox_shader);
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

   render_stage_t* shading = rs_create(RM_FRAMEBUFFER, shading_shader);
   shading->attachments = TF_COLOR0;
   shading->color0_format.tex_width = info->w;
   shading->color0_format.tex_height = info->h;
   shading->color0_format.tex_format = GL_RGB;
   shading->color0_format.tex_int_format = GL_RGB16F;
   shading->bind_func = bind_shading;
   shading->unbind_func = unbind_shading;
   shading->vao = rc_get_quad_vao();
   geometry_shader_data_t* shading_data = (shading->data = malloc(sizeof(geometry_shader_data_t)));
   shading_data->camera = camera;
   shading_data->buffmat = cmat4();

   render_stage_t* bypass = rs_create(RM_BYPASS, gamma_shader);
   bypass->width = info->w;
   bypass->height = info->h;
   bypass->bind_func = bind_bypass;
   bypass->unbind_func = unbind_bypass;
   bypass->vao = rc_get_quad_vao();

   rc1 = rc_create();
   list_push(rc1->stages, g_buffer);
   list_push(rc1->stages, ssao);
   list_push(rc1->stages, ssao_blur);
   list_push(rc1->stages, skybox);
   list_push(rc1->stages, shading);
   list_push(rc1->stages, bypass);

   rc_build(rc1);
   GL_PCALL(glEnable(GL_DEPTH_TEST));
   return rc1;
}

void free_stages(void)
{

}
