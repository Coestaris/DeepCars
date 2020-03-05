//
// Created by maxim on 8/26/19.
//

#ifdef __GNUC__
#pragma implementation "win.h"
#endif
#include "win.h"
#include "../coredefs.h"

#define W_LOG(format, ...) DC_LOG("win.c", format, __VA_ARGS__)
#define W_ERROR(format, ...) DC_ERROR("win.c", format, __VA_ARGS__)

//
// GLX setup parameters
//
int glx_major_version   = GLX_DEFAULT_MAJOR_VERSION;
int glx_minor_version   = GLX_DEFAULT_MINOR_VERSION;
int glx_flags           = GLX_DEFAULT_FLAGS;

// ??
typedef GLXContext (* gl_x_create_context_attribs_arb_proc_t)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

// Check is list of extensions ext_list contains extension
static bool w_is_extension_supported(const char* ext_list, const char* extension)
{
   const char* start;
   const char* where, * terminator;
   where = strchr(extension, ' ');
   if (where || *extension == '\0')
      return false;

   for (start = ext_list;;)
   {
      where = strstr(start, extension);
      if (!where)
         break;

      terminator = where + strlen(extension);
      if (where == start || *(where - 1) == ' ')
         if (*terminator == ' ' || *terminator == '\0')
            return true;

      start = terminator;
   }

   return false;
}

// Temporary GLX error handler. Required for creating 2.x contexts
static int w_ctx_error_handler(Display* dpy, XErrorEvent* ev)
{
   char buff[1024];
   snprintf(buff, 1024, "== CONTEXT ERROR ==\n\
      Type: %i, Error code: %i\n\
      Minor code: %i, Request code: %i,\n\
      Resource ID: %lu, Serial number: %lu\n",
      ev->type, ev->error_code, ev->minor_code,
      ev->request_code, ev->resourceid, ev->serial);

   W_ERROR((const char*)buff, 0);
   return 0;
}

//
// w_set_glx_context_version()
//
void w_set_glx_context_version(int major, int minor, int flags)
{
   glx_major_version = major;
   glx_minor_version = minor;
   glx_flags = flags;
}

//
// w_create()
//
win_info_t* w_create(uint16_t win_w, uint16_t win_h, uint16_t win_x, uint16_t win_y,
                     const char* win_caption, bool verbose, bool use_double_buffer)
{
   W_LOG("[INIT]: ==============================",0);
   W_LOG("[INIT]: %s (version %s) by %s", DEEPCARS, DEEPCARS_VERSION, DEEPCARS_DEVS);
   W_LOG("[INIT]: Verbose: %s", VERBOSE ? "true" : "false");
   W_LOG("[INIT]: Debug level: %i", DEBUG_LEVEL);
   W_LOG("[INIT]: ==============================",0);

   win_info_t* info = DEEPCARS_MALLOC(sizeof(win_info_t));
   info->w = win_w;
   info->h = win_h;
   info->caption = win_caption;

   if (verbose)
   {
      W_LOG("Trying to open X display",0);
   }
   info->display = XOpenDisplay(NULL);
   if (!info->display)
   {
      W_ERROR("Failed to open X display",0);
   }

   // Get a matching FB config
   static int visual_attribs[] =
   {
          GLX_X_RENDERABLE, True,
          GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
          GLX_RENDER_TYPE, GLX_RGBA_BIT,
          GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
          GLX_RED_SIZE, 8,
          GLX_GREEN_SIZE, 8,
          GLX_BLUE_SIZE, 8,
          GLX_ALPHA_SIZE, 8,
          GLX_DEPTH_SIZE, 24,
          GLX_STENCIL_SIZE, 8,
          GLX_DOUBLEBUFFER, True,
          GLX_SAMPLE_BUFFERS  , 1,            // <-- MSAA
          GLX_SAMPLES         , 4,            // <-- MSAA
          None
   };

   if (verbose)
      W_LOG("Checking GLX version",0);

   int glx_major, glx_minor;

   if (!glXQueryVersion(info->display, &glx_major, &glx_minor) ||
       ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1))
   {
      W_ERROR("Invalid GLX version. Required GLX 1.3 or higher",0);
   }

   if (verbose)
      W_LOG("Getting matching framebuffer configs",0);

   int fbcount;
   GLXFBConfig* fbc = glXChooseFBConfig(info->display, DefaultScreen(info->display), visual_attribs, &fbcount);
   if (!fbc)
   {
      W_ERROR("Failed to retrieve a framebuffer config",0);
   }
   if (verbose)
      W_LOG("Found %d matching FB configs", fbcount);

   // Pick the FB config/visual with the most samples per pixel
   if (verbose)
      W_LOG("Getting XVisualInfos",0);
   int best_fbc_index = -1, worst_fbc_index = -1, best_num_samp = -1, worst_num_samp = 999;

   for (size_t i = 0; i < (size_t)fbcount; i++)
   {
      XVisualInfo* vi = glXGetVisualFromFBConfig(info->display, fbc[i]);
      if (vi)
      {
         int samp_buf, samples;
         glXGetFBConfigAttrib(info->display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
         glXGetFBConfigAttrib(info->display, fbc[i], GLX_SAMPLES, &samples);

         if (verbose)
            W_LOG("[%2lu/%2i] Fbconfig: visual ID 0x%2lx: SAMPLE_BUFFERS = %d, SAMPLES = %d",
                    i + 1, fbcount, vi->visualid, samp_buf, samples);

         if (best_fbc_index < 0 || (samp_buf && samples > best_num_samp))
            best_fbc_index = i, best_num_samp = samples;
         if (worst_fbc_index < 0 || !samp_buf || samples < worst_num_samp)
            worst_fbc_index = i, worst_num_samp = samples;
      }
      XFree(vi);
   }

   GLXFBConfig best_fbc = fbc[best_fbc_index];

   // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
   XFree(fbc);

   // Get a visual
   if (verbose)
      W_LOG("Getting XVisual from FrameBuffer Config",0);
   XVisualInfo* vi = glXGetVisualFromFBConfig(info->display, best_fbc);
   if (verbose)
      W_LOG("Chosen visual ID = 0x%lx", vi->visualid);

   if (verbose)
      W_LOG("Creating colormap",0);
   XSetWindowAttributes swa;
   Colormap cmap;
   swa.colormap = cmap = XCreateColormap(info->display, RootWindow(info->display, vi->screen),
                                         vi->visual, AllocNone);
   swa.background_pixmap = None;
   swa.border_pixel = 0;
   swa.event_mask = StructureNotifyMask;

   info->color_map = cmap;

   if (verbose)
   {
     W_LOG("Creating window",0);
     W_LOG("WinW: %i, win_h: %i, win_x: %i, win_y: %i, caption: \"%s\"",
              win_w, win_h, win_x, win_y, win_caption);
   }
   info->win = XCreateWindow(info->display, RootWindow(info->display, vi->screen),
                             win_x, win_y, win_w, win_h, 0, vi->depth, InputOutput, vi->visual,
                             CWBorderPixel | CWColormap | CWEventMask, &swa);
   if (!info->win)
   {
      W_ERROR("Failed to create window",0);
   }

   // Done with the visual info data
   XFree(vi);

   XStoreName(info->display, info->win, win_caption);

   if (verbose)
      W_LOG("Mapping window",0);
   XMapWindow(info->display, info->win);

   // Get the default screen's GLX extension list
   const char* glx_exts = glXQueryExtensionsString(info->display, DefaultScreen(info->display));

   // NOTE: It is not necessary to create or make current to a context before
   // calling glXGetProcAddressARB
   gl_x_create_context_attribs_arb_proc_t glXCreateContextAttribsARB = 0;
   glXCreateContextAttribsARB = (gl_x_create_context_attribs_arb_proc_t)
           glXGetProcAddressARB((const GLubyte*) "glXCreateContextAttribsARB");

   GLXContext ctx = 0;

   // Install an X error handler so the application won't exit if GL 3.0
   // context allocation fails.
   //
   // Note this error handler is global.  All display connections in all threads
   // of a process use the same error handler, so be sure to guard against other
   // threads issuing X commands while this code is running.
   int (* old_handler)(Display*, XErrorEvent*) = XSetErrorHandler(&w_ctx_error_handler);

   if (verbose)
      W_LOG("Creating GL Context",0);

   // Check for the GLX_ARB_create_context extension string and the function.
   // If either is not present, use GLX 1.3 context creation method.
   if (!w_is_extension_supported(glx_exts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB)
   {
      W_LOG("glXCreateContextAttribsARB() not found... using old-style GLX context",0);
      ctx = glXCreateNewContext(info->display, best_fbc, GLX_RGBA_TYPE, 0, True);
   }
      // If it does, try to get a GL 3.0 context!
   else
   {
      int context_attribs[] =
              {
                      GLX_CONTEXT_MAJOR_VERSION_ARB, glx_major_version,
                      GLX_CONTEXT_MINOR_VERSION_ARB, glx_minor_version,
                      GLX_CONTEXT_FLAGS_ARB, glx_flags,
                      None
              };

      if (verbose)
         W_LOG("Creating context",0);

      ctx = glXCreateContextAttribsARB(info->display, best_fbc, 0, True, context_attribs);

      // Sync to ensure any errors generated are processed.
      XSync(info->display, False);
      if (ctx)
      {
         if (verbose)
            W_LOG("Created GL 3.0 context",0);
      }
      else
      {
         // Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
         // When a context version below 3.0 is requested, implementations will
         // return the newest context version compatible with OpenGL versions less
         // than version 3.0.
         // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
         context_attribs[1] = 1;
         // GLX_CONTEXT_MINOR_VERSION_ARB = 0
         context_attribs[3] = 0;

         W_LOG("Failed to create GL 3.0 context... using old-style GLX context",0);
         ctx = glXCreateContextAttribsARB(info->display, best_fbc, 0, True, context_attribs);
      }
   }

   // Sync to ensure any errors generated are processed.
   XSync(info->display, False);

   // Restore the original error handler
   XSetErrorHandler(old_handler);

   if (!ctx)
   {
      W_ERROR("Failed to create an OpenGL context",0);
   }

   // Verifying that context is a direct context
   if (!glXIsDirect(info->display, ctx))
   {
      if (verbose)
         W_LOG("Indirect GLX rendering context obtained",0);
   }
   else
   {
      if (verbose)
         W_LOG("Direct GLX rendering context obtained",0);
   }

   if (verbose)
      W_LOG("Making context current",0);

   info->context = ctx;
   glXMakeCurrent(info->display, info->win, ctx);

   GL_CALL(glEnable(GL_DEPTH_TEST));
   GL_CALL(glEnable(GL_MULTISAMPLE));
   return info;
}

//
// w_destroy()
//
void w_destroy(win_info_t* w)
{
   glXMakeCurrent(w->display, 0, 0);
   glXDestroyContext(w->display, w->context);
   W_LOG("Context destroyed",0);

   XDestroyWindow(w->display, w->win);
   XFreeColormap(w->display, w->color_map);
   XCloseDisplay(w->display);

   DEEPCARS_FREE(w);
}

//
// w_swap_buffers()
//
inline void w_swap_buffers(win_info_t* w)
{
   GL_PCALL(glXSwapBuffers(w->display, w->win))
   GL_PCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
   //GL_PCALL(glClear(GL_DEPTH_BUFFER_BIT))
}

void w_print_info()
{
   W_LOG("[GL_INFO]: Vendor: %s", (char*) glGetString(GL_VENDOR));
   W_LOG("[GL_INFO]: Using OpenGL Version: %s", (char*) glGetString(GL_VERSION));
   W_LOG("[GL_INFO]: Using OpenGL Rendered: %s", (char*) glGetString(GL_RENDERER));
   W_LOG("[GL_INFO]: GLSH Version: %s", (char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
}
