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

vec4        COLOR_WHITE;
vec4        COLOR_SILVER;
vec4        COLOR_GRAY;
vec4        COLOR_BLACK;
vec4        COLOR_RED;
vec4        COLOR_MAROON;
vec4        COLOR_YELLOW;
vec4        COLOR_OLIVE;
vec4        COLOR_LIME;
vec4        COLOR_GREEN;
vec4        COLOR_AQUA;
vec4        COLOR_TEAL;
vec4        COLOR_BLUE;
vec4        COLOR_NAVY;
vec4        COLOR_FUCHSIA;
vec4        COLOR_PURPLE;

mat4        proj_mat;
mat4        view_mat;
mat4        model_mat;

static mat4 x_rot_mat;
static mat4 y_rot_mat;
static mat4 z_rot_mat;

enum pq_type
{
   SPRITE,
   STRING,
   LINE
};

static struct _queue_item
{
   uint8_t mode;

   texture_t* tex;
   vec2 center;
   float angle;
   sprite_renderer_t* sprite_renderer;
   bool bind_shader;

   font_t* font;
   char* string;

   vec2 p1;
   vec2 p2;
   float width;
   vec4 color;
   primitive_renderer_t* primitive_renderer;

   vec2 position;
   vec2 scale;
   void* data;
} queue[MAX_DEPTH][MAX_QUEUE_COUNT];

static size_t queue_length[MAX_DEPTH];

inline void gr_fill(vec4 color)
{
   GL_PCALL(glClearColor(color[0], color[1], color[2], color[3]));
}

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
}

inline void gr_render_vao(GLuint vao)
{
   GL_PCALL(glBindVertexArray(vao));
   //GL_PCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

   GL_PCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

   GL_PCALL(glBindVertexArray(0));
   //GL_PCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void gr_release(void)
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

static inline void gr_draw_model(model_t* model)
{
   GL_PCALL(glBindVertexArray(model->VAO));
   GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, model->triangles_count * 3));
   GL_PCALL(glBindVertexArray(0));
}

inline mat4 gr_transform(vec3 pos, vec3 scale, vec3 rot)
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

void gr_render_instance(instance_collection_t* ic)
{
   GL_PCALL(glBindVertexArray(ic->model->VAO));
   GL_PCALL(glDrawElementsInstanced(GL_TRIANGLES, ic->model->triangles_count * 3, GL_UNSIGNED_INT, 0, ic->amount));
   GL_PCALL(glBindVertexArray(0));
}


inline void gr_render_object(object_t* obj)
{
   gr_draw_model(obj->draw_info->model);
}

inline void gr_bind(render_stage_t* stage)
{
   GL_PCALL(glViewport(0, 0, stage->width, stage->height));
   if(stage->render_mode != RM_BYPASS &&
      stage->render_mode != RM_CUSTOM &&
      stage->render_mode != RM_GEOMETRY_NOFRAMEBUFFER)
   {
      GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, stage->fbo));
   }
}

inline void gr_unbind(render_stage_t* stage)
{
   if(stage->render_mode != RM_BYPASS &&
      stage->render_mode != RM_CUSTOM &&
      stage->render_mode != RM_GEOMETRY_NOFRAMEBUFFER)
   {
      GL_PCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }
}

void gr_draw_string(font_t* f, vec2 position, vec2 scale, char* string, void* data)
{
   float start_x = position.x;

   sh_use(f->shader);

   f->bind_func(f, data);

   const float half_w = (float)f->win->w / 2.0f;
   const float half_h = (float)f->win->h / 2.0f;

   GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, f->vbo));
   GL_PCALL(glBindVertexArray(f->vao));

   for(size_t i = 0; i < strlen(string); i++)
   {
      charinfo_t* ci = &f->infos[(size_t)string[i]];
      if(string[i] == '\n')
      {
         position.y += f->base * scale.y;
         position.x = start_x;
         continue;
      }

      if(ci->id == -1)
         GR_ERROR("Trying to draw uninitialized symbol",0);

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

   sh_use(NULL);
}

inline void gr_pq_push_sprite(uint8_t depth, texture_t* texture, vec2 position,
                       vec2 scale, vec2 center, float angle,
                       sprite_renderer_t* sprite_renderer, void* data)
{
   PASSERT(texture);
   PASSERT(sprite_renderer);

   if(depth >= MAX_DEPTH)
      GR_ERROR("Passed wrong depth. Max depth: %i, passed depth: %i", MAX_DEPTH, depth);
   if(queue_length[depth] >= MAX_QUEUE_COUNT)
      GR_ERROR("Too many items in queue. Max length: %i at depth: %i", MAX_QUEUE_COUNT, depth);

   size_t index = queue_length[depth];
   struct _queue_item* qi = &queue[depth][index];
   qi->mode = SPRITE;
   qi->tex = texture;
   qi->position = position;
   qi->scale = scale;
   qi->center = center;
   qi->angle = angle;
   qi->sprite_renderer = sprite_renderer;
   qi->data = data;

   queue_length[depth]++;
}

inline void gr_pq_push_string(uint8_t depth, font_t* f, vec2 position,
                       vec2 scale, char* string, void* data)
{
   PASSERT(f);
   PASSERT(string);

   if(depth >= MAX_DEPTH)
      GR_ERROR("Passed wrong depth. Max depth: %i, passed depth: %i", MAX_DEPTH, depth);
   if(queue_length[depth] >= MAX_QUEUE_COUNT)
      GR_ERROR("Too many items in queue. Max length: %i at depth: %i", MAX_QUEUE_COUNT, depth);

   size_t index = queue_length[depth];
   struct _queue_item* qi = &queue[depth][index];
   qi->mode = STRING;
   qi->font = f;
   qi->position = position;
   qi->scale = scale;
   qi->string = string;
   qi->data = data;

   queue_length[depth]++;
}

inline void gr_pq_push_line(uint8_t depth, vec2 p1, vec2 p2, float width, vec4 color,
                     primitive_renderer_t* primitive_renderer, void* data)
{
   PASSERT(primitive_renderer);
   PASSERT(color);

   if(depth >= MAX_DEPTH)
      GR_ERROR("Passed wrong depth. Max depth: %i, passed depth: %i", MAX_DEPTH, depth);
   if(queue_length[depth] >= MAX_QUEUE_COUNT)
      GR_ERROR("Too many items in queue. Max length: %i at depth: %i", MAX_QUEUE_COUNT, depth);

   size_t index = queue_length[depth];
   struct _queue_item* qi = &queue[depth][index];
   qi->mode = LINE;
   qi->p1 = p1;
   qi->p2 = p2;
   qi->width = width;
   qi->color = color;
   qi->primitive_renderer = primitive_renderer;
   qi->data = data;

   queue_length[depth]++;
}

void gr_pq_flush(void)
{
   for(size_t d = 0; d < MAX_DEPTH; d++)
   {
      for(size_t i = 0; i < queue_length[d]; i++)
      {
         struct _queue_item* qi = &queue[d][i];
         switch(qi->mode)
         {
            case SPRITE:
               gr_draw_sprite(
                     qi->tex,
                     qi->position,
                     qi->scale,
                     qi->center,
                     qi->angle,
                     qi->sprite_renderer,
                     qi->data);
               break;
            case STRING:
               gr_draw_string(
                     qi->font,
                     qi->position,
                     qi->scale,
                     qi->string,
                     qi->data);
               break;
            case LINE:
               gr_draw_line(
                     qi->p1,
                     qi->p2,
                     qi->width,
                     qi->color,
                     qi->primitive_renderer,
                     qi->data);
            default:
               break;
         }
      }
      queue_length[d] = 0;
   }
}

void gr_draw_sprite(texture_t* texture, vec2 position, vec2 scale, vec2 center, float angle,
                    sprite_renderer_t* sprite_renderer, void* data)
{
   sh_use(sprite_renderer->shader);

   mat4_identity(model_mat);

   mat4_translate(model_mat, center.x / 2.0f, center.y / 2.0f, 0);
   mat4_rotate_y(model_mat, angle);
   mat4_translate(model_mat, -center.x / 2.0f, -center.y / 2.0f, 0);

   mat4_translate(model_mat, position.x - (float)default_win->w / 2.0f, (float)default_win->h / 2.0f - position.y, 0);
   mat4_scale(model_mat, texture->width * scale.x, -texture->height * scale.y, 1);

   sprite_renderer->bind_func(sprite_renderer, model_mat, data);

   t_bind(texture, 0);

   GL_PCALL(glBindVertexArray(sprite_renderer->vao));
   GL_PCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
   GL_PCALL(glBindVertexArray(0));

   sh_use(NULL);
}

void gr_draw_line(vec2 p1, vec2 p2, float width, vec4 color,
                  primitive_renderer_t* primitive_renderer, void* data)
{
   p1.x = p1.x - (float)default_win->w / 2.0f;
   p1.y = (float)default_win->h / 2.0f - p1.y;
   p2.x = p2.x - (float)default_win->w / 2.0f;
   p2.y = (float)default_win->h / 2.0f - p2.y;

   sh_use(primitive_renderer->line_shader);

   primitive_renderer->bind_line_func(primitive_renderer, width, color, data);

   GL_PCALL(glBindVertexArray(primitive_renderer->vao));
   GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, primitive_renderer->vbo));

   float gl_data[2 * 2] = {
         p1.x, p1.y,
         p2.x, p2.y
   };

   //glEnable(GL_PROGRAM_POINT_SIZE);
   //glPointSize(10);

   GL_PCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(gl_data), gl_data, GL_DYNAMIC_DRAW));

   //glLineWidth(width);
   //glGetError();

   GL_PCALL(glDrawArrays(GL_LINES, 0, 2));

   GL_PCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
   GL_PCALL(glBindVertexArray(0));

   sh_use(NULL);
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

sprite_renderer_t* gr_create_sprite_renderer(shader_t* shader)
{
   sprite_renderer_t* sprite_renderer = DEEPCARS_MALLOC(sizeof(sprite_renderer_t));
   sprite_renderer->shader = shader;
   sprite_renderer->bind_func = NULL;

   // Configure VAO/VBO
   GLuint vbo;
   GLfloat vertices[] =
   {
      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f
   };

   GL_CALL(glGenVertexArrays(1, &sprite_renderer->vao));
   GL_CALL(glGenBuffers(1, &vbo));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

   GL_CALL(glBindVertexArray(sprite_renderer->vao));
   GL_CALL(glEnableVertexAttribArray(0));
   GL_CALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*) 0));
   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
   GL_CALL(glBindVertexArray(0));
   return sprite_renderer;
}

primitive_renderer_t* gr_create_primitive_renderer(shader_t* shader)
{
   primitive_renderer_t* primitive_renderer = DEEPCARS_MALLOC(sizeof(primitive_renderer_t));
   primitive_renderer->shader = shader;
   primitive_renderer->bind_line_func = NULL;

   GL_CALL(glGenVertexArrays(1, &primitive_renderer->vao));
   GL_CALL(glGenBuffers(1, &primitive_renderer->vbo));

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, primitive_renderer->vbo));
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW));

   GL_CALL(glBindVertexArray(primitive_renderer->vao));
   GL_CALL(glEnableVertexAttribArray(0));
   GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*) 0));
   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
   GL_CALL(glBindVertexArray(0));
   return primitive_renderer;
}
