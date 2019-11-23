//
// Created by maxim on 9/11/19.
//

#ifdef __GNUC__
#pragma implementation "model.h"
#endif
#include "model.h"

// for FLT_MIN, FLT_MAX
#include <float.h>


// Initial length of all vectors
#define START_LEN_COUNT 10
// Increase size multiplier of vectors
#define INCREASE_LEN 1.5

// Adds new element to a specified model in a specified property
// If vector is not big enough to fit new element it will be resized
// and size will be increased in INCREASE_LEN times
#define M_PUSH_MODEL_PROP(model, prop, type, item) {                                            \
        if(model->model_len->prop ## _count > model->model_len->prop ##_max_count - 1)          \
        {                                                                                       \
            size_t newLen = (int)((float)model->model_len->prop ##_max_count * INCREASE_LEN);   \
            model->prop = realloc(model-> prop, sizeof(type) * newLen);                         \
            model->model_len->prop ##_max_count = newLen;                                       \
        }                                                                                       \
        model->prop[model->model_len->prop ##_count++] = item;                                  \
    }

// Maximum length of token in a file.
// Token is any char sequence between white spaces
#define MAX_WORD_LEN 256

// Count of vertex per face to be stored in VAO. Always use triangles...
#define VERTEX_PER_FACE 3

// All supported descriptor types
typedef enum _obj_descriptor_type
{
   OD_VERTEX,
   OD_VERTEX_NORMAL,
   OD_VERTEX_TEX,
   OD_FACE,
   OD_GROUP,
   OD_OBJECT,
   OD_MTLLIB,
   OD_USEMLT,
   OD_COMMENT,
   OD_SMOOTH

} obj_descriptor_type_t;

// Represents descriptor and it's string value to compare with words
typedef struct
{
   const char* string;
   obj_descriptor_type_t type;

} obj_descriptor_t;

// Push element to a specified array of model
void m_push_vertex(model_t* model, vec4 vec);
void m_push_normal(model_t* model, vec4 vec);
void m_push_tex_coord(model_t* model, vec4 vec);
void m_push_face(model_t* model, model_face_t* face);
void m_push_group_name(model_t* model, char* group_name);
void m_push_mtllib(model_t* model, char* mtllib);
void m_push_used_material(model_t* model, material_t* used_material);

// List of all supported descriptors
obj_descriptor_t allowed_descriptors[] =
{
        // Contains single vertex information, possible 4 values
       {"v",      OD_VERTEX},
        // Contains single normal information, possible 4 values
       {"vn",     OD_VERTEX_NORMAL},
        // Contains single texture coordinate information, possible 4 values
       {"vt",     OD_VERTEX_TEX},
        // Contains single face information, can have from 3 to MAX_FACE_LEN values
       {"f",      OD_FACE},
        // Describes group name of all following faces, can have several values separated with spaces
       {"g",      OD_GROUP},
        // Describes name of current object
       {"o",      OD_OBJECT},
        // Describes name of material library
       {"mtllib", OD_MTLLIB},
        // Describes material of all next faces
       {"usemtl", OD_USEMLT},
       // Disable or enable mesh smoothing
       {"s", OD_SMOOTH},
        // Just a comment
       {"#",      OD_COMMENT}
};

// Array to store current word to proceed
char              word[MAX_WORD_LEN];
// Buffer face to store temp values
model_face_t*     face;
// Buffer vector to store temp values
vec4              buff_vec;
// Total count of supported descriptors
const size_t      descriptors_count = sizeof(allowed_descriptors) / sizeof(allowed_descriptors[0]);

// Return true if symbol is white space (separator)
bool m_is_separator(char sym)
{
   return (sym == ' ' || sym == '\n' || sym == '\t' || sym == '\0');
}

// Separate string to words storing result to word
bool m_read_next_word(const char* string, size_t* start_index, size_t end_index)
{
   if (*start_index >= end_index)
      return false;

   size_t start = *start_index;

   while (m_is_separator(string[start])) start++;
   *start_index = start;

   while (!m_is_separator(string[*start_index])) (*start_index)++;

   size_t len = *start_index - start;
   assert(len < MAX_WORD_LEN);

   memset(word, 0, len + 1);
   memcpy(word, string + start, len);
   return true;
}

// Parses face word and stores result to i1, i2, i3 separating it by whitespaces
void m_parse_face(int32_t* i1, int32_t* i2, int32_t* i3)
{
   int32_t* ptr = i1;
   size_t power = 0;
   for (int64_t i = strlen(word) - 1; i >= 0; i--)
   {
      if (word[i] == '/')
      {
         if (ptr == i1) ptr = i2;
         else ptr = i3;
         power = 0;
      }
      else
      {
         if (power == 0) *ptr = 0;
         *ptr += (word[i] - '0') * pow(10, power++);
      }
   }
}

// Parse double stored in word
double_t m_parse_double(size_t line_index)
{
   char* err;
   double_t d = strtod(word, &err);

   if (m_is_separator(*err) && errno != ERANGE && errno != HUGE_VAL)
      return d;
   else
   {
      printf("[model.c][ERROR]: Unable to parse %s. Expected float point value at line %li\n", word, line_index);
      exit(EXIT_FAILURE);
   }
}

// Separates string slice between specified indices into words
// and proceeding it storing results to a model. It will determine
// descriptor type and validate its inputs
void m_proceed_line(model_t* model, const char* string, size_t start_index, size_t end_index, size_t line_index)
{
   if (start_index == end_index)
      return;

   obj_descriptor_t* descriptor = NULL;
   size_t word_count = 0;

   // iterate through all words
   while ((m_read_next_word(string, &start_index, end_index)))
   {
      // first word is always descriptor's type
      if (word_count == 0)
      {
         for (size_t i = 0; i < descriptors_count; i++)
            if (!strcmp(word, allowed_descriptors[i].string))
            {
               descriptor = &allowed_descriptors[i];
            }

         if (descriptor == NULL)
         {
            printf("[model.c][ERROR]: %s is unknown line descriptor at line %li\n", word, line_index);
            exit(EXIT_FAILURE);
         }
      }
      else
      {
         if (descriptor->type == OD_COMMENT)
            continue;
         if (descriptor->type == OD_VERTEX)
         {
            if (word_count > 4)
            {
               printf("[model.c][ERROR]: Too many arguments for vertex descriptor at line %li\n", line_index);
               exit(EXIT_FAILURE);
            }
            buff_vec[word_count - 1] = (float) m_parse_double(line_index);
         }
         if (descriptor->type == OD_VERTEX_NORMAL)
         {
            if (word_count > 3)
            {
               printf("[model.c][ERROR]: Too many arguments for normal descriptor at line %li\n", line_index);
               exit(EXIT_FAILURE);
            }
            buff_vec[word_count - 1] = (float) m_parse_double(line_index);
         }
         if (descriptor->type == OD_VERTEX_TEX)
         {
            if (word_count > 2)
            {
               printf("[model.c][ERROR]: Too many arguments for tex coord descriptor at line %li\n", line_index);
               exit(EXIT_FAILURE);
            }
            buff_vec[word_count - 1] = (float) m_parse_double(line_index);
         }
         if (descriptor->type == OD_FACE)
         {
            assert(word_count < MAX_FACE_LEN);

            int32_t i1 = -1, i2 = -1, i3 = -1;
            m_parse_face(&i1, &i2, &i3);

            if (i2 == -1 && i3 == -1)
            {
               face->vert_id[word_count - 1] = i1;
            }
            else
            {
               face->vert_id[word_count - 1] = i3;
               face->tex_id[word_count - 1] = i2;
               face->normal_id[word_count - 1] = i1;
            }
         }
      }
      word_count++;
   }

   // push cached result to a model
   if (descriptor->type == OD_COMMENT)
      return;
   if (descriptor->type == OD_VERTEX)
   {
      m_push_vertex(model, vec4_ccpy(buff_vec));
      vec4_fill(buff_vec, 0, 0, 0, 0);
   }
   if (descriptor->type == OD_VERTEX_NORMAL)
   {
      m_push_normal(model, vec4_ccpy(buff_vec));
      vec4_fill(buff_vec, 0, 0, 0, 0);
   }
   if (descriptor->type == OD_VERTEX_TEX)
   {
      m_push_tex_coord(model, vec4_ccpy(buff_vec));
      vec4_fill(buff_vec, 0, 0, 0, 0);
   }

   if (descriptor->type == OD_FACE)
   {
      model_face_t* new_face = malloc(sizeof(model_face_t));
      new_face->parent = model;
      new_face->count = word_count - 1;
      memcpy(new_face->normal_id, face->normal_id, sizeof(face->normal_id));
      memcpy(new_face->vert_id, face->vert_id, sizeof(face->vert_id));
      memcpy(new_face->tex_id, face->tex_id, sizeof(face->tex_id));
      m_push_face(model, new_face);

      memset(face->normal_id, -1, sizeof(face->normal_id));
      memset(face->tex_id, -1, sizeof(face->tex_id));
   }
}

// Separates input to lines and for each of them calls m_proceed_line
void m_parse_lines(model_t* model, const char* str)
{
   size_t start_index = 0;
   size_t end_index = 0;
   size_t line_index = 0;

   size_t i = 0;
   while (str[i] != '\0')
   {
      if (str[i] != '\n' && str[i] != '\0')
      {
         start_index = i;
         while (str[i] != '\n' && str[i] != '\0')
         {
            i++;
         }
         end_index = i - 1;
         m_proceed_line(model, str, start_index, end_index, line_index++);
      }
      else
      {
         if (str[i] == '\n' && end_index != i - 1)
            line_index++;
         i++;
      }
   }
}

//
// m_create()
//
model_t* m_create()
{
   model_t* model = malloc(sizeof(model_t));
   model->model_len = malloc(sizeof(model_len_t));
   model->model_len->vertices_max_count = START_LEN_COUNT;
   model->model_len->normals_max_count = START_LEN_COUNT;
   model->model_len->tex_coords_max_count = START_LEN_COUNT;
   model->model_len->faces_max_count = START_LEN_COUNT;
   model->model_len->group_names_max_count = START_LEN_COUNT;
   model->model_len->mtl_libs_max_count = START_LEN_COUNT;

   model->model_len->vertices_count = 0;
   model->model_len->normals_count = 0;
   model->model_len->tex_coords_count = 0;
   model->model_len->faces_count = 0;
   model->model_len->group_names_count = 0;
   model->model_len->mtl_libs_count = 0;

   model->vertices = malloc(sizeof(vec4) * model->model_len->vertices_max_count);
   memset(model->vertices, 0, sizeof(vec4) * model->model_len->vertices_max_count);

   model->normals = malloc(sizeof(vec4) * model->model_len->normals_max_count);
   memset(model->normals, 0, sizeof(vec4) * model->model_len->normals_max_count);

   model->tex_coords = malloc(sizeof(vec4) * model->model_len->tex_coords_max_count);
   memset(model->tex_coords, 0, sizeof(vec4) * model->model_len->tex_coords_max_count);

   model->faces = malloc(sizeof(model_face_t**) * model->model_len->faces_max_count);
   memset(model->faces, 0, sizeof(model_face_t**) * model->model_len->faces_max_count);

   model->group_names = malloc(sizeof(char*) * model->model_len->group_names_max_count);
   memset(model->group_names, 0, sizeof(char*) * model->model_len->group_names_max_count);

   model->mtl_libs = malloc(sizeof(char*) * model->model_len->mtl_libs_max_count);
   memset(model->mtl_libs, 0, sizeof(char*) * model->model_len->mtl_libs_max_count);

   model->object_name = NULL;
   model->filename = NULL;

   return model;
}

//
// m_load()
//
model_t* m_load(const char* filename)
{
   if (!buff_vec)
      buff_vec = cvec4(0, 0, 0, 0);

   if (!face)
   {
      face = malloc(sizeof(model_face_t));
      memset(face->normal_id, -1, sizeof(face->normal_id));
      memset(face->tex_id, -1, sizeof(face->tex_id));
   }
   FILE* f = fopen(filename, "r");
   if (!f)
   {
      printf("[model.c][ERROR]: Unable to open file \"%s\"", filename);
      return NULL;
   }

   fseek(f, 0, SEEK_END);
   size_t size = ftell(f);
   fseek(f, 0, SEEK_SET);

   char* data = malloc(size + 1);
   memset(data, 0, size + 1);

   fread(data, size, 1, f);

   model_t* model = m_create();
   model->filename = filename;

   m_parse_lines(model, data);

   free(data);
   fclose(f);

   printf("[model.c]: Loaded model %s. Vertices: %li, Faces: %li\n",
          filename, model->model_len->vertices_count, model->model_len->faces_count);

   return model;
}

//
// m_free()
//
void m_free(model_t* model)
{
   for (size_t i = 0; i < model->model_len->vertices_count; i++)
      vec4_free(model->vertices[i]);
   free(model->vertices);

   for (size_t i = 0; i < model->model_len->normals_count; i++)
      vec4_free(model->normals[i]);
   free(model->normals);

   for (size_t i = 0; i < model->model_len->tex_coords_count; i++)
      vec4_free(model->tex_coords[i]);
   free(model->tex_coords);

   for (size_t i = 0; i < model->model_len->faces_count; i++)
      free(model->faces[i]);
   free(model->faces);

   for (size_t i = 0; i < model->model_len->group_names_count; i++)
      free(model->group_names[i]);
   free(model->group_names);

   for (size_t i = 0; i < model->model_len->mtl_libs_count; i++)
      free(model->mtl_libs[i]);
   free(model->mtl_libs);

   free(model->object_name);
   free(model->model_len);

   if (model->VBO != 0)
      GL_CALL(glDeleteBuffers(1, &(model->VBO)))

   if (model->VAO != 0)
      GL_CALL(glDeleteBuffers(1, &(model->VAO)))

   printf("[model.c]: Freed model %s\n", model->filename);
   free(model);
}

inline void m_push_vertex(model_t* model, vec4 vec)
{
   assert(vec);
   M_PUSH_MODEL_PROP(model, vertices, vec4, vec);
}

inline void m_push_normal(model_t* model, vec4 vec)
{
   assert(vec);
   M_PUSH_MODEL_PROP(model, normals, vec4, vec);
}

inline void m_push_tex_coord(model_t* model, vec4 vec)
{
   assert(vec);
   M_PUSH_MODEL_PROP(model, tex_coords, vec4, vec);
}

inline void m_push_face(model_t* model, model_face_t* face)
{
   assert(face);
   M_PUSH_MODEL_PROP(model, faces, model_face_t*, face);
}

inline void m_push_group_name(model_t* model, char* group_name)
{
   assert(group_name);
   M_PUSH_MODEL_PROP(model, group_names, char*, group_name);
}

inline void m_push_mtllib(model_t* model, char* mtllib)
{
   assert(mtllib);
   M_PUSH_MODEL_PROP(model, mtl_libs, char*, mtllib);
}

//
// m_info
//
void m_info(model_t* model)
{
   printf("[model.c]: Vertices (%li): \n", model->model_len->vertices_count);
   for (size_t i = 0; i < model->model_len->vertices_count; i++)
      printf("%li: %f %f %f [%f]\n",
             i, model->vertices[i][0],
             model->vertices[i][1],
             model->vertices[i][2],
             model->vertices[i][3]);

   printf("\n[model.c]: Normals (%li): \n", model->model_len->normals_count);
   for (size_t i = 0; i < model->model_len->normals_count; i++)
      printf("%li: %f %f %f\n",
             i, model->normals[i][0],
             model->normals[i][1],
             model->normals[i][2]);

   printf("\n[model.c]: Faces (%li): \n", model->model_len->faces_count);
   for (size_t i = 0; i < model->model_len->faces_count; i++)
   {
      printf("%li. ", i);
      for (size_t j = 0; j < model->faces[j]->count; j++)
      {
         printf("%i/%i/%i ",
                model->faces[i]->vert_id[j],
                model->faces[i]->tex_id[j],
                model->faces[i]->normal_id[j]);
      }
      putchar('\n');
   }
}

// Stores vertex data to a buffer according to model parameters
void m_store_vertex(
        model_t* model,
        model_face_t* f,
        size_t id,
        float* buffer,
        size_t* buffer_index,
        bool use_tex_coords,
        bool use_normal,
        bool supposed_normals)
{
   size_t vert_index = f->vert_id[id] - 1;
   buffer[(*buffer_index)++] = model->vertices[vert_index][0];
   buffer[(*buffer_index)++] = model->vertices[vert_index][1];
   buffer[(*buffer_index)++] = model->vertices[vert_index][2];

   if(use_normal)
   {
      size_t norm_index = supposed_normals ? vert_index : f->normal_id[id] - 1;
      buffer[(*buffer_index)++] = model->normals[norm_index][0];
      buffer[(*buffer_index)++] = model->normals[norm_index][1];
      buffer[(*buffer_index)++] = model->normals[norm_index][2];
   }

   if(use_tex_coords)
   {
      size_t tex_index = f->tex_id[id] - 1;
      buffer[(*buffer_index)++] = model->tex_coords[tex_index][0];
      buffer[(*buffer_index)++] = model->tex_coords[tex_index][1];
   }

}

//
// m_build
//
void m_build(model_t* model)
{
   bool use_tex_coords = model->model_len->tex_coords_count != 0 && model->faces[0]->tex_id[0] != -1;
   bool use_normals = model->model_len->normals_count != 0 && model->faces[0]->normal_id[0] != -1;
   //bool supposed_normals = use_normals && model->model_len->vertices_count == model->model_len->normals_count;

   size_t floats_per_triangle =
                 3 +
                 (use_tex_coords ? 2 : 0) +
                 (use_normals ? 3 : 0);

   size_t size = 0;
   model->triangles_count = 0;
   for (size_t i = 0; i < model->model_len->faces_count; i++)
   {
      size += (model->faces[i]->count - 2) * floats_per_triangle * 3;
      model->triangles_count += model->faces[i]->count - 2;
   }

   float* buffer = malloc(sizeof(float) * size);
   //uint32_t* index_buffer = malloc(sizeof(uint32_t) * model->triangles_count* 2);
   size_t buffer_index = 0;
   //size_t index_buffer_index = 0;

   // storing all vertices from object to a buffer
   //for (size_t i = 0; i < model->model_len->vertices_count; i++)


   // storing all data to a buffer
   for (size_t i = 0; i < model->model_len->faces_count; i++)
   {
      model_face_t* f = model->faces[i];
      for(size_t tria = 0; tria < f->count - 2; tria++)
      {
         m_store_vertex(model, f, 0, buffer, &buffer_index, use_tex_coords, use_normals, false);
         m_store_vertex(model, f, tria + 1, buffer, &buffer_index, use_tex_coords, use_normals, false);
         m_store_vertex(model, f, tria + 2, buffer, &buffer_index, use_tex_coords, use_normals, false);
      }
   }
   assert(buffer_index == size);

   GL_CALL(glGenBuffers(1, &model->VBO))
   //GL_CALL(glGenBuffers(1, &model->EBO))
   GL_CALL(glGenVertexArrays(1, &model->VAO))

   // 1. bind Vertex Array Object
   GL_CALL(glBindVertexArray(model->VAO))

   // 2. copy our vertices array in a vertex buffer for OpenGL to use
   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, model->VBO))
   GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffer_index, buffer, GL_STATIC_DRAW))

   // 3. copy our index array in a element buffer for OpenGL to use
   //GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->EBO))
   //GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indices_buffer_index, indices_buffer, GL_STATIC_DRAW))

   // 4. then set the vertex attributes pointers
   if (use_normals && use_tex_coords)
   {
      // vertices (3 floats)
      GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                            (void*) 0))
      GL_CALL(glEnableVertexAttribArray(0))

      // normals (3 floats)
      GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                            (void*)(3 * sizeof(float))))
      GL_CALL(glEnableVertexAttribArray(1))

      // texture coordinates (2 floats)
      GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                            (void*) (6 * sizeof(float))))
      GL_CALL(glEnableVertexAttribArray(2))
   }
   else if (!use_normals && use_tex_coords)
   {
      // vertices (3 floats)
      GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                            (void*) 0))
      GL_CALL(glEnableVertexAttribArray(0))

      // texture coordinates (2 floats)
      GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                            (void*) (3 * sizeof(float))))
      GL_CALL(glEnableVertexAttribArray(2))
   }
   else if (use_normals && !use_tex_coords)
   {
      // vertices (3 floats)
      GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                            (void*) 0))
      GL_CALL(glEnableVertexAttribArray(0))

      // normals (3 floats)
      GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                            (void*) (3 * sizeof(float))))
      GL_CALL(glEnableVertexAttribArray(1))
   }
   else //no normals and coords
   {
      // vertices (3 floats)
      GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void*) 0))
      GL_CALL(glEnableVertexAttribArray(0))
   }

   GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0))
   GL_CALL(glBindVertexArray(0))

   free(buffer);
}

model_t* m_create_plane()
{
   model_t* model = m_create();
   m_push_vertex(model, cvec4(0, 0, 1, 0));
   m_push_vertex(model, cvec4(1, 0, 1, 0));
   m_push_vertex(model, cvec4(1, 0, 0, 0));
   m_push_vertex(model, cvec4(0, 0, 0, 0));

   m_push_normal(model, cvec4(0, 1, 0, 0));
   m_push_normal(model, cvec4(0, 1, 0, 0));
   m_push_normal(model, cvec4(0, 1, 0, 0));
   m_push_normal(model, cvec4(0, 1, 0, 0));

    m_push_tex_coord(model, cvec4(0, 1, 0, 0));
    m_push_tex_coord(model, cvec4(1, 1, 0, 0));
    m_push_tex_coord(model, cvec4(0, 0, 0, 0));
    m_push_tex_coord(model, cvec4(1, 0, 0, 0));

   model_face_t* f = malloc(sizeof(model_face_t));
   f->count = 4;
   f->vert_id[0] = 1;    f->vert_id[1] = 2;    f->vert_id[2] = 3;    f->vert_id[3] = 4;
   f->normal_id[0] = 1;  f->normal_id[1] = 2;  f->normal_id[2] = 3;  f->normal_id[3] = 4;
   f->tex_id[0] = 1;     f->tex_id[1] = 2;     f->tex_id[2] = 3;     f->tex_id[3] = 4;
   m_push_face(model, f);

   return model;
}

//
// m_normalize()
//
void m_normalize(model_t* model, bool norm_x_pos, bool norm_y_pos, bool norm_z_pos, bool norm_scale)
{
   float maxX = FLT_MIN, maxY = FLT_MIN, maxZ = FLT_MIN;
   float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;

   for(size_t i = 0; i < model->model_len->vertices_count; i++)
   {
      vec4 vertex = model->vertices[i];
      if(vertex[0] < minX) minX = vertex[0];
      if(vertex[1] < minY) minY = vertex[1];
      if(vertex[2] < minZ) minZ = vertex[2];
      if(vertex[0] > maxX) maxX = vertex[0];
      if(vertex[1] > maxY) maxY = vertex[1];
      if(vertex[2] > maxZ) maxZ = vertex[2];
   }

   float scale = 0; // maximum of 3 variables

   if(norm_scale)
   {
      float dx = maxX - minX;
      float dy = maxY - minY;
      float dz = maxZ - minZ;
      if (dx > dy)
      {
         if (dx > dz) scale = dx;
         else scale = dz;
      }
      else
      {
         if (dx > dz) scale = dy;
         else if (dy > dz) scale = dy;
         else scale = dz;
      }
   }

   for(size_t i = 0; i < model->model_len->vertices_count; i++)
   {
      vec4 vertex = model->vertices[i];
      if(norm_x_pos) vertex[0] -= minX;
      if(norm_y_pos) vertex[1] -= minY;
      if(norm_z_pos) vertex[2] -= minZ;
      if(norm_scale)
      {
         vertex[0] /= scale;
         vertex[1] /= scale;
         vertex[2] /= scale;
      }
   }
}