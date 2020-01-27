//
// Created by maxim on 1/10/20.
//

#ifndef DEEPCARS_SHADER_SETUP_H
#define DEEPCARS_SHADER_SETUP_H

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#include "../../lib/resources/shader.h"
#include "../win_defaults.h"

struct {
   GLint model;
   GLint view;

   GLint diffuse_tex;
   GLint spec_tex;
} UNIFORM_GBUFF;

struct {
   GLint pos_tex;
   GLint norm_tex;
   GLint noise_tex;

} UNIFORM_SSAO;

struct {
   GLint tex;
} UNIFORM_SSAO_BLUR;

struct {
   GLint view;

   GLint skybox_tex;
} UNIFORM_SKYBOX;

struct {
   GLint light_space;
   GLint model;

} UNIFORM_SHADOWMAP;

struct {

   GLint view_pos;
   GLint view;

   GLint lights_position[NR_LIGHTS];
   GLint lights_color[NR_LIGHTS];
   GLint lights_linear[NR_LIGHTS];
   GLint lights_quadratic[NR_LIGHTS];
   GLint lights_radius[NR_LIGHTS];

   GLint shadow_light_position;
   GLint shadow_light_direction;
   GLint shadow_light_bright;
   GLint shadow_light_lightspace;

   GLint pos_tex;
   GLint norm_tex;
   GLint albedoSpec_tex;
   GLint ssao_tex;
   GLint shadow_light_shadowmap_tex;

} UNIFORM_SHADING;

struct {
   GLint tex;

} UNIFORM_GAMMA;

struct {
   GLint tex;

   GLint color;
   GLint params;
   GLint borderColor;

} UNIFORM_FONT;

struct {
   GLint tex;
   GLint back_tex;

   GLint grayscale;
   GLint gray_color;

} UNIFORM_BR;

struct {
   GLint tex;
   GLint model;

} UNIFORM_SPRITE;

shader_t* setup_g_buffer(mat4 proj);
shader_t* setup_ssao(vec4* kernel, mat4 proj);
shader_t* setup_ssao_blur(void);
shader_t* setup_skybox(mat4 proj);
shader_t* setup_shadowmap(void);
shader_t* setup_shading(void);
shader_t* setup_gamma(void);
shader_t* setup_font(mat4 primitive_proj);
shader_t* setup_br(mat4 primitive_proj, float sigma, size_t kernel_len);
shader_t* setup_sprite(mat4 primitive_proj);

#pragma clang diagnostic pop

#endif //DEEPCARS_SHADER_SETUP_H