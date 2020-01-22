//
// Created by maxim on 1/10/20.
//

#ifdef __GNUC__
#pragma implementation "shader_setup.h"
#endif
#include "shader_setup.h"

#include "../../lib/resources/shm.h"
#include "../win_defaults.h"

GLint get_loc(shader_t* sh, const char* name)
{
   GLint location = glGetUniformLocation(sh->prog_id, name);
   if(location == -1)
   {
      //APP_ERROR("glGetUniformLocation of \"%s\" in shader \"%s\" returned -1", name, sh->name);
   }
   else return location;
}

shader_t* setup_g_buffer(mat4 proj)
{
   shader_t* sh = s_getn_shader("g_buffer");

   UNIFORM_GBUFF.model = get_loc(sh, "model");
   UNIFORM_GBUFF.view = get_loc(sh, "view");

   sh_use(sh);
   sh_nset_int(sh, "texture_diffuse", UNIFORM_GBUFF.diffuse_tex = 0);
   sh_nset_int(sh, "texture_specular", UNIFORM_GBUFF.spec_tex = 1);
   sh_nset_mat4(sh, "projection", proj);
   sh_use(NULL);

   return sh;
}

shader_t* setup_ssao(vec4* kernel, mat4 proj)
{
   shader_t* sh = s_getn_shader("ssao");

   sh_use(sh);
   sh_nset_int(sh, "gPosition", UNIFORM_SSAO.pos_tex = 0);
   sh_nset_int(sh, "gNormal", UNIFORM_SSAO.norm_tex = 1);
   sh_nset_int(sh, "texNoise", UNIFORM_SSAO.noise_tex = 2);
   sh_nset_mat4(sh, "projection", proj);
   for (size_t i = 0; i < KERNEL_SIZE; i++)
   {
      char buffer[50];
      snprintf(buffer, 50, "samples[%li]", i);
      sh_nset_vec3(sh, buffer, kernel[i]);
   }

   sh_use(NULL);

   return sh;
}

shader_t* setup_ssao_blur(void)
{
   shader_t* sh = s_getn_shader("ssao_blur");

   sh_use(sh);
   sh_nset_int(sh, "ssaoInput", UNIFORM_SSAO_BLUR.tex = 0);
   sh_use(NULL);

   return sh;
}

shader_t* setup_skybox(mat4 proj)
{
   shader_t* sh = s_getn_shader("skybox");

   UNIFORM_SKYBOX.view = get_loc(sh, "view");
   sh_use(sh);
   sh_nset_int(sh, "skybox", UNIFORM_SKYBOX.skybox_tex = 0);
   sh_nset_mat4(sh, "projection", proj);
   sh_use(NULL);

   return sh;
}

shader_t* setup_shadowmap(void)
{
   shader_t* sh = s_getn_shader("shadowmap");

   UNIFORM_SHADOWMAP.light_space = get_loc(sh, "lightSpaceMatrix");
   UNIFORM_SHADOWMAP.model = get_loc(sh, "model");

   return sh;
}

shader_t* setup_shading(void)
{
   shader_t* sh = s_getn_shader("shading");

   UNIFORM_SHADING.shadow_light_position = get_loc(sh, "shadowLight.Position");
   UNIFORM_SHADING.shadow_light_direction = get_loc(sh, "shadowLight.Direction");
   UNIFORM_SHADING.shadow_light_bright = get_loc(sh, "shadowLight.Brightness");
   UNIFORM_SHADING.shadow_light_lightspace = get_loc(sh, "shadowLight.lightSpaceMatrix");

   for(size_t i = 0; i < NR_LIGHTS; i++)
   {
      char buffer[255];
      snprintf(buffer, 255, "lights[%li].Position", i);
      UNIFORM_SHADING.lights_position[i] = get_loc(sh, buffer);

      snprintf(buffer, 255, "lights[%li].Color", i);
      UNIFORM_SHADING.lights_color[i] = get_loc(sh, buffer);

      snprintf(buffer, 255, "lights[%li].Linear", i);
      UNIFORM_SHADING.lights_linear[i] = get_loc(sh, buffer);

      snprintf(buffer, 255, "lights[%li].Quadratic", i);
      UNIFORM_SHADING.lights_quadratic[i] = get_loc(sh, buffer);

      snprintf(buffer, 255, "lights[%li].Radius", i);
      UNIFORM_SHADING.lights_radius[i] = get_loc(sh, buffer);
   }

   UNIFORM_SHADING.view_pos = get_loc(sh, "viewPos");
   UNIFORM_SHADING.view = get_loc(sh, "view");

   sh_use(sh);
   sh_nset_int(sh, "gPosition", UNIFORM_SHADING.pos_tex = 0);
   sh_nset_int(sh, "gNormal", UNIFORM_SHADING.norm_tex = 1);
   sh_nset_int(sh, "gAlbedoSpec", UNIFORM_SHADING.albedoSpec_tex = 2);
   sh_nset_int(sh, "ssao", UNIFORM_SHADING.ssao_tex = 3);
   sh_nset_int(sh, "shadowLight.shadowMap", UNIFORM_SHADING.shadow_light_shadowmap_tex = 4);
   sh_use(NULL);

   return sh;
}

shader_t* setup_gamma(void)
{
   shader_t* sh = s_getn_shader("gamma");

   sh_use(sh);
   sh_nset_int(sh, "tex", UNIFORM_GAMMA.tex = 0);
   sh_use(NULL);

   return sh;
}

shader_t* setup_font(mat4 font_ortho)
{
   shader_t* sh = s_getn_shader("font");

   UNIFORM_FONT.color = get_loc(sh, "color");
   UNIFORM_FONT.params = get_loc(sh, "params");
   UNIFORM_FONT.borderColor = get_loc(sh, "borderColor");

   sh_use(sh);
   sh_nset_int(sh, "tex", UNIFORM_FONT.tex = 0);
   sh_nset_mat4(sh, "proj", font_ortho);
   sh_use(NULL);

   return sh;
}