//
// Created by maxim on 8/26/19.
//

#ifdef __GNUC__
#pragma implementation "win.h"
#endif
#include "win.h"

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
   printf("[win.c][ERROR]: == CONTEXT ERROR ==");
   printf("[win.c][ERROR]: Type: %i, Error code: %i", ev->type, ev->error_code);
   printf("[win.c][ERROR]: Minor code: %i, Request code: %i,", ev->minor_code, ev->request_code);
   printf("[win.c][ERROR]: Resource ID: %lu, Serial number: %lu", ev->resourceid, ev->serial);
   exit(EXIT_FAILURE);
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
   win_info_t* info = malloc(sizeof(win_info_t));
   info->w = win_w;
   info->h = win_h;
   info->caption = win_caption;

   if (verbose)
   {
      printf("[win.c]: Trying to open X display\n");
   }
   info->display = XOpenDisplay(NULL);
   if (!info->display)
   {
      printf("[win.c][ERROR]: Failed to open X display\n");
      exit(EXIT_FAILURE);
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
                   None
           };

   if (verbose)
      printf("[win.c]: Checking GLX version\n");

   int glx_major, glx_minor;

   if (!glXQueryVersion(info->display, &glx_major, &glx_minor) ||
       ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1))
   {
      printf("[win.c][ERROR]: Invalid GLX version. Required GLX 1.3 or higher\n");
      exit(EXIT_FAILURE);
   }

   if (verbose)
      printf("[win.c]: Getting matching framebuffer configs\n");

   int fbcount;
   GLXFBConfig* fbc = glXChooseFBConfig(info->display, DefaultScreen(info->display), visual_attribs, &fbcount);
   if (!fbc)
   {
      printf("[win.c][ERROR]: Failed to retrieve a framebuffer config\n");
      exit(EXIT_FAILURE);
   }
   if (verbose)
      printf("[win.c]: Found %d matching FB configs.\n", fbcount);

   // Pick the FB config/visual with the most samples per pixel
   if (verbose)
      printf("[win.c]: Getting XVisualInfos\n");
   int best_fbc_index = -1, worst_fbc_index = -1, best_num_samp = -1, worst_num_samp = 999;

   for (size_t i = 0; i < fbcount; i++)
   {
      XVisualInfo* vi = glXGetVisualFromFBConfig(info->display, fbc[i]);
      if (vi)
      {
         int samp_buf, samples;
         glXGetFBConfigAttrib(info->display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
         glXGetFBConfigAttrib(info->display, fbc[i], GLX_SAMPLES, &samples);

         if (verbose)
            printf("[win.c]: [%2lu/%2i] Fbconfig: visual ID 0x%2lx: SAMPLE_BUFFERS = %d, SAMPLES = %d\n",
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
      printf("[win.c]: Getting XVisual from FrameBuffer Config\n");
   XVisualInfo* vi = glXGetVisualFromFBConfig(info->display, best_fbc);
   if (verbose)
      printf("[win.c]: Chosen visual ID = 0x%lx\n", vi->visualid);

   if (verbose)
      printf("[win.c]: Creating colormap\n");
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
      printf("[win.c]: Creating window\n");
      printf("[win.c]: WinW: %i, win_h: %i, win_x: %i, win_y: %i, caption: \"%s\"\n",
              win_w, win_h, win_x, win_y, win_caption);
   }
   info->win = XCreateWindow(info->display, RootWindow(info->display, vi->screen),
                             win_x, win_y, win_w, win_h, 0, vi->depth, InputOutput, vi->visual,
                             CWBorderPixel | CWColormap | CWEventMask, &swa);
   if (!info->win)
   {
      printf("[win.c][ERROR]: Failed to create window.\n");
      exit(EXIT_FAILURE);
   }

   // Done with the visual info data
   XFree(vi);

   XStoreName(info->display, info->win, win_caption);

   if (verbose)
      printf("[win.c]: Mapping window\n");
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
      printf("[win.c]: Creating GL Context\n");

   // Check for the GLX_ARB_create_context extension string and the function.
   // If either is not present, use GLX 1.3 context creation method.
   if (!w_is_extension_supported(glx_exts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB)
   {
      printf("[win.c]: glXCreateContextAttribsARB() not found... using old-style GLX context\n");
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
         printf("[win.c]: Creating context\n");
      ctx = glXCreateContextAttribsARB(info->display, best_fbc, 0, True, context_attribs);

      // Sync to ensure any errors generated are processed.
      XSync(info->display, False);
      if (ctx)
      {
         if (verbose)
            printf("[win.c]: Created GL 3.0 context\n");
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

         printf("[win.c]: Failed to create GL 3.0 context... using old-style GLX context\n");
         ctx = glXCreateContextAttribsARB(info->display, best_fbc, 0, True, context_attribs);
      }
   }

   // Sync to ensure any errors generated are processed.
   XSync(info->display, False);

   // Restore the original error handler
   XSetErrorHandler(old_handler);

   if (!ctx)
   {
      printf("[win.c][ERROR]: Failed to create an OpenGL context\n");
      exit(EXIT_FAILURE);
   }

   // Verifying that context is a direct context
   if (!glXIsDirect(info->display, ctx))
   {
      if (verbose)
         printf("[win.c]: Indirect GLX rendering context obtained\n");
   }
   else
   {
      if (verbose)
         printf("[win.c]: Direct GLX rendering context obtained\n");
   }

   if (verbose)
      printf("[win.c]: Making context current\n");

   info->context = ctx;
   glXMakeCurrent(info->display, info->win, ctx);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);

   return info;
}

//
// w_destroy()
//
void w_destroy(win_info_t* w)
{
   glXMakeCurrent(w->display, 0, 0);
   glXDestroyContext(w->display, w->context);

   XDestroyWindow(w->display, w->win);
   XFreeColormap(w->display, w->color_map);
   XCloseDisplay(w->display);

   free(w);
}

//
// w_swap_buffers()
//
void w_swap_buffers(win_info_t* w)
{
   glXSwapBuffers(w->display, w->win);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void w_print_info()
{
   printf("[win.c][GL Spec]: Vendor: %s\n", (char*) glGetString(GL_VENDOR));
   printf("[win.c][GL Spec]: Using OpenGL Version: %s\n", (char*) glGetString(GL_VERSION));
   printf("[win.c][GL Spec]: Using OpenGL Rendered: %s\n", (char*) glGetString(GL_RENDERER));
   printf("[win.c][GL Spec]: GLSH Version: %s\n", (char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
}
