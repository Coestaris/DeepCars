//
// Created by maxim on 11/30/19.
//

#ifndef DEEPCARS_RENDER_STAGE_H
#define DEEPCARS_RENDER_STAGE_H

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#include "../../coredefs.h"
#include "../../object.h"
#include "../../resources/shader.h"
#include "instance_collection.h"

// Measure time spent by every stage
#define MEASURE_RENDER_TIME
// Time interpolation amount
#define MEASURE_RENDER_SMOOTH 0.9
// Pediod of printing stage times
//#define OUTPUT_RENDER_TIME 5

typedef enum _render_mode
{
   // User must specify custom draw func
   RM_CUSTOM,
   // User must specify custom draw func. Allocating framebuffer
   RM_CUSTOM_FRAMEBUFFER,
   // Call draw function for every object. Allocating framebuffer
   RM_GEOMETRY,
   // Call draw function for every object. But we dont allocating framebuffer
   RM_GEOMETRY_NOFRAMEBUFFER,
   // No draw function specified (only shader rendering). Allocating framebuffer
   RM_FRAMEBUFFER,
   // No draw function specified (only shader rendering). Rendering to a screen.
   RM_BYPASS

} render_mode_t;

// Specifies levels of framebuffer in stage
typedef enum tex_format
{
  TF_STENCIL   = 1,
  TF_DEPTH     = 2,
  TF_COLOR0    = 4,
  TF_COLOR1    = 8,
  TF_COLOR2    = 16,
  TF_COLOR3    = 32,
  TF_COLOR4    = 64,
  TF_COLOR5    = 128,

} tex_format_t;

// Parameters of every attachment in framebuffer
typedef struct _attachment_options
{
   // OpenGL texture format
   GLenum tex_format;
   // OpenGL internal texture format
   GLenum tex_int_format;

   // Texture size
   GLint tex_width;
   GLint tex_height;

   // Downscale texture function
   GLenum tex_min_filter;
   // Upscale texture function
   GLenum tex_mag_filter;
   // Vertical wrapping mode
   GLenum tex_wrapping_s;
   // Horizontal wrapping mode
   GLenum tex_wrapping_t;

   // Texture rendering target
   GLenum tex_target;

   // Texture border color
   float tex_border_color[4];

} attachment_options_t;

// Describes rendering stage that could contain up to 6 color attachments, stencil and depth buffers.
// Will act differently depending on specified rendering mode.
typedef struct _render_stage
{
   // Name of the stage
   const char* name;

   // Stage transform matrices
   mat4 proj;
   mat4 view;

   // Main shader program of the stage
   shader_t* shader;

   // Stage mode
   render_mode_t render_mode;

   // Skip that stage while rendering
   bool skip;

   // Always called before drawing
   void (*bind_func)(struct _render_stage* this);
   // Always called after drawing
   void (*unbind_func)(struct _render_stage* this);
   // Called for every object in scene before rendering its VAO (only if render_mode set to GEOMETRY)
   void (*setup_obj_func)(struct _render_stage* this, object_t* render_obj, mat4 model_mat);
   // Called for every instanced collection (only if render_mode set to GEOMETRY)
   void (*setup_instance_func)(struct _render_stage* this, instance_collection_t* collection);
   // Called as main draw func if render_mode set to CUSTOM_*
   void (*custom_draw_func)(struct _render_stage* this);

   // User data of the stage
   void* data;

   // List of all attachments of the framebuffer
   tex_format_t attachments;

   // General size of the stage
   GLuint width;
   GLuint height;

   // Framebuffer object
   GLuint fbo;

   // Color attachments and its parameters
   texture_t* color0_tex;
   attachment_options_t color0_format;
   texture_t* color1_tex;
   attachment_options_t color1_format;
   texture_t* color2_tex;
   attachment_options_t color2_format;
   texture_t* color3_tex;
   attachment_options_t color3_format;
   texture_t* color4_tex;
   attachment_options_t color4_format;
   texture_t* color5_tex;
   attachment_options_t color5_format;

   // Depth buffer and its parameters
   texture_t* depth_tex;
   attachment_options_t depth_format;

   // Stencil buffer and its parameters
   texture_t* stencil_tex;
   attachment_options_t stencil_format;

   // Optional VAO of stage (for rendering from to a framebuffer)
   GLuint vao;

#ifdef MEASURE_RENDER_TIME
   // Time spend by rendering current stage
   double render_time;
#endif

   // Previous stage. Set to NULL for first stages. Set by rc_link function
   struct _render_stage* prev_stage;

} render_stage_t;

// render_stage_t constructor
render_stage_t* rs_create(const char* name, render_mode_t render_mode, shader_t* shader);

// Frees all resources allocated by stage
void rs_free(render_stage_t* rs);

// Creates OpenGL textures for the stage
void rs_build_tex(render_stage_t* rs);

#pragma clang diagnostic pop

#endif //DEEPCARS_RENDER_STAGE_H

