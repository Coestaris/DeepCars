//
// Created by maxim on 8/28/19.
//

#ifdef __GNUC__
#pragma implementation "updater.h"
#endif
#include "updater.h"

#include "graphics/rendering/graphics.h"
#include "resources/shader.h"
#include "scm.h"
#include "graphics/rendering/render_chain.h"

#define U_LOG(format, ...) DC_LOG("updater.c", format, __VA_ARGS__)
#define U_ERROR(format, ...) DC_ERROR("updater.c", format, __VA_ARGS__)

// Initial count of objects
#define OBJECT_COUNT_START 50
// Maximum available count of key states
#define MAX_KEYS 256
// Maximum available count of mouse states
#define MAX_MOUSE_STATES 10

//
// FPS Lock variables
//
// Set true to finish loop
static bool        closed = false;
// Frame counter for FPS measurements
static uint64_t    counter = 0;
// Elapsed time between current time and last FPS measurement
static double_t    elapsed = 0;
// Average FPS count
static double_t    fps = 0;
// Current frame counter
static uint64_t    frames;

//
// Event processing variables
//
static uint32_t    keys_state[MAX_KEYS];
static uint32_t    mouse_state[MAX_MOUSE_STATES];
static uint32_t    mouse_x;
static uint32_t    mouse_y;

// Current X window allocated in win.h
win_info_t*  default_win;

// Global list of all drawable objects
static list_t*     objects;
// Preprocessed lists of listening objects
static list_t*     key_listeners;
static list_t*     mouse_listeners;
static list_t*     mousemove_listeners;
static list_t*     update_listeners;

#ifdef OUTPUT_RENDER_TIME
static size_t render_time_counter = 0;
#endif

// Get current millisecond of global time
static double_t u_get_millis(void)
{
   struct timeval tv;
   gettimeofday(&tv, NULL);

   return (tv.tv_sec) * 1000.0 + (tv.tv_usec) / 1000.0;
}

//
// u_close()
//
void u_close(void)
{
   closed = true;
}

// Main draw function.
// Looping through all objects and draws it
static void u_draw_func(void)
{
   // Fill scene with background color
   if(current_scene->backcolor)
   {
      GL_PCALL(glClearColor(
            current_scene->backcolor[0],
            current_scene->backcolor[1],
            current_scene->backcolor[2],
            current_scene->backcolor[3]));
   }

   render_chain_t* chain = rc_get_current();
   list_t* stages = chain->stages;

   // Iterate thought render stages
   for(size_t i = 0; i < stages->count; i++)
   {
      render_stage_t* stage = (render_stage_t*)stages->collection[i];

      if(stage->skip) continue;

      gr_bind(stage);
      sh_use(stage->shader);
      stage->bind_func(stage);

#ifdef MEASURE_RENDER_TIME
      double start_time = u_get_millis();
#endif

      if(stage->render_mode == RM_GEOMETRY)
      {
         // Render all objects
         for(size_t j = 0; j < objects->count; j++)
         {
            object_t* obj = objects->collection[j];
            if(obj->draw_info->drawable)
            {
               gr_transform(obj->position, obj->scale, obj->rotation);
               stage->setup_obj_func(stage, obj, model_mat);
               gr_render_object(obj);
            }
         }
      }
      else if(stage->render_mode == RM_BYPASS || stage->render_mode == RM_FRAMEBUFFER)
      {
         // Render from buffer to a buffer
         gr_render_vao(stage->vao);
      }
      else if(stage->render_mode == RM_CUSTOM || stage->render_mode == RM_CUSTOM_FRAMEBUFFER)
      {
         stage->custom_draw_func(stage);
      }

#ifdef MEASURE_RENDER_TIME
      stage->render_time =
            (MEASURE_RENDER_SMOOTH * (u_get_millis() - start_time)) +
            (1.0 - MEASURE_RENDER_SMOOTH) * stage->render_time;
#endif

      stage->unbind_func(stage);
      gr_unbind(stage);
      sh_use(NULL);

   }

   w_swap_buffers(default_win);
}

//
// u_get_mouse_pos()
//
vec2 u_get_mouse_pos()
{
   return vec2f(mouse_x, mouse_y);
}

//
// u_get_key_state()
//
uint32_t u_get_key_state(uint32_t key)
{
   assert(key < MAX_KEYS);
   return keys_state[key];
}

//
// u_get_mouse_state()
//
uint32_t u_get_mouse_state(uint32_t mouse)
{
   assert(key < MAX_MOUSE_STATES);
   return mouse_state[mouse];
}

// Main event handler function.
// Sets all necessary states and run all callbacks in listeners
static void u_event_handler(XEvent event)
{
   switch (event.type)
   {
      case ClientMessage:
      {
         U_LOG("Force closing...",0);
         u_close();
         break;
      }
      case KeyPress:
      {
         //printf("%i\n", event.xkey.keycode);
         keys_state[event.xkey.keycode] = 1;
         for (size_t i = 0; i < key_listeners->count; i++)
            ((object_t*) key_listeners->collection[i])->key_event_func(
                    key_listeners->collection[i],
                    event.xkey.keycode,
                    KEY_PRESSED);
      }
         break;

      case KeyRelease:
      {
         keys_state[event.xkey.keycode] = 0;
         for (size_t i = 0; i < key_listeners->count; i++)
            ((object_t*) key_listeners->collection[i])->key_event_func(
                    key_listeners->collection[i],
                    event.xkey.keycode,
                    KEY_RELEASE);
      }
         break;
      case ButtonPress:
      {
         mouse_state[event.xbutton.button] = 1;
         for (size_t i = 0; i < mouse_listeners->count; i++)
            ((object_t*) mouse_listeners->collection[i])->mouse_event_func(
                    mouse_listeners->collection[i],
                    event.xbutton.x,
                    event.xbutton.y,
                    MOUSE_PRESSED,
                    event.xbutton.button);
      }
         break;
      case ButtonRelease:
      {
         mouse_state[event.xbutton.button] = 0;
         for (size_t i = 0; i < mouse_listeners->count; i++)
            ((object_t*) mouse_listeners->collection[i])->mouse_event_func(
                    mouse_listeners->collection[i],
                    event.xbutton.x,
                    event.xbutton.y,
                    MOUSE_RELEASE,
                    event.xbutton.button);
      }
         break;
      case MotionNotify:
      {
         mouse_x = event.xbutton.x;
         mouse_y = event.xbutton.y;
         for (size_t i = 0; i < mousemove_listeners->count; i++)
            ((object_t*) mousemove_listeners->collection[i])->mousemove_event_func(
                    mousemove_listeners->collection[i],
                    event.xbutton.x,
                    event.xbutton.y);
      }
         break;
      case Expose:
         break;
   }
}

//
// u_clear_objects
//
void u_clear_objects(bool free)
{
   if (free)
   {
      for (size_t i = 0; i < objects->count; i++)
         o_free((object_t*) objects->collection[i]);
   }

   // Just reset all vector lengths
   objects->count = 0;
   mouse_listeners->count = 0;
   key_listeners->count = 0;
   update_listeners->count = 0;
   mousemove_listeners->count = 0;
}

//
// u_push_object
//
void u_push_object(object_t* object)
{
   assert(object);

   list_push(objects, object);
   if (object->mouse_event_func)
      list_push(mouse_listeners, object);
   if (object->key_event_func)
      list_push(key_listeners, object);
   if (object->update_func)
      list_push(update_listeners, object);
   if (object->mousemove_event_func)
      list_push(mousemove_listeners, object);
}

//
// u_get_frames
//
uint64_t u_get_frames()
{
   return frames;
}

// Measures the time between frames and
// waits for the required amount of time
static void u_measure_time(void)
{
   // Measure time of draw cycle
   double_t tick_start = u_get_millis();

   // Call main draw routine
   u_draw_func();

   double_t diff = u_get_millis() - tick_start;
   counter++;
   frames++;

   // Wait the difference between two frames
   if (diff < FPS_DELAY)
   {
      usleep((uint32_t) (FPS_DELAY - diff) * 1000);
   }

   elapsed += u_get_millis() - tick_start;
   // calculate FPS
   if (elapsed > FPS_AV_COUNTER)
   {
      fps = 1000.0 * counter / elapsed;
      counter = 0;
      elapsed = 0;

      U_LOG("FPS: %lf. Objects: %li", fps, objects->count);

#ifdef OUTPUT_RENDER_TIME
      if(render_time_counter++ % OUTPUT_RENDER_TIME == 0)
      {
         render_chain_t* chain = rc_get_current();
         for(size_t i = 0; i < chain->stages->count; i++)
         {
            render_stage_t* stage = chain->stages->collection[i];
            U_LOG("%s: %lf ms", stage->name, (double_t)stage->render_time);
         }
      }
#endif
   }
}

list_t* u_get_objects(void)
{
   return objects;
}

//
// u_init
//
void u_init(void)
{
   objects = list_create();
   key_listeners = list_create();
   mouse_listeners = list_create();
   mousemove_listeners = list_create();
   update_listeners = list_create();
}

//
// u_start_loop
//
void u_start_loop(win_info_t* info)
{
   assert(info);

   XEvent event;
   XSelectInput(info->display, info->win,
       KeyPressMask      | KeyReleaseMask   | ButtonPressMask | ButtonReleaseMask    |
       PointerMotionMask | ButtonMotionMask | ExposureMask);

   Atom wmDelete = XInternAtom(info->display, "WM_DELETE_WINDOW", True);
   XSetWMProtocols(info->display, info->win, &wmDelete, 1);

   default_win = info;
   // main app loop
   while (1)
   {
      // CHECK close condition
      if (closed) break;
      // if there's no pending events
      if (XPending(info->display) == 0)
      {
         //update all objects
         for (size_t i = 0; i < update_listeners->count; i++)
            ((object_t*) update_listeners->collection[i])->update_func(update_listeners->collection[i]);

         // call draw func and wait to save constant frame rate
         u_measure_time();
         continue;
      }

      // process event queue
      XNextEvent(info->display, &event);
      u_event_handler(event);
   }

}

//
// u_get_fps()
//
double_t u_get_fps()
{
   return fps;
}

//
// u_release
//
void u_release()
{
   list_free(objects);
   list_free(key_listeners);
   list_free(mouse_listeners);
   list_free(mousemove_listeners);
   list_free(update_listeners);
}