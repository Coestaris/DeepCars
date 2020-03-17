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
#include "rfuncs/rfuncs.h"


static char stage_string_buffer[140];
//ssao

static vec4*      ssao_kernel        = NULL;
static texture_t* ssao_dummy_texture = NULL;

//stages
static uint32_t   ssao_state      = 0;
static uint32_t   render_state    = -1;

texture_t* texture_to_draw = NULL;
uint32_t   fxaa_state      = 1;
uint32_t   fxaa_edges      = 0;
shader_t*  br_shader       = NULL;
uint32_t   wireframe       = 0;
texture_t* ssao_texture    = NULL;
texture_t* noise_texture   = NULL;

list_t*               blurred_regions            = NULL;
sprite_renderer_t*    default_sprite_renderer    = NULL;
primitive_renderer_t* default_primitive_renderer = NULL;

render_chain_t* default_rc = NULL;
render_chain_t* editor_rc  = NULL;

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

char* get_stage_string()
{
   if(render_state == 0)
      snprintf(stage_string_buffer, sizeof(stage_string_buffer), "Default view, SSAO %s, FXAA: %s",
            ssao_state ? "on" : "off",
            fxaa_state ? "on" : "off");
   else
   {
      snprintf(stage_string_buffer, sizeof(stage_string_buffer),
               render_state == 1 ? "Local positions view" :
               render_state == 2 ? "Local normals view" :
               render_state == 3 ? "Colors view" :
               render_state == 4 ? "World position view" :
               render_state == 5 ? "SSAO view" :
               render_state == 6 ? "Blurred SSAO view" :
               "ShadowMap view");
   }
   return stage_string_buffer;
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

   render_state = (render_state + 1) % 8;
   switch(render_state)
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

render_chain_t* get_chain(win_info_t* info, camera_t* camera, mat4 proj)
{
   blurred_regions = list_create();

   primitive_proj = cmat4();
   mat4_ortho(primitive_proj, -1, 1, info->w, info->h);

   noise_texture = generate_noise(4);
   ssao_kernel = generate_kernel(KERNEL_SIZE);
   ssao_dummy_texture = mt_create_colored_tex(COLOR_WHITE);
   render_view = cmat4();

   shader_t* g_buffer_shader = setup_g_buffer(proj);
   shader_t* g_buffer_instanced_shader = setup_g_buffer_instance(proj);
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
   shader_t* line_primitive_shader = setup_line_primitive(primitive_proj, vec2f(win->w, win->h));

   default_sprite_renderer = gr_create_sprite_renderer(sprite_shader);
   default_sprite_renderer->bind_func = bind_sprite_renderer;

   default_primitive_renderer = gr_create_primitive_renderer(primitive_shader);
   default_primitive_renderer->line_shader = line_primitive_shader;
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
   g_buffer->geometry_filter = GF_OBJECTS;

   geometry_shader_data_t* g_buffer_data = (g_buffer->data = DEEPCARS_MALLOC(sizeof(geometry_shader_data_t)));
   g_buffer_data->camera = camera;
   g_buffer_data->buffmat = cmat4();

   render_stage_t* g_buffer_instanced = rs_create("gbuffer_inst", RM_GEOMETRY_NOFRAMEBUFFER, g_buffer_instanced_shader);
   g_buffer_instanced->width = (float)info->w;
   g_buffer_instanced->height = (float)info->h;
   g_buffer_instanced->bind_func = bind_g_buffer_inst;
   g_buffer_instanced->unbind_func = unbind_g_buffer_inst;
   g_buffer_instanced->setup_instance_func = setup_g_buffer_inst;
   g_buffer_instanced->geometry_filter = GF_INSTANCED;

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
   shadowmap->geometry_filter = GF_OBJECTS;


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
   list_push(default_rc->stages, g_buffer_instanced);
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

static void free_geometry_shader_data(render_chain_t* render_chain, size_t index)
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
   rs_free(default_rc->stages->collection[STAGE_G_BUFFER_INST]);
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

   mat4_free(render_view);

   for(size_t i = 0; i < KERNEL_SIZE; i++)
      vec4_free(ssao_kernel[i]);
   DEEPCARS_FREE(ssao_kernel);
}
