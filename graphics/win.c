//
// Created by maxim on 8/26/19.
//

#include "win.h"

typedef GLXContext (* glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
static bool ctxErrorOccurred = false;

static bool w_isExtensionSupported(const char* extList, const char* extension)
{
    const char* start;
    const char* where, * terminator;
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return false;

    for (start = extList;;)
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

static int ctxErrorHandler(Display* dpy, XErrorEvent* ev)
{
    ctxErrorOccurred = true;
    return 0;
}

int glx_major_version = GLX_DEFAULT_MAJOR_VERSION;
int glx_minor_version = GLX_DEFAULT_MINOR_VERSION;
int glx_flags = GLX_DEFAULT_FLAGS;

void w_setGLXContextVersion(int major, int minor, int flags)
{
    glx_major_version = major;
    glx_minor_version = minor;
    glx_flags = flags;
}

winInfo_t* w_create(uint16_t winW, uint16_t winH, uint16_t winX, uint16_t winY,
        char* winCaption, bool verbose, FILE* vf)
{
    if(verbose)
        fprintf(vf, "[*] Trying to open X display\n");

    winInfo_t* info = malloc(sizeof(winInfo_t));
    info->display = XOpenDisplay(NULL);
    if (!info->display)
    {
        fprintf(vf, "[-] Failed to open X display\n");
        exit(1);
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

    if(verbose)
        fprintf(vf, "[*] Checking GLX version\n");

    int glx_major, glx_minor;

    if (!glXQueryVersion(info->display, &glx_major, &glx_minor) ||
        ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1))
    {
        fprintf(vf,"[-] Invalid GLX version. Required GLX 1.3 or higher\n");
        exit(1);
    }

    if(verbose)
        fprintf(vf, "[*] Getting matching framebuffer configs\n");

    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(info->display, DefaultScreen(info->display), visual_attribs, &fbcount);
    if (!fbc)
    {
        fprintf(vf, "[-] Failed to retrieve a framebuffer config\n");
        exit(1);
    }
    if(verbose)
        fprintf(vf,"[*] Found %d matching FB configs.\n", fbcount);

    // Pick the FB config/visual with the most samples per pixel
    if(verbose)
        fprintf(vf,"[*] Getting XVisualInfos\n");
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    for (size_t i = 0; i < fbcount; i++)
    {
        XVisualInfo* vi = glXGetVisualFromFBConfig(info->display, fbc[i]);
        if (vi)
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib(info->display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(info->display, fbc[i], GLX_SAMPLES, &samples);

            if(verbose)
                fprintf(vf,"[*] [%2lu/%2i] Fbconfig: visual ID 0x%2lx: SAMPLE_BUFFERS = %d, SAMPLES = %d\n",
                   i + 1, fbcount, vi->visualid, samp_buf, samples);

            if (best_fbc < 0 || (samp_buf && samples > best_num_samp))
                best_fbc = i, best_num_samp = samples;
            if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
                worst_fbc = i, worst_num_samp = samples;
        }
        XFree(vi);
    }

    GLXFBConfig bestFbc = fbc[best_fbc];

    // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
    XFree(fbc);

    // Get a visual
    if(verbose)
        fprintf(vf,"[*] Getting XVisual from FrameBuffer Config\n");
    XVisualInfo* vi = glXGetVisualFromFBConfig(info->display, bestFbc);
    if(verbose)
        fprintf(vf, "[*] Chosen visual ID = 0x%x\n", vi->visualid);

    if(verbose)
        fprintf(vf,"[*] Creating colormap\n");
    XSetWindowAttributes swa;
    Colormap cmap;
    swa.colormap = cmap = XCreateColormap(info->display, RootWindow(info->display, vi->screen),
                                          vi->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;

    info->colorMap = cmap;

    if(verbose)
        fprintf(vf,"[*] Creating window\n");
    info->win = XCreateWindow(info->display, RootWindow(info->display, vi->screen),
                               0, 0, 100, 100, 0, vi->depth, InputOutput, vi->visual,
                               CWBorderPixel | CWColormap | CWEventMask, &swa);
    if (!info->win)
    {
        fprintf(vf, "[-] Failed to create window.\n");
        exit(1);
    }

    // Done with the visual info data
    XFree(vi);

    XStoreName(info->display, info->win, winCaption);

    if(verbose)
        fprintf(vf,"[*] Mapping window\n");
    XMapWindow(info->display, info->win);

    // Get the default screen's GLX extension list
    const char* glxExts = glXQueryExtensionsString(info->display, DefaultScreen(info->display));

    // NOTE: It is not necessary to create or make current to a context before
    // calling glXGetProcAddressARB
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
            glXGetProcAddressARB((const GLubyte*) "glXCreateContextAttribsARB");

    GLXContext ctx = 0;

    // Install an X error handler so the application won't exit if GL 3.0
    // context allocation fails.
    //
    // Note this error handler is global.  All display connections in all threads
    // of a process use the same error handler, so be sure to guard against other
    // threads issuing X commands while this code is running.
    ctxErrorOccurred = false;
    int (* oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

    if(verbose)
        fprintf(vf,"[*] Creating GL Context\n");

    // Check for the GLX_ARB_create_context extension string and the function.
    // If either is not present, use GLX 1.3 context creation method.
    if (!w_isExtensionSupported(glxExts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB)
    {
        fprintf(vf, "glXCreateContextAttribsARB() not found... using old-style GLX context\n");
        ctx = glXCreateNewContext(info->display, bestFbc, GLX_RGBA_TYPE, 0, True);
    }
    // If it does, try to get a GL 3.0 context!
    else
    {
        int context_attribs[] =
        {
                GLX_CONTEXT_MAJOR_VERSION_ARB, glx_major_version,
                GLX_CONTEXT_MINOR_VERSION_ARB, glx_minor_version,
                GLX_CONTEXT_FLAGS_ARB        , glx_flags,
                None
        };

        if(verbose)
            fprintf(vf,"[*] Creating context\n");
        ctx = glXCreateContextAttribsARB(info->display, bestFbc, 0, True, context_attribs);

        // Sync to ensure any errors generated are processed.
        XSync(info->display, False);
        if (!ctxErrorOccurred && ctx)
        {
            if (verbose)
                fprintf(vf, "[*] Created GL 3.0 context\n");
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

            ctxErrorOccurred = false;

            fprintf(vf, "[*] Failed to create GL 3.0 context... using old-style GLX context\n");
            ctx = glXCreateContextAttribsARB(info->display, bestFbc, 0, True, context_attribs);
        }
    }

    // Sync to ensure any errors generated are processed.
    XSync(info->display, False);

    // Restore the original error handler
    XSetErrorHandler(oldHandler);

    if (ctxErrorOccurred || !ctx)
    {
        fprintf(vf ,"[-] Failed to create an OpenGL context\n");
        exit(1);
    }

    // Verifying that context is a direct context
    if (!glXIsDirect(info->display, ctx))
    {
        if(verbose)
            fprintf(vf, "[*] Indirect GLX rendering context obtained\n");
    }
    else
    {
        if(verbose)
            fprintf(vf, "[*] Direct GLX rendering context obtained\n");
    }

    if(verbose)
        fprintf(vf,"[*] Making context current\n");

    glXMakeCurrent(info->display, info->win, ctx);

    return info;
}

void w_destroy(winInfo_t* w)
{
    glXMakeCurrent(w->display, 0, 0);
    glXDestroyContext(w->display, w->context);

    XDestroyWindow(w->display, w->win);
    XFreeColormap(w->display, w->colorMap);
    XCloseDisplay(w->display);
}

void w_swapBuffers(winInfo_t* w)
{
    glXSwapBuffers(w->display, w->win);
}