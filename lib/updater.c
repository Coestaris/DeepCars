//
// Created by maxim on 8/28/19.
//

#ifdef __GNUC__
#pragma implementation "updater.h"
#endif
#include "updater.h"

#include "graphics/rendering/graphics.h"
#include "shaders/shader.h"
#include "scene.h"
#include "scm.h"
#include "graphics/rendering/render_chain.h"
#include "resources/txm.h"

#define U_LOG(format, ...) DC_LOG("updater.c", format, __VA_ARGS__)
#define U_ERROR(format, ...) DC_ERROR("updater.c", format, __VA_ARGS__)

// Initial count of objects
#define OBJECT_COUNT_START 50

//
// FPS Lock variables
//
// Set true to finish loop
bool        closed = false;
// Frame counter for FPS measurements
uint64_t    counter = 0;
// Elapsed time between current time and last FPS measurement
double_t    elapsed = 0;
// Average FPS count
double_t    fps = 0;
// Current frame counter
uint64_t    frames;

//
// Event processing variables
//
uint32_t    keys_state[256];
uint32_t    mouse_state[10];
uint32_t    mouse_x;
uint32_t    mouse_y;

// Current X window allocated in win.h
win_info_t*  default_win;

// Global list of all drawable objects
list_t*     objects;
// Preprocessed lists of listening objects
list_t*     key_listeners;
list_t*     mouse_listeners;
list_t*     mousemove_listeners;
list_t*     update_listeners;


// Get current millisecond of global time
double_t u_get_millis(void)
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
void u_draw_func(void)
{
   render_chain_t* chain = rc_get_current();
   list_t* stages = chain->stages;

   for(size_t i = 0; i < stages->count; i++)
   {
      render_stage_t* stage = (render_stage_t*)stages->collection[i];
      gr_bind(stage);
      sh_use(stage->shader);
      stage->bind_shader(stage);

      if(stage->render_mode == RM_GEOMETRY)
      {
         //render objects
         glEnable(GL_DEPTH_TEST);
         for(size_t j = 0; j < objects->count; j++)
         {
            object_t* obj = objects->collection[j];
            if(obj->draw_info->drawable)
            {
               gr_transform(obj->position, obj->scale, obj->rotation);
               stage->setup_obj_shader(stage, obj, model_mat);
               gr_render_object(obj);
            }
         }
         glDisable(GL_DEPTH_TEST);

      }
      else if(stage->render_mode == RM_BYPASS || stage->render_mode == RM_FRAMEBUFFER)
      {
         //render from buffer to a buffer
         gr_render_vao(stage->vao);
      }
      else if(stage->render_mode == RM_CUSTOM || stage->render_mode == RM_CUSTOM_FRAMEBUFFER)
      {
         stage->custom_draw_func(stage);
      }

      stage->unbind_shader(stage);
      gr_unbind(stage);
      sh_use(NULL);
   }

   w_swap_buffers(default_win);
}

//
// u_get_mouse_pos()
//
vec2f_t u_get_mouse_pos()
{
   return vec2f(mouse_x, mouse_y);
}

//
// u_get_key_state()
//
int u_get_key_state(int key)
{
   return keys_state[key];
}

//
// u_get_mouse_state()
//
int u_get_mouse_state(int mouse)
{
   return mouse_state[mouse];
}

// Main event handler function.
// Sets all necessary states and run all callbacks in listeners
void u_event_handler(XEvent event)
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
void u_measure_time(void)
{
   double_t tick_start = u_get_millis();
   u_draw_func(); //draw routine

   double_t diff = u_get_millis() - tick_start;
   counter++;
   frames++;

   // wait
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
   }
}

//
// u_init
//
void u_init(void)
{
   objects = list_create(OBJECT_COUNT_START);
   key_listeners = list_create(OBJECT_COUNT_START);
   mouse_listeners = list_create(OBJECT_COUNT_START);
   mousemove_listeners = list_create(OBJECT_COUNT_START);
   update_listeners = list_create(OBJECT_COUNT_START);
}

//
// u_start_loop
//
void u_start_loop(win_info_t* info)
{

   XEvent event;
   XSelectInput(info->display, info->win,
       KeyPressMask      | KeyReleaseMask   | ButtonPressMask | ButtonReleaseMask    |
       PointerMotionMask | ButtonMotionMask | ExposureMask    | VisibilityChangeMask |
       ResizeRedirectMask);

   Atom wmDelete = XInternAtom(info->display, "WM_DELETE_WINDOW", True);
   XSetWMProtocols(info->display, info->win, &wmDelete, 1);

   default_win = info;
   // main app loop
   while (1)
   {
      // check close condition
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
// u_free
//
void u_free()
{
   list_free(objects);
   list_free(key_listeners);
   list_free(mouse_listeners);
   list_free(mousemove_listeners);
   list_free(update_listeners);
}