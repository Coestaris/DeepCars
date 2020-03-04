//
// Created by maxim on 2/24/20.
//

#ifdef __GNUC__
#pragma implementation "map_saver.h"
#endif

#include <dirent.h>
#include "map_saver.h"
#include "../../win_defaults.h"

void map_save(list_t* walls, list_t* objects, char* file, vec2 prev_point, bool first_point_set)
{
   assert(walls);
   assert(objects);

   FILE* f = fopen(file, "wb");
   if(!f)
      APP_ERROR("Unable to open file \"%s\"", file);

   fwrite(&prev_point, sizeof(vec2), 1, f);
   fwrite(&first_point_set, sizeof(bool), 1, f);

   size_t c = sizeof(wall_t);
   fwrite(&c, sizeof(c), 1, f);
   c = sizeof(map_object_t);
   fwrite(&c, sizeof(c), 1, f);

   c = walls->count;
   fwrite(&c, sizeof(c), 1, f);
   for(size_t i = 0; i < c; i++)
   {
      wall_t* wall = walls->collection[i];
      fwrite(wall, sizeof(wall_t), 1, f);
   }

   c = objects->count;
   fwrite(&c, sizeof(c), 1, f);
   for(size_t i = 0; i < c; i++)
   {
      map_object_t* object = objects->collection[i];
      fwrite(object, sizeof(map_object_t), 1, f);
   }

   fclose(f);
}

void map_load(list_t* walls, list_t* objects, char* file, vec2* prev_point, bool* first_point_set)
{
   list_free_elements(walls);
   list_free_elements(objects);

   FILE* f = fopen(file, "rb");
   if(!f)
      APP_ERROR("Unable to open file \"%s\"", file);

   size_t c = 0;

   fread(prev_point, sizeof(vec2), 1, f);
   fread(first_point_set, sizeof(bool), 1, f);

   fread(&c, sizeof(c), 1, f);
   if(c != sizeof(wall_t))
      APP_ERROR("Sizes of sizeof(wall_t) and size stored in file doesn't match. Stored value: %li, sizeof(wall_t): %li",
            sizeof(wall_t), c);

   c = 0;
   fread(&c, sizeof(c), 1, f);
   if(c != sizeof(map_object_t))
   APP_ERROR("Sizes of sizeof(map_object_t) and size stored in file doesn't match. Stored value: %li, sizeof(map_object_t): %li",
             sizeof(map_object_t), c);

   fread(&c, sizeof(c), 1, f);
   for(size_t i = 0; i < c; i++)
   {
      wall_t* wall = malloc(sizeof(wall_t));
      fread(wall, sizeof(wall_t), 1, f);
      list_push(walls, wall);
   }

   fread(&c, sizeof(c), 1, f);
   for(size_t i = 0; i < c; i++)
   {
      map_object_t* object = malloc(sizeof(map_object_t));
      fread(object, sizeof(map_object_t), 1, f);
      list_push(objects, object);
   }

   fclose(f);
}
