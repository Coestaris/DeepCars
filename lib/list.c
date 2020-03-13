//
// Created by maxim on 3/5/20.
//

#ifdef __GNUC__
#pragma implementation "list.h"
#endif
#include "list.h"
#include "coredefs.h"

#include <string.h>

//
// list_push()
//
inline void list_push(list_t* list, void* object)
{
   PASSERT(list);
   PASSERT(object);

   // current collection if full increase its size in 1.5 times
   if (list->count > list->max_size - 1)
   {
      if (list->max_size == LIST_BOOTSTRAP_SIZE) {
         size_t new_len = (int) ((float) list->max_size * LIST_SIZE_INCREASE);

         void** new_collection = DEEPCARS_MALLOC(sizeof(void*) * new_len);
         memcpy(new_collection, list->bootstrap, list->count * sizeof(void*));
         list->collection = new_collection;
         list->max_size = new_len;

      } else {
         size_t new_len = (int) ((float) list->max_size * LIST_SIZE_INCREASE);
         list->collection = realloc(list->collection, sizeof(void*) * new_len);
         list->max_size = new_len;
      }
   }
   list->collection[list->count++] = object;
}

//
// list_free_elements()
//
void list_free_elements(list_t* list)
{
   ASSERT(list);

   for (size_t i = 0; i < list->count; i++)
      DEEPCARS_FREE(list->collection[i]);
   list->count = 0;
}

//
// list_free()
//
void list_free(list_t* list)
{
   ASSERT(list);

   if (list->collection != list->bootstrap) {
      DEEPCARS_FREE(list->collection);
   }
   DEEPCARS_FREE(list);
}

//
// list_remove()
//
void list_remove(list_t* list, void* object)
{
   ASSERT(list);
   ASSERT(object);

   //todo
}

//
// list_create
//
list_t* list_create()
{
   list_t* list = DEEPCARS_MALLOC(sizeof(list_t));
   list->count = 0;
   list->max_size = LIST_BOOTSTRAP_SIZE;
   list->collection = list->bootstrap;

   memset(list->bootstrap, 0, sizeof(list->bootstrap));

   return list;
}



