//
// Created by maxim on 8/26/19.
//

#ifdef __GNUC__
#pragma implementation "graphics.h"
#endif
#include "graphics.h"

#define GR_LOG(format, ...) DC_LOG("graphics.c", format, __VA_ARGS__)
#define GR_ERROR(format, ...) DC_ERROR("graphics.c", format, __VA_ARGS__)

#include "../light.h"
#include "../../resources/font.h"

// for current scene
#include "../../scm.h"

vec4 COLOR_WHITE;
vec4 COLOR_SILVER;
vec4 COLOR_GRAY;
vec4 COLOR_BLACK;
vec4 COLOR_RED;
vec4 COLOR_MAROON;
vec4 COLOR_YELLOW;
vec4 COLOR_OLIVE;
vec4 COLOR_LIME;
vec4 COLOR_GREEN;
vec4 COLOR_AQUA;
vec4 COLOR_TEAL;
vec4 COLOR_BLUE;
vec4 COLOR_NAVY;
vec4 COLOR_FUCHSIA;
vec4 COLOR_PURPLE;

mat4 proj_mat;
mat4 view_mat;
mat4 model_mat;

mat4 x_rot_mat;
mat4 y_rot_mat;
mat4 z_rot_mat;

struct queue_item {

   uint8_t mode;

   texture_t* tex;
   vec2f_t center;
   float angle;
   bool bind_shader;

   font_t* font;
   char* string;

   vec2f_t position;
   vec2f_t scale;
   void* data;
} queue[MAX_DEPTH][MAX_QUEUE_COUNT];

size_t queue_length[MAX_DEPTH];

inline void gr_fill(vec4 color)
{
   GL_PCALL(glClearColor(color[0], color[1], color[2], color[3]));
}

shader_t* sprite_shader;
GLuint vao;
void gr_init()
{
   COLOR_WHITE = cvec4(1, 1, 1, 0);
   COLOR_SILVER = cvec4(.75, .75, .75, 0);
   COLOR_GRAY = cvec4(.5, .5, .5, 0);
   COLOR_BLACK = cvec4(0, 0, 0, 0);
   COLOR_RED = cvec4(1, 0, 0, 0);
   COLOR_MAROON = cvec4(.5, 0, 0, 0);
   COLOR_YELLOW = cvec4(1, 1, 0, 0);
   COLOR_OLIVE = cvec4(.5, .5, 0, 0);
   COLOR_LIME = cvec4(0, 1, 0, 0);
   COLOR_GREEN = cvec4(0, 0.5, 0, 0);
   COLOR_AQUA = cvec4(0, 1, 1, 0);
   COLOR_TEAL = cvec4(0, .5, .5, 0);
   COLOR_BLUE = cvec4(0, 0, 1, 0);
   COLOR_NAVY = cvec4(0, 0, .5, 0);
   COLOR_FUCHSIA = cvec4(1, 0, 1, 0);
   COLOR_PURPLE = cvec4(.5, 0, .5, 0);

   proj_mat = cmat4();
   view_mat = cmat4();
   model_mat = cmat4();

   x_rot_mat = cmat4();
   y_rot_mat = cmat4();
   z_rot_mat = cmat4();

   memset(queue, 0, sizeof(queue));
   memset(queue_length, 0, sizeof(queue_length));

   sprite_shader = s_getn_shader("sprite");

   // Configure VAO/VBO
   GLuint vbo;
   GLfloat vertices[] = {
         // Pos      // Tex
         0.0f, 1.0f, 0.0f, 1.0f,
         1.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f,

         0.0f, 1.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 1.0f, 1.0f,
         1.0f, 0.0f, 1.0f, 0.0f
   };

   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);

   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glBindVertexArray(vao);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}

inline void gr_render_vao(GLuint vao)
{
   GL_PCALL(glBindVertexArray(vao));
   //GL_PCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

   GL_PCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

   GL_PCALL(glBindVertexArray(0));
   //GL_PCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void gr_free(void)
{
   vec4_free(COLOR_WHITE);
   vec4_free(COLOR_SILVER);
   vec4_free(COLOR_GRAY);
   vec4_free(COLOR_BLACK);
   vec4_free(COLOR_RED);
   vec4_free(COLOR_MAROON);
   vec4_free(COLOR_YELLOW);
   vec4_free(COLOR_OLIVE);
   vec4_free(COLOR_LIME);
   vec4_free(COLOR_GREEN);
   vec4_free(COLOR_AQUA);
   vec4_free(COLOR_TEAL);
   vec4_free(COLOR_BLUE);
   vec4_free(COLOR_NAVY);
   vec4_free(COLOR_FUCHSIA);
   vec4_free(COLOR_PURPLE);

   mat4_free(proj_mat);
   mat4_free(view_mat);
   mat4_free(model_mat);

   mat4_free(x_rot_mat);
   mat4_free(y_rot_mat);
   mat4_free(z_rot_mat);
}

void gr_draw_model(model_t* model)
{
   GL_PCALL(glBindVertexArray(model->VAO))
   GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, model->triangles_count * 3))
   //GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, 0))
   GL_PCALL(glBindVertexArray(0))
}

inline mat4 gr_transform(vec3f_t pos, vec3f_t scale, vec3f_t rot)
{
   mat4_identity(model_mat);
   mat4_translate(model_mat, (float) pos.x, (float) pos.y, (float) pos.z);
   mat4_scale(model_mat, (float) scale.x, (float) scale.y, (float) scale.z);

   if(rot.x != 0)
   {
      mat4_rotate_x(x_rot_mat, rot.x);
      mat4_mulm(model_mat, x_rot_mat);
   }

   if(rot.y != 0)
   {
      mat4_rotate_y(y_rot_mat, rot.y);
      mat4_mulm(model_mat, y_rot_mat);
   }

   if(rot.z != 0)
   {
      mat4_rotate_z(z_rot_mat, rot.z);
      mat4_mulm(model_mat, z_rot_mat);
   }

   return model_mat;
}

inline void gr_render_object(object_t* obj)
{
   gr_draw_model(obj->draw_info->model);
}

inline void gr_bind(render_stage_t* stage)
{
   GL_PCALL(glViewport(0, 0, stage->width, stage->height))
   if(stage->render_mode != RM_BYPASS && stage->render_mode != RM_CUSTOM)
   {
      GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, stage->fbo));
   }
}

inline void gr_unbind(render_stage_t* stage)
{
   if(stage->render_mode != RM_BYPASS && stage->render_mode != RM_CUSTOM)
   {
      GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }
}

void gr_draw_string(font_t* f, vec2f_t position, vec2f_t scale, char* string, bool bind_shader, void* data)
{
   float start_x = position.x;

   if(bind_shader)
      sh_use(f->shader);

   f->bind_func(f, data);

   const float half_w = (float)f->win->w / 2.0f;
   const float half_h = (float)f->win->h / 2.0f;

   GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, f->vbo));
   GL_PCALL(glBindVertexArray(f->vao));

   for(size_t i = 0; i < strlen(string); i++)
   {
      charinfo_t* ci = &f->infos[string[i]];
      if(string[i] == '\n')
      {
         position.y += f->base * scale.y;
         position.x = start_x;
         continue;
      }

      assert(ci->id != -1);

      float x1 = (float)position.x + (float)scale.x * (ci->xoffset);
      float y1 = (float)position.y + (float)scale.y * (ci->yoffset);
      float x2 = (float)position.x + (float)scale.x * (ci->xoffset + ci->width);
      float y2 = (float)position.y + (float)scale.y * (ci->yoffset + ci->height);

      float values[] = {
            x2 - half_w, half_h - y2, 0, ci->tex_coord[0], ci->tex_coord[1],
            x2 - half_w, half_h - y1, 0, ci->tex_coord[2], ci->tex_coord[3],
            x1 - half_w, half_h - y1, 0, ci->tex_coord[4], ci->tex_coord[5],
            x1 - half_w, half_h - y2, 0, ci->tex_coord[6], ci->tex_coord[7],
      };

      GL_PCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(values), values, GL_DYNAMIC_DRAW));
      GL_PCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

      position.x += scale.x * ci->xadvance;
   }

   GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
   GL_PCALL(glBindVertexArray(0));

   if(bind_shader)
      sh_use(NULL);
}

inline void gr_pq_push_sprite(uint8_t depth, texture_t* texture, vec2f_t position,
                       vec2f_t scale, vec2f_t center, float angle, bool bind_shader, void* data)
{
   assert(depth < MAX_DEPTH);
   assert(queue_length[depth] < MAX_QUEUE_COUNT);

   size_t index = queue_length[depth];
   struct queue_item* qi = &queue[depth][index];
   qi->mode = 0;
   qi->tex = texture;
   qi->position = position;
   qi->scale = scale;
   qi->center = center;
   qi->angle = angle;
   qi->bind_shader = bind_shader;
   qi->data = data;

   queue_length[depth]++;
}

inline void gr_pq_push_string(uint8_t depth, font_t* f, vec2f_t position,
                       vec2f_t scale, char* string, bool bind_shader, void* data)
{
   assert(depth < MAX_DEPTH);
   assert(queue_length[depth] < MAX_QUEUE_COUNT);

   size_t index = queue_length[depth];
   struct queue_item* qi = &queue[depth][index];
   qi->mode = 1;
   qi->font = f;
   qi->position = position;
   qi->scale = scale;
   qi->string = string;
   qi->bind_shader = bind_shader;
   qi->data = data;

   queue_length[depth]++;
}


void gr_pq_flush(void)
{
   for(size_t d = 0; d < MAX_DEPTH; d++)
   {
      for(size_t i = 0; i < queue_length[d]; i++)
      {
         struct queue_item* qi = &queue[d][i];
         switch(qi->mode)
         {
            case 0:
               gr_draw_sprite(
                     qi->tex,
                     qi->position,
                     qi->scale,
                     qi->center,
                     qi->angle,
                     qi->bind_shader,
                     qi->data);
               break;
            case 1:
               gr_draw_string(
                     qi->font,
                     qi->position,
                     qi->scale,
                     qi->string,
                     qi->bind_shader,
                     qi->data);
               break;
            default:
               break;
         }
      }
      queue_length[d] = 0;
   }
}

void gr_draw_sprite(texture_t* texture, vec2f_t position, vec2f_t scale, vec2f_t center, float angle,
                    bool bind_shader, void* data)
{
   if(bind_shader)
      sh_use(sprite_shader);

   float trans = *(float*)data;

   mat4_identity(model_mat);
   mat4_translate(model_mat, position.x - default_win->w / 2.0f, default_win->h / 2.0f - position.y, 0);
   mat4_scale(model_mat, texture->width * scale.x, -texture->height * scale.y, 1);

   sh_nset_mat4(sprite_shader, "model", model_mat);
   sh_nset_float(sprite_shader, "transparency", trans);

   t_bind(texture, 0);

   glBindVertexArray(vao);
   glDrawArrays(GL_TRIANGLES, 0, 6);
   glBindVertexArray(0);

   if(bind_shader)
      sh_use(NULL);
}
