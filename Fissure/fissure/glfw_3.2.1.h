/*************************************************************************
 * GLFW 3.3 - www.glfw.org
 * A library for OpenGL, window and input
 *------------------------------------------------------------------------
 * Copyright (c) 2002-2006 Marcus Geelnard
 * Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 *************************************************************************/

// For implementation:
// #define GLFW_IMPL


#ifndef _glfw3_h_
#define _glfw3_h_

#ifdef __cplusplus
extern "C" {
#endif


/*************************************************************************
 * Doxygen documentation
 *************************************************************************/

/*! @file glfw3.h
 *  @brief The header of the GLFW 3 API.
 *
 *  This is the header file of the GLFW 3 API.  It defines all its types and
 *  declares all its functions.
 *
 *  For more information about how to use this file, see @ref build_include.
 */
/*! @defgroup context Context reference
 *
 *  This is the reference documentation for OpenGL and OpenGL ES context related
 *  functions.  For more task-oriented information, see the @ref context_guide.
 */
/*! @defgroup vulkan Vulkan reference
 *
 *  This is the reference documentation for Vulkan related functions and types.
 *  For more task-oriented information, see the @ref vulkan_guide.
 */
/*! @defgroup init Initialization, version and error reference
 *
 *  This is the reference documentation for initialization and termination of
 *  the library, version management and error handling.  For more task-oriented
 *  information, see the @ref intro_guide.
 */
/*! @defgroup input Input reference
 *
 *  This is the reference documentation for input related functions and types.
 *  For more task-oriented information, see the @ref input_guide.
 */
/*! @defgroup monitor Monitor reference
 *
 *  This is the reference documentation for monitor related functions and types.
 *  For more task-oriented information, see the @ref monitor_guide.
 */
/*! @defgroup window Window reference
 *
 *  This is the reference documentation for window related functions and types,
 *  including creation, deletion and event polling.  For more task-oriented
 *  information, see the @ref window_guide.
 */


/*************************************************************************
 * Compiler- and platform-specific preprocessor work
 *************************************************************************/

/* If we are we on Windows, we want a single define for it.
 */
#if !defined(_WIN32) && (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
 #define _WIN32
#endif /* _WIN32 */

/* It is customary to use APIENTRY for OpenGL function pointer declarations on
 * all platforms.  Additionally, the Windows OpenGL header needs APIENTRY.
 */
#ifndef APIENTRY
 #ifdef _WIN32
  #define APIENTRY __stdcall
 #else
  #define APIENTRY
 #endif
#endif /* APIENTRY */

/* Some OpenGL related headers use GLAPIENTRY instead.
 */
#ifndef GLAPIENTRY
 #define GLAPIENTRY APIENTRY
#endif /* GLAPIENTRY */

/* Some Windows OpenGL headers need this.
 */
#if !defined(WINGDIAPI) && defined(_WIN32)
 #define WINGDIAPI __declspec(dllimport)
 #define GLFW_WINGDIAPI_DEFINED
#endif /* WINGDIAPI */

/* Some Windows GLU headers need this.
 */
#if !defined(CALLBACK) && defined(_WIN32)
 #define CALLBACK __stdcall
 #define GLFW_CALLBACK_DEFINED
#endif /* CALLBACK */

/* Include because most Windows GLU headers need wchar_t and
 * the macOS OpenGL header blocks the definition of ptrdiff_t by glext.h.
 * Include it unconditionally to avoid surprising side-effects.
 */
#include <stddef.h>

/* Include because it is needed by Vulkan and related functions.
 */
#include <stdint.h>

/* Include the chosen client API headers.
 */
#if defined(__APPLE__)
 #if defined(GLFW_INCLUDE_GLCOREARB)
  #include <OpenGL/gl3.h>
  #if defined(GLFW_INCLUDE_GLEXT)
   #include <OpenGL/gl3ext.h>
  #endif
 #elif !defined(GLFW_INCLUDE_NONE)
  #if !defined(GLFW_INCLUDE_GLEXT)
   #define GL_GLEXT_LEGACY
  #endif
  #include <OpenGL/gl.h>
 #endif
 #if defined(GLFW_INCLUDE_GLU)
  #include <OpenGL/glu.h>
 #endif
#else
 #if defined(GLFW_INCLUDE_GLCOREARB)
  #include <GL/glcorearb.h>
 #elif defined(GLFW_INCLUDE_ES1)
  #include <GLES/gl.h>
  #if defined(GLFW_INCLUDE_GLEXT)
   #include <GLES/glext.h>
  #endif
 #elif defined(GLFW_INCLUDE_ES2)
  #include <GLES2/gl2.h>
  #if defined(GLFW_INCLUDE_GLEXT)
   #include <GLES2/gl2ext.h>
  #endif
 #elif defined(GLFW_INCLUDE_ES3)
  #include <GLES3/gl3.h>
  #if defined(GLFW_INCLUDE_GLEXT)
   #include <GLES2/gl2ext.h>
  #endif
 #elif defined(GLFW_INCLUDE_ES31)
  #include <GLES3/gl31.h>
  #if defined(GLFW_INCLUDE_GLEXT)
   #include <GLES2/gl2ext.h>
  #endif
 #elif defined(GLFW_INCLUDE_ES32)
  #include <GLES3/gl32.h>
  #if defined(GLFW_INCLUDE_GLEXT)
   #include <GLES2/gl2ext.h>
  #endif
 #elif !defined(GLFW_INCLUDE_NONE)
  #include <GL/gl.h>
  #if defined(GLFW_INCLUDE_GLEXT)
   #include <GL/glext.h>
  #endif
 #endif
 #if defined(GLFW_INCLUDE_GLU)
  #include <GL/glu.h>
 #endif
#endif
#if defined(GLFW_INCLUDE_VULKAN)
 #if defined(__APPLE__)
  #include <MoltenVK/vulkan/vulkan.h>
 #else
  #include <vulkan/vulkan.h>
 #endif
#endif

#if defined(GLFW_DLL) && defined(_GLFW_BUILD_DLL)
 /* GLFW_DLL must be defined by applications that are linking against the DLL
  * version of the GLFW library.  _GLFW_BUILD_DLL is defined by the GLFW
  * configuration header when compiling the DLL version of the library.
  */
 #error "You must not have both GLFW_DLL and _GLFW_BUILD_DLL defined"
#endif

/* GLFWAPI is used to declare public API functions for export
 * from the DLL / shared library / dynamic library.
 */
#if defined(_WIN32) && defined(_GLFW_BUILD_DLL)
 /* We are building GLFW as a Win32 DLL */
 #define GLFWAPI __declspec(dllexport)
#elif defined(_WIN32) && defined(GLFW_DLL)
 /* We are calling GLFW as a Win32 DLL */
 #define GLFWAPI __declspec(dllimport)
#elif defined(__GNUC__) && defined(_GLFW_BUILD_DLL)
 /* We are building GLFW as a shared / dynamic library */
 #define GLFWAPI __attribute__((visibility("default")))
#else
 /* We are building or calling GLFW as a static library */
 #define GLFWAPI
#endif


/*************************************************************************
 * GLFW API tokens
 *************************************************************************/

/*! @name GLFW version macros
 *  @{ */
/*! @brief The major version number of the GLFW library.
 *
 *  This is incremented when the API is changed in non-compatible ways.
 *  @ingroup init
 */
#define GLFW_VERSION_MAJOR          3
/*! @brief The minor version number of the GLFW library.
 *
 *  This is incremented when features are added to the API but it remains
 *  backward-compatible.
 *  @ingroup init
 */
#define GLFW_VERSION_MINOR          3
/*! @brief The revision number of the GLFW library.
 *
 *  This is incremented when a bug fix release is made that does not contain any
 *  API changes.
 *  @ingroup init
 */
#define GLFW_VERSION_REVISION       0
/*! @} */

/*! @name Boolean values
 *  @{ */
/*! @brief One.
 *
 *  One.  Seriously.  You don't _need_ to use this symbol in your code.  It's
 *  just semantic sugar for the number 1.  You can use `1` or `true` or `_True`
 *  or `GL_TRUE` or whatever you want.
 */
#define GLFW_TRUE                   1
/*! @brief Zero.
 *
 *  Zero.  Seriously.  You don't _need_ to use this symbol in your code.  It's
 *  just just semantic sugar for the number 0.  You can use `0` or `false` or
 *  `_False` or `GL_FALSE` or whatever you want.
 */
#define GLFW_FALSE                  0
/*! @} */

/*! @name Key and button actions
 *  @{ */
/*! @brief The key or mouse button was released.
 *
 *  The key or mouse button was released.
 *
 *  @ingroup input
 */
#define GLFW_RELEASE                0
/*! @brief The key or mouse button was pressed.
 *
 *  The key or mouse button was pressed.
 *
 *  @ingroup input
 */
#define GLFW_PRESS                  1
/*! @brief The key was held down until it repeated.
 *
 *  The key was held down until it repeated.
 *
 *  @ingroup input
 */
#define GLFW_REPEAT                 2
/*! @} */

/*! @defgroup keys Keyboard keys
 *
 *  See [key input](@ref input_key) for how these are used.
 *
 *  These key codes are inspired by the _USB HID Usage Tables v1.12_ (p. 53-60),
 *  but re-arranged to map to 7-bit ASCII for printable keys (function keys are
 *  put in the 256+ range).
 *
 *  The naming of the key codes follow these rules:
 *   - The US keyboard layout is used
 *   - Names of printable alpha-numeric characters are used (e.g. "A", "R",
 *     "3", etc.)
 *   - For non-alphanumeric characters, Unicode:ish names are used (e.g.
 *     "COMMA", "LEFT_SQUARE_BRACKET", etc.). Note that some names do not
 *     correspond to the Unicode standard (usually for brevity)
 *   - Keys that lack a clear US mapping are named "WORLD_x"
 *   - For non-printable keys, custom names are used (e.g. "F4",
 *     "BACKSPACE", etc.)
 *
 *  @ingroup input
 *  @{
 */

/* The unknown key */
#define GLFW_KEY_UNKNOWN            -1

/* Printable keys */
#define GLFW_KEY_SPACE              32
#define GLFW_KEY_APOSTROPHE         39  /* ' */
#define GLFW_KEY_COMMA              44  /* , */
#define GLFW_KEY_MINUS              45  /* - */
#define GLFW_KEY_PERIOD             46  /* . */
#define GLFW_KEY_SLASH              47  /* / */
#define GLFW_KEY_0                  48
#define GLFW_KEY_1                  49
#define GLFW_KEY_2                  50
#define GLFW_KEY_3                  51
#define GLFW_KEY_4                  52
#define GLFW_KEY_5                  53
#define GLFW_KEY_6                  54
#define GLFW_KEY_7                  55
#define GLFW_KEY_8                  56
#define GLFW_KEY_9                  57
#define GLFW_KEY_SEMICOLON          59  /* ; */
#define GLFW_KEY_EQUAL              61  /* = */
#define GLFW_KEY_A                  65
#define GLFW_KEY_B                  66
#define GLFW_KEY_C                  67
#define GLFW_KEY_D                  68
#define GLFW_KEY_E                  69
#define GLFW_KEY_F                  70
#define GLFW_KEY_G                  71
#define GLFW_KEY_H                  72
#define GLFW_KEY_I                  73
#define GLFW_KEY_J                  74
#define GLFW_KEY_K                  75
#define GLFW_KEY_L                  76
#define GLFW_KEY_M                  77
#define GLFW_KEY_N                  78
#define GLFW_KEY_O                  79
#define GLFW_KEY_P                  80
#define GLFW_KEY_Q                  81
#define GLFW_KEY_R                  82
#define GLFW_KEY_S                  83
#define GLFW_KEY_T                  84
#define GLFW_KEY_U                  85
#define GLFW_KEY_V                  86
#define GLFW_KEY_W                  87
#define GLFW_KEY_X                  88
#define GLFW_KEY_Y                  89
#define GLFW_KEY_Z                  90
#define GLFW_KEY_LEFT_BRACKET       91  /* [ */
#define GLFW_KEY_BACKSLASH          92  /* \ */
#define GLFW_KEY_RIGHT_BRACKET      93  /* ] */
#define GLFW_KEY_GRAVE_ACCENT       96  /* ` */
#define GLFW_KEY_WORLD_1            161 /* non-US #1 */
#define GLFW_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define GLFW_KEY_ESCAPE             256
#define GLFW_KEY_ENTER              257
#define GLFW_KEY_TAB                258
#define GLFW_KEY_BACKSPACE          259
#define GLFW_KEY_INSERT             260
#define GLFW_KEY_DELETE             261
#define GLFW_KEY_RIGHT              262
#define GLFW_KEY_LEFT               263
#define GLFW_KEY_DOWN               264
#define GLFW_KEY_UP                 265
#define GLFW_KEY_PAGE_UP            266
#define GLFW_KEY_PAGE_DOWN          267
#define GLFW_KEY_HOME               268
#define GLFW_KEY_END                269
#define GLFW_KEY_CAPS_LOCK          280
#define GLFW_KEY_SCROLL_LOCK        281
#define GLFW_KEY_NUM_LOCK           282
#define GLFW_KEY_PRINT_SCREEN       283
#define GLFW_KEY_PAUSE              284
#define GLFW_KEY_F1                 290
#define GLFW_KEY_F2                 291
#define GLFW_KEY_F3                 292
#define GLFW_KEY_F4                 293
#define GLFW_KEY_F5                 294
#define GLFW_KEY_F6                 295
#define GLFW_KEY_F7                 296
#define GLFW_KEY_F8                 297
#define GLFW_KEY_F9                 298
#define GLFW_KEY_F10                299
#define GLFW_KEY_F11                300
#define GLFW_KEY_F12                301
#define GLFW_KEY_F13                302
#define GLFW_KEY_F14                303
#define GLFW_KEY_F15                304
#define GLFW_KEY_F16                305
#define GLFW_KEY_F17                306
#define GLFW_KEY_F18                307
#define GLFW_KEY_F19                308
#define GLFW_KEY_F20                309
#define GLFW_KEY_F21                310
#define GLFW_KEY_F22                311
#define GLFW_KEY_F23                312
#define GLFW_KEY_F24                313
#define GLFW_KEY_F25                314
#define GLFW_KEY_KP_0               320
#define GLFW_KEY_KP_1               321
#define GLFW_KEY_KP_2               322
#define GLFW_KEY_KP_3               323
#define GLFW_KEY_KP_4               324
#define GLFW_KEY_KP_5               325
#define GLFW_KEY_KP_6               326
#define GLFW_KEY_KP_7               327
#define GLFW_KEY_KP_8               328
#define GLFW_KEY_KP_9               329
#define GLFW_KEY_KP_DECIMAL         330
#define GLFW_KEY_KP_DIVIDE          331
#define GLFW_KEY_KP_MULTIPLY        332
#define GLFW_KEY_KP_SUBTRACT        333
#define GLFW_KEY_KP_ADD             334
#define GLFW_KEY_KP_ENTER           335
#define GLFW_KEY_KP_EQUAL           336
#define GLFW_KEY_LEFT_SHIFT         340
#define GLFW_KEY_LEFT_CONTROL       341
#define GLFW_KEY_LEFT_ALT           342
#define GLFW_KEY_LEFT_SUPER         343
#define GLFW_KEY_RIGHT_SHIFT        344
#define GLFW_KEY_RIGHT_CONTROL      345
#define GLFW_KEY_RIGHT_ALT          346
#define GLFW_KEY_RIGHT_SUPER        347
#define GLFW_KEY_MENU               348

#define GLFW_KEY_LAST               GLFW_KEY_MENU

/*! @} */

/*! @defgroup mods Modifier key flags
 *
 *  See [key input](@ref input_key) for how these are used.
 *
 *  @ingroup input
 *  @{ */

/*! @brief If this bit is set one or more Shift keys were held down.
 */
#define GLFW_MOD_SHIFT           0x0001
/*! @brief If this bit is set one or more Control keys were held down.
 */
#define GLFW_MOD_CONTROL         0x0002
/*! @brief If this bit is set one or more Alt keys were held down.
 */
#define GLFW_MOD_ALT             0x0004
/*! @brief If this bit is set one or more Super keys were held down.
 */
#define GLFW_MOD_SUPER           0x0008

/*! @} */

/*! @defgroup buttons Mouse buttons
 *
 *  See [mouse button input](@ref input_mouse_button) for how these are used.
 *
 *  @ingroup input
 *  @{ */
#define GLFW_MOUSE_BUTTON_1         0
#define GLFW_MOUSE_BUTTON_2         1
#define GLFW_MOUSE_BUTTON_3         2
#define GLFW_MOUSE_BUTTON_4         3
#define GLFW_MOUSE_BUTTON_5         4
#define GLFW_MOUSE_BUTTON_6         5
#define GLFW_MOUSE_BUTTON_7         6
#define GLFW_MOUSE_BUTTON_8         7
#define GLFW_MOUSE_BUTTON_LAST      GLFW_MOUSE_BUTTON_8
#define GLFW_MOUSE_BUTTON_LEFT      GLFW_MOUSE_BUTTON_1
#define GLFW_MOUSE_BUTTON_RIGHT     GLFW_MOUSE_BUTTON_2
#define GLFW_MOUSE_BUTTON_MIDDLE    GLFW_MOUSE_BUTTON_3
/*! @} */

/*! @defgroup joysticks Joysticks
 *
 *  See [joystick input](@ref joystick) for how these are used.
 *
 *  @ingroup input
 *  @{ */
#define GLFW_JOYSTICK_1             0
#define GLFW_JOYSTICK_2             1
#define GLFW_JOYSTICK_3             2
#define GLFW_JOYSTICK_4             3
#define GLFW_JOYSTICK_5             4
#define GLFW_JOYSTICK_6             5
#define GLFW_JOYSTICK_7             6
#define GLFW_JOYSTICK_8             7
#define GLFW_JOYSTICK_9             8
#define GLFW_JOYSTICK_10            9
#define GLFW_JOYSTICK_11            10
#define GLFW_JOYSTICK_12            11
#define GLFW_JOYSTICK_13            12
#define GLFW_JOYSTICK_14            13
#define GLFW_JOYSTICK_15            14
#define GLFW_JOYSTICK_16            15
#define GLFW_JOYSTICK_LAST          GLFW_JOYSTICK_16
/*! @} */

/*! @defgroup errors Error codes
 *
 *  See [error handling](@ref error_handling) for how these are used.
 *
 *  @ingroup init
 *  @{ */
/*! @brief GLFW has not been initialized.
 *
 *  This occurs if a GLFW function was called that must not be called unless the
 *  library is [initialized](@ref intro_init).
 *
 *  @analysis Application programmer error.  Initialize GLFW before calling any
 *  function that requires initialization.
 */
#define GLFW_NOT_INITIALIZED        0x00010001
/*! @brief No context is current for this thread.
 *
 *  This occurs if a GLFW function was called that needs and operates on the
 *  current OpenGL or OpenGL ES context but no context is current on the calling
 *  thread.  One such function is @ref glfwSwapInterval.
 *
 *  @analysis Application programmer error.  Ensure a context is current before
 *  calling functions that require a current context.
 */
#define GLFW_NO_CURRENT_CONTEXT     0x00010002
/*! @brief One of the arguments to the function was an invalid enum value.
 *
 *  One of the arguments to the function was an invalid enum value, for example
 *  requesting @ref GLFW_RED_BITS with @ref glfwGetWindowAttrib.
 *
 *  @analysis Application programmer error.  Fix the offending call.
 */
#define GLFW_INVALID_ENUM           0x00010003
/*! @brief One of the arguments to the function was an invalid value.
 *
 *  One of the arguments to the function was an invalid value, for example
 *  requesting a non-existent OpenGL or OpenGL ES version like 2.7.
 *
 *  Requesting a valid but unavailable OpenGL or OpenGL ES version will instead
 *  result in a @ref GLFW_VERSION_UNAVAILABLE error.
 *
 *  @analysis Application programmer error.  Fix the offending call.
 */
#define GLFW_INVALID_VALUE          0x00010004
/*! @brief A memory allocation failed.
 *
 *  A memory allocation failed.
 *
 *  @analysis A bug in GLFW or the underlying operating system.  Report the bug
 *  to our [issue tracker](https://github.com/glfw/glfw/issues).
 */
#define GLFW_OUT_OF_MEMORY          0x00010005
/*! @brief GLFW could not find support for the requested API on the system.
 *
 *  GLFW could not find support for the requested API on the system.
 *
 *  @analysis The installed graphics driver does not support the requested
 *  API, or does not support it via the chosen context creation backend.
 *  Below are a few examples.
 *
 *  @par
 *  Some pre-installed Windows graphics drivers do not support OpenGL.  AMD only
 *  supports OpenGL ES via EGL, while Nvidia and Intel only support it via
 *  a WGL or GLX extension.  macOS does not provide OpenGL ES at all.  The Mesa
 *  EGL, OpenGL and OpenGL ES libraries do not interface with the Nvidia binary
 *  driver.  Older graphics drivers do not support Vulkan.
 */
#define GLFW_API_UNAVAILABLE        0x00010006
/*! @brief The requested OpenGL or OpenGL ES version is not available.
 *
 *  The requested OpenGL or OpenGL ES version (including any requested context
 *  or framebuffer hints) is not available on this machine.
 *
 *  @analysis The machine does not support your requirements.  If your
 *  application is sufficiently flexible, downgrade your requirements and try
 *  again.  Otherwise, inform the user that their machine does not match your
 *  requirements.
 *
 *  @par
 *  Future invalid OpenGL and OpenGL ES versions, for example OpenGL 4.8 if 5.0
 *  comes out before the 4.x series gets that far, also fail with this error and
 *  not @ref GLFW_INVALID_VALUE, because GLFW cannot know what future versions
 *  will exist.
 */
#define GLFW_VERSION_UNAVAILABLE    0x00010007
/*! @brief A platform-specific error occurred that does not match any of the
 *  more specific categories.
 *
 *  A platform-specific error occurred that does not match any of the more
 *  specific categories.
 *
 *  @analysis A bug or configuration error in GLFW, the underlying operating
 *  system or its drivers, or a lack of required resources.  Report the issue to
 *  our [issue tracker](https://github.com/glfw/glfw/issues).
 */
#define GLFW_PLATFORM_ERROR         0x00010008
/*! @brief The requested format is not supported or available.
 *
 *  If emitted during window creation, the requested pixel format is not
 *  supported.
 *
 *  If emitted when querying the clipboard, the contents of the clipboard could
 *  not be converted to the requested format.
 *
 *  @analysis If emitted during window creation, one or more
 *  [hard constraints](@ref window_hints_hard) did not match any of the
 *  available pixel formats.  If your application is sufficiently flexible,
 *  downgrade your requirements and try again.  Otherwise, inform the user that
 *  their machine does not match your requirements.
 *
 *  @par
 *  If emitted when querying the clipboard, ignore the error or report it to
 *  the user, as appropriate.
 */
#define GLFW_FORMAT_UNAVAILABLE     0x00010009
/*! @brief The specified window does not have an OpenGL or OpenGL ES context.
 *
 *  A window that does not have an OpenGL or OpenGL ES context was passed to
 *  a function that requires it to have one.
 *
 *  @analysis Application programmer error.  Fix the offending call.
 */
#define GLFW_NO_WINDOW_CONTEXT      0x0001000A
/*! @} */

/*! @addtogroup window
 *  @{ */
/*! @brief Input focus window hint and attribute
 *
 *  Input focus [window hint](@ref GLFW_FOCUSED_hint) or
 *  [window attribute](@ref GLFW_FOCUSED_attrib).
 */
#define GLFW_FOCUSED                0x00020001
/*! @brief Window iconification window attribute
 *
 *  Window iconification [window attribute](@ref GLFW_ICONIFIED_attrib).
 */
#define GLFW_ICONIFIED              0x00020002
/*! @brief Window resize-ability window hint and attribute
 *
 *  Window resize-ability [window hint](@ref GLFW_RESIZABLE_hint) and
 *  [window attribute](@ref GLFW_RESIZABLE_attrib).
 */
#define GLFW_RESIZABLE              0x00020003
/*! @brief Window visibility window hint and attribute
 *
 *  Window visibility [window hint](@ref GLFW_VISIBLE_hint) and
 *  [window attribute](@ref GLFW_VISIBLE_attrib).
 */
#define GLFW_VISIBLE                0x00020004
/*! @brief Window decoration window hint and attribute
 *
 *  Window decoration [window hint](@ref GLFW_DECORATED_hint) and
 *  [window attribute](@ref GLFW_DECORATED_attrib).
 */
#define GLFW_DECORATED              0x00020005
/*! @brief Window auto-iconification window hint and attribute
 *
 *  Window auto-iconification [window hint](@ref GLFW_AUTO_ICONIFY_hint) and
 *  [window attribute](@ref GLFW_AUTO_ICONIFY_attrib).
 */
#define GLFW_AUTO_ICONIFY           0x00020006
/*! @brief Window decoration window hint and attribute
 *
 *  Window decoration [window hint](@ref GLFW_FLOATING_hint) and
 *  [window attribute](@ref GLFW_FLOATING_attrib).
 */
#define GLFW_FLOATING               0x00020007
/*! @brief Window maximization window hint and attribute
 *
 *  Window maximization [window hint](@ref GLFW_MAXIMIZED_hint) and
 *  [window attribute](@ref GLFW_MAXIMIZED_attrib).
 */
#define GLFW_MAXIMIZED              0x00020008

/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_RED_BITS).
 */
#define GLFW_RED_BITS               0x00021001
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_GREEN_BITS).
 */
#define GLFW_GREEN_BITS             0x00021002
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_BLUE_BITS).
 */
#define GLFW_BLUE_BITS              0x00021003
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ALPHA_BITS).
 */
#define GLFW_ALPHA_BITS             0x00021004
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_DEPTH_BITS).
 */
#define GLFW_DEPTH_BITS             0x00021005
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_STENCIL_BITS).
 */
#define GLFW_STENCIL_BITS           0x00021006
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ACCUM_RED_BITS).
 */
#define GLFW_ACCUM_RED_BITS         0x00021007
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ACCUM_GREEN_BITS).
 */
#define GLFW_ACCUM_GREEN_BITS       0x00021008
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ACCUM_BLUE_BITS).
 */
#define GLFW_ACCUM_BLUE_BITS        0x00021009
/*! @brief Framebuffer bit depth hint.
 *
 *  Framebuffer bit depth [hint](@ref GLFW_ACCUM_ALPHA_BITS).
 */
#define GLFW_ACCUM_ALPHA_BITS       0x0002100A
/*! @brief Framebuffer auxiliary buffer hint.
 *
 *  Framebuffer auxiliary buffer [hint](@ref GLFW_AUX_BUFFERS).
 */
#define GLFW_AUX_BUFFERS            0x0002100B
/*! @brief OpenGL stereoscopic rendering hint.
 *
 *  OpenGL stereoscopic rendering [hint](@ref GLFW_STEREO).
 */
#define GLFW_STEREO                 0x0002100C
/*! @brief Framebuffer MSAA samples hint.
 *
 *  Framebuffer MSAA samples [hint](@ref GLFW_SAMPLES).
 */
#define GLFW_SAMPLES                0x0002100D
/*! @brief Framebuffer sRGB hint.
 *
 *  Framebuffer sRGB [hint](@ref GLFW_SRGB_CAPABLE).
 */
#define GLFW_SRGB_CAPABLE           0x0002100E
/*! @brief Monitor refresh rate hint.
 *
 *  Monitor refresh rate [hint](@ref GLFW_REFRESH_RATE).
 */
#define GLFW_REFRESH_RATE           0x0002100F
/*! @brief Framebuffer double buffering hint.
 *
 *  Framebuffer double buffering [hint](@ref GLFW_DOUBLEBUFFER).
 */
#define GLFW_DOUBLEBUFFER           0x00021010
/*! @brief Context client API hint and attribute.
 *
 *  Context client API [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_CLIENT_API             0x00022001
/*! @brief Context client API major version hint and attribute.
 *
 *  Context client API major version [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_CONTEXT_VERSION_MAJOR  0x00022002
/*! @brief Context client API minor version hint and attribute.
 *
 *  Context client API minor version [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_CONTEXT_VERSION_MINOR  0x00022003
/*! @brief Context client API revision number hint and attribute.
 *
 *  Context client API revision number [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_CONTEXT_REVISION       0x00022004
/*! @brief Context robustness hint and attribute.
 *
 *  Context client API revision number [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_CONTEXT_ROBUSTNESS     0x00022005
/*! @brief OpenGL forward-compatibility hint and attribute.
 *
 *  OpenGL forward-compatibility [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_OPENGL_FORWARD_COMPAT  0x00022006
/*! @brief OpenGL debug context hint and attribute.
 *
 *  OpenGL debug context [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_OPENGL_DEBUG_CONTEXT   0x00022007
/*! @brief OpenGL profile hint and attribute.
 *
 *  OpenGL profile [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_OPENGL_PROFILE         0x00022008
/*! @brief Context flush-on-release hint and attribute.
 *
 *  Context flush-on-release [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_CONTEXT_RELEASE_BEHAVIOR 0x00022009
/*! @brief Context error suppression hint and attribute.
 *
 *  Context error suppression [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_CONTEXT_NO_ERROR       0x0002200A
/*! @brief Context creation API hint and attribute.
 *
 *  Context creation API [hint](@ref GLFW_CLIENT_API_hint) and
 *  [attribute](@ref GLFW_CLIENT_API_attrib).
 */
#define GLFW_CONTEXT_CREATION_API   0x0002200B

#define GLFW_COCOA_RETINA_FRAMEBUFFER 0x00023001
#define GLFW_COCOA_FRAME_AUTOSAVE     0x00023002
/*! @} */

#define GLFW_NO_API                          0
#define GLFW_OPENGL_API             0x00030001
#define GLFW_OPENGL_ES_API          0x00030002

#define GLFW_NO_ROBUSTNESS                   0
#define GLFW_NO_RESET_NOTIFICATION  0x00031001
#define GLFW_LOSE_CONTEXT_ON_RESET  0x00031002

#define GLFW_OPENGL_ANY_PROFILE              0
#define GLFW_OPENGL_CORE_PROFILE    0x00032001
#define GLFW_OPENGL_COMPAT_PROFILE  0x00032002

#define GLFW_CURSOR                 0x00033001
#define GLFW_STICKY_KEYS            0x00033002
#define GLFW_STICKY_MOUSE_BUTTONS   0x00033003

#define GLFW_CURSOR_NORMAL          0x00034001
#define GLFW_CURSOR_HIDDEN          0x00034002
#define GLFW_CURSOR_DISABLED        0x00034003

#define GLFW_ANY_RELEASE_BEHAVIOR            0
#define GLFW_RELEASE_BEHAVIOR_FLUSH 0x00035001
#define GLFW_RELEASE_BEHAVIOR_NONE  0x00035002

#define GLFW_NATIVE_CONTEXT_API     0x00036001
#define GLFW_EGL_CONTEXT_API        0x00036002

/*! @defgroup shapes Standard cursor shapes
 *
 *  See [standard cursor creation](@ref cursor_standard) for how these are used.
 *
 *  @ingroup input
 *  @{ */

/*! @brief The regular arrow cursor shape.
 *
 *  The regular arrow cursor.
 */
#define GLFW_ARROW_CURSOR           0x00036001
/*! @brief The text input I-beam cursor shape.
 *
 *  The text input I-beam cursor shape.
 */
#define GLFW_IBEAM_CURSOR           0x00036002
/*! @brief The crosshair shape.
 *
 *  The crosshair shape.
 */
#define GLFW_CROSSHAIR_CURSOR       0x00036003
/*! @brief The hand shape.
 *
 *  The hand shape.
 */
#define GLFW_HAND_CURSOR            0x00036004
/*! @brief The horizontal resize arrow shape.
 *
 *  The horizontal resize arrow shape.
 */
#define GLFW_HRESIZE_CURSOR         0x00036005
/*! @brief The vertical resize arrow shape.
 *
 *  The vertical resize arrow shape.
 */
#define GLFW_VRESIZE_CURSOR         0x00036006
/*! @} */

#define GLFW_CONNECTED              0x00040001
#define GLFW_DISCONNECTED           0x00040002

#define GLFW_DONT_CARE              -1


/*************************************************************************
 * GLFW API types
 *************************************************************************/

/*! @brief Client API function pointer type.
 *
 *  Generic function pointer used for returning client API function pointers
 *  without forcing a cast from a regular pointer.
 *
 *  @sa @ref context_glext
 *  @sa @ref glfwGetProcAddress
 *
 *  @since Added in version 3.0.
 
 *  @ingroup context
 */
typedef void (*GLFWglproc)(void);

/*! @brief Vulkan API function pointer type.
 *
 *  Generic function pointer used for returning Vulkan API function pointers
 *  without forcing a cast from a regular pointer.
 *
 *  @sa @ref vulkan_proc
 *  @sa @ref glfwGetInstanceProcAddress
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup vulkan
 */
typedef void (*GLFWvkproc)(void);

/*! @brief Opaque monitor object.
 *
 *  Opaque monitor object.
 *
 *  @see @ref monitor_object
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
typedef struct GLFWmonitor GLFWmonitor;

/*! @brief Opaque window object.
 *
 *  Opaque window object.
 *
 *  @see @ref window_object
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
typedef struct GLFWwindow GLFWwindow;

/*! @brief Opaque cursor object.
 *
 *  Opaque cursor object.
 *
 *  @see @ref cursor_object
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup cursor
 */
typedef struct GLFWcursor GLFWcursor;

/*! @brief The function signature for error callbacks.
 *
 *  This is the function signature for error callback functions.
 *
 *  @param[in] error An [error code](@ref errors).
 *  @param[in] description A UTF-8 encoded string describing the error.
 *
 *  @sa @ref error_handling
 *  @sa @ref glfwSetErrorCallback
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup init
 */
typedef void (* GLFWerrorfun)(int,const char*);

/*! @brief The function signature for window position callbacks.
 *
 *  This is the function signature for window position callback functions.
 *
 *  @param[in] window The window that was moved.
 *  @param[in] xpos The new x-coordinate, in screen coordinates, of the
 *  upper-left corner of the client area of the window.
 *  @param[in] ypos The new y-coordinate, in screen coordinates, of the
 *  upper-left corner of the client area of the window.
 *
 *  @sa @ref window_pos
 *  @sa @ref glfwSetWindowPosCallback
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
typedef void (* GLFWwindowposfun)(GLFWwindow*,int,int);

/*! @brief The function signature for window resize callbacks.
 *
 *  This is the function signature for window size callback functions.
 *
 *  @param[in] window The window that was resized.
 *  @param[in] width The new width, in screen coordinates, of the window.
 *  @param[in] height The new height, in screen coordinates, of the window.
 *
 *  @sa @ref window_size
 *  @sa @ref glfwSetWindowSizeCallback
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
typedef void (* GLFWwindowsizefun)(GLFWwindow*,int,int);

/*! @brief The function signature for window close callbacks.
 *
 *  This is the function signature for window close callback functions.
 *
 *  @param[in] window The window that the user attempted to close.
 *
 *  @sa @ref window_close
 *  @sa @ref glfwSetWindowCloseCallback
 *
 *  @since Added in version 2.5.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
typedef void (* GLFWwindowclosefun)(GLFWwindow*);

/*! @brief The function signature for window content refresh callbacks.
 *
 *  This is the function signature for window refresh callback functions.
 *
 *  @param[in] window The window whose content needs to be refreshed.
 *
 *  @sa @ref window_refresh
 *  @sa @ref glfwSetWindowRefreshCallback
 *
 *  @since Added in version 2.5.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
typedef void (* GLFWwindowrefreshfun)(GLFWwindow*);

/*! @brief The function signature for window focus/defocus callbacks.
 *
 *  This is the function signature for window focus callback functions.
 *
 *  @param[in] window The window that gained or lost input focus.
 *  @param[in] focused `GLFW_TRUE` if the window was given input focus, or
 *  `GLFW_FALSE` if it lost it.
 *
 *  @sa @ref window_focus
 *  @sa @ref glfwSetWindowFocusCallback
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
typedef void (* GLFWwindowfocusfun)(GLFWwindow*,int);

/*! @brief The function signature for window iconify/restore callbacks.
 *
 *  This is the function signature for window iconify/restore callback
 *  functions.
 *
 *  @param[in] window The window that was iconified or restored.
 *  @param[in] iconified `GLFW_TRUE` if the window was iconified, or
 *  `GLFW_FALSE` if it was restored.
 *
 *  @sa @ref window_iconify
 *  @sa @ref glfwSetWindowIconifyCallback
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
typedef void (* GLFWwindowiconifyfun)(GLFWwindow*,int);

/*! @brief The function signature for window maximize/restore callbacks.
 *
 *  This is the function signature for window maximize/restore callback
 *  functions.
 *
 *  @param[in] window The window that was maximized or restored.
 *  @param[in] iconified `GLFW_TRUE` if the window was maximized, or
 *  `GLFW_FALSE` if it was restored.
 *
 *  @sa @ref window_maximize
 *  @sa glfwSetWindowMaximizeCallback
 *
 *  @since Added in version 3.3.
 *
 *  @ingroup window
 */
typedef void (* GLFWwindowmaximizefun)(GLFWwindow*,int);

/*! @brief The function signature for framebuffer resize callbacks.
 *
 *  This is the function signature for framebuffer resize callback
 *  functions.
 *
 *  @param[in] window The window whose framebuffer was resized.
 *  @param[in] width The new width, in pixels, of the framebuffer.
 *  @param[in] height The new height, in pixels, of the framebuffer.
 *
 *  @sa @ref window_fbsize
 *  @sa @ref glfwSetFramebufferSizeCallback
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
typedef void (* GLFWframebuffersizefun)(GLFWwindow*,int,int);

/*! @brief The function signature for mouse button callbacks.
 *
 *  This is the function signature for mouse button callback functions.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] button The [mouse button](@ref buttons) that was pressed or
 *  released.
 *  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.
 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
 *  held down.
 *
 *  @sa @ref input_mouse_button
 *  @sa @ref glfwSetMouseButtonCallback
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle and modifier mask parameters.
 *
 *  @ingroup input
 */
typedef void (* GLFWmousebuttonfun)(GLFWwindow*,int,int,int);

/*! @brief The function signature for cursor position callbacks.
 *
 *  This is the function signature for cursor position callback functions.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] xpos The new cursor x-coordinate, relative to the left edge of
 *  the client area.
 *  @param[in] ypos The new cursor y-coordinate, relative to the top edge of the
 *  client area.
 *
 *  @sa @ref cursor_pos
 *  @sa @ref glfwSetCursorPosCallback
 *
 *  @since Added in version 3.0.  Replaces `GLFWmouseposfun`.
 *
 *  @ingroup input
 */
typedef void (* GLFWcursorposfun)(GLFWwindow*,double,double);

/*! @brief The function signature for cursor enter/leave callbacks.
 *
 *  This is the function signature for cursor enter/leave callback functions.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] entered `GLFW_TRUE` if the cursor entered the window's client
 *  area, or `GLFW_FALSE` if it left it.
 *
 *  @sa @ref cursor_enter
 *  @sa @ref glfwSetCursorEnterCallback
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup input
 */
typedef void (* GLFWcursorenterfun)(GLFWwindow*,int);

/*! @brief The function signature for scroll callbacks.
 *
 *  This is the function signature for scroll callback functions.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] xoffset The scroll offset along the x-axis.
 *  @param[in] yoffset The scroll offset along the y-axis.
 *
 *  @sa @ref scrolling
 *  @sa @ref glfwSetScrollCallback
 *
 *  @since Added in version 3.0.  Replaces `GLFWmousewheelfun`.
 *
 *  @ingroup input
 */
typedef void (* GLFWscrollfun)(GLFWwindow*,double,double);

/*! @brief The function signature for keyboard key callbacks.
 *
 *  This is the function signature for keyboard key callback functions.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] key The [keyboard key](@ref keys) that was pressed or released.
 *  @param[in] scancode The system-specific scancode of the key.
 *  @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.
 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
 *  held down.
 *
 *  @sa @ref input_key
 *  @sa @ref glfwSetKeyCallback
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle, scancode and modifier mask parameters.
 *
 *  @ingroup input
 */
typedef void (* GLFWkeyfun)(GLFWwindow*,int,int,int,int);

/*! @brief The function signature for Unicode character callbacks.
 *
 *  This is the function signature for Unicode character callback functions.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] codepoint The Unicode code point of the character.
 *
 *  @sa @ref input_char
 *  @sa @ref glfwSetCharCallback
 *
 *  @since Added in version 2.4.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup input
 */
typedef void (* GLFWcharfun)(GLFWwindow*,unsigned int);

/*! @brief The function signature for Unicode character with modifiers
 *  callbacks.
 *
 *  This is the function signature for Unicode character with modifiers callback
 *  functions.  It is called for each input character, regardless of what
 *  modifier keys are held down.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] codepoint The Unicode code point of the character.
 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
 *  held down.
 *
 *  @sa @ref input_char
 *  @sa @ref glfwSetCharModsCallback
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup input
 */
typedef void (* GLFWcharmodsfun)(GLFWwindow*,unsigned int,int);

/*! @brief The function signature for file drop callbacks.
 *
 *  This is the function signature for file drop callbacks.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] count The number of dropped files.
 *  @param[in] paths The UTF-8 encoded file and/or directory path names.
 *
 *  @sa @ref path_drop
 *  @sa @ref glfwSetDropCallback
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup input
 */
typedef void (* GLFWdropfun)(GLFWwindow*,int,const char**);

/*! @brief The function signature for monitor configuration callbacks.
 *
 *  This is the function signature for monitor configuration callback functions.
 *
 *  @param[in] monitor The monitor that was connected or disconnected.
 *  @param[in] event One of `GLFW_CONNECTED` or `GLFW_DISCONNECTED`.
 *
 *  @sa @ref monitor_event
 *  @sa @ref glfwSetMonitorCallback
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
typedef void (* GLFWmonitorfun)(GLFWmonitor*,int);

/*! @brief The function signature for joystick configuration callbacks.
 *
 *  This is the function signature for joystick configuration callback
 *  functions.
 *
 *  @param[in] jid The joystick that was connected or disconnected.
 *  @param[in] event One of `GLFW_CONNECTED` or `GLFW_DISCONNECTED`.
 *
 *  @sa @ref joystick_event
 *  @sa @ref glfwSetJoystickCallback
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup input
 */
typedef void (* GLFWjoystickfun)(int,int);

/*! @brief Video mode type.
 *
 *  This describes a single video mode.
 *
 *  @sa @ref monitor_modes
 *  @sa @ref glfwGetVideoMode
 *  @sa @ref glfwGetVideoModes
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added refresh rate member.
 *
 *  @ingroup monitor
 */
typedef struct GLFWvidmode
{
    /*! The width, in screen coordinates, of the video mode.
     */
    int width;
    /*! The height, in screen coordinates, of the video mode.
     */
    int height;
    /*! The bit depth of the red channel of the video mode.
     */
    int redBits;
    /*! The bit depth of the green channel of the video mode.
     */
    int greenBits;
    /*! The bit depth of the blue channel of the video mode.
     */
    int blueBits;
    /*! The refresh rate, in Hz, of the video mode.
     */
    int refreshRate;
} GLFWvidmode;

/*! @brief Gamma ramp.
 *
 *  This describes the gamma ramp for a monitor.
 *
 *  @sa @ref monitor_gamma
 *  @sa @ref glfwGetGammaRamp
 *  @sa @ref glfwSetGammaRamp
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
typedef struct GLFWgammaramp
{
    /*! An array of value describing the response of the red channel.
     */
    unsigned short* red;
    /*! An array of value describing the response of the green channel.
     */
    unsigned short* green;
    /*! An array of value describing the response of the blue channel.
     */
    unsigned short* blue;
    /*! The number of elements in each array.
     */
    unsigned int size;
} GLFWgammaramp;

/*! @brief Image data.
 *
 *  @sa @ref cursor_custom
 *  @sa @ref window_icon
 *
 *  @since Added in version 2.1.
 *  @glfw3 Removed format and bytes-per-pixel members.
 */
typedef struct GLFWimage
{
    /*! The width, in pixels, of this image.
     */
    int width;
    /*! The height, in pixels, of this image.
     */
    int height;
    /*! The pixel data of this image, arranged left-to-right, top-to-bottom.
     */
    unsigned char* pixels;
} GLFWimage;


/*************************************************************************
 * GLFW API functions
 *************************************************************************/

/*! @brief Initializes the GLFW library.
 *
 *  This function initializes the GLFW library.  Before most GLFW functions can
 *  be used, GLFW must be initialized, and before an application terminates GLFW
 *  should be terminated in order to free any resources allocated during or
 *  after initialization.
 *
 *  If this function fails, it calls @ref glfwTerminate before returning.  If it
 *  succeeds, you should call @ref glfwTerminate before the application exits.
 *
 *  Additional calls to this function after successful initialization but before
 *  termination will return `GLFW_TRUE` immediately.
 *
 *  @return `GLFW_TRUE` if successful, or `GLFW_FALSE` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark @macos This function will change the current directory of the
 *  application to the `Contents/Resources` subdirectory of the application's
 *  bundle, if present.  This can be disabled with a
 *  [compile-time option](@ref compile_options_osx).
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref intro_init
 *  @sa @ref glfwTerminate
 *
 *  @since Added in version 1.0.
 *
 *  @ingroup init
 */
GLFWAPI int glfwInit(void);

/*! @brief Terminates the GLFW library.
 *
 *  This function destroys all remaining windows and cursors, restores any
 *  modified gamma ramps and frees any other allocated resources.  Once this
 *  function is called, you must again call @ref glfwInit successfully before
 *  you will be able to use most GLFW functions.
 *
 *  If GLFW has been successfully initialized, this function should be called
 *  before the application exits.  If initialization fails, there is no need to
 *  call this function, as it is called by @ref glfwInit before it returns
 *  failure.
 *
 *  @errors Possible errors include @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark This function may be called before @ref glfwInit.
 *
 *  @warning The contexts of any remaining windows must not be current on any
 *  other thread when this function is called.
 *
 *  @reentrancy This function must not be called from a callback.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref intro_init
 *  @sa @ref glfwInit
 *
 *  @since Added in version 1.0.
 *
 *  @ingroup init
 */
GLFWAPI void glfwTerminate(void);

/*! @brief Retrieves the version of the GLFW library.
 *
 *  This function retrieves the major, minor and revision numbers of the GLFW
 *  library.  It is intended for when you are using GLFW as a shared library and
 *  want to ensure that you are using the minimum required version.
 *
 *  Any or all of the version arguments may be `NULL`.
 *
 *  @param[out] major Where to store the major version number, or `NULL`.
 *  @param[out] minor Where to store the minor version number, or `NULL`.
 *  @param[out] rev Where to store the revision number, or `NULL`.
 *
 *  @errors None.
 *
 *  @remark This function may be called before @ref glfwInit.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref intro_version
 *  @sa @ref glfwGetVersionString
 *
 *  @since Added in version 1.0.
 *
 *  @ingroup init
 */
GLFWAPI void glfwGetVersion(int* major, int* minor, int* rev);

/*! @brief Returns a string describing the compile-time configuration.
 *
 *  This function returns the compile-time generated
 *  [version string](@ref intro_version_string) of the GLFW library binary.  It
 *  describes the version, platform, compiler and any platform-specific
 *  compile-time options.  It should not be confused with the OpenGL or OpenGL
 *  ES version string, queried with `glGetString`.
 *
 *  __Do not use the version string__ to parse the GLFW library version.  The
 *  @ref glfwGetVersion function provides the version of the running library
 *  binary in numerical format.
 *
 *  @return The ASCII encoded GLFW version string.
 *
 *  @errors None.
 *
 *  @remark This function may be called before @ref glfwInit.
 *
 *  @pointer_lifetime The returned string is static and compile-time generated.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref intro_version
 *  @sa @ref glfwGetVersion
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup init
 */
GLFWAPI const char* glfwGetVersionString(void);

/*! @brief Sets the error callback.
 *
 *  This function sets the error callback, which is called with an error code
 *  and a human-readable description each time a GLFW error occurs.
 *
 *  The error callback is called on the thread where the error occurred.  If you
 *  are using GLFW from multiple threads, your error callback needs to be
 *  written accordingly.
 *
 *  Because the description string may have been generated specifically for that
 *  error, it is not guaranteed to be valid after the callback has returned.  If
 *  you wish to use it after the callback returns, you need to make a copy.
 *
 *  Once set, the error callback remains set even after the library has been
 *  terminated.
 *
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set.
 *
 *  @errors None.
 *
 *  @remark This function may be called before @ref glfwInit.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref error_handling
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup init
 */
GLFWAPI GLFWerrorfun glfwSetErrorCallback(GLFWerrorfun cbfun);

/*! @brief Returns the currently connected monitors.
 *
 *  This function returns an array of handles for all currently connected
 *  monitors.  The primary monitor is always first in the returned array.  If no
 *  monitors were found, this function returns `NULL`.
 *
 *  @param[out] count Where to store the number of monitors in the returned
 *  array.  This is set to zero if an error occurred.
 *  @return An array of monitor handles, or `NULL` if no monitors were found or
 *  if an [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @pointer_lifetime The returned array is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is guaranteed to be valid only until the
 *  monitor configuration changes or the library is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_monitors
 *  @sa @ref monitor_event
 *  @sa @ref glfwGetPrimaryMonitor
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
GLFWAPI GLFWmonitor** glfwGetMonitors(int* count);

/*! @brief Returns the primary monitor.
 *
 *  This function returns the primary monitor.  This is usually the monitor
 *  where elements like the task bar or global menu bar are located.
 *
 *  @return The primary monitor, or `NULL` if no monitors were found or if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @remark The primary monitor is always first in the array returned by @ref
 *  glfwGetMonitors.
 *
 *  @sa @ref monitor_monitors
 *  @sa @ref glfwGetMonitors
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
GLFWAPI GLFWmonitor* glfwGetPrimaryMonitor(void);

/*! @brief Returns the position of the monitor's viewport on the virtual screen.
 *
 *  This function returns the position, in screen coordinates, of the upper-left
 *  corner of the specified monitor.
 *
 *  Any or all of the position arguments may be `NULL`.  If an error occurs, all
 *  non-`NULL` position arguments will be set to zero.
 *
 *  @param[in] monitor The monitor to query.
 *  @param[out] xpos Where to store the monitor x-coordinate, or `NULL`.
 *  @param[out] ypos Where to store the monitor y-coordinate, or `NULL`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_properties
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
GLFWAPI void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos);

/*! @brief Returns the physical size of the monitor.
 *
 *  This function returns the size, in millimetres, of the display area of the
 *  specified monitor.
 *
 *  Some systems do not provide accurate monitor size information, either
 *  because the monitor
 *  [EDID](https://en.wikipedia.org/wiki/Extended_display_identification_data)
 *  data is incorrect or because the driver does not report it accurately.
 *
 *  Any or all of the size arguments may be `NULL`.  If an error occurs, all
 *  non-`NULL` size arguments will be set to zero.
 *
 *  @param[in] monitor The monitor to query.
 *  @param[out] widthMM Where to store the width, in millimetres, of the
 *  monitor's display area, or `NULL`.
 *  @param[out] heightMM Where to store the height, in millimetres, of the
 *  monitor's display area, or `NULL`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @remark @win32 calculates the returned physical size from the
 *  current resolution and system DPI instead of querying the monitor EDID data.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_properties
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
GLFWAPI void glfwGetMonitorPhysicalSize(GLFWmonitor* monitor, int* widthMM, int* heightMM);

/*! @brief Returns the name of the specified monitor.
 *
 *  This function returns a human-readable name, encoded as UTF-8, of the
 *  specified monitor.  The name typically reflects the make and model of the
 *  monitor and is not guaranteed to be unique among the connected monitors.
 *
 *  @param[in] monitor The monitor to query.
 *  @return The UTF-8 encoded name of the monitor, or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @pointer_lifetime The returned string is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is valid until the specified monitor is
 *  disconnected or the library is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_properties
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
GLFWAPI const char* glfwGetMonitorName(GLFWmonitor* monitor);

/*! @brief Sets the monitor configuration callback.
 *
 *  This function sets the monitor configuration callback, or removes the
 *  currently set callback.  This is called when a monitor is connected to or
 *  disconnected from the system.
 *
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_event
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
GLFWAPI GLFWmonitorfun glfwSetMonitorCallback(GLFWmonitorfun cbfun);

/*! @brief Returns the available video modes for the specified monitor.
 *
 *  This function returns an array of all video modes supported by the specified
 *  monitor.  The returned array is sorted in ascending order, first by color
 *  bit depth (the sum of all channel depths) and then by resolution area (the
 *  product of width and height).
 *
 *  @param[in] monitor The monitor to query.
 *  @param[out] count Where to store the number of video modes in the returned
 *  array.  This is set to zero if an error occurred.
 *  @return An array of video modes, or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The returned array is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is valid until the specified monitor is
 *  disconnected, this function is called again for that monitor or the library
 *  is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_modes
 *  @sa @ref glfwGetVideoMode
 *
 *  @since Added in version 1.0.
 *  @glfw3 Changed to return an array of modes for a specific monitor.
 *
 *  @ingroup monitor
 */
GLFWAPI const GLFWvidmode* glfwGetVideoModes(GLFWmonitor* monitor, int* count);

/*! @brief Returns the current mode of the specified monitor.
 *
 *  This function returns the current video mode of the specified monitor.  If
 *  you have created a full screen window for that monitor, the return value
 *  will depend on whether that window is iconified.
 *
 *  @param[in] monitor The monitor to query.
 *  @return The current mode of the monitor, or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The returned array is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is valid until the specified monitor is
 *  disconnected or the library is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_modes
 *  @sa @ref glfwGetVideoModes
 *
 *  @since Added in version 3.0.  Replaces `glfwGetDesktopMode`.
 *
 *  @ingroup monitor
 */
GLFWAPI const GLFWvidmode* glfwGetVideoMode(GLFWmonitor* monitor);

/*! @brief Generates a gamma ramp and sets it for the specified monitor.
 *
 *  This function generates a 256-element gamma ramp from the specified exponent
 *  and then calls @ref glfwSetGammaRamp with it.  The value must be a finite
 *  number greater than zero.
 *
 *  The software controlled gamma ramp is applied _in addition_ to the hardware
 *  gamma correction, which today is usually an approximation of sRGB gamma.
 *  This means that setting a perfectly linear ramp, or gamma 1.0, will produce
 *  the default (usually sRGB-like) behavior.
 *
 *  For gamma correct rendering with OpenGL or OpenGL ES, see the @ref
 *  GLFW_SRGB_CAPABLE hint.
 *
 *  @param[in] monitor The monitor whose gamma ramp to set.
 *  @param[in] gamma The desired exponent.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_VALUE and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland Gamma handling is currently unavailable, this function will
 *  always emit @ref GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_gamma
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
GLFWAPI void glfwSetGamma(GLFWmonitor* monitor, float gamma);

/*! @brief Returns the current gamma ramp for the specified monitor.
 *
 *  This function returns the current gamma ramp of the specified monitor.
 *
 *  @param[in] monitor The monitor to query.
 *  @return The current gamma ramp, or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland Gamma handling is currently unavailable, this function will
 *  always return `NULL` and emit @ref GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The returned structure and its arrays are allocated and
 *  freed by GLFW.  You should not free them yourself.  They are valid until the
 *  specified monitor is disconnected, this function is called again for that
 *  monitor or the library is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_gamma
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
GLFWAPI const GLFWgammaramp* glfwGetGammaRamp(GLFWmonitor* monitor);

/*! @brief Sets the current gamma ramp for the specified monitor.
 *
 *  This function sets the current gamma ramp for the specified monitor.  The
 *  original gamma ramp for that monitor is saved by GLFW the first time this
 *  function is called and is restored by @ref glfwTerminate.
 *
 *  The software controlled gamma ramp is applied _in addition_ to the hardware
 *  gamma correction, which today is usually an approximation of sRGB gamma.
 *  This means that setting a perfectly linear ramp, or gamma 1.0, will produce
 *  the default (usually sRGB-like) behavior.
 *
 *  For gamma correct rendering with OpenGL or OpenGL ES, see the @ref
 *  GLFW_SRGB_CAPABLE hint.
 *
 *  @param[in] monitor The monitor whose gamma ramp to set.
 *  @param[in] ramp The gamma ramp to use.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark Gamma ramp sizes other than 256 are not supported by all platforms
 *  or graphics hardware.
 *
 *  @remark @win32 The gamma ramp size must be 256.
 *
 *  @remark @wayland Gamma handling is currently unavailable, this function will
 *  always emit @ref GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The specified gamma ramp is copied before this function
 *  returns.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref monitor_gamma
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup monitor
 */
GLFWAPI void glfwSetGammaRamp(GLFWmonitor* monitor, const GLFWgammaramp* ramp);

/*! @brief Resets all window hints to their default values.
 *
 *  This function resets all window hints to their
 *  [default values](@ref window_hints_values).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_hints
 *  @sa @ref glfwWindowHint
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI void glfwDefaultWindowHints(void);

/*! @brief Sets the specified window hint to the desired value.
 *
 *  This function sets hints for the next call to @ref glfwCreateWindow.  The
 *  hints, once set, retain their values until changed by a call to @ref
 *  glfwWindowHint or @ref glfwDefaultWindowHints, or until the library is
 *  terminated.
 *
 *  This function does not check whether the specified hint values are valid.
 *  If you set hints to invalid values this will instead be reported by the next
 *  call to @ref glfwCreateWindow.
 *
 *  @param[in] hint The [window hint](@ref window_hints) to set.
 *  @param[in] value The new value of the window hint.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_INVALID_ENUM.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_hints
 *  @sa @ref glfwDefaultWindowHints
 *
 *  @since Added in version 3.0.  Replaces `glfwOpenWindowHint`.
 *
 *  @ingroup window
 */
GLFWAPI void glfwWindowHint(int hint, int value);

/*! @brief Creates a window and its associated context.
 *
 *  This function creates a window and its associated OpenGL or OpenGL ES
 *  context.  Most of the options controlling how the window and its context
 *  should be created are specified with [window hints](@ref window_hints).
 *
 *  Successful creation does not change which context is current.  Before you
 *  can use the newly created context, you need to
 *  [make it current](@ref context_current).  For information about the `share`
 *  parameter, see @ref context_sharing.
 *
 *  The created window, framebuffer and context may differ from what you
 *  requested, as not all parameters and hints are
 *  [hard constraints](@ref window_hints_hard).  This includes the size of the
 *  window, especially for full screen windows.  To query the actual attributes
 *  of the created window, framebuffer and context, see @ref
 *  glfwGetWindowAttrib, @ref glfwGetWindowSize and @ref glfwGetFramebufferSize.
 *
 *  To create a full screen window, you need to specify the monitor the window
 *  will cover.  If no monitor is specified, the window will be windowed mode.
 *  Unless you have a way for the user to choose a specific monitor, it is
 *  recommended that you pick the primary monitor.  For more information on how
 *  to query connected monitors, see @ref monitor_monitors.
 *
 *  For full screen windows, the specified size becomes the resolution of the
 *  window's _desired video mode_.  As long as a full screen window is not
 *  iconified, the supported video mode most closely matching the desired video
 *  mode is set for the specified monitor.  For more information about full
 *  screen windows, including the creation of so called _windowed full screen_
 *  or _borderless full screen_ windows, see @ref window_windowed_full_screen.
 *
 *  Once you have created the window, you can switch it between windowed and
 *  full screen mode with @ref glfwSetWindowMonitor.  This will not affect its
 *  OpenGL or OpenGL ES context.
 *
 *  By default, newly created windows use the placement recommended by the
 *  window system.  To create the window at a specific position, make it
 *  initially invisible using the [GLFW_VISIBLE](@ref GLFW_VISIBLE_hint) window
 *  hint, set its [position](@ref window_pos) and then [show](@ref window_hide)
 *  it.
 *
 *  As long as at least one full screen window is not iconified, the screensaver
 *  is prohibited from starting.
 *
 *  Window systems put limits on window sizes.  Very large or very small window
 *  dimensions may be overridden by the window system on creation.  Check the
 *  actual [size](@ref window_size) after creation.
 *
 *  The [swap interval](@ref buffer_swap) is not set during window creation and
 *  the initial value may vary depending on driver settings and defaults.
 *
 *  @param[in] width The desired width, in screen coordinates, of the window.
 *  This must be greater than zero.
 *  @param[in] height The desired height, in screen coordinates, of the window.
 *  This must be greater than zero.
 *  @param[in] title The initial, UTF-8 encoded window title.
 *  @param[in] monitor The monitor to use for full screen mode, or `NULL` for
 *  windowed mode.
 *  @param[in] share The window whose context to share resources with, or `NULL`
 *  to not share resources.
 *  @return The handle of the created window, or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM, @ref GLFW_INVALID_VALUE, @ref GLFW_API_UNAVAILABLE, @ref
 *  GLFW_VERSION_UNAVAILABLE, @ref GLFW_FORMAT_UNAVAILABLE and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @win32 Window creation will fail if the Microsoft GDI software
 *  OpenGL implementation is the only one available.
 *
 *  @remark @win32 If the executable has an icon resource named `GLFW_ICON,` it
 *  will be set as the initial icon for the window.  If no such icon is present,
 *  the `IDI_WINLOGO` icon will be used instead.  To set a different icon, see
 *  @ref glfwSetWindowIcon.
 *
 *  @remark @win32 The context to share resources with must not be current on
 *  any other thread.
 *
 *  @remark @macos The OS only supports forward-compatible core profile contexts
 *  for OpenGL versions 3.2 and later.  Before creating an OpenGL context of
 *  version 3.2 or later you must set the
 *  [GLFW_OPENGL_FORWARD_COMPAT](@ref GLFW_OPENGL_FORWARD_COMPAT_hint) and
 *  [GLFW_OPENGL_PROFILE](@ref GLFW_OPENGL_PROFILE_hint) hints accordingly.
 *  OpenGL 3.0 and 3.1 contexts are not supported at all on macOS.
 *
 *  @remark @macos The GLFW window has no icon, as it is not a document
 *  window, but the dock icon will be the same as the application bundle's icon.
 *  For more information on bundles, see the
 *  [Bundle Programming Guide](https://developer.apple.com/library/mac/documentation/CoreFoundation/Conceptual/CFBundles/)
 *  in the Mac Developer Library.
 *
 *  @remark @macos The first time a window is created the menu bar is populated
 *  with common commands like Hide, Quit and About.  The About entry opens
 *  a minimal about dialog with information from the application's bundle.  The
 *  menu bar can be disabled with a
 *  [compile-time option](@ref compile_options_osx).
 *
 *  @remark @macos On OS X 10.10 and later the window frame will not be rendered
 *  at full resolution on Retina displays unless the
 *  [GLFW_COCOA_RETINA_FRAMEBUFFER](@ref GLFW_COCOA_RETINA_FRAMEBUFFER_hint)
 *  hint is `GLFW_TRUE` and the `NSHighResolutionCapable` key is enabled in the
 *  application bundle's `Info.plist`.  For more information, see
 *  [High Resolution Guidelines for OS X](https://developer.apple.com/library/mac/documentation/GraphicsAnimation/Conceptual/HighResolutionOSX/Explained/Explained.html)
 *  in the Mac Developer Library.  The GLFW test and example programs use
 *  a custom `Info.plist` template for this, which can be found as
 *  `CMake/MacOSXBundleInfo.plist.in` in the source tree.
 *
 *  @remark @macos When activating frame autosaving with
 *  [GLFW_COCOA_FRAME_AUTOSAVE](@ref GLFW_COCOA_FRAME_AUTOSAVE_hint), the
 *  specified window size may be overriden by a previously saved size and
 *  position.
 *
 *  @remark @x11 Some window managers will not respect the placement of
 *  initially hidden windows.
 *
 *  @remark @x11 Due to the asynchronous nature of X11, it may take a moment for
 *  a window to reach its requested state.  This means you may not be able to
 *  query the final size, position or other attributes directly after window
 *  creation.
 *
 *  @remark @wayland The window frame is currently unimplemented, as if
 *  [GLFW_DECORATED](@ref GLFW_DECORATED_hint) was always set to `GLFW_FALSE`.
 *  A compositor can still emit close, resize or maximize events, using for
 *  example a keybind mechanism.
 *
 *  @remark @wayland A full screen window will not attempt to change the mode,
 *  no matter what the requested size or refresh rate.
 *
 *  @remark @wayland The wl_shell protocol does not support window
 *  icons, the window will inherit the one defined in the application's
 *  desktop file, so this function emits @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland Screensaver inhibition is currently unimplemented.
 *
 *  @reentrancy This function must not be called from a callback.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_creation
 *  @sa @ref glfwDestroyWindow
 *
 *  @since Added in version 3.0.  Replaces `glfwOpenWindow`.
 *
 *  @ingroup window
 */
GLFWAPI GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);

/*! @brief Destroys the specified window and its context.
 *
 *  This function destroys the specified window and its context.  On calling
 *  this function, no further callbacks will be called for that window.
 *
 *  If the context of the specified window is current on the main thread, it is
 *  detached before being destroyed.
 *
 *  @param[in] window The window to destroy.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @note The context of the specified window must not be current on any other
 *  thread when this function is called.
 *
 *  @reentrancy This function must not be called from a callback.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_creation
 *  @sa @ref glfwCreateWindow
 *
 *  @since Added in version 3.0.  Replaces `glfwCloseWindow`.
 *
 *  @ingroup window
 */
GLFWAPI void glfwDestroyWindow(GLFWwindow* window);

/*! @brief Checks the close flag of the specified window.
 *
 *  This function returns the value of the close flag of the specified window.
 *
 *  @param[in] window The window to query.
 *  @return The value of the close flag.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function may be called from any thread.  Access is not
 *  synchronized.
 *
 *  @sa @ref window_close
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI int glfwWindowShouldClose(GLFWwindow* window);

/*! @brief Sets the close flag of the specified window.
 *
 *  This function sets the value of the close flag of the specified window.
 *  This can be used to override the user's attempt to close the window, or
 *  to signal that it should be closed.
 *
 *  @param[in] window The window whose flag to change.
 *  @param[in] value The new value.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function may be called from any thread.  Access is not
 *  synchronized.
 *
 *  @sa @ref window_close
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowShouldClose(GLFWwindow* window, int value);

/*! @brief Sets the title of the specified window.
 *
 *  This function sets the window title, encoded as UTF-8, of the specified
 *  window.
 *
 *  @param[in] window The window whose title to change.
 *  @param[in] title The UTF-8 encoded window title.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @macos The window title will not be updated until the next time you
 *  process events.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_title
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowTitle(GLFWwindow* window, const char* title);

/*! @brief Sets the icon for the specified window.
 *
 *  This function sets the icon of the specified window.  If passed an array of
 *  candidate images, those of or closest to the sizes desired by the system are
 *  selected.  If no images are specified, the window reverts to its default
 *  icon.
 *
 *  The desired image sizes varies depending on platform and system settings.
 *  The selected images will be rescaled as needed.  Good sizes include 16x16,
 *  32x32 and 48x48.
 *
 *  @param[in] window The window whose icon to set.
 *  @param[in] count The number of images in the specified array, or zero to
 *  revert to the default window icon.
 *  @param[in] images The images to create the icon from.  This is ignored if
 *  count is zero.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The specified image data is copied before this function
 *  returns.
 *
 *  @remark @macos The GLFW window has no icon, as it is not a document
 *  window, so this function does nothing.  The dock icon will be the same as
 *  the application bundle's icon.  For more information on bundles, see the
 *  [Bundle Programming Guide](https://developer.apple.com/library/mac/documentation/CoreFoundation/Conceptual/CFBundles/)
 *  in the Mac Developer Library.
 *
 *  @remark @wayland The wl_shell protocol does not support icons, the window
 *  will inherit the one defined in the application's desktop file, so this
 *  function emits @ref GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_icon
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowIcon(GLFWwindow* window, int count, const GLFWimage* images);

/*! @brief Retrieves the position of the client area of the specified window.
 *
 *  This function retrieves the position, in screen coordinates, of the
 *  upper-left corner of the client area of the specified window.
 *
 *  Any or all of the position arguments may be `NULL`.  If an error occurs, all
 *  non-`NULL` position arguments will be set to zero.
 *
 *  @param[in] window The window to query.
 *  @param[out] xpos Where to store the x-coordinate of the upper-left corner of
 *  the client area, or `NULL`.
 *  @param[out] ypos Where to store the y-coordinate of the upper-left corner of
 *  the client area, or `NULL`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland There is no way for an application to retrieve the global
 *  position of its windows, this function will always emit @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_pos
 *  @sa @ref glfwSetWindowPos
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI void glfwGetWindowPos(GLFWwindow* window, int* xpos, int* ypos);

/*! @brief Sets the position of the client area of the specified window.
 *
 *  This function sets the position, in screen coordinates, of the upper-left
 *  corner of the client area of the specified windowed mode window.  If the
 *  window is a full screen window, this function does nothing.
 *
 *  __Do not use this function__ to move an already visible window unless you
 *  have very good reasons for doing so, as it will confuse and annoy the user.
 *
 *  The window manager may put limits on what positions are allowed.  GLFW
 *  cannot and should not override these limits.
 *
 *  @param[in] window The window to query.
 *  @param[in] xpos The x-coordinate of the upper-left corner of the client area.
 *  @param[in] ypos The y-coordinate of the upper-left corner of the client area.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland There is no way for an application to set the global
 *  position of its windows, this function will always emit @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_pos
 *  @sa @ref glfwGetWindowPos
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowPos(GLFWwindow* window, int xpos, int ypos);

/*! @brief Retrieves the size of the client area of the specified window.
 *
 *  This function retrieves the size, in screen coordinates, of the client area
 *  of the specified window.  If you wish to retrieve the size of the
 *  framebuffer of the window in pixels, see @ref glfwGetFramebufferSize.
 *
 *  Any or all of the size arguments may be `NULL`.  If an error occurs, all
 *  non-`NULL` size arguments will be set to zero.
 *
 *  @param[in] window The window whose size to retrieve.
 *  @param[out] width Where to store the width, in screen coordinates, of the
 *  client area, or `NULL`.
 *  @param[out] height Where to store the height, in screen coordinates, of the
 *  client area, or `NULL`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_size
 *  @sa @ref glfwSetWindowSize
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
GLFWAPI void glfwGetWindowSize(GLFWwindow* window, int* width, int* height);

/*! @brief Sets the size limits of the specified window.
 *
 *  This function sets the size limits of the client area of the specified
 *  window.  If the window is full screen, the size limits only take effect
 *  once it is made windowed.  If the window is not resizable, this function
 *  does nothing.
 *
 *  The size limits are applied immediately to a windowed mode window and may
 *  cause it to be resized.
 *
 *  The maximum dimensions must be greater than or equal to the minimum
 *  dimensions and all must be greater than or equal to zero.
 *
 *  @param[in] window The window to set limits for.
 *  @param[in] minwidth The minimum width, in screen coordinates, of the client
 *  area, or `GLFW_DONT_CARE`.
 *  @param[in] minheight The minimum height, in screen coordinates, of the
 *  client area, or `GLFW_DONT_CARE`.
 *  @param[in] maxwidth The maximum width, in screen coordinates, of the client
 *  area, or `GLFW_DONT_CARE`.
 *  @param[in] maxheight The maximum height, in screen coordinates, of the
 *  client area, or `GLFW_DONT_CARE`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_VALUE and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark If you set size limits and an aspect ratio that conflict, the
 *  results are undefined.
 *
 *  @remark @wayland The size limits will not be applied until the window is
 *  actually resized, either by the user or by the compositor.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_sizelimits
 *  @sa @ref glfwSetWindowAspectRatio
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowSizeLimits(GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight);

/*! @brief Sets the aspect ratio of the specified window.
 *
 *  This function sets the required aspect ratio of the client area of the
 *  specified window.  If the window is full screen, the aspect ratio only takes
 *  effect once it is made windowed.  If the window is not resizable, this
 *  function does nothing.
 *
 *  The aspect ratio is specified as a numerator and a denominator and both
 *  values must be greater than zero.  For example, the common 16:9 aspect ratio
 *  is specified as 16 and 9, respectively.
 *
 *  If the numerator and denominator is set to `GLFW_DONT_CARE` then the aspect
 *  ratio limit is disabled.
 *
 *  The aspect ratio is applied immediately to a windowed mode window and may
 *  cause it to be resized.
 *
 *  @param[in] window The window to set limits for.
 *  @param[in] numer The numerator of the desired aspect ratio, or
 *  `GLFW_DONT_CARE`.
 *  @param[in] denom The denominator of the desired aspect ratio, or
 *  `GLFW_DONT_CARE`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_VALUE and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark If you set size limits and an aspect ratio that conflict, the
 *  results are undefined.
 *
 *  @remark @wayland The aspect ratio will not be applied until the window is
 *  actually resized, either by the user or by the compositor.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_sizelimits
 *  @sa @ref glfwSetWindowSizeLimits
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowAspectRatio(GLFWwindow* window, int numer, int denom);

/*! @brief Sets the size of the client area of the specified window.
 *
 *  This function sets the size, in screen coordinates, of the client area of
 *  the specified window.
 *
 *  For full screen windows, this function updates the resolution of its desired
 *  video mode and switches to the video mode closest to it, without affecting
 *  the window's context.  As the context is unaffected, the bit depths of the
 *  framebuffer remain unchanged.
 *
 *  If you wish to update the refresh rate of the desired video mode in addition
 *  to its resolution, see @ref glfwSetWindowMonitor.
 *
 *  The window manager may put limits on what sizes are allowed.  GLFW cannot
 *  and should not override these limits.
 *
 *  @param[in] window The window to resize.
 *  @param[in] width The desired width, in screen coordinates, of the window
 *  client area.
 *  @param[in] height The desired height, in screen coordinates, of the window
 *  client area.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland A full screen window will not attempt to change the mode,
 *  no matter what the requested size.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_size
 *  @sa @ref glfwGetWindowSize
 *  @sa @ref glfwSetWindowMonitor
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowSize(GLFWwindow* window, int width, int height);

/*! @brief Retrieves the size of the framebuffer of the specified window.
 *
 *  This function retrieves the size, in pixels, of the framebuffer of the
 *  specified window.  If you wish to retrieve the size of the window in screen
 *  coordinates, see @ref glfwGetWindowSize.
 *
 *  Any or all of the size arguments may be `NULL`.  If an error occurs, all
 *  non-`NULL` size arguments will be set to zero.
 *
 *  @param[in] window The window whose framebuffer to query.
 *  @param[out] width Where to store the width, in pixels, of the framebuffer,
 *  or `NULL`.
 *  @param[out] height Where to store the height, in pixels, of the framebuffer,
 *  or `NULL`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_fbsize
 *  @sa @ref glfwSetFramebufferSizeCallback
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI void glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height);

/*! @brief Retrieves the size of the frame of the window.
 *
 *  This function retrieves the size, in screen coordinates, of each edge of the
 *  frame of the specified window.  This size includes the title bar, if the
 *  window has one.  The size of the frame may vary depending on the
 *  [window-related hints](@ref window_hints_wnd) used to create it.
 *
 *  Because this function retrieves the size of each window frame edge and not
 *  the offset along a particular coordinate axis, the retrieved values will
 *  always be zero or positive.
 *
 *  Any or all of the size arguments may be `NULL`.  If an error occurs, all
 *  non-`NULL` size arguments will be set to zero.
 *
 *  @param[in] window The window whose frame size to query.
 *  @param[out] left Where to store the size, in screen coordinates, of the left
 *  edge of the window frame, or `NULL`.
 *  @param[out] top Where to store the size, in screen coordinates, of the top
 *  edge of the window frame, or `NULL`.
 *  @param[out] right Where to store the size, in screen coordinates, of the
 *  right edge of the window frame, or `NULL`.
 *  @param[out] bottom Where to store the size, in screen coordinates, of the
 *  bottom edge of the window frame, or `NULL`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland The window frame is currently unimplemented, as if
 *  [GLFW_DECORATED](@ref GLFW_DECORATED_hint) was always set to `GLFW_FALSE`,
 *  so the returned values will always be zero.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_size
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup window
 */
GLFWAPI void glfwGetWindowFrameSize(GLFWwindow* window, int* left, int* top, int* right, int* bottom);

/*! @brief Iconifies the specified window.
 *
 *  This function iconifies (minimizes) the specified window if it was
 *  previously restored.  If the window is already iconified, this function does
 *  nothing.
 *
 *  If the specified window is a full screen window, the original monitor
 *  resolution is restored until the window is restored.
 *
 *  @param[in] window The window to iconify.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland There is no concept of iconification in wl_shell, this
 *  function will always emit @ref GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_iconify
 *  @sa @ref glfwRestoreWindow
 *  @sa @ref glfwMaximizeWindow
 *
 *  @since Added in version 2.1.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
GLFWAPI void glfwIconifyWindow(GLFWwindow* window);

/*! @brief Restores the specified window.
 *
 *  This function restores the specified window if it was previously iconified
 *  (minimized) or maximized.  If the window is already restored, this function
 *  does nothing.
 *
 *  If the specified window is a full screen window, the resolution chosen for
 *  the window is restored on the selected monitor.
 *
 *  @param[in] window The window to restore.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_iconify
 *  @sa @ref glfwIconifyWindow
 *  @sa @ref glfwMaximizeWindow
 *
 *  @since Added in version 2.1.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
GLFWAPI void glfwRestoreWindow(GLFWwindow* window);

/*! @brief Maximizes the specified window.
 *
 *  This function maximizes the specified window if it was previously not
 *  maximized.  If the window is already maximized, this function does nothing.
 *
 *  If the specified window is a full screen window, this function does nothing.
 *
 *  @param[in] window The window to maximize.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @par Thread Safety
 *  This function may only be called from the main thread.
 *
 *  @sa @ref window_iconify
 *  @sa @ref glfwIconifyWindow
 *  @sa @ref glfwRestoreWindow
 *
 *  @since Added in GLFW 3.2.
 *
 *  @ingroup window
 */
GLFWAPI void glfwMaximizeWindow(GLFWwindow* window);

/*! @brief Makes the specified window visible.
 *
 *  This function makes the specified window visible if it was previously
 *  hidden.  If the window is already visible or is in full screen mode, this
 *  function does nothing.
 *
 *  @param[in] window The window to make visible.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_hide
 *  @sa @ref glfwHideWindow
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI void glfwShowWindow(GLFWwindow* window);

/*! @brief Hides the specified window.
 *
 *  This function hides the specified window if it was previously visible.  If
 *  the window is already hidden or is in full screen mode, this function does
 *  nothing.
 *
 *  @param[in] window The window to hide.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_hide
 *  @sa @ref glfwShowWindow
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI void glfwHideWindow(GLFWwindow* window);

/*! @brief Brings the specified window to front and sets input focus.
 *
 *  This function brings the specified window to front and sets input focus.
 *  The window should already be visible and not iconified.
 *
 *  By default, both windowed and full screen mode windows are focused when
 *  initially created.  Set the [GLFW_FOCUSED](@ref GLFW_FOCUSED_hint) to
 *  disable this behavior.
 *
 *  __Do not use this function__ to steal focus from other applications unless
 *  you are certain that is what the user wants.  Focus stealing can be
 *  extremely disruptive.
 *
 *  @param[in] window The window to give input focus.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland It is not possible for an application to bring its windows
 *  to front, this function will always emit @ref GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_focus
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup window
 */
GLFWAPI void glfwFocusWindow(GLFWwindow* window);

/*! @brief Returns the monitor that the window uses for full screen mode.
 *
 *  This function returns the handle of the monitor that the specified window is
 *  in full screen on.
 *
 *  @param[in] window The window to query.
 *  @return The monitor, or `NULL` if the window is in windowed mode or an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_monitor
 *  @sa @ref glfwSetWindowMonitor
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI GLFWmonitor* glfwGetWindowMonitor(GLFWwindow* window);

/*! @brief Sets the mode, monitor, video mode and placement of a window.
 *
 *  This function sets the monitor that the window uses for full screen mode or,
 *  if the monitor is `NULL`, makes it windowed mode.
 *
 *  When setting a monitor, this function updates the width, height and refresh
 *  rate of the desired video mode and switches to the video mode closest to it.
 *  The window position is ignored when setting a monitor.
 *
 *  When the monitor is `NULL`, the position, width and height are used to
 *  place the window client area.  The refresh rate is ignored when no monitor
 *  is specified.
 *
 *  If you only wish to update the resolution of a full screen window or the
 *  size of a windowed mode window, see @ref glfwSetWindowSize.
 *
 *  When a window transitions from full screen to windowed mode, this function
 *  restores any previous window settings such as whether it is decorated,
 *  floating, resizable, has size or aspect ratio limits, etc.
 *
 *  @param[in] window The window whose monitor, size or video mode to set.
 *  @param[in] monitor The desired monitor, or `NULL` to set windowed mode.
 *  @param[in] xpos The desired x-coordinate of the upper-left corner of the
 *  client area.
 *  @param[in] ypos The desired y-coordinate of the upper-left corner of the
 *  client area.
 *  @param[in] width The desired with, in screen coordinates, of the client area
 *  or video mode.
 *  @param[in] height The desired height, in screen coordinates, of the client
 *  area or video mode.
 *  @param[in] refreshRate The desired refresh rate, in Hz, of the video mode,
 *  or `GLFW_DONT_CARE`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark The OpenGL or OpenGL ES context will not be destroyed or otherwise
 *  affected by any resizing or mode switching, although you may need to update
 *  your viewport if the framebuffer size has changed.
 *
 *  @remark @wayland The desired window position is ignored, as there is no way
 *  for an application to set this property.
 *
 *  @remark @wayland Setting the window to full screen will not attempt to
 *  change the mode, no matter what the requested size or refresh rate.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_monitor
 *  @sa @ref window_full_screen
 *  @sa @ref glfwGetWindowMonitor
 *  @sa @ref glfwSetWindowSize
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowMonitor(GLFWwindow* window, GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate);

/*! @brief Returns an attribute of the specified window.
 *
 *  This function returns the value of an attribute of the specified window or
 *  its OpenGL or OpenGL ES context.
 *
 *  @param[in] window The window to query.
 *  @param[in] attrib The [window attribute](@ref window_attribs) whose value to
 *  return.
 *  @return The value of the attribute, or zero if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark Framebuffer related hints are not window attributes.  See @ref
 *  window_attribs_fb for more information.
 *
 *  @remark Zero is a valid value for many window and context related
 *  attributes so you cannot use a return value of zero as an indication of
 *  errors.  However, this function should not fail as long as it is passed
 *  valid arguments and the library has been [initialized](@ref intro_init).
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_attribs
 *  @sa @ref glfwSetWindowAttrib
 *
 *  @since Added in version 3.0.  Replaces `glfwGetWindowParam` and
 *  `glfwGetGLVersion`.
 *
 *  @ingroup window
 */
GLFWAPI int glfwGetWindowAttrib(GLFWwindow* window, int attrib);

/*! @brief Sets an attribute of the specified window.
 *
 *  This function sets the value of an attribute of the specified window.
 *
 *  The supported attributes are [GLFW_DECORATED](@ref GLFW_DECORATED_attrib),
 *  [GLFW_RESIZABLE](@ref GLFW_RESIZABLE_attrib),
 *  [GLFW_FLOATING](@ref GLFW_FLOATING_attrib) and
 *  [GLFW_AUTO_ICONIFY](@ref GLFW_AUTO_ICONIFY_attrib).   
 *
 *  Some of these attributes are ignored for full screen windows.  The new
 *  value will take effect if the window is later made windowed.
 *
 *  Some of these attributes are ignored for windowed mode windows.  The new
 *  value will take effect if the window is later made full screen.
 *
 *  @param[in] window The window to set the attribute for.
 *  @param[in] attrib A supported window attribute.
 *  @param[in] value `GLFW_TRUE` or `GLFW_FALSE`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM, @ref GLFW_INVALID_VALUE and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark Calling @ref glfwGetWindowAttrib will always return the latest
 *  value, even if that value is ignored by the current mode of the window.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_attribs
 *  @sa @ref glfwGetWindowAttrib
 *
 *  @since Added in version 3.3.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowAttrib(GLFWwindow* window, int attrib, int value);

/*! @brief Sets the user pointer of the specified window.
 *
 *  This function sets the user-defined pointer of the specified window.  The
 *  current value is retained until the window is destroyed.  The initial value
 *  is `NULL`.
 *
 *  @param[in] window The window whose pointer to set.
 *  @param[in] pointer The new value.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function may be called from any thread.  Access is not
 *  synchronized.
 *
 *  @sa @ref window_userptr
 *  @sa @ref glfwGetWindowUserPointer
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSetWindowUserPointer(GLFWwindow* window, void* pointer);

/*! @brief Returns the user pointer of the specified window.
 *
 *  This function returns the current value of the user-defined pointer of the
 *  specified window.  The initial value is `NULL`.
 *
 *  @param[in] window The window whose pointer to return.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function may be called from any thread.  Access is not
 *  synchronized.
 *
 *  @sa @ref window_userptr
 *  @sa @ref glfwSetWindowUserPointer
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI void* glfwGetWindowUserPointer(GLFWwindow* window);

/*! @brief Sets the position callback for the specified window.
 *
 *  This function sets the position callback of the specified window, which is
 *  called when the window is moved.  The callback is provided with the screen
 *  position of the upper-left corner of the client area of the window.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @remark @wayland This callback will never be called, as there is no way for
 *  an application to know its global position.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_pos
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun cbfun);

/*! @brief Sets the size callback for the specified window.
 *
 *  This function sets the size callback of the specified window, which is
 *  called when the window is resized.  The callback is provided with the size,
 *  in screen coordinates, of the client area of the window.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_size
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter and return value.
 *
 *  @ingroup window
 */
GLFWAPI GLFWwindowsizefun glfwSetWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun cbfun);

/*! @brief Sets the close callback for the specified window.
 *
 *  This function sets the close callback of the specified window, which is
 *  called when the user attempts to close the window, for example by clicking
 *  the close widget in the title bar.
 *
 *  The close flag is set before this callback is called, but you can modify it
 *  at any time with @ref glfwSetWindowShouldClose.
 *
 *  The close callback is not triggered by @ref glfwDestroyWindow.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @remark @macos Selecting Quit from the application menu will trigger the
 *  close callback for all windows.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_close
 *
 *  @since Added in version 2.5.
 *  @glfw3 Added window handle parameter and return value.
 *
 *  @ingroup window
 */
GLFWAPI GLFWwindowclosefun glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun cbfun);

/*! @brief Sets the refresh callback for the specified window.
 *
 *  This function sets the refresh callback of the specified window, which is
 *  called when the client area of the window needs to be redrawn, for example
 *  if the window has been exposed after having been covered by another window.
 *
 *  On compositing window systems such as Aero, Compiz, Aqua or Wayland, where
 *  the window contents are saved off-screen, this callback may be called only
 *  very infrequently or never at all.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_refresh
 *
 *  @since Added in version 2.5.
 *  @glfw3 Added window handle parameter and return value.
 *
 *  @ingroup window
 */
GLFWAPI GLFWwindowrefreshfun glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun cbfun);

/*! @brief Sets the focus callback for the specified window.
 *
 *  This function sets the focus callback of the specified window, which is
 *  called when the window gains or loses input focus.
 *
 *  After the focus callback is called for a window that lost input focus,
 *  synthetic key and mouse button release events will be generated for all such
 *  that had been pressed.  For more information, see @ref glfwSetKeyCallback
 *  and @ref glfwSetMouseButtonCallback.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_focus
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI GLFWwindowfocusfun glfwSetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun cbfun);

/*! @brief Sets the iconify callback for the specified window.
 *
 *  This function sets the iconification callback of the specified window, which
 *  is called when the window is iconified or restored.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @remark @wayland The wl_shell protocol has no concept of iconification,
 *  this callback will never be called.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_iconify
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI GLFWwindowiconifyfun glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun cbfun);

/*! @brief Sets the maximize callback for the specified window.
 *
 *  This function sets the maximization callback of the specified window, which
 *  is called when the window is maximized or restored.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_maximize
 *
 *  @since Added in version 3.3.
 *
 *  @ingroup window
 */
GLFWAPI GLFWwindowmaximizefun glfwSetWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun cbfun);

/*! @brief Sets the framebuffer resize callback for the specified window.
 *
 *  This function sets the framebuffer resize callback of the specified window,
 *  which is called when the framebuffer of the specified window is resized.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref window_fbsize
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup window
 */
GLFWAPI GLFWframebuffersizefun glfwSetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun cbfun);

/*! @brief Processes all pending events.
 *
 *  This function processes only those events that are already in the event
 *  queue and then returns immediately.  Processing events will cause the window
 *  and input callbacks associated with those events to be called.
 *
 *  On some platforms, a window move, resize or menu operation will cause event
 *  processing to block.  This is due to how event processing is designed on
 *  those platforms.  You can use the
 *  [window refresh callback](@ref window_refresh) to redraw the contents of
 *  your window when necessary during such operations.
 *
 *  Do not assume that callbacks you set will _only_ be called in response to
 *  event processing functions like this one.  While it is necessary to poll for
 *  events, window systems that require GLFW to register callbacks of its own
 *  can pass events to GLFW in response to many window system function calls.
 *  GLFW will pass those events on to the application callbacks before
 *  returning.
 *
 *  Event processing is not required for joystick input to work.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @reentrancy This function must not be called from a callback.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref events
 *  @sa @ref glfwWaitEvents
 *  @sa @ref glfwWaitEventsTimeout
 *
 *  @since Added in version 1.0.
 *
 *  @ingroup window
 */
GLFWAPI void glfwPollEvents(void);

/*! @brief Waits until events are queued and processes them.
 *
 *  This function puts the calling thread to sleep until at least one event is
 *  available in the event queue.  Once one or more events are available,
 *  it behaves exactly like @ref glfwPollEvents, i.e. the events in the queue
 *  are processed and the function then returns immediately.  Processing events
 *  will cause the window and input callbacks associated with those events to be
 *  called.
 *
 *  Since not all events are associated with callbacks, this function may return
 *  without a callback having been called even if you are monitoring all
 *  callbacks.
 *
 *  On some platforms, a window move, resize or menu operation will cause event
 *  processing to block.  This is due to how event processing is designed on
 *  those platforms.  You can use the
 *  [window refresh callback](@ref window_refresh) to redraw the contents of
 *  your window when necessary during such operations.
 *
 *  Do not assume that callbacks you set will _only_ be called in response to
 *  event processing functions like this one.  While it is necessary to poll for
 *  events, window systems that require GLFW to register callbacks of its own
 *  can pass events to GLFW in response to many window system function calls.
 *  GLFW will pass those events on to the application callbacks before
 *  returning.
 *
 *  If no windows exist, this function returns immediately.  For synchronization
 *  of threads in applications that do not create windows, use your threading
 *  library of choice.
 *
 *  Event processing is not required for joystick input to work.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @reentrancy This function must not be called from a callback.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref events
 *  @sa @ref glfwPollEvents
 *  @sa @ref glfwWaitEventsTimeout
 *
 *  @since Added in version 2.5.
 *
 *  @ingroup window
 */
GLFWAPI void glfwWaitEvents(void);

/*! @brief Waits with timeout until events are queued and processes them.
 *
 *  This function puts the calling thread to sleep until at least one event is
 *  available in the event queue, or until the specified timeout is reached.  If
 *  one or more events are available, it behaves exactly like @ref
 *  glfwPollEvents, i.e. the events in the queue are processed and the function
 *  then returns immediately.  Processing events will cause the window and input
 *  callbacks associated with those events to be called.
 *
 *  The timeout value must be a positive finite number.
 *
 *  Since not all events are associated with callbacks, this function may return
 *  without a callback having been called even if you are monitoring all
 *  callbacks.
 *
 *  On some platforms, a window move, resize or menu operation will cause event
 *  processing to block.  This is due to how event processing is designed on
 *  those platforms.  You can use the
 *  [window refresh callback](@ref window_refresh) to redraw the contents of
 *  your window when necessary during such operations.
 *
 *  Do not assume that callbacks you set will _only_ be called in response to
 *  event processing functions like this one.  While it is necessary to poll for
 *  events, window systems that require GLFW to register callbacks of its own
 *  can pass events to GLFW in response to many window system function calls.
 *  GLFW will pass those events on to the application callbacks before
 *  returning.
 *
 *  If no windows exist, this function returns immediately.  For synchronization
 *  of threads in applications that do not create windows, use your threading
 *  library of choice.
 *
 *  Event processing is not required for joystick input to work.
 *
 *  @param[in] timeout The maximum amount of time, in seconds, to wait.
 *
 *  @reentrancy This function must not be called from a callback.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref events
 *  @sa @ref glfwPollEvents
 *  @sa @ref glfwWaitEvents
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup window
 */
GLFWAPI void glfwWaitEventsTimeout(double timeout);

/*! @brief Posts an empty event to the event queue.
 *
 *  This function posts an empty event from the current thread to the event
 *  queue, causing @ref glfwWaitEvents or @ref glfwWaitEventsTimeout to return.
 *
 *  If no windows exist, this function returns immediately.  For synchronization
 *  of threads in applications that do not create windows, use your threading
 *  library of choice.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref events
 *  @sa @ref glfwWaitEvents
 *  @sa @ref glfwWaitEventsTimeout
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup window
 */
GLFWAPI void glfwPostEmptyEvent(void);

/*! @brief Returns the value of an input option for the specified window.
 *
 *  This function returns the value of an input option for the specified window.
 *  The mode must be one of `GLFW_CURSOR`, `GLFW_STICKY_KEYS` or
 *  `GLFW_STICKY_MOUSE_BUTTONS`.
 *
 *  @param[in] window The window to query.
 *  @param[in] mode One of `GLFW_CURSOR`, `GLFW_STICKY_KEYS` or
 *  `GLFW_STICKY_MOUSE_BUTTONS`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_INVALID_ENUM.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref glfwSetInputMode
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup input
 */
GLFWAPI int glfwGetInputMode(GLFWwindow* window, int mode);

/*! @brief Sets an input option for the specified window.
 *
 *  This function sets an input mode option for the specified window.  The mode
 *  must be one of `GLFW_CURSOR`, `GLFW_STICKY_KEYS` or
 *  `GLFW_STICKY_MOUSE_BUTTONS`.
 *
 *  If the mode is `GLFW_CURSOR`, the value must be one of the following cursor
 *  modes:
 *  - `GLFW_CURSOR_NORMAL` makes the cursor visible and behaving normally.
 *  - `GLFW_CURSOR_HIDDEN` makes the cursor invisible when it is over the client
 *    area of the window but does not restrict the cursor from leaving.
 *  - `GLFW_CURSOR_DISABLED` hides and grabs the cursor, providing virtual
 *    and unlimited cursor movement.  This is useful for implementing for
 *    example 3D camera controls.
 *
 *  If the mode is `GLFW_STICKY_KEYS`, the value must be either `GLFW_TRUE` to
 *  enable sticky keys, or `GLFW_FALSE` to disable it.  If sticky keys are
 *  enabled, a key press will ensure that @ref glfwGetKey returns `GLFW_PRESS`
 *  the next time it is called even if the key had been released before the
 *  call.  This is useful when you are only interested in whether keys have been
 *  pressed but not when or in which order.
 *
 *  If the mode is `GLFW_STICKY_MOUSE_BUTTONS`, the value must be either
 *  `GLFW_TRUE` to enable sticky mouse buttons, or `GLFW_FALSE` to disable it.
 *  If sticky mouse buttons are enabled, a mouse button press will ensure that
 *  @ref glfwGetMouseButton returns `GLFW_PRESS` the next time it is called even
 *  if the mouse button had been released before the call.  This is useful when
 *  you are only interested in whether mouse buttons have been pressed but not
 *  when or in which order.
 *
 *  @param[in] window The window whose input mode to set.
 *  @param[in] mode One of `GLFW_CURSOR`, `GLFW_STICKY_KEYS` or
 *  `GLFW_STICKY_MOUSE_BUTTONS`.
 *  @param[in] value The new value of the specified input mode.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM and @ref GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref glfwGetInputMode
 *
 *  @since Added in version 3.0.  Replaces `glfwEnable` and `glfwDisable`.
 *
 *  @ingroup input
 */
GLFWAPI void glfwSetInputMode(GLFWwindow* window, int mode, int value);

/*! @brief Returns the localized name of the specified printable key.
 *
 *  This function returns the localized name of the specified printable key.
 *  This is intended for displaying key bindings to the user.
 *
 *  If the key is `GLFW_KEY_UNKNOWN`, the scancode is used instead, otherwise
 *  the scancode is ignored.  If a non-printable key or (if the key is
 *  `GLFW_KEY_UNKNOWN`) a scancode that maps to a non-printable key is
 *  specified, this function returns `NULL`.          
 *
 *  This behavior allows you to pass in the arguments passed to the
 *  [key callback](@ref input_key) without modification.
 *
 *  The printable keys are:
 *  - `GLFW_KEY_APOSTROPHE`
 *  - `GLFW_KEY_COMMA`
 *  - `GLFW_KEY_MINUS`
 *  - `GLFW_KEY_PERIOD`
 *  - `GLFW_KEY_SLASH`
 *  - `GLFW_KEY_SEMICOLON`
 *  - `GLFW_KEY_EQUAL`
 *  - `GLFW_KEY_LEFT_BRACKET`
 *  - `GLFW_KEY_RIGHT_BRACKET`
 *  - `GLFW_KEY_BACKSLASH`
 *  - `GLFW_KEY_WORLD_1`
 *  - `GLFW_KEY_WORLD_2`
 *  - `GLFW_KEY_0` to `GLFW_KEY_9`
 *  - `GLFW_KEY_A` to `GLFW_KEY_Z`
 *  - `GLFW_KEY_KP_0` to `GLFW_KEY_KP_9`
 *  - `GLFW_KEY_KP_DECIMAL`
 *  - `GLFW_KEY_KP_DIVIDE`
 *  - `GLFW_KEY_KP_MULTIPLY`
 *  - `GLFW_KEY_KP_SUBTRACT`
 *  - `GLFW_KEY_KP_ADD`
 *  - `GLFW_KEY_KP_EQUAL`
 *
 *  @param[in] key The key to query, or `GLFW_KEY_UNKNOWN`.
 *  @param[in] scancode The scancode of the key to query.
 *  @return The localized name of the key, or `NULL`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The returned string is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is valid until the next call to @ref
 *  glfwGetKeyName, or until the library is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref input_key_name
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup input
 */
GLFWAPI const char* glfwGetKeyName(int key, int scancode);

/*! @brief Returns the platform-specific scancode of the specified key.
 *
 *  This function returns the platform-specific scancode of the specified key.
 *
 *  If the key is `GLFW_KEY_UNKNOWN` or does not exist on the keyboard this
 *  method will return `-1`.
 *
 *  @param[in] key Any [named key](@ref keys).
 *  @return The platform-specific scancode for the key, or `-1` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM and @ref GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref input_key
 *
 *  @since Added in version 3.3.
 *
 *  @ingroup input
 */
GLFWAPI int glfwGetKeyScancode(int key);

/*! @brief Returns the last reported state of a keyboard key for the specified
 *  window.
 *
 *  This function returns the last state reported for the specified key to the
 *  specified window.  The returned state is one of `GLFW_PRESS` or
 *  `GLFW_RELEASE`.  The higher-level action `GLFW_REPEAT` is only reported to
 *  the key callback.
 *
 *  If the `GLFW_STICKY_KEYS` input mode is enabled, this function returns
 *  `GLFW_PRESS` the first time you call it for a key that was pressed, even if
 *  that key has already been released.
 *
 *  The key functions deal with physical keys, with [key tokens](@ref keys)
 *  named after their use on the standard US keyboard layout.  If you want to
 *  input text, use the Unicode character callback instead.
 *
 *  The [modifier key bit masks](@ref mods) are not key tokens and cannot be
 *  used with this function.
 *
 *  __Do not use this function__ to implement [text input](@ref input_char).
 *
 *  @param[in] window The desired window.
 *  @param[in] key The desired [keyboard key](@ref keys).  `GLFW_KEY_UNKNOWN` is
 *  not a valid key for this function.
 *  @return One of `GLFW_PRESS` or `GLFW_RELEASE`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_INVALID_ENUM.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref input_key
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup input
 */
GLFWAPI int glfwGetKey(GLFWwindow* window, int key);

/*! @brief Returns the last reported state of a mouse button for the specified
 *  window.
 *
 *  This function returns the last state reported for the specified mouse button
 *  to the specified window.  The returned state is one of `GLFW_PRESS` or
 *  `GLFW_RELEASE`.
 *
 *  If the `GLFW_STICKY_MOUSE_BUTTONS` input mode is enabled, this function
 *  `GLFW_PRESS` the first time you call it for a mouse button that was pressed,
 *  even if that mouse button has already been released.
 *
 *  @param[in] window The desired window.
 *  @param[in] button The desired [mouse button](@ref buttons).
 *  @return One of `GLFW_PRESS` or `GLFW_RELEASE`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_INVALID_ENUM.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref input_mouse_button
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup input
 */
GLFWAPI int glfwGetMouseButton(GLFWwindow* window, int button);

/*! @brief Retrieves the position of the cursor relative to the client area of
 *  the window.
 *
 *  This function returns the position of the cursor, in screen coordinates,
 *  relative to the upper-left corner of the client area of the specified
 *  window.
 *
 *  If the cursor is disabled (with `GLFW_CURSOR_DISABLED`) then the cursor
 *  position is unbounded and limited only by the minimum and maximum values of
 *  a `double`.
 *
 *  The coordinate can be converted to their integer equivalents with the
 *  `floor` function.  Casting directly to an integer type works for positive
 *  coordinates, but fails for negative ones.
 *
 *  Any or all of the position arguments may be `NULL`.  If an error occurs, all
 *  non-`NULL` position arguments will be set to zero.
 *
 *  @param[in] window The desired window.
 *  @param[out] xpos Where to store the cursor x-coordinate, relative to the
 *  left edge of the client area, or `NULL`.
 *  @param[out] ypos Where to store the cursor y-coordinate, relative to the to
 *  top edge of the client area, or `NULL`.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref cursor_pos
 *  @sa @ref glfwSetCursorPos
 *
 *  @since Added in version 3.0.  Replaces `glfwGetMousePos`.
 *
 *  @ingroup input
 */
GLFWAPI void glfwGetCursorPos(GLFWwindow* window, double* xpos, double* ypos);

/*! @brief Sets the position of the cursor, relative to the client area of the
 *  window.
 *
 *  This function sets the position, in screen coordinates, of the cursor
 *  relative to the upper-left corner of the client area of the specified
 *  window.  The window must have input focus.  If the window does not have
 *  input focus when this function is called, it fails silently.
 *
 *  __Do not use this function__ to implement things like camera controls.  GLFW
 *  already provides the `GLFW_CURSOR_DISABLED` cursor mode that hides the
 *  cursor, transparently re-centers it and provides unconstrained cursor
 *  motion.  See @ref glfwSetInputMode for more information.
 *
 *  If the cursor mode is `GLFW_CURSOR_DISABLED` then the cursor position is
 *  unconstrained and limited only by the minimum and maximum values of
 *  a `double`.
 *
 *  @param[in] window The desired window.
 *  @param[in] xpos The desired x-coordinate, relative to the left edge of the
 *  client area.
 *  @param[in] ypos The desired y-coordinate, relative to the top edge of the
 *  client area.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland This function will only work when the cursor mode is
 *  `GLFW_CURSOR_DISABLED`, otherwise it will do nothing.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref cursor_pos
 *  @sa @ref glfwGetCursorPos
 *
 *  @since Added in version 3.0.  Replaces `glfwSetMousePos`.
 *
 *  @ingroup input
 */
GLFWAPI void glfwSetCursorPos(GLFWwindow* window, double xpos, double ypos);

/*! @brief Creates a custom cursor.
 *
 *  Creates a new custom cursor image that can be set for a window with @ref
 *  glfwSetCursor.  The cursor can be destroyed with @ref glfwDestroyCursor.
 *  Any remaining cursors are destroyed by @ref glfwTerminate.
 *
 *  The pixels are 32-bit, little-endian, non-premultiplied RGBA, i.e. eight
 *  bits per channel.  They are arranged canonically as packed sequential rows,
 *  starting from the top-left corner.
 *
 *  The cursor hotspot is specified in pixels, relative to the upper-left corner
 *  of the cursor image.  Like all other coordinate systems in GLFW, the X-axis
 *  points to the right and the Y-axis points down.
 *
 *  @param[in] image The desired cursor image.
 *  @param[in] xhot The desired x-coordinate, in pixels, of the cursor hotspot.
 *  @param[in] yhot The desired y-coordinate, in pixels, of the cursor hotspot.
 *  @return The handle of the created cursor, or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The specified image data is copied before this function
 *  returns.
 *
 *  @reentrancy This function must not be called from a callback.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref cursor_object
 *  @sa @ref glfwDestroyCursor
 *  @sa @ref glfwCreateStandardCursor
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup input
 */
GLFWAPI GLFWcursor* glfwCreateCursor(const GLFWimage* image, int xhot, int yhot);

/*! @brief Creates a cursor with a standard shape.
 *
 *  Returns a cursor with a [standard shape](@ref shapes), that can be set for
 *  a window with @ref glfwSetCursor.
 *
 *  @param[in] shape One of the [standard shapes](@ref shapes).
 *  @return A new cursor ready to use or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM and @ref GLFW_PLATFORM_ERROR.
 *
 *  @reentrancy This function must not be called from a callback.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref cursor_object
 *  @sa @ref glfwCreateCursor
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup input
 */
GLFWAPI GLFWcursor* glfwCreateStandardCursor(int shape);

/*! @brief Destroys a cursor.
 *
 *  This function destroys a cursor previously created with @ref
 *  glfwCreateCursor.  Any remaining cursors will be destroyed by @ref
 *  glfwTerminate.
 *
 *  @param[in] cursor The cursor object to destroy.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @reentrancy This function must not be called from a callback.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref cursor_object
 *  @sa @ref glfwCreateCursor
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup input
 */
GLFWAPI void glfwDestroyCursor(GLFWcursor* cursor);

/*! @brief Sets the cursor for the window.
 *
 *  This function sets the cursor image to be used when the cursor is over the
 *  client area of the specified window.  The set cursor will only be visible
 *  when the [cursor mode](@ref cursor_mode) of the window is
 *  `GLFW_CURSOR_NORMAL`.
 *
 *  On some platforms, the set cursor may not be visible unless the window also
 *  has input focus.
 *
 *  @param[in] window The window to set the cursor for.
 *  @param[in] cursor The cursor to set, or `NULL` to switch back to the default
 *  arrow cursor.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref cursor_object
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup input
 */
GLFWAPI void glfwSetCursor(GLFWwindow* window, GLFWcursor* cursor);

/*! @brief Sets the key callback.
 *
 *  This function sets the key callback of the specified window, which is called
 *  when a key is pressed, repeated or released.
 *
 *  The key functions deal with physical keys, with layout independent
 *  [key tokens](@ref keys) named after their values in the standard US keyboard
 *  layout.  If you want to input text, use the
 *  [character callback](@ref glfwSetCharCallback) instead.
 *
 *  When a window loses input focus, it will generate synthetic key release
 *  events for all pressed keys.  You can tell these events from user-generated
 *  events by the fact that the synthetic ones are generated after the focus
 *  loss event has been processed, i.e. after the
 *  [window focus callback](@ref glfwSetWindowFocusCallback) has been called.
 *
 *  The scancode of a key is specific to that platform or sometimes even to that
 *  machine.  Scancodes are intended to allow users to bind keys that don't have
 *  a GLFW key token.  Such keys have `key` set to `GLFW_KEY_UNKNOWN`, their
 *  state is not saved and so it cannot be queried with @ref glfwGetKey.
 *
 *  Sometimes GLFW needs to generate synthetic key events, in which case the
 *  scancode may be zero.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new key callback, or `NULL` to remove the currently
 *  set callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref input_key
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter and return value.
 *
 *  @ingroup input
 */
GLFWAPI GLFWkeyfun glfwSetKeyCallback(GLFWwindow* window, GLFWkeyfun cbfun);

/*! @brief Sets the Unicode character callback.
 *
 *  This function sets the character callback of the specified window, which is
 *  called when a Unicode character is input.
 *
 *  The character callback is intended for Unicode text input.  As it deals with
 *  characters, it is keyboard layout dependent, whereas the
 *  [key callback](@ref glfwSetKeyCallback) is not.  Characters do not map 1:1
 *  to physical keys, as a key may produce zero, one or more characters.  If you
 *  want to know whether a specific physical key was pressed or released, see
 *  the key callback instead.
 *
 *  The character callback behaves as system text input normally does and will
 *  not be called if modifier keys are held down that would prevent normal text
 *  input on that platform, for example a Super (Command) key on macOS or Alt key
 *  on Windows.  There is a
 *  [character with modifiers callback](@ref glfwSetCharModsCallback) that
 *  receives these events.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref input_char
 *
 *  @since Added in version 2.4.
 *  @glfw3 Added window handle parameter and return value.
 *
 *  @ingroup input
 */
GLFWAPI GLFWcharfun glfwSetCharCallback(GLFWwindow* window, GLFWcharfun cbfun);

/*! @brief Sets the Unicode character with modifiers callback.
 *
 *  This function sets the character with modifiers callback of the specified
 *  window, which is called when a Unicode character is input regardless of what
 *  modifier keys are used.
 *
 *  The character with modifiers callback is intended for implementing custom
 *  Unicode character input.  For regular Unicode text input, see the
 *  [character callback](@ref glfwSetCharCallback).  Like the character
 *  callback, the character with modifiers callback deals with characters and is
 *  keyboard layout dependent.  Characters do not map 1:1 to physical keys, as
 *  a key may produce zero, one or more characters.  If you want to know whether
 *  a specific physical key was pressed or released, see the
 *  [key callback](@ref glfwSetKeyCallback) instead.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref input_char
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup input
 */
GLFWAPI GLFWcharmodsfun glfwSetCharModsCallback(GLFWwindow* window, GLFWcharmodsfun cbfun);

/*! @brief Sets the mouse button callback.
 *
 *  This function sets the mouse button callback of the specified window, which
 *  is called when a mouse button is pressed or released.
 *
 *  When a window loses input focus, it will generate synthetic mouse button
 *  release events for all pressed mouse buttons.  You can tell these events
 *  from user-generated events by the fact that the synthetic ones are generated
 *  after the focus loss event has been processed, i.e. after the
 *  [window focus callback](@ref glfwSetWindowFocusCallback) has been called.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref input_mouse_button
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter and return value.
 *
 *  @ingroup input
 */
GLFWAPI GLFWmousebuttonfun glfwSetMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun cbfun);

/*! @brief Sets the cursor position callback.
 *
 *  This function sets the cursor position callback of the specified window,
 *  which is called when the cursor is moved.  The callback is provided with the
 *  position, in screen coordinates, relative to the upper-left corner of the
 *  client area of the window.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref cursor_pos
 *
 *  @since Added in version 3.0.  Replaces `glfwSetMousePosCallback`.
 *
 *  @ingroup input
 */
GLFWAPI GLFWcursorposfun glfwSetCursorPosCallback(GLFWwindow* window, GLFWcursorposfun cbfun);

/*! @brief Sets the cursor enter/exit callback.
 *
 *  This function sets the cursor boundary crossing callback of the specified
 *  window, which is called when the cursor enters or leaves the client area of
 *  the window.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref cursor_enter
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup input
 */
GLFWAPI GLFWcursorenterfun glfwSetCursorEnterCallback(GLFWwindow* window, GLFWcursorenterfun cbfun);

/*! @brief Sets the scroll callback.
 *
 *  This function sets the scroll callback of the specified window, which is
 *  called when a scrolling device is used, such as a mouse wheel or scrolling
 *  area of a touchpad.
 *
 *  The scroll callback receives all scrolling input, like that from a mouse
 *  wheel or a touchpad scrolling area.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new scroll callback, or `NULL` to remove the currently
 *  set callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref scrolling
 *
 *  @since Added in version 3.0.  Replaces `glfwSetMouseWheelCallback`.
 *
 *  @ingroup input
 */
GLFWAPI GLFWscrollfun glfwSetScrollCallback(GLFWwindow* window, GLFWscrollfun cbfun);

/*! @brief Sets the file drop callback.
 *
 *  This function sets the file drop callback of the specified window, which is
 *  called when one or more dragged files are dropped on the window.
 *
 *  Because the path array and its strings may have been generated specifically
 *  for that event, they are not guaranteed to be valid after the callback has
 *  returned.  If you wish to use them after the callback returns, you need to
 *  make a deep copy.
 *
 *  @param[in] window The window whose callback to set.
 *  @param[in] cbfun The new file drop callback, or `NULL` to remove the
 *  currently set callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @remark @wayland File drop is currently unimplemented.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref path_drop
 *
 *  @since Added in version 3.1.
 *
 *  @ingroup input
 */
GLFWAPI GLFWdropfun glfwSetDropCallback(GLFWwindow* window, GLFWdropfun cbfun);

/*! @brief Returns whether the specified joystick is present.
 *
 *  This function returns whether the specified joystick is present.
 *
 *  @param[in] jid The [joystick](@ref joysticks) to query.
 *  @return `GLFW_TRUE` if the joystick is present, or `GLFW_FALSE` otherwise.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM and @ref GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref joystick
 *
 *  @since Added in version 3.0.  Replaces `glfwGetJoystickParam`.
 *
 *  @ingroup input
 */
GLFWAPI int glfwJoystickPresent(int jid);

/*! @brief Returns the values of all axes of the specified joystick.
 *
 *  This function returns the values of all axes of the specified joystick.
 *  Each element in the array is a value between -1.0 and 1.0.
 *
 *  Querying a joystick slot with no device present is not an error, but will
 *  cause this function to return `NULL`.  Call @ref glfwJoystickPresent to
 *  check device presence.
 *
 *  @param[in] jid The [joystick](@ref joysticks) to query.
 *  @param[out] count Where to store the number of axis values in the returned
 *  array.  This is set to zero if the joystick is not present or an error
 *  occurred.
 *  @return An array of axis values, or `NULL` if the joystick is not present or
 *  an [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM and @ref GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The returned array is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is valid until the specified joystick is
 *  disconnected, this function is called again for that joystick or the library
 *  is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref joystick_axis
 *
 *  @since Added in version 3.0.  Replaces `glfwGetJoystickPos`.
 *
 *  @ingroup input
 */
GLFWAPI const float* glfwGetJoystickAxes(int jid, int* count);

/*! @brief Returns the state of all buttons of the specified joystick.
 *
 *  This function returns the state of all buttons of the specified joystick.
 *  Each element in the array is either `GLFW_PRESS` or `GLFW_RELEASE`.
 *
 *  Querying a joystick slot with no device present is not an error, but will
 *  cause this function to return `NULL`.  Call @ref glfwJoystickPresent to
 *  check device presence.
 *
 *  @param[in] jid The [joystick](@ref joysticks) to query.
 *  @param[out] count Where to store the number of button states in the returned
 *  array.  This is set to zero if the joystick is not present or an error
 *  occurred.
 *  @return An array of button states, or `NULL` if the joystick is not present
 *  or an [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM and @ref GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The returned array is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is valid until the specified joystick is
 *  disconnected, this function is called again for that joystick or the library
 *  is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref joystick_button
 *
 *  @since Added in version 2.2.
 *  @glfw3 Changed to return a dynamic array.
 *
 *  @ingroup input
 */
GLFWAPI const unsigned char* glfwGetJoystickButtons(int jid, int* count);

/*! @brief Returns the name of the specified joystick.
 *
 *  This function returns the name, encoded as UTF-8, of the specified joystick.
 *  The returned string is allocated and freed by GLFW.  You should not free it
 *  yourself.
 *
 *  Querying a joystick slot with no device present is not an error, but will
 *  cause this function to return `NULL`.  Call @ref glfwJoystickPresent to
 *  check device presence.
 *
 *  @param[in] jid The [joystick](@ref joysticks) to query.
 *  @return The UTF-8 encoded name of the joystick, or `NULL` if the joystick
 *  is not present or an [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_INVALID_ENUM and @ref GLFW_PLATFORM_ERROR.
 *
 *  @pointer_lifetime The returned string is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is valid until the specified joystick is
 *  disconnected, this function is called again for that joystick or the library
 *  is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref joystick_name
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup input
 */
GLFWAPI const char* glfwGetJoystickName(int jid);

/*! @brief Sets the joystick configuration callback.
 *
 *  This function sets the joystick configuration callback, or removes the
 *  currently set callback.  This is called when a joystick is connected to or
 *  disconnected from the system.
 *
 *  @param[in] cbfun The new callback, or `NULL` to remove the currently set
 *  callback.
 *  @return The previously set callback, or `NULL` if no callback was set or the
 *  library had not been [initialized](@ref intro_init).
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref joystick_event
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup input
 */
GLFWAPI GLFWjoystickfun glfwSetJoystickCallback(GLFWjoystickfun cbfun);

/*! @brief Sets the clipboard to the specified string.
 *
 *  This function sets the system clipboard to the specified, UTF-8 encoded
 *  string.
 *
 *  @param[in] window The window that will own the clipboard contents.
 *  @param[in] string A UTF-8 encoded string.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland Clipboard is currently unimplemented.
 *
 *  @pointer_lifetime The specified string is copied before this function
 *  returns.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref clipboard
 *  @sa @ref glfwGetClipboardString
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup input
 */
GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char* string);

/*! @brief Returns the contents of the clipboard as a string.
 *
 *  This function returns the contents of the system clipboard, if it contains
 *  or is convertible to a UTF-8 encoded string.  If the clipboard is empty or
 *  if its contents cannot be converted, `NULL` is returned and a @ref
 *  GLFW_FORMAT_UNAVAILABLE error is generated.
 *
 *  @param[in] window The window that will request the clipboard contents.
 *  @return The contents of the clipboard as a UTF-8 encoded string, or `NULL`
 *  if an [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @remark @wayland Clipboard is currently unimplemented.
 *
 *  @pointer_lifetime The returned string is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is valid until the next call to @ref
 *  glfwGetClipboardString or @ref glfwSetClipboardString, or until the library
 *  is terminated.
 *
 *  @thread_safety This function must only be called from the main thread.
 *
 *  @sa @ref clipboard
 *  @sa @ref glfwSetClipboardString
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup input
 */
GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window);

/*! @brief Returns the value of the GLFW timer.
 *
 *  This function returns the value of the GLFW timer.  Unless the timer has
 *  been set using @ref glfwSetTime, the timer measures time elapsed since GLFW
 *  was initialized.
 *
 *  The resolution of the timer is system dependent, but is usually on the order
 *  of a few micro- or nanoseconds.  It uses the highest-resolution monotonic
 *  time source on each supported platform.
 *
 *  @return The current value, in seconds, or zero if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function may be called from any thread.  Reading and
 *  writing of the internal timer offset is not atomic, so it needs to be
 *  externally synchronized with calls to @ref glfwSetTime.
 *
 *  @sa @ref time
 *
 *  @since Added in version 1.0.
 *
 *  @ingroup input
 */
GLFWAPI double glfwGetTime(void);

/*! @brief Sets the GLFW timer.
 *
 *  This function sets the value of the GLFW timer.  It then continues to count
 *  up from that value.  The value must be a positive finite number less than
 *  or equal to 18446744073.0, which is approximately 584.5 years.
 *
 *  @param[in] time The new value, in seconds.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_INVALID_VALUE.
 *
 *  @remark The upper limit of the timer is calculated as
 *  floor((2<sup>64</sup> - 1) / 10<sup>9</sup>) and is due to implementations
 *  storing nanoseconds in 64 bits.  The limit may be increased in the future.
 *
 *  @thread_safety This function may be called from any thread.  Reading and
 *  writing of the internal timer offset is not atomic, so it needs to be
 *  externally synchronized with calls to @ref glfwGetTime.
 *
 *  @sa @ref time
 *
 *  @since Added in version 2.2.
 *
 *  @ingroup input
 */
GLFWAPI void glfwSetTime(double time);

/*! @brief Returns the current value of the raw timer.
 *
 *  This function returns the current value of the raw timer, measured in
 *  1&nbsp;/&nbsp;frequency seconds.  To get the frequency, call @ref
 *  glfwGetTimerFrequency.
 *
 *  @return The value of the timer, or zero if an 
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref time
 *  @sa @ref glfwGetTimerFrequency
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup input
 */
GLFWAPI uint64_t glfwGetTimerValue(void);

/*! @brief Returns the frequency, in Hz, of the raw timer.
 *
 *  This function returns the frequency, in Hz, of the raw timer.
 *
 *  @return The frequency of the timer, in Hz, or zero if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref time
 *  @sa @ref glfwGetTimerValue
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup input
 */
GLFWAPI uint64_t glfwGetTimerFrequency(void);

/*! @brief Makes the context of the specified window current for the calling
 *  thread.
 *
 *  This function makes the OpenGL or OpenGL ES context of the specified window
 *  current on the calling thread.  A context can only be made current on
 *  a single thread at a time and each thread can have only a single current
 *  context at a time.
 *
 *  By default, making a context non-current implicitly forces a pipeline flush.
 *  On machines that support `GL_KHR_context_flush_control`, you can control
 *  whether a context performs this flush by setting the
 *  [GLFW_CONTEXT_RELEASE_BEHAVIOR](@ref GLFW_CONTEXT_RELEASE_BEHAVIOR_hint)
 *  hint.
 *
 *  The specified window must have an OpenGL or OpenGL ES context.  Specifying
 *  a window without a context will generate a @ref GLFW_NO_WINDOW_CONTEXT
 *  error.
 *
 *  @param[in] window The window whose context to make current, or `NULL` to
 *  detach the current context.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_NO_WINDOW_CONTEXT and @ref GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref context_current
 *  @sa @ref glfwGetCurrentContext
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup context
 */
GLFWAPI void glfwMakeContextCurrent(GLFWwindow* window);

/*! @brief Returns the window whose context is current on the calling thread.
 *
 *  This function returns the window whose OpenGL or OpenGL ES context is
 *  current on the calling thread.
 *
 *  @return The window whose context is current, or `NULL` if no window's
 *  context is current.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref context_current
 *  @sa @ref glfwMakeContextCurrent
 *
 *  @since Added in version 3.0.
 *
 *  @ingroup context
 */
GLFWAPI GLFWwindow* glfwGetCurrentContext(void);

/*! @brief Swaps the front and back buffers of the specified window.
 *
 *  This function swaps the front and back buffers of the specified window when
 *  rendering with OpenGL or OpenGL ES.  If the swap interval is greater than
 *  zero, the GPU driver waits the specified number of screen updates before
 *  swapping the buffers.
 *
 *  The specified window must have an OpenGL or OpenGL ES context.  Specifying
 *  a window without a context will generate a @ref GLFW_NO_WINDOW_CONTEXT
 *  error.
 *
 *  This function does not apply to Vulkan.  If you are rendering with Vulkan,
 *  see `vkQueuePresentKHR` instead.
 *
 *  @param[in] window The window whose buffers to swap.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_NO_WINDOW_CONTEXT and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark __EGL:__ The context of the specified window must be current on the
 *  calling thread.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref buffer_swap
 *  @sa @ref glfwSwapInterval
 *
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle parameter.
 *
 *  @ingroup window
 */
GLFWAPI void glfwSwapBuffers(GLFWwindow* window);

/*! @brief Sets the swap interval for the current context.
 *
 *  This function sets the swap interval for the current OpenGL or OpenGL ES
 *  context, i.e. the number of screen updates to wait from the time @ref
 *  glfwSwapBuffers was called before swapping the buffers and returning.  This
 *  is sometimes called _vertical synchronization_, _vertical retrace
 *  synchronization_ or just _vsync_.
 *
 *  Contexts that support either of the `WGL_EXT_swap_control_tear` and
 *  `GLX_EXT_swap_control_tear` extensions also accept negative swap intervals,
 *  which allow the driver to swap even if a frame arrives a little bit late.
 *  You can check for the presence of these extensions using @ref
 *  glfwExtensionSupported.  For more information about swap tearing, see the
 *  extension specifications.
 *
 *  A context must be current on the calling thread.  Calling this function
 *  without a current context will cause a @ref GLFW_NO_CURRENT_CONTEXT error.
 *
 *  This function does not apply to Vulkan.  If you are rendering with Vulkan,
 *  see the present mode of your swapchain instead.
 *
 *  @param[in] interval The minimum number of screen updates to wait for
 *  until the buffers are swapped by @ref glfwSwapBuffers.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_NO_CURRENT_CONTEXT and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark This function is not called during context creation, leaving the
 *  swap interval set to whatever is the default on that platform.  This is done
 *  because some swap interval extensions used by GLFW do not allow the swap
 *  interval to be reset to zero once it has been set to a non-zero value.
 *
 *  @remark Some GPU drivers do not honor the requested swap interval, either
 *  because of a user setting that overrides the application's request or due to
 *  bugs in the driver.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref buffer_swap
 *  @sa @ref glfwSwapBuffers
 *
 *  @since Added in version 1.0.
 *
 *  @ingroup context
 */
GLFWAPI void glfwSwapInterval(int interval);

/*! @brief Returns whether the specified extension is available.
 *
 *  This function returns whether the specified
 *  [API extension](@ref context_glext) is supported by the current OpenGL or
 *  OpenGL ES context.  It searches both for client API extension and context
 *  creation API extensions.
 *
 *  A context must be current on the calling thread.  Calling this function
 *  without a current context will cause a @ref GLFW_NO_CURRENT_CONTEXT error.
 *
 *  As this functions retrieves and searches one or more extension strings each
 *  call, it is recommended that you cache its results if it is going to be used
 *  frequently.  The extension strings will not change during the lifetime of
 *  a context, so there is no danger in doing this.
 *
 *  This function does not apply to Vulkan.  If you are using Vulkan, see @ref
 *  glfwGetRequiredInstanceExtensions, `vkEnumerateInstanceExtensionProperties`
 *  and `vkEnumerateDeviceExtensionProperties` instead.
 *
 *  @param[in] extension The ASCII encoded name of the extension.
 *  @return `GLFW_TRUE` if the extension is available, or `GLFW_FALSE`
 *  otherwise.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_NO_CURRENT_CONTEXT, @ref GLFW_INVALID_VALUE and @ref
 *  GLFW_PLATFORM_ERROR.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref context_glext
 *  @sa @ref glfwGetProcAddress
 *
 *  @since Added in version 1.0.
 *
 *  @ingroup context
 */
GLFWAPI int glfwExtensionSupported(const char* extension);

/*! @brief Returns the address of the specified function for the current
 *  context.
 *
 *  This function returns the address of the specified OpenGL or OpenGL ES
 *  [core or extension function](@ref context_glext), if it is supported
 *  by the current context.
 *
 *  A context must be current on the calling thread.  Calling this function
 *  without a current context will cause a @ref GLFW_NO_CURRENT_CONTEXT error.
 *
 *  This function does not apply to Vulkan.  If you are rendering with Vulkan,
 *  see @ref glfwGetInstanceProcAddress, `vkGetInstanceProcAddr` and
 *  `vkGetDeviceProcAddr` instead.
 *
 *  @param[in] procname The ASCII encoded name of the function.
 *  @return The address of the function, or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_NO_CURRENT_CONTEXT and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark The address of a given function is not guaranteed to be the same
 *  between contexts.
 *
 *  @remark This function may return a non-`NULL` address despite the
 *  associated version or extension not being available.  Always check the
 *  context version or extension string first.
 *
 *  @pointer_lifetime The returned function pointer is valid until the context
 *  is destroyed or the library is terminated.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref context_glext
 *  @sa @ref glfwExtensionSupported
 *
 *  @since Added in version 1.0.
 *
 *  @ingroup context
 */
GLFWAPI GLFWglproc glfwGetProcAddress(const char* procname);

/*! @brief Returns whether the Vulkan loader has been found.
 *
 *  This function returns whether the Vulkan loader has been found.  This check
 *  is performed by @ref glfwInit.
 *
 *  The availability of a Vulkan loader does not by itself guarantee that window
 *  surface creation or even device creation is possible.  Call @ref
 *  glfwGetRequiredInstanceExtensions to check whether the extensions necessary
 *  for Vulkan surface creation are available and @ref
 *  glfwGetPhysicalDevicePresentationSupport to check whether a queue family of
 *  a physical device supports image presentation.
 *
 *  @return `GLFW_TRUE` if Vulkan is available, or `GLFW_FALSE` otherwise.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref vulkan_support
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup vulkan
 */
GLFWAPI int glfwVulkanSupported(void);

/*! @brief Returns the Vulkan instance extensions required by GLFW.
 *
 *  This function returns an array of names of Vulkan instance extensions required
 *  by GLFW for creating Vulkan surfaces for GLFW windows.  If successful, the
 *  list will always contains `VK_KHR_surface`, so if you don't require any
 *  additional extensions you can pass this list directly to the
 *  `VkInstanceCreateInfo` struct.
 *
 *  If Vulkan is not available on the machine, this function returns `NULL` and
 *  generates a @ref GLFW_API_UNAVAILABLE error.  Call @ref glfwVulkanSupported
 *  to check whether Vulkan is available.
 *
 *  If Vulkan is available but no set of extensions allowing window surface
 *  creation was found, this function returns `NULL`.  You may still use Vulkan
 *  for off-screen rendering and compute work.
 *
 *  @param[out] count Where to store the number of extensions in the returned
 *  array.  This is set to zero if an error occurred.
 *  @return An array of ASCII encoded extension names, or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_API_UNAVAILABLE.
 *
 *  @remark Additional extensions may be required by future versions of GLFW.
 *  You should check if any extensions you wish to enable are already in the
 *  returned array, as it is an error to specify an extension more than once in
 *  the `VkInstanceCreateInfo` struct.
 *
 *  @remark @macos This function currently only supports the
 *  `VK_MVK_macos_surface` extension from MoltenVK.
 *
 *  @pointer_lifetime The returned array is allocated and freed by GLFW.  You
 *  should not free it yourself.  It is guaranteed to be valid only until the
 *  library is terminated.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref vulkan_ext
 *  @sa @ref glfwCreateWindowSurface
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup vulkan
 */
GLFWAPI const char** glfwGetRequiredInstanceExtensions(uint32_t* count);

#if defined(VK_VERSION_1_0)

/*! @brief Returns the address of the specified Vulkan instance function.
 *
 *  This function returns the address of the specified Vulkan core or extension
 *  function for the specified instance.  If instance is set to `NULL` it can
 *  return any function exported from the Vulkan loader, including at least the
 *  following functions:
 *
 *  - `vkEnumerateInstanceExtensionProperties`
 *  - `vkEnumerateInstanceLayerProperties`
 *  - `vkCreateInstance`
 *  - `vkGetInstanceProcAddr`
 *
 *  If Vulkan is not available on the machine, this function returns `NULL` and
 *  generates a @ref GLFW_API_UNAVAILABLE error.  Call @ref glfwVulkanSupported
 *  to check whether Vulkan is available.
 *
 *  This function is equivalent to calling `vkGetInstanceProcAddr` with
 *  a platform-specific query of the Vulkan loader as a fallback.
 *
 *  @param[in] instance The Vulkan instance to query, or `NULL` to retrieve
 *  functions related to instance creation.
 *  @param[in] procname The ASCII encoded name of the function.
 *  @return The address of the function, or `NULL` if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED and @ref
 *  GLFW_API_UNAVAILABLE.
 *
 *  @pointer_lifetime The returned function pointer is valid until the library
 *  is terminated.
 *
 *  @thread_safety This function may be called from any thread.
 *
 *  @sa @ref vulkan_proc
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup vulkan
 */
GLFWAPI GLFWvkproc glfwGetInstanceProcAddress(VkInstance instance, const char* procname);

/*! @brief Returns whether the specified queue family can present images.
 *
 *  This function returns whether the specified queue family of the specified
 *  physical device supports presentation to the platform GLFW was built for.
 *
 *  If Vulkan or the required window surface creation instance extensions are
 *  not available on the machine, or if the specified instance was not created
 *  with the required extensions, this function returns `GLFW_FALSE` and
 *  generates a @ref GLFW_API_UNAVAILABLE error.  Call @ref glfwVulkanSupported
 *  to check whether Vulkan is available and @ref
 *  glfwGetRequiredInstanceExtensions to check what instance extensions are
 *  required.
 *
 *  @param[in] instance The instance that the physical device belongs to.
 *  @param[in] device The physical device that the queue family belongs to.
 *  @param[in] queuefamily The index of the queue family to query.
 *  @return `GLFW_TRUE` if the queue family supports presentation, or
 *  `GLFW_FALSE` otherwise.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_API_UNAVAILABLE and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark @macos This function currently always returns `GLFW_TRUE`, as the
 *  `VK_MVK_macos_surface` extension does not provide
 *  a `vkGetPhysicalDevice*PresentationSupport` type function.
 *
 *  @thread_safety This function may be called from any thread.  For
 *  synchronization details of Vulkan objects, see the Vulkan specification.
 *
 *  @sa @ref vulkan_present
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup vulkan
 */
GLFWAPI int glfwGetPhysicalDevicePresentationSupport(VkInstance instance, VkPhysicalDevice device, uint32_t queuefamily);

/*! @brief Creates a Vulkan surface for the specified window.
 *
 *  This function creates a Vulkan surface for the specified window.
 *
 *  If the Vulkan loader was not found at initialization, this function returns
 *  `VK_ERROR_INITIALIZATION_FAILED` and generates a @ref GLFW_API_UNAVAILABLE
 *  error.  Call @ref glfwVulkanSupported to check whether the Vulkan loader was
 *  found.
 *
 *  If the required window surface creation instance extensions are not
 *  available or if the specified instance was not created with these extensions
 *  enabled, this function returns `VK_ERROR_EXTENSION_NOT_PRESENT` and
 *  generates a @ref GLFW_API_UNAVAILABLE error.  Call @ref
 *  glfwGetRequiredInstanceExtensions to check what instance extensions are
 *  required.
 *
 *  The window surface must be destroyed before the specified Vulkan instance.
 *  It is the responsibility of the caller to destroy the window surface.  GLFW
 *  does not destroy it for you.  Call `vkDestroySurfaceKHR` to destroy the
 *  surface.
 *
 *  @param[in] instance The Vulkan instance to create the surface in.
 *  @param[in] window The window to create the surface for.
 *  @param[in] allocator The allocator to use, or `NULL` to use the default
 *  allocator.
 *  @param[out] surface Where to store the handle of the surface.  This is set
 *  to `VK_NULL_HANDLE` if an error occurred.
 *  @return `VK_SUCCESS` if successful, or a Vulkan error code if an
 *  [error](@ref error_handling) occurred.
 *
 *  @errors Possible errors include @ref GLFW_NOT_INITIALIZED, @ref
 *  GLFW_API_UNAVAILABLE and @ref GLFW_PLATFORM_ERROR.
 *
 *  @remark If an error occurs before the creation call is made, GLFW returns
 *  the Vulkan error code most appropriate for the error.  Appropriate use of
 *  @ref glfwVulkanSupported and @ref glfwGetRequiredInstanceExtensions should
 *  eliminate almost all occurrences of these errors.
 *
 *  @remark @macos This function currently only supports the
 *  `VK_MVK_macos_surface` extension from MoltenVK.
 *
 *  @remark @macos This function creates and sets a `CAMetalLayer` instance for
 *  the window content view, which is required for MoltenVK to function.
 *
 *  @thread_safety This function may be called from any thread.  For
 *  synchronization details of Vulkan objects, see the Vulkan specification.
 *
 *  @sa @ref vulkan_surface
 *  @sa @ref glfwGetRequiredInstanceExtensions
 *
 *  @since Added in version 3.2.
 *
 *  @ingroup vulkan
 */
GLFWAPI VkResult glfwCreateWindowSurface(VkInstance instance, GLFWwindow* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);

#endif /*VK_VERSION_1_0*/


/*************************************************************************
 * Global definition cleanup
 *************************************************************************/

/* ------------------- BEGIN SYSTEM/COMPILER SPECIFIC -------------------- */

#ifdef GLFW_WINGDIAPI_DEFINED
 #undef WINGDIAPI
 #undef GLFW_WINGDIAPI_DEFINED
#endif

#ifdef GLFW_CALLBACK_DEFINED
 #undef CALLBACK
 #undef GLFW_CALLBACK_DEFINED
#endif

/* -------------------- END SYSTEM/COMPILER SPECIFIC --------------------- */


#ifdef __cplusplus
}
#endif

#endif /* _glfw3_h_ */


//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2010-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
// As glfw_config.h.in, this file is used by CMake to produce the
// glfw_config.h configuration header file.  If you are adding a feature
// requiring conditional compilation, this is where to add the macro.
//========================================================================
// As glfw_config.h, this file defines compile-time option macros for a
// specific platform and development environment.  If you are using the
// GLFW CMake files, modify glfw_config.h.in instead of this file.  If you
// are using your own build system, make this file define the appropriate
// macros in whatever way is suitable.
//========================================================================

// Define this to 1 if building GLFW for X11
/* #undef _GLFW_X11 */
// Define this to 1 if building GLFW for Win32
#define _GLFW_WIN32
// Define this to 1 if building GLFW for Cocoa
/* #undef _GLFW_COCOA */
// Define this to 1 if building GLFW for Wayland
/* #undef _GLFW_WAYLAND */
// Define this to 1 if building GLFW for Mir
/* #undef _GLFW_MIR */
// Define this to 1 if building GLFW for OSMesa
/* #undef _GLFW_OSMESA */

// Define this to 1 if building as a shared library / dynamic library / DLL
/* #undef _GLFW_BUILD_DLL */
// Define this to 1 to use Vulkan loader linked statically into application
/* #undef _GLFW_VULKAN_STATIC */

// Define this to 1 to force use of high-performance GPU on hybrid systems
/* #undef _GLFW_USE_HYBRID_HPG */

// Define this to 1 if glfwInit should change the current directory
/* #undef _GLFW_USE_CHDIR */
// Define this to 1 if glfwCreateWindow should populate the menu bar
/* #undef _GLFW_USE_MENUBAR */


//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#ifndef _glfw3_internal_h_
#define _glfw3_internal_h_


#if defined(_GLFW_USE_CONFIG_H)
 #include "glfw_config.h"
#endif

#if defined(GLFW_INCLUDE_GLCOREARB) || \
    defined(GLFW_INCLUDE_ES1)       || \
    defined(GLFW_INCLUDE_ES2)       || \
    defined(GLFW_INCLUDE_ES3)       || \
    defined(GLFW_INCLUDE_ES31)      || \
    defined(GLFW_INCLUDE_ES32)      || \
    defined(GLFW_INCLUDE_NONE)      || \
    defined(GLFW_INCLUDE_GLEXT)     || \
    defined(GLFW_INCLUDE_GLU)       || \
    defined(GLFW_INCLUDE_VULKAN)    || \
    defined(GLFW_DLL)
 #error "You must not define any header option macros when compiling GLFW"
#endif

#define GLFW_INCLUDE_NONE
//#include "../include/GLFW/glfw3.h"
//#include "glfw3.h"

#define _GLFW_INSERT_FIRST      0
#define _GLFW_INSERT_LAST       1

typedef int GLFWbool;

typedef struct _GLFWwndconfig   _GLFWwndconfig;
typedef struct _GLFWctxconfig   _GLFWctxconfig;
typedef struct _GLFWfbconfig    _GLFWfbconfig;
typedef struct _GLFWcontext     _GLFWcontext;
typedef struct _GLFWwindow      _GLFWwindow;
typedef struct _GLFWlibrary     _GLFWlibrary;
typedef struct _GLFWmonitor     _GLFWmonitor;
typedef struct _GLFWcursor      _GLFWcursor;

typedef void (* _GLFWmakecontextcurrentfun)(_GLFWwindow*);
typedef void (* _GLFWswapbuffersfun)(_GLFWwindow*);
typedef void (* _GLFWswapintervalfun)(int);
typedef int (* _GLFWextensionsupportedfun)(const char*);
typedef GLFWglproc (* _GLFWgetprocaddressfun)(const char*);
typedef void (* _GLFWdestroycontextfun)(_GLFWwindow*);

////#define GL_VERSION 0x1f02
//#define GL_NONE	0
//#define GL_COLOR_BUFFER_BIT	0x00004000
//#define GL_UNSIGNED_BYTE 0x1401
////#define GL_EXTENSIONS 0x1f03
#define GL_NUM_EXTENSIONS 0x821d
#define GL_CONTEXT_FLAGS 0x821e
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define GL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define GL_NO_RESET_NOTIFICATION_ARB 0x8261
#define GL_CONTEXT_RELEASE_BEHAVIOR 0x82fb
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82fc
#define GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR 0x00000008

typedef int	GLint;
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned char GLubyte;

typedef void (APIENTRY * PFNGLCLEARPROC)(GLbitfield);
typedef const GLubyte* (APIENTRY * PFNGLGETSTRINGPROC)(GLenum);
typedef void (APIENTRY * PFNGLGETINTEGERVPROC)(GLenum,GLint*);
typedef const GLubyte* (APIENTRY * PFNGLGETSTRINGIPROC)(GLenum,GLuint);

#define VK_NULL_HANDLE 0

typedef void* VkInstance;
typedef void* VkPhysicalDevice;
typedef uint64_t VkSurfaceKHR;
typedef uint32_t VkFlags;
typedef uint32_t VkBool32;

typedef enum VkStructureType
{
    VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR = 1000004000,
    VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR = 1000005000,
    VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR = 1000006000,
    VK_STRUCTURE_TYPE_MIR_SURFACE_CREATE_INFO_KHR = 1000007000,
    VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR = 1000009000,
    VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK = 1000053000,
    VK_STRUCTURE_TYPE_MAX_ENUM = 0x7FFFFFFF
} VkStructureType;

typedef enum VkResult
{
    VK_SUCCESS = 0,
    VK_NOT_READY = 1,
    VK_TIMEOUT = 2,
    VK_EVENT_SET = 3,
    VK_EVENT_RESET = 4,
    VK_INCOMPLETE = 5,
    VK_ERROR_OUT_OF_HOST_MEMORY = -1,
    VK_ERROR_OUT_OF_DEVICE_MEMORY = -2,
    VK_ERROR_INITIALIZATION_FAILED = -3,
    VK_ERROR_DEVICE_LOST = -4,
    VK_ERROR_MEMORY_MAP_FAILED = -5,
    VK_ERROR_LAYER_NOT_PRESENT = -6,
    VK_ERROR_EXTENSION_NOT_PRESENT = -7,
    VK_ERROR_FEATURE_NOT_PRESENT = -8,
    VK_ERROR_INCOMPATIBLE_DRIVER = -9,
    VK_ERROR_TOO_MANY_OBJECTS = -10,
    VK_ERROR_FORMAT_NOT_SUPPORTED = -11,
    VK_ERROR_SURFACE_LOST_KHR = -1000000000,
    VK_SUBOPTIMAL_KHR = 1000001003,
    VK_ERROR_OUT_OF_DATE_KHR = -1000001004,
    VK_ERROR_INCOMPATIBLE_DISPLAY_KHR = -1000003001,
    VK_ERROR_NATIVE_WINDOW_IN_USE_KHR = -1000000001,
    VK_ERROR_VALIDATION_FAILED_EXT = -1000011001,
    VK_RESULT_MAX_ENUM = 0x7FFFFFFF
} VkResult;

typedef struct VkAllocationCallbacks VkAllocationCallbacks;

typedef struct VkExtensionProperties
{
    char            extensionName[256];
    uint32_t        specVersion;
} VkExtensionProperties;

typedef void (APIENTRY * PFN_vkVoidFunction)(void);

#if defined(_GLFW_VULKAN_STATIC)
  PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance,const char*);
  VkResult vkEnumerateInstanceExtensionProperties(const char*,uint32_t*,VkExtensionProperties*);
#else
  typedef PFN_vkVoidFunction (APIENTRY * PFN_vkGetInstanceProcAddr)(VkInstance,const char*);
  typedef VkResult (APIENTRY * PFN_vkEnumerateInstanceExtensionProperties)(const char*,uint32_t*,VkExtensionProperties*);
  #define vkEnumerateInstanceExtensionProperties _glfw.vk.EnumerateInstanceExtensionProperties
  #define vkGetInstanceProcAddr _glfw.vk.GetInstanceProcAddr
#endif

#if defined(_GLFW_COCOA)
 #include "cocoa_platform.h"
#elif defined(_GLFW_WIN32)
 //#include "win32_platform.h"
 //========================================================================
// GLFW 3.3 Win32 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#ifndef _glfw3_win32_platform_h_
#define _glfw3_win32_platform_h_

// We don't need all the fancy stuff
#ifndef NOMINMAX
 #define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
 #define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
#endif

// This is a workaround for the fact that glfw3.h needs to export APIENTRY (for
// example to allow applications to correctly declare a GL_ARB_debug_output
// callback) but windows.h assumes no one will define APIENTRY before it does
#undef APIENTRY

// GLFW on Windows is Unicode only and does not work in MBCS mode
#ifndef UNICODE
 #define UNICODE
#endif

// GLFW requires Windows XP or later
#if WINVER < 0x0501
 #undef WINVER
 #define WINVER 0x0501
#endif
#if _WIN32_WINNT < 0x0501
 #undef _WIN32_WINNT
 #define _WIN32_WINNT 0x0501
#endif

// GLFW uses DirectInput8 interfaces
#define DIRECTINPUT_VERSION 0x0800

#include <wctype.h>
#include <windows.h>
#include <mmsystem.h>
#include <dinput.h>
#include <xinput.h>
#include <dbt.h>

#if defined(_MSC_VER)
 #include <malloc.h>
 #define strdup _strdup
#endif

// HACK: Define macros that some windows.h variants don't
#ifndef WM_MOUSEHWHEEL
 #define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef WM_DWMCOMPOSITIONCHANGED
 #define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif
#ifndef WM_COPYGLOBALDATA
 #define WM_COPYGLOBALDATA 0x0049
#endif
#ifndef WM_UNICHAR
 #define WM_UNICHAR 0x0109
#endif
#ifndef UNICODE_NOCHAR
 #define UNICODE_NOCHAR 0xFFFF
#endif
#ifndef WM_DPICHANGED
 #define WM_DPICHANGED 0x02E0
#endif
#ifndef GET_XBUTTON_WPARAM
 #define GET_XBUTTON_WPARAM(w) (HIWORD(w))
#endif
#ifndef EDS_ROTATEDMODE
 #define EDS_ROTATEDMODE 0x00000004
#endif
#ifndef DISPLAY_DEVICE_ACTIVE
 #define DISPLAY_DEVICE_ACTIVE 0x00000001
#endif

#if WINVER < 0x0601
typedef struct tagCHANGEFILTERSTRUCT
{
    DWORD cbSize;
    DWORD ExtStatus;

} CHANGEFILTERSTRUCT, *PCHANGEFILTERSTRUCT;
#ifndef MSGFLT_ALLOW
 #define MSGFLT_ALLOW 1
#endif
#endif /*Windows 7*/

#ifndef DPI_ENUMS_DECLARED
typedef enum PROCESS_DPI_AWARENESS
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
#endif /*DPI_ENUMS_DECLARED*/

// HACK: Define macros that some xinput.h variants don't
#ifndef XINPUT_CAPS_WIRELESS
 #define XINPUT_CAPS_WIRELESS 0x0002
#endif
#ifndef XINPUT_DEVSUBTYPE_WHEEL
 #define XINPUT_DEVSUBTYPE_WHEEL 0x02
#endif
#ifndef XINPUT_DEVSUBTYPE_ARCADE_STICK
 #define XINPUT_DEVSUBTYPE_ARCADE_STICK 0x03
#endif
#ifndef XINPUT_DEVSUBTYPE_FLIGHT_STICK
 #define XINPUT_DEVSUBTYPE_FLIGHT_STICK 0x04
#endif
#ifndef XINPUT_DEVSUBTYPE_DANCE_PAD
 #define XINPUT_DEVSUBTYPE_DANCE_PAD 0x05
#endif
#ifndef XINPUT_DEVSUBTYPE_GUITAR
 #define XINPUT_DEVSUBTYPE_GUITAR 0x06
#endif
#ifndef XINPUT_DEVSUBTYPE_DRUM_KIT
 #define XINPUT_DEVSUBTYPE_DRUM_KIT 0x08
#endif
#ifndef XINPUT_DEVSUBTYPE_ARCADE_PAD
 #define XINPUT_DEVSUBTYPE_ARCADE_PAD 0x13
#endif
#ifndef XUSER_MAX_COUNT
 #define XUSER_MAX_COUNT 4
#endif

// HACK: Define macros that some dinput.h variants don't
#ifndef DIDFT_OPTIONAL
 #define DIDFT_OPTIONAL	0x80000000
#endif

// winmm.dll function pointer typedefs
typedef DWORD (WINAPI * PFN_timeGetTime)(void);
#define _glfw_timeGetTime _glfw.win32.winmm.timeGetTime

// xinput.dll function pointer typedefs
typedef DWORD (WINAPI * PFN_XInputGetCapabilities)(DWORD,DWORD,XINPUT_CAPABILITIES*);
typedef DWORD (WINAPI * PFN_XInputGetState)(DWORD,XINPUT_STATE*);
#define _glfw_XInputGetCapabilities _glfw.win32.xinput.XInputGetCapabilities
#define _glfw_XInputGetState _glfw.win32.xinput.XInputGetState

// dinput8.dll function pointer typedefs
typedef HRESULT (WINAPI * PFN_DirectInput8Create)(HINSTANCE,DWORD,REFIID,LPVOID*,LPUNKNOWN);
#define _glfw_DirectInput8Create _glfw.win32.dinput8.DirectInput8Create

// user32.dll function pointer typedefs
typedef BOOL (WINAPI * PFN_SetProcessDPIAware)(void);
typedef BOOL (WINAPI * PFN_ChangeWindowMessageFilterEx)(HWND,UINT,DWORD,PCHANGEFILTERSTRUCT);
#define _glfw_SetProcessDPIAware _glfw.win32.user32.SetProcessDPIAware
#define _glfw_ChangeWindowMessageFilterEx _glfw.win32.user32.ChangeWindowMessageFilterEx

// dwmapi.dll function pointer typedefs
typedef HRESULT (WINAPI * PFN_DwmIsCompositionEnabled)(BOOL*);
typedef HRESULT (WINAPI * PFN_DwmFlush)(VOID);
#define _glfw_DwmIsCompositionEnabled _glfw.win32.dwmapi.DwmIsCompositionEnabled
#define _glfw_DwmFlush _glfw.win32.dwmapi.DwmFlush

// shcore.dll function pointer typedefs
typedef HRESULT (WINAPI * PFN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
#define _glfw_SetProcessDpiAwareness _glfw.win32.shcore.SetProcessDpiAwareness

typedef VkFlags VkWin32SurfaceCreateFlagsKHR;

typedef struct VkWin32SurfaceCreateInfoKHR
{
    VkStructureType                 sType;
    const void*                     pNext;
    VkWin32SurfaceCreateFlagsKHR    flags;
    HINSTANCE                       hinstance;
    HWND                            hwnd;
} VkWin32SurfaceCreateInfoKHR;

typedef VkResult (APIENTRY *PFN_vkCreateWin32SurfaceKHR)(VkInstance,const VkWin32SurfaceCreateInfoKHR*,const VkAllocationCallbacks*,VkSurfaceKHR*);
typedef VkBool32 (APIENTRY *PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(VkPhysicalDevice,uint32_t);

//#include "win32_joystick.h"
//========================================================================
// GLFW 3.3 Win32 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#ifndef _glfw3_win32_joystick_h_
#define _glfw3_win32_joystick_h_

#define _GLFW_PLATFORM_LIBRARY_JOYSTICK_STATE \
    _GLFWjoystickWin32 win32_js[GLFW_JOYSTICK_LAST + 1]

// Joystick element (axis, button or slider)
//
typedef struct _GLFWjoyobjectWin32
{
    int                     offset;
    int                     type;
} _GLFWjoyobjectWin32;

// Win32-specific per-joystick data
//
typedef struct _GLFWjoystickWin32
{
    GLFWbool                present;
    float*                  axes;
    int                     axisCount;
    unsigned char*          buttons;
    int                     buttonCount;
    _GLFWjoyobjectWin32*    objects;
    int                     objectCount;
    char*                   name;
    IDirectInputDevice8W*   device;
    DWORD                   index;
    GUID                    guid;
} _GLFWjoystickWin32;


void _glfwInitJoysticksWin32(void);
void _glfwTerminateJoysticksWin32(void);
void _glfwDetectJoystickConnectionWin32(void);
void _glfwDetectJoystickDisconnectionWin32(void);

#endif // _glfw3_win32_joystick_h_


//#include "wgl_context.h"
//========================================================================
// GLFW 3.3 WGL - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#ifndef _glfw3_wgl_context_h_
#define _glfw3_wgl_context_h_


#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_TYPE_RGBA_ARB 0x202b
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_NO_ACCELERATION_ARB 0x2025
#define WGL_RED_BITS_ARB 0x2015
#define WGL_RED_SHIFT_ARB 0x2016
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_GREEN_SHIFT_ARB 0x2018
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_BLUE_SHIFT_ARB 0x201a
#define WGL_ALPHA_BITS_ARB 0x201b
#define WGL_ALPHA_SHIFT_ARB 0x201c
#define WGL_ACCUM_BITS_ARB 0x201d
#define WGL_ACCUM_RED_BITS_ARB 0x201e
#define WGL_ACCUM_GREEN_BITS_ARB 0x201f
#define WGL_ACCUM_BLUE_BITS_ARB 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_AUX_BUFFERS_ARB 0x2024
#define WGL_STEREO_ARB 0x2012
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_SAMPLES_ARB 0x2042
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20a9
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
#define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define WGL_NO_RESET_NOTIFICATION_ARB 0x8261
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097
#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098

#define ERROR_INVALID_VERSION_ARB 0x2095
#define ERROR_INVALID_PROFILE_ARB 0x2096

typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int);
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC,int,int,UINT,const int*,int*);
typedef const char* (WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
typedef const char* (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC,HGLRC,const int*);

typedef HGLRC (WINAPI * PFN_wglCreateContext)(HDC);
typedef BOOL (WINAPI * PFN_wglDeleteContext)(HGLRC);
typedef PROC (WINAPI * PFN_wglGetProcAddress)(LPCSTR);
typedef HDC (WINAPI * PFN_wglGetCurrentDC)(void);
typedef BOOL (WINAPI * PFN_wglMakeCurrent)(HDC,HGLRC);
typedef BOOL (WINAPI * PFN_wglShareLists)(HGLRC,HGLRC);

// opengl32.dll function pointer typedefs
#define wglCreateContext _glfw.wgl.CreateContext
#define wglDeleteContext _glfw.wgl.DeleteContext
#define wglGetProcAddress _glfw.wgl.GetProcAddress
#define wglGetCurrentDC _glfw.wgl.GetCurrentDC
#define wglMakeCurrent _glfw.wgl.MakeCurrent
#define wglShareLists _glfw.wgl.ShareLists

#define _GLFW_RECREATION_NOT_NEEDED 0
#define _GLFW_RECREATION_REQUIRED   1
#define _GLFW_RECREATION_IMPOSSIBLE 2

#define _GLFW_PLATFORM_CONTEXT_STATE            _GLFWcontextWGL wgl
#define _GLFW_PLATFORM_LIBRARY_CONTEXT_STATE    _GLFWlibraryWGL wgl


// WGL-specific per-context data
//
typedef struct _GLFWcontextWGL
{
    HDC       dc;
    HGLRC     handle;
    int       interval;

} _GLFWcontextWGL;

// WGL-specific global data
//
typedef struct _GLFWlibraryWGL
{
    HINSTANCE                           instance;
    PFN_wglCreateContext                CreateContext;
    PFN_wglDeleteContext                DeleteContext;
    PFN_wglGetProcAddress               GetProcAddress;
    PFN_wglGetCurrentDC                 GetCurrentDC;
    PFN_wglMakeCurrent                  MakeCurrent;
    PFN_wglShareLists                   ShareLists;

    GLFWbool                            extensionsLoaded;

    PFNWGLSWAPINTERVALEXTPROC           SwapIntervalEXT;
    PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB;
    PFNWGLGETEXTENSIONSSTRINGEXTPROC    GetExtensionsStringEXT;
    PFNWGLGETEXTENSIONSSTRINGARBPROC    GetExtensionsStringARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC   CreateContextAttribsARB;
    GLFWbool                            EXT_swap_control;
    GLFWbool                            ARB_multisample;
    GLFWbool                            ARB_framebuffer_sRGB;
    GLFWbool                            EXT_framebuffer_sRGB;
    GLFWbool                            ARB_pixel_format;
    GLFWbool                            ARB_create_context;
    GLFWbool                            ARB_create_context_profile;
    GLFWbool                            EXT_create_context_es2_profile;
    GLFWbool                            ARB_create_context_robustness;
    GLFWbool                            ARB_context_flush_control;

} _GLFWlibraryWGL;


GLFWbool _glfwInitWGL(void);
void _glfwTerminateWGL(void);
GLFWbool _glfwCreateContextWGL(_GLFWwindow* window,
                               const _GLFWctxconfig* ctxconfig,
                               const _GLFWfbconfig* fbconfig);

#endif // _glfw3_wgl_context_h_

//#include "egl_context.h"
//========================================================================
// GLFW 3.3 EGL - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#ifndef _glfw3_egl_context_h_
#define _glfw3_egl_context_h_

#if defined(_GLFW_USE_EGLPLATFORM_H)
 #include <EGL/eglplatform.h>
#elif defined(_GLFW_WIN32)
 #define EGLAPIENTRY __stdcall
typedef HDC EGLNativeDisplayType;
typedef HWND EGLNativeWindowType;
#elif defined(_GLFW_COCOA)
 #define EGLAPIENTRY
typedef void* EGLNativeDisplayType;
typedef id EGLNativeWindowType;
#elif defined(_GLFW_X11)
 #define EGLAPIENTRY
typedef Display* EGLNativeDisplayType;
typedef Window EGLNativeWindowType;
#elif defined(_GLFW_WAYLAND)
 #define EGLAPIENTRY
typedef struct wl_display* EGLNativeDisplayType;
typedef struct wl_egl_window* EGLNativeWindowType;
#elif defined(_GLFW_MIR)
 #define EGLAPIENTRY
typedef MirEGLNativeDisplayType EGLNativeDisplayType;
typedef MirEGLNativeWindowType EGLNativeWindowType;
#else
 #error "No supported EGL platform selected"
#endif

#define EGL_SUCCESS	0x3000
#define EGL_NOT_INITIALIZED	0x3001
#define EGL_BAD_ACCESS 0x3002
#define EGL_BAD_ALLOC 0x3003
#define EGL_BAD_ATTRIBUTE 0x3004
#define EGL_BAD_CONFIG 0x3005
#define EGL_BAD_CONTEXT	0x3006
#define EGL_BAD_CURRENT_SURFACE	0x3007
#define EGL_BAD_DISPLAY	0x3008
#define EGL_BAD_MATCH 0x3009
#define EGL_BAD_NATIVE_PIXMAP 0x300a
#define EGL_BAD_NATIVE_WINDOW 0x300b
#define EGL_BAD_PARAMETER 0x300c
#define EGL_BAD_SURFACE	0x300d
#define EGL_CONTEXT_LOST 0x300e
#define EGL_COLOR_BUFFER_TYPE 0x303f
#define EGL_RGB_BUFFER 0x308e
#define EGL_SURFACE_TYPE 0x3033
#define EGL_WINDOW_BIT 0x0004
#define EGL_RENDERABLE_TYPE	0x3040
#define EGL_OPENGL_ES_BIT 0x0001
#define EGL_OPENGL_ES2_BIT 0x0004
#define EGL_OPENGL_BIT 0x0008
#define EGL_ALPHA_SIZE 0x3021
#define EGL_BLUE_SIZE 0x3022
#define EGL_GREEN_SIZE 0x3023
#define EGL_RED_SIZE 0x3024
#define EGL_DEPTH_SIZE 0x3025
#define EGL_STENCIL_SIZE 0x3026
#define EGL_SAMPLES	0x3031
#define EGL_OPENGL_ES_API 0x30a0
#define EGL_OPENGL_API 0x30a2
#define EGL_NONE 0x3038
#define EGL_EXTENSIONS 0x3055
#define EGL_CONTEXT_CLIENT_VERSION 0x3098
#define EGL_NATIVE_VISUAL_ID 0x302e
#define EGL_NO_SURFACE ((EGLSurface) 0)
#define EGL_NO_DISPLAY ((EGLDisplay) 0)
#define EGL_NO_CONTEXT ((EGLContext) 0)
#define EGL_DEFAULT_DISPLAY ((EGLNativeDisplayType) 0)

#define EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR 0x00000002
#define EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR 0x00000001
#define EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR 0x00000002
#define EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR 0x00000001
#define EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR 0x31bd
#define EGL_NO_RESET_NOTIFICATION_KHR 0x31be
#define EGL_LOSE_CONTEXT_ON_RESET_KHR 0x31bf
#define EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR 0x00000004
#define EGL_CONTEXT_MAJOR_VERSION_KHR 0x3098
#define EGL_CONTEXT_MINOR_VERSION_KHR 0x30fb
#define EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR 0x30fd
#define EGL_CONTEXT_FLAGS_KHR 0x30fc
#define EGL_CONTEXT_OPENGL_NO_ERROR_KHR 0x31b3
#define EGL_GL_COLORSPACE_KHR 0x309d
#define EGL_GL_COLORSPACE_SRGB_KHR 0x3089

typedef int EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;
typedef void* EGLConfig;
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLSurface;

// EGL function pointer typedefs
typedef EGLBoolean (EGLAPIENTRY * PFN_eglGetConfigAttrib)(EGLDisplay,EGLConfig,EGLint,EGLint*);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglGetConfigs)(EGLDisplay,EGLConfig*,EGLint,EGLint*);
typedef EGLDisplay (EGLAPIENTRY * PFN_eglGetDisplay)(EGLNativeDisplayType);
typedef EGLint (EGLAPIENTRY * PFN_eglGetError)(void);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglInitialize)(EGLDisplay,EGLint*,EGLint*);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglTerminate)(EGLDisplay);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglBindAPI)(EGLenum);
typedef EGLContext (EGLAPIENTRY * PFN_eglCreateContext)(EGLDisplay,EGLConfig,EGLContext,const EGLint*);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglDestroySurface)(EGLDisplay,EGLSurface);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglDestroyContext)(EGLDisplay,EGLContext);
typedef EGLSurface (EGLAPIENTRY * PFN_eglCreateWindowSurface)(EGLDisplay,EGLConfig,EGLNativeWindowType,const EGLint*);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglMakeCurrent)(EGLDisplay,EGLSurface,EGLSurface,EGLContext);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglSwapBuffers)(EGLDisplay,EGLSurface);
typedef EGLBoolean (EGLAPIENTRY * PFN_eglSwapInterval)(EGLDisplay,EGLint);
typedef const char* (EGLAPIENTRY * PFN_eglQueryString)(EGLDisplay,EGLint);
typedef GLFWglproc (EGLAPIENTRY * PFN_eglGetProcAddress)(const char*);
#define eglGetConfigAttrib _glfw.egl.GetConfigAttrib
#define eglGetConfigs _glfw.egl.GetConfigs
#define eglGetDisplay _glfw.egl.GetDisplay
#define eglGetError _glfw.egl.GetError
#define eglInitialize _glfw.egl.Initialize
#define eglTerminate _glfw.egl.Terminate
#define eglBindAPI _glfw.egl.BindAPI
#define eglCreateContext _glfw.egl.CreateContext
#define eglDestroySurface _glfw.egl.DestroySurface
#define eglDestroyContext _glfw.egl.DestroyContext
#define eglCreateWindowSurface _glfw.egl.CreateWindowSurface
#define eglMakeCurrent _glfw.egl.MakeCurrent
#define eglSwapBuffers _glfw.egl.SwapBuffers
#define eglSwapInterval _glfw.egl.SwapInterval
#define eglQueryString _glfw.egl.QueryString
#define eglGetProcAddress _glfw.egl.GetProcAddress

#define _GLFW_EGL_CONTEXT_STATE            _GLFWcontextEGL egl
#define _GLFW_EGL_LIBRARY_CONTEXT_STATE    _GLFWlibraryEGL egl


// EGL-specific per-context data
//
typedef struct _GLFWcontextEGL
{
   EGLConfig        config;
   EGLContext       handle;
   EGLSurface       surface;

   void*            client;

} _GLFWcontextEGL;

// EGL-specific global data
//
typedef struct _GLFWlibraryEGL
{
    EGLDisplay      display;
    EGLint          major, minor;
    GLFWbool        prefix;

    GLFWbool        KHR_create_context;
    GLFWbool        KHR_create_context_no_error;
    GLFWbool        KHR_gl_colorspace;
    GLFWbool        KHR_get_all_proc_addresses;

    void*           handle;

    PFN_eglGetConfigAttrib      GetConfigAttrib;
    PFN_eglGetConfigs           GetConfigs;
    PFN_eglGetDisplay           GetDisplay;
    PFN_eglGetError             GetError;
    PFN_eglInitialize           Initialize;
    PFN_eglTerminate            Terminate;
    PFN_eglBindAPI              BindAPI;
    PFN_eglCreateContext        CreateContext;
    PFN_eglDestroySurface       DestroySurface;
    PFN_eglDestroyContext       DestroyContext;
    PFN_eglCreateWindowSurface  CreateWindowSurface;
    PFN_eglMakeCurrent          MakeCurrent;
    PFN_eglSwapBuffers          SwapBuffers;
    PFN_eglSwapInterval         SwapInterval;
    PFN_eglQueryString          QueryString;
    PFN_eglGetProcAddress       GetProcAddress;

} _GLFWlibraryEGL;


GLFWbool _glfwInitEGL(void);
void _glfwTerminateEGL(void);
GLFWbool _glfwCreateContextEGL(_GLFWwindow* window,
                               const _GLFWctxconfig* ctxconfig,
                               const _GLFWfbconfig* fbconfig);
#if defined(_GLFW_X11)
GLFWbool _glfwChooseVisualEGL(const _GLFWctxconfig* ctxconfig,
                              const _GLFWfbconfig* fbconfig,
                              Visual** visual, int* depth);
#endif /*_GLFW_X11*/

#endif // _glfw3_egl_context_h_



#define _GLFW_WNDCLASSNAME L"GLFW30"

#define _glfw_dlopen(name) LoadLibraryA(name)
#define _glfw_dlclose(handle) FreeLibrary((HMODULE) handle)
#define _glfw_dlsym(handle, name) GetProcAddress((HMODULE) handle, name)

#define _GLFW_EGL_NATIVE_WINDOW  ((EGLNativeWindowType) window->win32.handle)
#define _GLFW_EGL_NATIVE_DISPLAY EGL_DEFAULT_DISPLAY

#define _GLFW_PLATFORM_WINDOW_STATE         _GLFWwindowWin32  win32
#define _GLFW_PLATFORM_LIBRARY_WINDOW_STATE _GLFWlibraryWin32 win32
#define _GLFW_PLATFORM_LIBRARY_TIME_STATE   _GLFWtimeWin32    win32_time
#define _GLFW_PLATFORM_LIBRARY_TLS_STATE    _GLFWtlsWin32     win32_tls
#define _GLFW_PLATFORM_MONITOR_STATE        _GLFWmonitorWin32 win32
#define _GLFW_PLATFORM_CURSOR_STATE         _GLFWcursorWin32  win32


// Win32-specific per-window data
//
typedef struct _GLFWwindowWin32
{
    HWND                handle;
    HICON               bigIcon;
    HICON               smallIcon;

    GLFWbool            cursorTracked;
    GLFWbool            iconified;
    GLFWbool            maximized;

    // The last received cursor position, regardless of source
    int                 lastCursorPosX, lastCursorPosY;

} _GLFWwindowWin32;

// Win32-specific global data
//
typedef struct _GLFWlibraryWin32
{
    HWND                helperWindowHandle;
    DWORD               foregroundLockTimeout;
    char*               clipboardString;
    char                keyName[64];
    short int           keycodes[512];
    short int           scancodes[GLFW_KEY_LAST + 1];
    // Where to place the cursor when re-enabled
    double              restoreCursorPosX, restoreCursorPosY;
    // The window whose disabled cursor mode is active
    _GLFWwindow*        disabledCursorWindow;

    struct {
        HINSTANCE                       instance;
        PFN_timeGetTime                 timeGetTime;
    } winmm;

    struct {
        HINSTANCE                       instance;
        PFN_DirectInput8Create          DirectInput8Create;
        IDirectInput8W*                 api;
    } dinput8;

    struct {
        HINSTANCE                       instance;
        PFN_XInputGetCapabilities       XInputGetCapabilities;
        PFN_XInputGetState              XInputGetState;
    } xinput;

    struct {
        HINSTANCE                       instance;
        PFN_SetProcessDPIAware          SetProcessDPIAware;
        PFN_ChangeWindowMessageFilterEx ChangeWindowMessageFilterEx;
    } user32;

    struct {
        HINSTANCE                       instance;
        PFN_DwmIsCompositionEnabled     DwmIsCompositionEnabled;
        PFN_DwmFlush                    DwmFlush;
    } dwmapi;

    struct {
        HINSTANCE                       instance;
        PFN_SetProcessDpiAwareness      SetProcessDpiAwareness;
    } shcore;

} _GLFWlibraryWin32;

// Win32-specific per-monitor data
//
typedef struct _GLFWmonitorWin32
{
    // This size matches the static size of DISPLAY_DEVICE.DeviceName
    WCHAR               adapterName[32];
    WCHAR               displayName[32];
    char                publicAdapterName[64];
    char                publicDisplayName[64];
    GLFWbool            modesPruned;
    GLFWbool            modeChanged;

} _GLFWmonitorWin32;

// Win32-specific per-cursor data
//
typedef struct _GLFWcursorWin32
{
    HCURSOR             handle;

} _GLFWcursorWin32;

// Win32-specific global timer data
//
typedef struct _GLFWtimeWin32
{
    GLFWbool            hasPC;
    uint64_t            frequency;

} _GLFWtimeWin32;

// Win32-specific global TLS data
//
typedef struct _GLFWtlsWin32
{
    GLFWbool            allocated;
    DWORD               context;

} _GLFWtlsWin32;


GLFWbool _glfwRegisterWindowClassWin32(void);
void _glfwUnregisterWindowClassWin32(void);

GLFWbool _glfwInitThreadLocalStorageWin32(void);
void _glfwTerminateThreadLocalStorageWin32(void);

WCHAR* _glfwCreateWideStringFromUTF8Win32(const char* source);
char* _glfwCreateUTF8FromWideStringWin32(const WCHAR* source);

void _glfwInitTimerWin32(void);

void _glfwPollMonitorsWin32(void);
GLFWbool _glfwSetVideoModeWin32(_GLFWmonitor* monitor, const GLFWvidmode* desired);
void _glfwRestoreVideoModeWin32(_GLFWmonitor* monitor);

#endif // _glfw3_win32_platform_h_

#elif defined(_GLFW_X11)
 #include "x11_platform.h"
#elif defined(_GLFW_WAYLAND)
 #include "wl_platform.h"
#elif defined(_GLFW_MIR)
 #include "mir_platform.h"
#elif defined(_GLFW_OSMESA)
 #include "osmesa_platform.h"
#else
 #error "No supported window creation API selected"
#endif


//========================================================================
// Doxygen group definitions
//========================================================================

/*! @defgroup platform Platform interface
 *  @brief The interface implemented by the platform-specific code.
 *
 *  The platform API is the interface exposed by the platform-specific code for
 *  each platform and is called by the shared code of the public API It mirrors
 *  the public API except it uses objects instead of handles.
 */
/*! @defgroup event Event interface
 *  @brief The interface used by the platform-specific code to report events.
 *
 *  The event API is used by the platform-specific code to notify the shared
 *  code of events that can be translated into state changes and/or callback
 *  calls.
 */
/*! @defgroup utility Utility functions
 *  @brief Various utility functions for internal use.
 *
 *  These functions are shared code and may be used by any part of GLFW
 *  Each platform may add its own utility functions, but those must only be
 *  called by the platform-specific code
 */


//========================================================================
// Helper macros
//========================================================================

// Constructs a version number string from the public header macros
#define _GLFW_CONCAT_VERSION(m, n, r) #m "." #n "." #r
#define _GLFW_MAKE_VERSION(m, n, r) _GLFW_CONCAT_VERSION(m, n, r)
#define _GLFW_VERSION_NUMBER _GLFW_MAKE_VERSION(GLFW_VERSION_MAJOR, \
                                                GLFW_VERSION_MINOR, \
                                                GLFW_VERSION_REVISION)

// Checks for whether the library has been initialized
#define _GLFW_REQUIRE_INIT()                         \
    if (!_glfwInitialized)                           \
    {                                                \
        _glfwInputError(GLFW_NOT_INITIALIZED, NULL); \
        return;                                      \
    }
#define _GLFW_REQUIRE_INIT_OR_RETURN(x)              \
    if (!_glfwInitialized)                           \
    {                                                \
        _glfwInputError(GLFW_NOT_INITIALIZED, NULL); \
        return x;                                    \
    }

// Swaps the provided pointers
#define _GLFW_SWAP_POINTERS(x, y) \
    {                             \
        void* t;                  \
        t = x;                    \
        x = y;                    \
        y = t;                    \
    }


//========================================================================
// Platform-independent structures
//========================================================================

/*! @brief Window configuration.
 *
 *  Parameters relating to the creation of the window but not directly related
 *  to the framebuffer.  This is used to pass window creation parameters from
 *  shared code to the platform API.
 */
struct _GLFWwndconfig
{
    int           width;
    int           height;
    const char*   title;
    GLFWbool      resizable;
    GLFWbool      visible;
    GLFWbool      decorated;
    GLFWbool      focused;
    GLFWbool      autoIconify;
    GLFWbool      floating;
    GLFWbool      maximized;
    struct {
        GLFWbool  retina;
        GLFWbool  frame;
    } ns;
};

/*! @brief Context configuration.
 *
 *  Parameters relating to the creation of the context but not directly related
 *  to the framebuffer.  This is used to pass context creation parameters from
 *  shared code to the platform API.
 */
struct _GLFWctxconfig
{
    int           client;
    int           source;
    int           major;
    int           minor;
    GLFWbool      forward;
    GLFWbool      debug;
    GLFWbool      noerror;
    int           profile;
    int           robustness;
    int           release;
    _GLFWwindow*  share;
};

/*! @brief Framebuffer configuration.
 *
 *  This describes buffers and their sizes.  It also contains
 *  a platform-specific ID used to map back to the backend API object.
 *
 *  It is used to pass framebuffer parameters from shared code to the platform
 *  API and also to enumerate and select available framebuffer configs.
 */
struct _GLFWfbconfig
{
    int         redBits;
    int         greenBits;
    int         blueBits;
    int         alphaBits;
    int         depthBits;
    int         stencilBits;
    int         accumRedBits;
    int         accumGreenBits;
    int         accumBlueBits;
    int         accumAlphaBits;
    int         auxBuffers;
    GLFWbool    stereo;
    int         samples;
    GLFWbool    sRGB;
    GLFWbool    doublebuffer;
    uintptr_t   handle;
};

/*! @brief Context structure.
 */
struct _GLFWcontext
{
    int                 client;
    int                 source;
    int                 major, minor, revision;
    GLFWbool            forward, debug, noerror;
    int                 profile;
    int                 robustness;
    int                 release;

    PFNGLGETSTRINGIPROC GetStringi;
    PFNGLGETINTEGERVPROC GetIntegerv;
    PFNGLGETSTRINGPROC  GetString;

    _GLFWmakecontextcurrentfun  makeCurrent;
    _GLFWswapbuffersfun         swapBuffers;
    _GLFWswapintervalfun        swapInterval;
    _GLFWextensionsupportedfun  extensionSupported;
    _GLFWgetprocaddressfun      getProcAddress;
    _GLFWdestroycontextfun      destroy;

    // This is defined in the context API's context.h
    _GLFW_PLATFORM_CONTEXT_STATE;
    // This is defined in egl_context.h
    _GLFW_EGL_CONTEXT_STATE;
};

/*! @brief Window and context structure.
 */
struct _GLFWwindow
{
    struct _GLFWwindow* next;

    // Window settings and state
    GLFWbool            resizable;
    GLFWbool            decorated;
    GLFWbool            autoIconify;
    GLFWbool            floating;
    GLFWbool            shouldClose;
    void*               userPointer;
    GLFWvidmode         videoMode;
    _GLFWmonitor*       monitor;
    _GLFWcursor*        cursor;

    int                 minwidth, minheight;
    int                 maxwidth, maxheight;
    int                 numer, denom;

    GLFWbool            stickyKeys;
    GLFWbool            stickyMouseButtons;
    int                 cursorMode;
    char                mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];
    char                keys[GLFW_KEY_LAST + 1];
    // Virtual cursor position when cursor is disabled
    double              virtualCursorPosX, virtualCursorPosY;

    _GLFWcontext        context;

    struct {
        GLFWwindowposfun        pos;
        GLFWwindowsizefun       size;
        GLFWwindowclosefun      close;
        GLFWwindowrefreshfun    refresh;
        GLFWwindowfocusfun      focus;
        GLFWwindowiconifyfun    iconify;
        GLFWwindowmaximizefun   maximize;
        GLFWframebuffersizefun  fbsize;
        GLFWmousebuttonfun      mouseButton;
        GLFWcursorposfun        cursorPos;
        GLFWcursorenterfun      cursorEnter;
        GLFWscrollfun           scroll;
        GLFWkeyfun              key;
        GLFWcharfun             character;
        GLFWcharmodsfun         charmods;
        GLFWdropfun             drop;
    } callbacks;

    // This is defined in the window API's platform.h
    _GLFW_PLATFORM_WINDOW_STATE;
};

/*! @brief Monitor structure.
 */
struct _GLFWmonitor
{
    char*           name;

    // Physical dimensions in millimeters.
    int             widthMM, heightMM;

    // The window whose video mode is current on this monitor
    _GLFWwindow*    window;

    GLFWvidmode*    modes;
    int             modeCount;
    GLFWvidmode     currentMode;

    GLFWgammaramp   originalRamp;
    GLFWgammaramp   currentRamp;

    // This is defined in the window API's platform.h
    _GLFW_PLATFORM_MONITOR_STATE;
};

/*! @brief Cursor structure
 */
struct _GLFWcursor
{
    _GLFWcursor*    next;

    // This is defined in the window API's platform.h
    _GLFW_PLATFORM_CURSOR_STATE;
};

/*! @brief Library global data.
 */
struct _GLFWlibrary
{
    struct {
        _GLFWfbconfig   framebuffer;
        _GLFWwndconfig  window;
        _GLFWctxconfig  context;
        int             refreshRate;
    } hints;

    _GLFWcursor*        cursorListHead;

    _GLFWwindow*        windowListHead;

    _GLFWmonitor**      monitors;
    int                 monitorCount;

    uint64_t            timerOffset;

    struct {
        GLFWbool        available;
        void*           handle;
        char*           extensions[2];
#if !defined(_GLFW_VULKAN_STATIC)
        PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties;
        PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
#endif
        GLFWbool        KHR_surface;
#if defined(_GLFW_WIN32)
        GLFWbool        KHR_win32_surface;
#elif defined(_GLFW_COCOA)
        GLFWbool        MVK_macos_surface;
#elif defined(_GLFW_X11)
        GLFWbool        KHR_xlib_surface;
        GLFWbool        KHR_xcb_surface;
#elif defined(_GLFW_WAYLAND)
        GLFWbool        KHR_wayland_surface;
#elif defined(_GLFW_MIR)
        GLFWbool        KHR_mir_surface;
#endif
    } vk;

    struct {
        GLFWmonitorfun  monitor;
        GLFWjoystickfun joystick;
    } callbacks;

    // This is defined in the window API's platform.h
    _GLFW_PLATFORM_LIBRARY_WINDOW_STATE;
    // This is defined in the context API's context.h
    _GLFW_PLATFORM_LIBRARY_CONTEXT_STATE;
    // This is defined in the platform's time.h
    _GLFW_PLATFORM_LIBRARY_TIME_STATE;
    // This is defined in the platform's joystick.h
    _GLFW_PLATFORM_LIBRARY_JOYSTICK_STATE;
    // This is defined in the platform's tls.h
    _GLFW_PLATFORM_LIBRARY_TLS_STATE;
    // This is defined in egl_context.h
    _GLFW_EGL_LIBRARY_CONTEXT_STATE;
};


//========================================================================
// Global state shared between compilation units of GLFW
//========================================================================

/*! @brief Flag indicating whether GLFW has been successfully initialized.
 */
extern GLFWbool _glfwInitialized;

/*! @brief All global data protected by @ref _glfwInitialized.
 *  This should only be touched after a call to @ref glfwInit that has not been
 *  followed by a call to @ref glfwTerminate.
 */
extern _GLFWlibrary _glfw;


//========================================================================
// Platform API functions
//========================================================================

/*! @brief Initializes the platform-specific part of the library.
 *  @return `GLFW_TRUE` if successful, or `GLFW_FALSE` if an error occurred.
 *  @ingroup platform
 */
int _glfwPlatformInit(void);

/*! @brief Terminates the platform-specific part of the library.
 *  @ingroup platform
 */
void _glfwPlatformTerminate(void);

/*! @copydoc glfwGetVersionString
 *  @ingroup platform
 *
 *  @note The returned string must be available for the duration of the program.
 *
 *  @note The returned string must not change for the duration of the program.
 */
const char* _glfwPlatformGetVersionString(void);

/*! @copydoc glfwGetCursorPos
 *  @ingroup platform
 */
void _glfwPlatformGetCursorPos(_GLFWwindow* window, double* xpos, double* ypos);

/*! @copydoc glfwSetCursorPos
 *  @ingroup platform
 */
void _glfwPlatformSetCursorPos(_GLFWwindow* window, double xpos, double ypos);

/*! @brief Sets the specified cursor mode of the specified window.
 *  @param[in] window The window whose cursor mode to set.
 *  @ingroup platform
 */
void _glfwPlatformSetCursorMode(_GLFWwindow* window, int mode);

/*! @copydoc glfwGetKeyName
 *  @ingroup platform
 */
const char* _glfwPlatformGetKeyName(int key, int scancode);

/*! @copydoc glfwGetKeyScancode
 *  @ingroup platform
 */
int _glfwPlatformGetKeyScancode(int key);

/*! @copydoc glfwGetMonitorPos
 *  @ingroup platform
 */
void _glfwPlatformGetMonitorPos(_GLFWmonitor* monitor, int* xpos, int* ypos);

/*! @copydoc glfwGetVideoModes
 *  @ingroup platform
 */
GLFWvidmode* _glfwPlatformGetVideoModes(_GLFWmonitor* monitor, int* count);

/*! @ingroup platform
 */
void _glfwPlatformGetVideoMode(_GLFWmonitor* monitor, GLFWvidmode* mode);

/*! @copydoc glfwGetGammaRamp
 *  @ingroup platform
 */
void _glfwPlatformGetGammaRamp(_GLFWmonitor* monitor, GLFWgammaramp* ramp);

/*! @copydoc glfwSetGammaRamp
 *  @ingroup platform
 */
void _glfwPlatformSetGammaRamp(_GLFWmonitor* monitor, const GLFWgammaramp* ramp);

/*! @copydoc glfwSetClipboardString
 *  @ingroup platform
 */
void _glfwPlatformSetClipboardString(_GLFWwindow* window, const char* string);

/*! @copydoc glfwGetClipboardString
 *  @ingroup platform
 *
 *  @note The returned string must be valid until the next call to @ref
 *  _glfwPlatformGetClipboardString or @ref _glfwPlatformSetClipboardString.
 */
const char* _glfwPlatformGetClipboardString(_GLFWwindow* window);

/*! @copydoc glfwJoystickPresent
 *  @ingroup platform
 */
int _glfwPlatformJoystickPresent(int jid);

/*! @copydoc glfwGetJoystickAxes
 *  @ingroup platform
 */
const float* _glfwPlatformGetJoystickAxes(int jid, int* count);

/*! @copydoc glfwGetJoystickButtons
 *  @ingroup platform
 */
const unsigned char* _glfwPlatformGetJoystickButtons(int jid, int* count);

/*! @copydoc glfwGetJoystickName
 *  @ingroup platform
 */
const char* _glfwPlatformGetJoystickName(int jid);

/*! @copydoc glfwGetTimerValue
 *  @ingroup platform
 */
uint64_t _glfwPlatformGetTimerValue(void);

/*! @copydoc glfwGetTimerFrequency
 *  @ingroup platform
 */
uint64_t _glfwPlatformGetTimerFrequency(void);

/*! @ingroup platform
 */
int _glfwPlatformCreateWindow(_GLFWwindow* window,
                              const _GLFWwndconfig* wndconfig,
                              const _GLFWctxconfig* ctxconfig,
                              const _GLFWfbconfig* fbconfig);

/*! @ingroup platform
 */
void _glfwPlatformDestroyWindow(_GLFWwindow* window);

/*! @copydoc glfwSetWindowTitle
 *  @ingroup platform
 */
void _glfwPlatformSetWindowTitle(_GLFWwindow* window, const char* title);

/*! @copydoc glfwSetWindowIcon
 *  @ingroup platform
 */
void _glfwPlatformSetWindowIcon(_GLFWwindow* window, int count, const GLFWimage* images);

/*! @copydoc glfwGetWindowPos
 *  @ingroup platform
 */
void _glfwPlatformGetWindowPos(_GLFWwindow* window, int* xpos, int* ypos);

/*! @copydoc glfwSetWindowPos
 *  @ingroup platform
 */
void _glfwPlatformSetWindowPos(_GLFWwindow* window, int xpos, int ypos);

/*! @copydoc glfwGetWindowSize
 *  @ingroup platform
 */
void _glfwPlatformGetWindowSize(_GLFWwindow* window, int* width, int* height);

/*! @copydoc glfwSetWindowSize
 *  @ingroup platform
 */
void _glfwPlatformSetWindowSize(_GLFWwindow* window, int width, int height);

/*! @copydoc glfwSetWindowSizeLimits
 *  @ingroup platform
 */
void _glfwPlatformSetWindowSizeLimits(_GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight);

/*! @copydoc glfwSetWindowAspectRatio
 *  @ingroup platform
 */
void _glfwPlatformSetWindowAspectRatio(_GLFWwindow* window, int numer, int denom);

/*! @copydoc glfwGetFramebufferSize
 *  @ingroup platform
 */
void _glfwPlatformGetFramebufferSize(_GLFWwindow* window, int* width, int* height);

/*! @copydoc glfwGetWindowFrameSize
 *  @ingroup platform
 */
void _glfwPlatformGetWindowFrameSize(_GLFWwindow* window, int* left, int* top, int* right, int* bottom);

/*! @copydoc glfwIconifyWindow
 *  @ingroup platform
 */
void _glfwPlatformIconifyWindow(_GLFWwindow* window);

/*! @copydoc glfwRestoreWindow
 *  @ingroup platform
 */
void _glfwPlatformRestoreWindow(_GLFWwindow* window);

/*! @copydoc glfwMaximizeWindow
 *  @ingroup platform
 */
void _glfwPlatformMaximizeWindow(_GLFWwindow* window);

/*! @copydoc glfwShowWindow
 *  @ingroup platform
 */
void _glfwPlatformShowWindow(_GLFWwindow* window);

/*! @copydoc glfwHideWindow
 *  @ingroup platform
 */
void _glfwPlatformHideWindow(_GLFWwindow* window);

/*! @copydoc glfwFocusWindow
 *  @ingroup platform
 */
void _glfwPlatformFocusWindow(_GLFWwindow* window);

/*! @copydoc glfwSetWindowMonitor
 *  @ingroup platform
 */
void _glfwPlatformSetWindowMonitor(_GLFWwindow* window, _GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate);

/*! @brief Returns whether the window is focused.
 *  @ingroup platform
 */
int _glfwPlatformWindowFocused(_GLFWwindow* window);

/*! @brief Returns whether the window is iconified.
 *  @ingroup platform
 */
int _glfwPlatformWindowIconified(_GLFWwindow* window);

/*! @brief Returns whether the window is visible.
 *  @ingroup platform
 */
int _glfwPlatformWindowVisible(_GLFWwindow* window);

/*! @brief Returns whether the window is maximized.
 *  @ingroup platform
 */
int _glfwPlatformWindowMaximized(_GLFWwindow* window);

/*! @brief Sets whether the window is resizable by the user.
 *  @ingroup platform
 */
void _glfwPlatformSetWindowResizable(_GLFWwindow* window, GLFWbool enabled);

/*! @brief Sets whether the window is decorated.
 *  @ingroup platform
 */
void _glfwPlatformSetWindowDecorated(_GLFWwindow* window, GLFWbool enabled);

/*! @brief Sets whether the window is floating.
 *  @ingroup platform
 */
void _glfwPlatformSetWindowFloating(_GLFWwindow* window, GLFWbool enabled);

/*! @copydoc glfwPollEvents
 *  @ingroup platform
 */
void _glfwPlatformPollEvents(void);

/*! @copydoc glfwWaitEvents
 *  @ingroup platform
 */
void _glfwPlatformWaitEvents(void);

/*! @copydoc glfwWaitEventsTimeout
 *  @ingroup platform
 */
void _glfwPlatformWaitEventsTimeout(double timeout);

/*! @copydoc glfwPostEmptyEvent
 *  @ingroup platform
 */
void _glfwPlatformPostEmptyEvent(void);

/*! @ingroup platform
 */
void _glfwPlatformSetCurrentContext(_GLFWwindow* context);

/*! @copydoc glfwGetCurrentContext
 *  @ingroup platform
 */
_GLFWwindow* _glfwPlatformGetCurrentContext(void);

/*! @copydoc glfwCreateCursor
 *  @ingroup platform
 */
int _glfwPlatformCreateCursor(_GLFWcursor* cursor, const GLFWimage* image, int xhot, int yhot);

/*! @copydoc glfwCreateStandardCursor
 *  @ingroup platform
 */
int _glfwPlatformCreateStandardCursor(_GLFWcursor* cursor, int shape);

/*! @copydoc glfwDestroyCursor
 *  @ingroup platform
 */
void _glfwPlatformDestroyCursor(_GLFWcursor* cursor);

/*! @copydoc glfwSetCursor
 *  @ingroup platform
 */
void _glfwPlatformSetCursor(_GLFWwindow* window, _GLFWcursor* cursor);

/*! @ingroup platform
 */
void _glfwPlatformGetRequiredInstanceExtensions(char** extensions);

/*! @ingroup platform
 */
int _glfwPlatformGetPhysicalDevicePresentationSupport(VkInstance instance, VkPhysicalDevice device, uint32_t queuefamily);

/*! @ingroup platform
 */
VkResult _glfwPlatformCreateWindowSurface(VkInstance instance, _GLFWwindow* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);


//========================================================================
// Event API functions
//========================================================================

/*! @brief Notifies shared code of a window focus event.
 *  @param[in] window The window that received the event.
 *  @param[in] focused `GLFW_TRUE` if the window received focus, or `GLFW_FALSE`
 *  if it lost focus.
 *  @ingroup event
 */
void _glfwInputWindowFocus(_GLFWwindow* window, GLFWbool focused);

/*! @brief Notifies shared code of a window movement event.
 *  @param[in] window The window that received the event.
 *  @param[in] xpos The new x-coordinate of the client area of the window.
 *  @param[in] ypos The new y-coordinate of the client area of the window.
 *  @ingroup event
 */
void _glfwInputWindowPos(_GLFWwindow* window, int xpos, int ypos);

/*! @brief Notifies shared code of a window resize event.
 *  @param[in] window The window that received the event.
 *  @param[in] width The new width of the client area of the window.
 *  @param[in] height The new height of the client area of the window.
 *  @ingroup event
 */
void _glfwInputWindowSize(_GLFWwindow* window, int width, int height);

/*! @brief Notifies shared code of a framebuffer resize event.
 *  @param[in] window The window that received the event.
 *  @param[in] width The new width, in pixels, of the framebuffer.
 *  @param[in] height The new height, in pixels, of the framebuffer.
 *  @ingroup event
 */
void _glfwInputFramebufferSize(_GLFWwindow* window, int width, int height);

/*! @brief Notifies shared code of a window iconification event.
 *  @param[in] window The window that received the event.
 *  @param[in] iconified `GLFW_TRUE` if the window was iconified, or
 *  `GLFW_FALSE` if it was restored.
 *  @ingroup event
 */
void _glfwInputWindowIconify(_GLFWwindow* window, GLFWbool iconified);

/*! @brief Notifies shared code of a window maximization event.
 *  @param[in] window The window that received the event.
 *  @param[in] maximized `GLFW_TRUE` if the window was maximized, or
 *  `GLFW_FALSE` if it was restored.
 *  @ingroup event
 */
void _glfwInputWindowMaximize(_GLFWwindow* window, GLFWbool maximized);

/*! @brief Notifies shared code of a window damage event.
 *  @param[in] window The window that received the event.
 */
void _glfwInputWindowDamage(_GLFWwindow* window);

/*! @brief Notifies shared code of a window close request event
 *  @param[in] window The window that received the event.
 *  @ingroup event
 */
void _glfwInputWindowCloseRequest(_GLFWwindow* window);

void _glfwInputWindowMonitorChange(_GLFWwindow* window, _GLFWmonitor* monitor);

/*! @brief Notifies shared code of a physical key event.
 *  @param[in] window The window that received the event.
 *  @param[in] key The key that was pressed or released.
 *  @param[in] scancode The system-specific scan code of the key.
 *  @param[in] action @ref GLFW_PRESS or @ref GLFW_RELEASE.
 *  @param[in] mods The modifiers pressed when the event was generated.
 *  @ingroup event
 */
void _glfwInputKey(_GLFWwindow* window, int key, int scancode, int action, int mods);

/*! @brief Notifies shared code of a Unicode character input event.
 *  @param[in] window The window that received the event.
 *  @param[in] codepoint The Unicode code point of the input character.
 *  @param[in] mods Bit field describing which modifier keys were held down.
 *  @param[in] plain `GLFW_TRUE` if the character is regular text input, or
 *  `GLFW_FALSE` otherwise.
 *  @ingroup event
 */
void _glfwInputChar(_GLFWwindow* window, unsigned int codepoint, int mods, GLFWbool plain);

/*! @brief Notifies shared code of a scroll event.
 *  @param[in] window The window that received the event.
 *  @param[in] xoffset The scroll offset along the x-axis.
 *  @param[in] yoffset The scroll offset along the y-axis.
 *  @ingroup event
 */
void _glfwInputScroll(_GLFWwindow* window, double xoffset, double yoffset);

/*! @brief Notifies shared code of a mouse button click event.
 *  @param[in] window The window that received the event.
 *  @param[in] button The button that was pressed or released.
 *  @param[in] action @ref GLFW_PRESS or @ref GLFW_RELEASE.
 *  @ingroup event
 */
void _glfwInputMouseClick(_GLFWwindow* window, int button, int action, int mods);

/*! @brief Notifies shared code of a cursor motion event.
 *  @param[in] window The window that received the event.
 *  @param[in] xpos The new x-coordinate of the cursor, relative to the left
 *  edge of the client area of the window.
 *  @param[in] ypos The new y-coordinate of the cursor, relative to the top edge
 *  of the client area of the window.
 *  @ingroup event
 */
void _glfwInputCursorPos(_GLFWwindow* window, double xpos, double ypos);

/*! @brief Notifies shared code of a cursor enter/leave event.
 *  @param[in] window The window that received the event.
 *  @param[in] entered `GLFW_TRUE` if the cursor entered the client area of the
 *  window, or `GLFW_FALSE` if it left it.
 *  @ingroup event
 */
void _glfwInputCursorEnter(_GLFWwindow* window, GLFWbool entered);

/*! @ingroup event
 */
void _glfwInputMonitor(_GLFWmonitor* monitor, int action, int type);

/*! @ingroup event
 */
void _glfwInputMonitorWindowChange(_GLFWmonitor* monitor, _GLFWwindow* window);

/*! @brief Notifies shared code of an error.
 *  @param[in] error The error code most suitable for the error.
 *  @param[in] format The `printf` style format string of the error
 *  description.
 *  @ingroup event
 */
#if defined(__GNUC__)
void _glfwInputError(int error, const char* format, ...) __attribute__((format(printf, 2, 3)));
#else
void _glfwInputError(int error, const char* format, ...);
#endif

/*! @brief Notifies dropped object over window.
 *  @param[in] window The window that received the event.
 *  @param[in] count The number of dropped objects.
 *  @param[in] names The names of the dropped objects.
 *  @ingroup event
 */
void _glfwInputDrop(_GLFWwindow* window, int count, const char** names);

/*! @brief Notifies shared code of a joystick connection/disconnection event.
 *  @param[in] jid The joystick that was connected or disconnected.
 *  @param[in] event One of `GLFW_CONNECTED` or `GLFW_DISCONNECTED`.
 *  @ingroup event
 */
void _glfwInputJoystickChange(int jid, int event);


//========================================================================
// Utility functions
//========================================================================

/*! @ingroup utility
 */
const GLFWvidmode* _glfwChooseVideoMode(_GLFWmonitor* monitor,
                                        const GLFWvidmode* desired);

/*! @brief Performs lexical comparison between two @ref GLFWvidmode structures.
 *  @ingroup utility
 */
int _glfwCompareVideoModes(const GLFWvidmode* first, const GLFWvidmode* second);

/*! @brief Splits a color depth into red, green and blue bit depths.
 *  @ingroup utility
 */
void _glfwSplitBPP(int bpp, int* red, int* green, int* blue);

/*! @brief Searches an extension string for the specified extension.
 *  @param[in] string The extension string to search.
 *  @param[in] extensions The extension to search for.
 *  @return `GLFW_TRUE` if the extension was found, or `GLFW_FALSE` otherwise.
 *  @ingroup utility
 */
GLFWbool _glfwStringInExtensionString(const char* string, const char* extensions);

/*! @brief Chooses the framebuffer config that best matches the desired one.
 *  @param[in] desired The desired framebuffer config.
 *  @param[in] alternatives The framebuffer configs supported by the system.
 *  @param[in] count The number of entries in the alternatives array.
 *  @return The framebuffer config most closely matching the desired one, or @c
 *  NULL if none fulfilled the hard constraints of the desired values.
 *  @ingroup utility
 */
const _GLFWfbconfig* _glfwChooseFBConfig(const _GLFWfbconfig* desired,
                                         const _GLFWfbconfig* alternatives,
                                         unsigned int count);

/*! @brief Retrieves the attributes of the current context.
 *  @param[in] ctxconfig The desired context attributes.
 *  @return `GLFW_TRUE` if successful, or `GLFW_FALSE` if the context is
 *  unusable.
 *  @ingroup utility
 */
GLFWbool _glfwRefreshContextAttribs(const _GLFWctxconfig* ctxconfig);

/*! @brief Checks whether the desired context attributes are valid.
 *  @param[in] ctxconfig The context attributes to check.
 *  @return `GLFW_TRUE` if the context attributes are valid, or `GLFW_FALSE`
 *  otherwise.
 *  @ingroup utility
 *
 *  This function checks things like whether the specified client API version
 *  exists and whether all relevant options have supported and non-conflicting
 *  values.
 */
GLFWbool _glfwIsValidContextConfig(const _GLFWctxconfig* ctxconfig);

/*! @ingroup utility
 */
void _glfwAllocGammaArrays(GLFWgammaramp* ramp, unsigned int size);

/*! @ingroup utility
 */
void _glfwFreeGammaArrays(GLFWgammaramp* ramp);

/*! @brief Allocates and returns a monitor object with the specified name
 *  and dimensions.
 *  @param[in] name The name of the monitor.
 *  @param[in] widthMM The width, in mm, of the monitor's display area.
 *  @param[in] heightMM The height, in mm, of the monitor's display area.
 *  @return The newly created object.
 *  @ingroup utility
 */
_GLFWmonitor* _glfwAllocMonitor(const char* name, int widthMM, int heightMM);

/*! @brief Frees a monitor object and any data associated with it.
 *  @ingroup utility
  */
void _glfwFreeMonitor(_GLFWmonitor* monitor);

/*! @ingroup utility
  */
void _glfwFreeMonitors(_GLFWmonitor** monitors, int count);

/*! @ingroup utility
 */
GLFWbool _glfwIsPrintable(int key);

/*! @ingroup utility
 */
GLFWbool _glfwInitVulkan(int mode);

/*! @ingroup utility
 */
void _glfwTerminateVulkan(void);

/*! @ingroup utility
 */
const char* _glfwGetVulkanResultString(VkResult result);

#endif // _glfw3_internal_h_









#ifdef GLFW_IMPL

//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

GLFWbool _glfwIsValidContextConfig(const _GLFWctxconfig* ctxconfig)
{
    if (ctxconfig->source != GLFW_NATIVE_CONTEXT_API &&
        ctxconfig->source != GLFW_EGL_CONTEXT_API)
    {
        _glfwInputError(GLFW_INVALID_ENUM,
                        "Invalid context creation API %i",
                        ctxconfig->source);
        return GLFW_FALSE;
    }

    if (ctxconfig->client != GLFW_NO_API &&
        ctxconfig->client != GLFW_OPENGL_API &&
        ctxconfig->client != GLFW_OPENGL_ES_API)
    {
        _glfwInputError(GLFW_INVALID_ENUM,
                        "Invalid client API %i",
                        ctxconfig->client);
        return GLFW_FALSE;
    }

    if (ctxconfig->client == GLFW_OPENGL_API)
    {
        if ((ctxconfig->major < 1 || ctxconfig->minor < 0) ||
            (ctxconfig->major == 1 && ctxconfig->minor > 5) ||
            (ctxconfig->major == 2 && ctxconfig->minor > 1) ||
            (ctxconfig->major == 3 && ctxconfig->minor > 3))
        {
            // OpenGL 1.0 is the smallest valid version
            // OpenGL 1.x series ended with version 1.5
            // OpenGL 2.x series ended with version 2.1
            // OpenGL 3.x series ended with version 3.3
            // For now, let everything else through

            _glfwInputError(GLFW_INVALID_VALUE,
                            "Invalid OpenGL version %i.%i",
                            ctxconfig->major, ctxconfig->minor);
            return GLFW_FALSE;
        }

        if (ctxconfig->profile)
        {
            if (ctxconfig->profile != GLFW_OPENGL_CORE_PROFILE &&
                ctxconfig->profile != GLFW_OPENGL_COMPAT_PROFILE)
            {
                _glfwInputError(GLFW_INVALID_ENUM,
                                "Invalid OpenGL profile %i",
                                ctxconfig->profile);
                return GLFW_FALSE;
            }

            if (ctxconfig->major <= 2 ||
                (ctxconfig->major == 3 && ctxconfig->minor < 2))
            {
                // Desktop OpenGL context profiles are only defined for version 3.2
                // and above

                _glfwInputError(GLFW_INVALID_VALUE,
                                "Context profiles are only defined for OpenGL version 3.2 and above");
                return GLFW_FALSE;
            }
        }

        if (ctxconfig->forward && ctxconfig->major <= 2)
        {
            // Forward-compatible contexts are only defined for OpenGL version 3.0 and above
            _glfwInputError(GLFW_INVALID_VALUE,
                            "Forward-compatibility is only defined for OpenGL version 3.0 and above");
            return GLFW_FALSE;
        }
    }
    else if (ctxconfig->client == GLFW_OPENGL_ES_API)
    {
        if (ctxconfig->major < 1 || ctxconfig->minor < 0 ||
            (ctxconfig->major == 1 && ctxconfig->minor > 1) ||
            (ctxconfig->major == 2 && ctxconfig->minor > 0))
        {
            // OpenGL ES 1.0 is the smallest valid version
            // OpenGL ES 1.x series ended with version 1.1
            // OpenGL ES 2.x series ended with version 2.0
            // For now, let everything else through

            _glfwInputError(GLFW_INVALID_VALUE,
                            "Invalid OpenGL ES version %i.%i",
                            ctxconfig->major, ctxconfig->minor);
            return GLFW_FALSE;
        }
    }

    if (ctxconfig->robustness)
    {
        if (ctxconfig->robustness != GLFW_NO_RESET_NOTIFICATION &&
            ctxconfig->robustness != GLFW_LOSE_CONTEXT_ON_RESET)
        {
            _glfwInputError(GLFW_INVALID_ENUM,
                            "Invalid context robustness mode %i",
                            ctxconfig->robustness);
            return GLFW_FALSE;
        }
    }

    if (ctxconfig->release)
    {
        if (ctxconfig->release != GLFW_RELEASE_BEHAVIOR_NONE &&
            ctxconfig->release != GLFW_RELEASE_BEHAVIOR_FLUSH)
        {
            _glfwInputError(GLFW_INVALID_ENUM,
                            "Invalid context release behavior %i",
                            ctxconfig->release);
            return GLFW_FALSE;
        }
    }

    return GLFW_TRUE;
}

const _GLFWfbconfig* _glfwChooseFBConfig(const _GLFWfbconfig* desired,
                                         const _GLFWfbconfig* alternatives,
                                         unsigned int count)
{
    unsigned int i;
    unsigned int missing, leastMissing = UINT_MAX;
    unsigned int colorDiff, leastColorDiff = UINT_MAX;
    unsigned int extraDiff, leastExtraDiff = UINT_MAX;
    const _GLFWfbconfig* current;
    const _GLFWfbconfig* closest = NULL;

    for (i = 0;  i < count;  i++)
    {
        current = alternatives + i;

        if (desired->stereo > 0 && current->stereo == 0)
        {
            // Stereo is a hard constraint
            continue;
        }

        if (desired->doublebuffer != current->doublebuffer)
        {
            // Double buffering is a hard constraint
            continue;
        }

        // Count number of missing buffers
        {
            missing = 0;

            if (desired->alphaBits > 0 && current->alphaBits == 0)
                missing++;

            if (desired->depthBits > 0 && current->depthBits == 0)
                missing++;

            if (desired->stencilBits > 0 && current->stencilBits == 0)
                missing++;

            if (desired->auxBuffers > 0 &&
                current->auxBuffers < desired->auxBuffers)
            {
                missing += desired->auxBuffers - current->auxBuffers;
            }

            if (desired->samples > 0 && current->samples == 0)
            {
                // Technically, several multisampling buffers could be
                // involved, but that's a lower level implementation detail and
                // not important to us here, so we count them as one
                missing++;
            }
        }

        // These polynomials make many small channel size differences matter
        // less than one large channel size difference

        // Calculate color channel size difference value
        {
            colorDiff = 0;

            if (desired->redBits != GLFW_DONT_CARE)
            {
                colorDiff += (desired->redBits - current->redBits) *
                             (desired->redBits - current->redBits);
            }

            if (desired->greenBits != GLFW_DONT_CARE)
            {
                colorDiff += (desired->greenBits - current->greenBits) *
                             (desired->greenBits - current->greenBits);
            }

            if (desired->blueBits != GLFW_DONT_CARE)
            {
                colorDiff += (desired->blueBits - current->blueBits) *
                             (desired->blueBits - current->blueBits);
            }
        }

        // Calculate non-color channel size difference value
        {
            extraDiff = 0;

            if (desired->alphaBits != GLFW_DONT_CARE)
            {
                extraDiff += (desired->alphaBits - current->alphaBits) *
                             (desired->alphaBits - current->alphaBits);
            }

            if (desired->depthBits != GLFW_DONT_CARE)
            {
                extraDiff += (desired->depthBits - current->depthBits) *
                             (desired->depthBits - current->depthBits);
            }

            if (desired->stencilBits != GLFW_DONT_CARE)
            {
                extraDiff += (desired->stencilBits - current->stencilBits) *
                             (desired->stencilBits - current->stencilBits);
            }

            if (desired->accumRedBits != GLFW_DONT_CARE)
            {
                extraDiff += (desired->accumRedBits - current->accumRedBits) *
                             (desired->accumRedBits - current->accumRedBits);
            }

            if (desired->accumGreenBits != GLFW_DONT_CARE)
            {
                extraDiff += (desired->accumGreenBits - current->accumGreenBits) *
                             (desired->accumGreenBits - current->accumGreenBits);
            }

            if (desired->accumBlueBits != GLFW_DONT_CARE)
            {
                extraDiff += (desired->accumBlueBits - current->accumBlueBits) *
                             (desired->accumBlueBits - current->accumBlueBits);
            }

            if (desired->accumAlphaBits != GLFW_DONT_CARE)
            {
                extraDiff += (desired->accumAlphaBits - current->accumAlphaBits) *
                             (desired->accumAlphaBits - current->accumAlphaBits);
            }

            if (desired->samples != GLFW_DONT_CARE)
            {
                extraDiff += (desired->samples - current->samples) *
                             (desired->samples - current->samples);
            }

            if (desired->sRGB && !current->sRGB)
                extraDiff++;
        }

        // Figure out if the current one is better than the best one found so far
        // Least number of missing buffers is the most important heuristic,
        // then color buffer size match and lastly size match for other buffers

        if (missing < leastMissing)
            closest = current;
        else if (missing == leastMissing)
        {
            if ((colorDiff < leastColorDiff) ||
                (colorDiff == leastColorDiff && extraDiff < leastExtraDiff))
            {
                closest = current;
            }
        }

        if (current == closest)
        {
            leastMissing = missing;
            leastColorDiff = colorDiff;
            leastExtraDiff = extraDiff;
        }
    }

    return closest;
}

GLFWbool _glfwRefreshContextAttribs(const _GLFWctxconfig* ctxconfig)
{
    int i;
    _GLFWwindow* window;
    const char* version;
    const char* prefixes[] =
    {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };

    window = _glfwPlatformGetCurrentContext();

    window->context.source = ctxconfig->source;
    window->context.client = GLFW_OPENGL_API;

    window->context.GetIntegerv = (PFNGLGETINTEGERVPROC)
        window->context.getProcAddress("glGetIntegerv");
    window->context.GetString = (PFNGLGETSTRINGPROC)
        window->context.getProcAddress("glGetString");
    if (!window->context.GetIntegerv || !window->context.GetString)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "Entry point retrieval is broken");
        return GLFW_FALSE;
    }

    version = (const char*) window->context.GetString(GL_VERSION);
    if (!version)
    {
        if (ctxconfig->client == GLFW_OPENGL_API)
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "OpenGL version string retrieval is broken");
        }
        else
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "OpenGL ES version string retrieval is broken");
        }

        return GLFW_FALSE;
    }

    for (i = 0;  prefixes[i];  i++)
    {
        const size_t length = strlen(prefixes[i]);

        if (strncmp(version, prefixes[i], length) == 0)
        {
            version += length;
            window->context.client = GLFW_OPENGL_ES_API;
            break;
        }
    }

    if (!sscanf(version, "%d.%d.%d",
                &window->context.major,
                &window->context.minor,
                &window->context.revision))
    {
        if (window->context.client == GLFW_OPENGL_API)
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "No version found in OpenGL version string");
        }
        else
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "No version found in OpenGL ES version string");
        }

        return GLFW_FALSE;
    }

    if (window->context.major < ctxconfig->major ||
        (window->context.major == ctxconfig->major &&
         window->context.minor < ctxconfig->minor))
    {
        // The desired OpenGL version is greater than the actual version
        // This only happens if the machine lacks {GLX|WGL}_ARB_create_context
        // /and/ the user has requested an OpenGL version greater than 1.0

        // For API consistency, we emulate the behavior of the
        // {GLX|WGL}_ARB_create_context extension and fail here

        if (window->context.client == GLFW_OPENGL_API)
        {
            _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                            "Requested OpenGL version %i.%i, got version %i.%i",
                            ctxconfig->major, ctxconfig->minor,
                            window->context.major, window->context.minor);
        }
        else
        {
            _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                            "Requested OpenGL ES version %i.%i, got version %i.%i",
                            ctxconfig->major, ctxconfig->minor,
                            window->context.major, window->context.minor);
        }

        return GLFW_FALSE;
    }

    if (window->context.major >= 3)
    {
        // OpenGL 3.0+ uses a different function for extension string retrieval
        // We cache it here instead of in glfwExtensionSupported mostly to alert
        // users as early as possible that their build may be broken

        window->context.GetStringi = (PFNGLGETSTRINGIPROC)
            window->context.getProcAddress("glGetStringi");
        if (!window->context.GetStringi)
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "Entry point retrieval is broken");
            return GLFW_FALSE;
        }
    }

    if (window->context.client == GLFW_OPENGL_API)
    {
        // Read back context flags (OpenGL 3.0 and above)
        if (window->context.major >= 3)
        {
            GLint flags;
            window->context.GetIntegerv(GL_CONTEXT_FLAGS, &flags);

            if (flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
                window->context.forward = GLFW_TRUE;

            if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
                window->context.debug = GLFW_TRUE;
            else if (glfwExtensionSupported("GL_ARB_debug_output") &&
                     ctxconfig->debug)
            {
                // HACK: This is a workaround for older drivers (pre KHR_debug)
                //       not setting the debug bit in the context flags for
                //       debug contexts
                window->context.debug = GLFW_TRUE;
            }

            if (flags & GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR)
                window->context.noerror = GLFW_TRUE;
        }

        // Read back OpenGL context profile (OpenGL 3.2 and above)
        if (window->context.major >= 4 ||
            (window->context.major == 3 && window->context.minor >= 2))
        {
            GLint mask;
            window->context.GetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);

            if (mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
                window->context.profile = GLFW_OPENGL_COMPAT_PROFILE;
            else if (mask & GL_CONTEXT_CORE_PROFILE_BIT)
                window->context.profile = GLFW_OPENGL_CORE_PROFILE;
            else if (glfwExtensionSupported("GL_ARB_compatibility"))
            {
                // HACK: This is a workaround for the compatibility profile bit
                //       not being set in the context flags if an OpenGL 3.2+
                //       context was created without having requested a specific
                //       version
                window->context.profile = GLFW_OPENGL_COMPAT_PROFILE;
            }
        }

        // Read back robustness strategy
        if (glfwExtensionSupported("GL_ARB_robustness"))
        {
            // NOTE: We avoid using the context flags for detection, as they are
            //       only present from 3.0 while the extension applies from 1.1

            GLint strategy;
            window->context.GetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_ARB,
                                        &strategy);

            if (strategy == GL_LOSE_CONTEXT_ON_RESET_ARB)
                window->context.robustness = GLFW_LOSE_CONTEXT_ON_RESET;
            else if (strategy == GL_NO_RESET_NOTIFICATION_ARB)
                window->context.robustness = GLFW_NO_RESET_NOTIFICATION;
        }
    }
    else
    {
        // Read back robustness strategy
        if (glfwExtensionSupported("GL_EXT_robustness"))
        {
            // NOTE: The values of these constants match those of the OpenGL ARB
            //       one, so we can reuse them here

            GLint strategy;
            window->context.GetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_ARB,
                                        &strategy);

            if (strategy == GL_LOSE_CONTEXT_ON_RESET_ARB)
                window->context.robustness = GLFW_LOSE_CONTEXT_ON_RESET;
            else if (strategy == GL_NO_RESET_NOTIFICATION_ARB)
                window->context.robustness = GLFW_NO_RESET_NOTIFICATION;
        }
    }

    if (glfwExtensionSupported("GL_KHR_context_flush_control"))
    {
        GLint behavior;
        window->context.GetIntegerv(GL_CONTEXT_RELEASE_BEHAVIOR, &behavior);

        if (behavior == GL_NONE)
            window->context.release = GLFW_RELEASE_BEHAVIOR_NONE;
        else if (behavior == GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH)
            window->context.release = GLFW_RELEASE_BEHAVIOR_FLUSH;
    }

    // Clearing the front buffer to black to avoid garbage pixels left over from
    // previous uses of our bit of VRAM
    {
        PFNGLCLEARPROC glClear = (PFNGLCLEARPROC)
            window->context.getProcAddress("glClear");
        glClear(GL_COLOR_BUFFER_BIT);
        window->context.swapBuffers(window);
    }

    return GLFW_TRUE;
}

GLFWbool _glfwStringInExtensionString(const char* string, const char* extensions)
{
    const char* start = extensions;

    for (;;)
    {
        const char* where;
        const char* terminator;

        where = strstr(start, string);
        if (!where)
            return GLFW_FALSE;

        terminator = where + strlen(string);
        if (where == start || *(where - 1) == ' ')
        {
            if (*terminator == ' ' || *terminator == '\0')
                break;
        }

        start = terminator;
    }

    return GLFW_TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI void glfwMakeContextCurrent(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFWwindow* previous = _glfwPlatformGetCurrentContext();

    _GLFW_REQUIRE_INIT();

    if (window && window->context.client == GLFW_NO_API)
    {
        _glfwInputError(GLFW_NO_WINDOW_CONTEXT, NULL);
        return;
    }

    if (previous)
    {
        if (!window || window->context.source != previous->context.source)
            previous->context.makeCurrent(NULL);
    }

    if (window)
        window->context.makeCurrent(window);
}

GLFWAPI GLFWwindow* glfwGetCurrentContext(void)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return (GLFWwindow*) _glfwPlatformGetCurrentContext();
}

GLFWAPI void glfwSwapBuffers(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    if (window->context.client == GLFW_NO_API)
    {
        _glfwInputError(GLFW_NO_WINDOW_CONTEXT, NULL);
        return;
    }

    window->context.swapBuffers(window);
}

GLFWAPI void glfwSwapInterval(int interval)
{
    _GLFWwindow* window;

    _GLFW_REQUIRE_INIT();

    window = _glfwPlatformGetCurrentContext();
    if (!window)
    {
        _glfwInputError(GLFW_NO_CURRENT_CONTEXT, NULL);
        return;
    }

    window->context.swapInterval(interval);
}

GLFWAPI int glfwExtensionSupported(const char* extension)
{
    _GLFWwindow* window;

    assert(extension != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(GLFW_FALSE);

    window = _glfwPlatformGetCurrentContext();
    if (!window)
    {
        _glfwInputError(GLFW_NO_CURRENT_CONTEXT, NULL);
        return GLFW_FALSE;
    }

    if (*extension == '\0')
    {
        _glfwInputError(GLFW_INVALID_VALUE, "Extension name is empty string");
        return GLFW_FALSE;
    }

    if (window->context.major >= 3)
    {
        int i;
        GLint count;

        // Check if extension is in the modern OpenGL extensions string list

        window->context.GetIntegerv(GL_NUM_EXTENSIONS, &count);

        for (i = 0;  i < count;  i++)
        {
            const char* en = (const char*)
                window->context.GetStringi(GL_EXTENSIONS, i);
            if (!en)
            {
                _glfwInputError(GLFW_PLATFORM_ERROR,
                                "Extension string retrieval is broken");
                return GLFW_FALSE;
            }

            if (strcmp(en, extension) == 0)
                return GLFW_TRUE;
        }
    }
    else
    {
        // Check if extension is in the old style OpenGL extensions string

        const char* extensions = (const char*)
            window->context.GetString(GL_EXTENSIONS);
        if (!extensions)
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "Extension string retrieval is broken");
            return GLFW_FALSE;
        }

        if (_glfwStringInExtensionString(extension, extensions))
            return GLFW_TRUE;
    }

    // Check if extension is in the platform-specific string
    return window->context.extensionSupported(extension);
}

GLFWAPI GLFWglproc glfwGetProcAddress(const char* procname)
{
    _GLFWwindow* window;
    assert(procname != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    window = _glfwPlatformGetCurrentContext();
    if (!window)
    {
        _glfwInputError(GLFW_NO_CURRENT_CONTEXT, NULL);
        return NULL;
    }

    return window->context.getProcAddress(procname);
}

//========================================================================
// GLFW 3.3 EGL - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


// Return a description of the specified EGL error
//
static const char* getEGLErrorString(EGLint error)
{
    switch (error)
    {
        case EGL_SUCCESS:
            return "Success";
        case EGL_NOT_INITIALIZED:
            return "EGL is not or could not be initialized";
        case EGL_BAD_ACCESS:
            return "EGL cannot access a requested resource";
        case EGL_BAD_ALLOC:
            return "EGL failed to allocate resources for the requested operation";
        case EGL_BAD_ATTRIBUTE:
            return "An unrecognized attribute or attribute value was passed in the attribute list";
        case EGL_BAD_CONTEXT:
            return "An EGLContext argument does not name a valid EGL rendering context";
        case EGL_BAD_CONFIG:
            return "An EGLConfig argument does not name a valid EGL frame buffer configuration";
        case EGL_BAD_CURRENT_SURFACE:
            return "The current surface of the calling thread is a window, pixel buffer or pixmap that is no longer valid";
        case EGL_BAD_DISPLAY:
            return "An EGLDisplay argument does not name a valid EGL display connection";
        case EGL_BAD_SURFACE:
            return "An EGLSurface argument does not name a valid surface configured for GL rendering";
        case EGL_BAD_MATCH:
            return "Arguments are inconsistent";
        case EGL_BAD_PARAMETER:
            return "One or more argument values are invalid";
        case EGL_BAD_NATIVE_PIXMAP:
            return "A NativePixmapType argument does not refer to a valid native pixmap";
        case EGL_BAD_NATIVE_WINDOW:
            return "A NativeWindowType argument does not refer to a valid native window";
        case EGL_CONTEXT_LOST:
            return "The application must destroy all contexts and reinitialise";
        default:
            return "ERROR: UNKNOWN EGL ERROR";
    }
}

// Returns the specified attribute of the specified EGLConfig
//
static int getEGLConfigAttrib(EGLConfig config, int attrib)
{
    int value;
    eglGetConfigAttrib(_glfw.egl.display, config, attrib, &value);
    return value;
}

// Return the EGLConfig most closely matching the specified hints
//
static GLFWbool chooseEGLConfig(const _GLFWctxconfig* ctxconfig,
                                const _GLFWfbconfig* desired,
                                EGLConfig* result)
{
    EGLConfig* nativeConfigs;
    _GLFWfbconfig* usableConfigs;
    const _GLFWfbconfig* closest;
    int i, nativeCount, usableCount;

    eglGetConfigs(_glfw.egl.display, NULL, 0, &nativeCount);
    if (!nativeCount)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE, "EGL: No EGLConfigs returned");
        return GLFW_FALSE;
    }

    nativeConfigs = calloc(nativeCount, sizeof(EGLConfig));
    eglGetConfigs(_glfw.egl.display, nativeConfigs, nativeCount, &nativeCount);

    usableConfigs = calloc(nativeCount, sizeof(_GLFWfbconfig));
    usableCount = 0;

    for (i = 0;  i < nativeCount;  i++)
    {
        const EGLConfig n = nativeConfigs[i];
        _GLFWfbconfig* u = usableConfigs + usableCount;

        // Only consider RGB(A) EGLConfigs
        if (getEGLConfigAttrib(n, EGL_COLOR_BUFFER_TYPE) != EGL_RGB_BUFFER)
            continue;

        // Only consider window EGLConfigs
        if (!(getEGLConfigAttrib(n, EGL_SURFACE_TYPE) & EGL_WINDOW_BIT))
            continue;

#if defined(_GLFW_X11)
        // Only consider EGLConfigs with associated Visuals
        if (!getEGLConfigAttrib(n, EGL_NATIVE_VISUAL_ID))
            continue;
#endif // _GLFW_X11

        if (ctxconfig->client == GLFW_OPENGL_ES_API)
        {
            if (ctxconfig->major == 1)
            {
                if (!(getEGLConfigAttrib(n, EGL_RENDERABLE_TYPE) & EGL_OPENGL_ES_BIT))
                    continue;
            }
            else
            {
                if (!(getEGLConfigAttrib(n, EGL_RENDERABLE_TYPE) & EGL_OPENGL_ES2_BIT))
                    continue;
            }
        }
        else if (ctxconfig->client == GLFW_OPENGL_API)
        {
            if (!(getEGLConfigAttrib(n, EGL_RENDERABLE_TYPE) & EGL_OPENGL_BIT))
                continue;
        }

        u->redBits = getEGLConfigAttrib(n, EGL_RED_SIZE);
        u->greenBits = getEGLConfigAttrib(n, EGL_GREEN_SIZE);
        u->blueBits = getEGLConfigAttrib(n, EGL_BLUE_SIZE);

        u->alphaBits = getEGLConfigAttrib(n, EGL_ALPHA_SIZE);
        u->depthBits = getEGLConfigAttrib(n, EGL_DEPTH_SIZE);
        u->stencilBits = getEGLConfigAttrib(n, EGL_STENCIL_SIZE);

        u->samples = getEGLConfigAttrib(n, EGL_SAMPLES);
        u->doublebuffer = GLFW_TRUE;

        u->handle = (uintptr_t) n;
        usableCount++;
    }

    closest = _glfwChooseFBConfig(desired, usableConfigs, usableCount);
    if (closest)
        *result = (EGLConfig) closest->handle;

    free(nativeConfigs);
    free(usableConfigs);

    return closest != NULL;
}

static void makeContextCurrentEGL(_GLFWwindow* window)
{
    if (window)
    {
        if (!eglMakeCurrent(_glfw.egl.display,
                            window->context.egl.surface,
                            window->context.egl.surface,
                            window->context.egl.handle))
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "EGL: Failed to make context current: %s",
                            getEGLErrorString(eglGetError()));
            return;
        }
    }
    else
    {
        if (!eglMakeCurrent(_glfw.egl.display,
                            EGL_NO_SURFACE,
                            EGL_NO_SURFACE,
                            EGL_NO_CONTEXT))
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "EGL: Failed to clear current context: %s",
                            getEGLErrorString(eglGetError()));
            return;
        }
    }

    _glfwPlatformSetCurrentContext(window);
}

static void swapBuffersEGL(_GLFWwindow* window)
{
    if (window != _glfwPlatformGetCurrentContext())
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "EGL: The context must be current on the calling thread when swapping buffers");
        return;
    }

    eglSwapBuffers(_glfw.egl.display, window->context.egl.surface);
}

static void swapIntervalEGL(int interval)
{
    eglSwapInterval(_glfw.egl.display, interval);
}

static int extensionSupportedEGL(const char* extension)
{
    const char* extensions = eglQueryString(_glfw.egl.display, EGL_EXTENSIONS);
    if (extensions)
    {
        if (_glfwStringInExtensionString(extension, extensions))
            return GLFW_TRUE;
    }

    return GLFW_FALSE;
}

static GLFWglproc getProcAddressEGL(const char* procname)
{
    _GLFWwindow* window = _glfwPlatformGetCurrentContext();

    if (window->context.egl.client)
    {
        GLFWglproc proc = (GLFWglproc) _glfw_dlsym(window->context.egl.client,
                                                   procname);
        if (proc)
            return proc;
    }

    return eglGetProcAddress(procname);
}

static void destroyContextEGL(_GLFWwindow* window)
{
#if defined(_GLFW_X11)
    // NOTE: Do not unload libGL.so.1 while the X11 display is still open,
    //       as it will make XCloseDisplay segfault
    if (window->context.client != GLFW_OPENGL_API)
#endif // _GLFW_X11
    {
        if (window->context.egl.client)
        {
            _glfw_dlclose(window->context.egl.client);
            window->context.egl.client = NULL;
        }
    }

    if (window->context.egl.surface)
    {
        eglDestroySurface(_glfw.egl.display, window->context.egl.surface);
        window->context.egl.surface = EGL_NO_SURFACE;
    }

    if (window->context.egl.handle)
    {
        eglDestroyContext(_glfw.egl.display, window->context.egl.handle);
        window->context.egl.handle = EGL_NO_CONTEXT;
    }
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

// Initialize EGL
//
GLFWbool _glfwInitEGL(void)
{
    int i;
    const char* sonames[] =
    {
#if defined(_GLFW_WIN32)
        "libEGL.dll",
        "EGL.dll",
#elif defined(_GLFW_COCOA)
        "libEGL.dylib",
#else
        "libEGL.so.1",
#endif
        NULL
    };

    if (_glfw.egl.handle)
        return GLFW_TRUE;

    for (i = 0;  sonames[i];  i++)
    {
        _glfw.egl.handle = _glfw_dlopen(sonames[i]);
        if (_glfw.egl.handle)
            break;
    }

    if (!_glfw.egl.handle)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE, "EGL: Library not found");
        return GLFW_FALSE;
    }

    _glfw.egl.prefix = (strncmp(sonames[i], "lib", 3) == 0);

    _glfw.egl.GetConfigAttrib = (PFN_eglGetConfigAttrib)
        _glfw_dlsym(_glfw.egl.handle, "eglGetConfigAttrib");
    _glfw.egl.GetConfigs = (PFN_eglGetConfigs)
        _glfw_dlsym(_glfw.egl.handle, "eglGetConfigs");
    _glfw.egl.GetDisplay = (PFN_eglGetDisplay)
        _glfw_dlsym(_glfw.egl.handle, "eglGetDisplay");
    _glfw.egl.GetError = (PFN_eglGetError)
        _glfw_dlsym(_glfw.egl.handle, "eglGetError");
    _glfw.egl.Initialize = (PFN_eglInitialize)
        _glfw_dlsym(_glfw.egl.handle, "eglInitialize");
    _glfw.egl.Terminate = (PFN_eglTerminate)
        _glfw_dlsym(_glfw.egl.handle, "eglTerminate");
    _glfw.egl.BindAPI = (PFN_eglBindAPI)
        _glfw_dlsym(_glfw.egl.handle, "eglBindAPI");
    _glfw.egl.CreateContext = (PFN_eglCreateContext)
        _glfw_dlsym(_glfw.egl.handle, "eglCreateContext");
    _glfw.egl.DestroySurface = (PFN_eglDestroySurface)
        _glfw_dlsym(_glfw.egl.handle, "eglDestroySurface");
    _glfw.egl.DestroyContext = (PFN_eglDestroyContext)
        _glfw_dlsym(_glfw.egl.handle, "eglDestroyContext");
    _glfw.egl.CreateWindowSurface = (PFN_eglCreateWindowSurface)
        _glfw_dlsym(_glfw.egl.handle, "eglCreateWindowSurface");
    _glfw.egl.MakeCurrent = (PFN_eglMakeCurrent)
        _glfw_dlsym(_glfw.egl.handle, "eglMakeCurrent");
    _glfw.egl.SwapBuffers = (PFN_eglSwapBuffers)
        _glfw_dlsym(_glfw.egl.handle, "eglSwapBuffers");
    _glfw.egl.SwapInterval = (PFN_eglSwapInterval)
        _glfw_dlsym(_glfw.egl.handle, "eglSwapInterval");
    _glfw.egl.QueryString = (PFN_eglQueryString)
        _glfw_dlsym(_glfw.egl.handle, "eglQueryString");
    _glfw.egl.GetProcAddress = (PFN_eglGetProcAddress)
        _glfw_dlsym(_glfw.egl.handle, "eglGetProcAddress");

    if (!_glfw.egl.GetConfigAttrib ||
        !_glfw.egl.GetConfigs ||
        !_glfw.egl.GetDisplay ||
        !_glfw.egl.GetError ||
        !_glfw.egl.Initialize ||
        !_glfw.egl.Terminate ||
        !_glfw.egl.BindAPI ||
        !_glfw.egl.CreateContext ||
        !_glfw.egl.DestroySurface ||
        !_glfw.egl.DestroyContext ||
        !_glfw.egl.CreateWindowSurface ||
        !_glfw.egl.MakeCurrent ||
        !_glfw.egl.SwapBuffers ||
        !_glfw.egl.SwapInterval ||
        !_glfw.egl.QueryString ||
        !_glfw.egl.GetProcAddress)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "EGL: Failed to load required entry points");

        _glfwTerminateEGL();
        return GLFW_FALSE;
    }

    _glfw.egl.display = eglGetDisplay(_GLFW_EGL_NATIVE_DISPLAY);
    if (_glfw.egl.display == EGL_NO_DISPLAY)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "EGL: Failed to get EGL display: %s",
                        getEGLErrorString(eglGetError()));

        _glfwTerminateEGL();
        return GLFW_FALSE;
    }

    if (!eglInitialize(_glfw.egl.display, &_glfw.egl.major, &_glfw.egl.minor))
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "EGL: Failed to initialize EGL: %s",
                        getEGLErrorString(eglGetError()));

        _glfwTerminateEGL();
        return GLFW_FALSE;
    }

    _glfw.egl.KHR_create_context =
        extensionSupportedEGL("EGL_KHR_create_context");
    _glfw.egl.KHR_create_context_no_error =
        extensionSupportedEGL("EGL_KHR_create_context_no_error");
    _glfw.egl.KHR_gl_colorspace =
        extensionSupportedEGL("EGL_KHR_gl_colorspace");
    _glfw.egl.KHR_get_all_proc_addresses =
        extensionSupportedEGL("EGL_KHR_get_all_proc_addresses");

    return GLFW_TRUE;
}

// Terminate EGL
//
void _glfwTerminateEGL(void)
{
    if (_glfw.egl.display)
    {
        eglTerminate(_glfw.egl.display);
        _glfw.egl.display = EGL_NO_DISPLAY;
    }

    if (_glfw.egl.handle)
    {
        _glfw_dlclose(_glfw.egl.handle);
        _glfw.egl.handle = NULL;
    }
}

#define setEGLattrib(attribName, attribValue) \
{ \
    attribs[index++] = attribName; \
    attribs[index++] = attribValue; \
    assert((size_t) index < sizeof(attribs) / sizeof(attribs[0])); \
}

// Create the OpenGL or OpenGL ES context
//
GLFWbool _glfwCreateContextEGL(_GLFWwindow* window,
                               const _GLFWctxconfig* ctxconfig,
                               const _GLFWfbconfig* fbconfig)
{
    EGLint attribs[40];
    EGLConfig config;
    EGLContext share = NULL;

    if (!_glfw.egl.display)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE, "EGL: API not available");
        return GLFW_FALSE;
    }

    if (ctxconfig->share)
        share = ctxconfig->share->context.egl.handle;

    if (!chooseEGLConfig(ctxconfig, fbconfig, &config))
    {
        _glfwInputError(GLFW_FORMAT_UNAVAILABLE,
                        "EGL: Failed to find a suitable EGLConfig");
        return GLFW_FALSE;
    }

    if (ctxconfig->client == GLFW_OPENGL_ES_API)
    {
        if (!eglBindAPI(EGL_OPENGL_ES_API))
        {
            _glfwInputError(GLFW_API_UNAVAILABLE,
                            "EGL: Failed to bind OpenGL ES: %s",
                            getEGLErrorString(eglGetError()));
            return GLFW_FALSE;
        }
    }
    else
    {
        if (!eglBindAPI(EGL_OPENGL_API))
        {
            _glfwInputError(GLFW_API_UNAVAILABLE,
                            "EGL: Failed to bind OpenGL: %s",
                            getEGLErrorString(eglGetError()));
            return GLFW_FALSE;
        }
    }

    if (_glfw.egl.KHR_create_context)
    {
        int index = 0, mask = 0, flags = 0;

        if (ctxconfig->client == GLFW_OPENGL_API)
        {
            if (ctxconfig->forward)
                flags |= EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR;

            if (ctxconfig->profile == GLFW_OPENGL_CORE_PROFILE)
                mask |= EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR;
            else if (ctxconfig->profile == GLFW_OPENGL_COMPAT_PROFILE)
                mask |= EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR;

            if (_glfw.egl.KHR_create_context_no_error)
            {
                if (ctxconfig->noerror)
                    flags |= EGL_CONTEXT_OPENGL_NO_ERROR_KHR;
            }
        }

        if (ctxconfig->debug)
            flags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;

        if (ctxconfig->robustness)
        {
            if (ctxconfig->robustness == GLFW_NO_RESET_NOTIFICATION)
            {
                setEGLattrib(EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR,
                             EGL_NO_RESET_NOTIFICATION_KHR);
            }
            else if (ctxconfig->robustness == GLFW_LOSE_CONTEXT_ON_RESET)
            {
                setEGLattrib(EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR,
                             EGL_LOSE_CONTEXT_ON_RESET_KHR);
            }

            flags |= EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR;
        }

        if (ctxconfig->major != 1 || ctxconfig->minor != 0)
        {
            setEGLattrib(EGL_CONTEXT_MAJOR_VERSION_KHR, ctxconfig->major);
            setEGLattrib(EGL_CONTEXT_MINOR_VERSION_KHR, ctxconfig->minor);
        }

        if (mask)
            setEGLattrib(EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, mask);

        if (flags)
            setEGLattrib(EGL_CONTEXT_FLAGS_KHR, flags);

        setEGLattrib(EGL_NONE, EGL_NONE);
    }
    else
    {
        int index = 0;

        if (ctxconfig->client == GLFW_OPENGL_ES_API)
            setEGLattrib(EGL_CONTEXT_CLIENT_VERSION, ctxconfig->major);

        setEGLattrib(EGL_NONE, EGL_NONE);
    }

    // Context release behaviors (GL_KHR_context_flush_control) are not yet
    // supported on EGL but are not a hard constraint, so ignore and continue

    window->context.egl.handle = eglCreateContext(_glfw.egl.display,
                                                  config, share, attribs);

    if (window->context.egl.handle == EGL_NO_CONTEXT)
    {
        _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                        "EGL: Failed to create context: %s",
                        getEGLErrorString(eglGetError()));
        return GLFW_FALSE;
    }

    // Set up attributes for surface creation
    {
        int index = 0;

        if (fbconfig->sRGB)
        {
            if (_glfw.egl.KHR_gl_colorspace)
            {
                setEGLattrib(EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR);
            }
        }

        setEGLattrib(EGL_NONE, EGL_NONE);
    }

    window->context.egl.surface =
        eglCreateWindowSurface(_glfw.egl.display,
                               config,
                               _GLFW_EGL_NATIVE_WINDOW,
                               attribs);
    if (window->context.egl.surface == EGL_NO_SURFACE)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "EGL: Failed to create window surface: %s",
                        getEGLErrorString(eglGetError()));
        return GLFW_FALSE;
    }

    window->context.egl.config = config;

    // Load the appropriate client library
    if (!_glfw.egl.KHR_get_all_proc_addresses)
    {
        int i;
        const char** sonames;
        const char* es1sonames[] =
        {
#if defined(_GLFW_WIN32)
            "GLESv1_CM.dll",
            "libGLES_CM.dll",
#elif defined(_GLFW_COCOA)
            "libGLESv1_CM.dylib",
#else
            "libGLESv1_CM.so.1",
            "libGLES_CM.so.1",
#endif
            NULL
        };
        const char* es2sonames[] =
        {
#if defined(_GLFW_WIN32)
            "GLESv2.dll",
            "libGLESv2.dll",
#elif defined(_GLFW_COCOA)
            "libGLESv2.dylib",
#else
            "libGLESv2.so.2",
#endif
            NULL
        };
        const char* glsonames[] =
        {
#if defined(_GLFW_WIN32)
#elif defined(_GLFW_COCOA)
#else
            "libGL.so.1",
#endif
            NULL
        };

        if (ctxconfig->client == GLFW_OPENGL_ES_API)
        {
            if (ctxconfig->major == 1)
                sonames = es1sonames;
            else
                sonames = es2sonames;
        }
        else
            sonames = glsonames;

        for (i = 0;  sonames[i];  i++)
        {
            // HACK: Match presence of lib prefix to increase chance of finding
            //       a matching pair in the jungle that is Win32 EGL/GLES
            if (_glfw.egl.prefix != (strncmp(sonames[i], "lib", 3) == 0))
                continue;

            window->context.egl.client = _glfw_dlopen(sonames[i]);
            if (window->context.egl.client)
                break;
        }

        if (!window->context.egl.client)
        {
            _glfwInputError(GLFW_API_UNAVAILABLE,
                            "EGL: Failed to load client library");
            return GLFW_FALSE;
        }
    }

    window->context.makeCurrent = makeContextCurrentEGL;
    window->context.swapBuffers = swapBuffersEGL;
    window->context.swapInterval = swapIntervalEGL;
    window->context.extensionSupported = extensionSupportedEGL;
    window->context.getProcAddress = getProcAddressEGL;
    window->context.destroy = destroyContextEGL;

    return GLFW_TRUE;
}

#undef setEGLattrib

// Returns the Visual and depth of the chosen EGLConfig
//
#if defined(_GLFW_X11)
GLFWbool _glfwChooseVisualEGL(const _GLFWctxconfig* ctxconfig,
                              const _GLFWfbconfig* fbconfig,
                              Visual** visual, int* depth)
{
    XVisualInfo* result;
    XVisualInfo desired;
    EGLConfig native;
    EGLint visualID = 0, count = 0;
    const long vimask = VisualScreenMask | VisualIDMask;

    if (!chooseEGLConfig(ctxconfig, fbconfig, &native))
    {
        _glfwInputError(GLFW_FORMAT_UNAVAILABLE,
                        "EGL: Failed to find a suitable EGLConfig");
        return GLFW_FALSE;
    }

    eglGetConfigAttrib(_glfw.egl.display, native,
                       EGL_NATIVE_VISUAL_ID, &visualID);

    desired.screen = _glfw.x11.screen;
    desired.visualid = visualID;

    result = XGetVisualInfo(_glfw.x11.display, vimask, &desired, &count);
    if (!result)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "EGL: Failed to retrieve Visual for EGLConfig");
        return GLFW_FALSE;
    }

    *visual = result->visual;
    *depth = result->depth;

    XFree(result);
    return GLFW_TRUE;
}
#endif // _GLFW_X11


//////////////////////////////////////////////////////////////////////////
//////                        GLFW native API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI EGLDisplay glfwGetEGLDisplay(void)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(EGL_NO_DISPLAY);
    return _glfw.egl.display;
}

GLFWAPI EGLContext glfwGetEGLContext(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(EGL_NO_CONTEXT);

    if (window->context.client == GLFW_NO_API)
    {
        _glfwInputError(GLFW_NO_WINDOW_CONTEXT, NULL);
        return EGL_NO_CONTEXT;
    }

    return window->context.egl.handle;
}

GLFWAPI EGLSurface glfwGetEGLSurface(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(EGL_NO_SURFACE);

    if (window->context.client == GLFW_NO_API)
    {
        _glfwInputError(GLFW_NO_WINDOW_CONTEXT, NULL);
        return EGL_NO_SURFACE;
    }

    return window->context.egl.surface;
}

//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


// The three global variables below comprise all global data in GLFW.
// Any other global variable is a bug.

// Global state shared between compilation units of GLFW
// These are documented in internal.h
//
GLFWbool _glfwInitialized = GLFW_FALSE;
_GLFWlibrary _glfw;

// This is outside of _glfw so it can be initialized and usable before
// glfwInit is called, which lets that function report errors
//
static GLFWerrorfun _glfwErrorCallback = NULL;


// Returns a generic string representation of the specified error
//
static const char* getErrorString(int error)
{
    switch (error)
    {
        case GLFW_NOT_INITIALIZED:
            return "The GLFW library is not initialized";
        case GLFW_NO_CURRENT_CONTEXT:
            return "There is no current context";
        case GLFW_INVALID_ENUM:
            return "Invalid argument for enum parameter";
        case GLFW_INVALID_VALUE:
            return "Invalid value for parameter";
        case GLFW_OUT_OF_MEMORY:
            return "Out of memory";
        case GLFW_API_UNAVAILABLE:
            return "The requested API is unavailable";
        case GLFW_VERSION_UNAVAILABLE:
            return "The requested API version is unavailable";
        case GLFW_PLATFORM_ERROR:
            return "A platform-specific error occurred";
        case GLFW_FORMAT_UNAVAILABLE:
            return "The requested format is unavailable";
        case GLFW_NO_WINDOW_CONTEXT:
            return "The specified window has no context";
        default:
            return "ERROR: UNKNOWN GLFW ERROR";
    }
}


//////////////////////////////////////////////////////////////////////////
//////                         GLFW event API                       //////
//////////////////////////////////////////////////////////////////////////

void _glfwInputError(int error, const char* format, ...)
{
    if (_glfwErrorCallback)
    {
        char buffer[8192];
        const char* description;

        if (format)
        {
            int count;
            va_list vl;

            va_start(vl, format);
            count = vsnprintf(buffer, sizeof(buffer), format, vl);
            va_end(vl);

            if (count < 0)
                buffer[sizeof(buffer) - 1] = '\0';

            description = buffer;
        }
        else
            description = getErrorString(error);

        _glfwErrorCallback(error, description);
    }
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI int glfwInit(void)
{
    if (_glfwInitialized)
        return GLFW_TRUE;

    memset(&_glfw, 0, sizeof(_glfw));

    if (!_glfwPlatformInit())
    {
        _glfwPlatformTerminate();
        return GLFW_FALSE;
    }

    _glfwInitialized = GLFW_TRUE;

    _glfw.timerOffset = _glfwPlatformGetTimerValue();

    // Not all window hints have zero as their default value
    glfwDefaultWindowHints();

    return GLFW_TRUE;
}

GLFWAPI void glfwTerminate(void)
{
    int i;

    if (!_glfwInitialized)
        return;

    memset(&_glfw.callbacks, 0, sizeof(_glfw.callbacks));

    while (_glfw.windowListHead)
        glfwDestroyWindow((GLFWwindow*) _glfw.windowListHead);

    while (_glfw.cursorListHead)
        glfwDestroyCursor((GLFWcursor*) _glfw.cursorListHead);

    for (i = 0;  i < _glfw.monitorCount;  i++)
    {
        _GLFWmonitor* monitor = _glfw.monitors[i];
        if (monitor->originalRamp.size)
            _glfwPlatformSetGammaRamp(monitor, &monitor->originalRamp);
    }

    _glfwTerminateVulkan();

    _glfwFreeMonitors(_glfw.monitors, _glfw.monitorCount);
    _glfw.monitors = NULL;
    _glfw.monitorCount = 0;

    _glfwPlatformTerminate();

    memset(&_glfw, 0, sizeof(_glfw));
    _glfwInitialized = GLFW_FALSE;
}

GLFWAPI void glfwGetVersion(int* major, int* minor, int* rev)
{
    if (major != NULL)
        *major = GLFW_VERSION_MAJOR;

    if (minor != NULL)
        *minor = GLFW_VERSION_MINOR;

    if (rev != NULL)
        *rev = GLFW_VERSION_REVISION;
}

GLFWAPI const char* glfwGetVersionString(void)
{
    return _glfwPlatformGetVersionString();
}

GLFWAPI GLFWerrorfun glfwSetErrorCallback(GLFWerrorfun cbfun)
{
    _GLFW_SWAP_POINTERS(_glfwErrorCallback, cbfun);
    return cbfun;
}

//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <assert.h>
#include <float.h>
#include <stdlib.h>

// Internal key state used for sticky keys
#define _GLFW_STICK 3


//////////////////////////////////////////////////////////////////////////
//////                         GLFW event API                       //////
//////////////////////////////////////////////////////////////////////////

void _glfwInputKey(_GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key <= GLFW_KEY_LAST)
    {
        GLFWbool repeated = GLFW_FALSE;

        if (action == GLFW_RELEASE && window->keys[key] == GLFW_RELEASE)
            return;

        if (action == GLFW_PRESS && window->keys[key] == GLFW_PRESS)
            repeated = GLFW_TRUE;

        if (action == GLFW_RELEASE && window->stickyKeys)
            window->keys[key] = _GLFW_STICK;
        else
            window->keys[key] = (char) action;

        if (repeated)
            action = GLFW_REPEAT;
    }

    if (window->callbacks.key)
        window->callbacks.key((GLFWwindow*) window, key, scancode, action, mods);
}

void _glfwInputChar(_GLFWwindow* window, unsigned int codepoint, int mods, GLFWbool plain)
{
    if (codepoint < 32 || (codepoint > 126 && codepoint < 160))
        return;

    if (window->callbacks.charmods)
        window->callbacks.charmods((GLFWwindow*) window, codepoint, mods);

    if (plain)
    {
        if (window->callbacks.character)
            window->callbacks.character((GLFWwindow*) window, codepoint);
    }
}

void _glfwInputScroll(_GLFWwindow* window, double xoffset, double yoffset)
{
    if (window->callbacks.scroll)
        window->callbacks.scroll((GLFWwindow*) window, xoffset, yoffset);
}

void _glfwInputMouseClick(_GLFWwindow* window, int button, int action, int mods)
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return;

    // Register mouse button action
    if (action == GLFW_RELEASE && window->stickyMouseButtons)
        window->mouseButtons[button] = _GLFW_STICK;
    else
        window->mouseButtons[button] = (char) action;

    if (window->callbacks.mouseButton)
        window->callbacks.mouseButton((GLFWwindow*) window, button, action, mods);
}

void _glfwInputCursorPos(_GLFWwindow* window, double xpos, double ypos)
{
    if (window->virtualCursorPosX == xpos && window->virtualCursorPosY == ypos)
        return;

    window->virtualCursorPosX = xpos;
    window->virtualCursorPosY = ypos;

    if (window->callbacks.cursorPos)
        window->callbacks.cursorPos((GLFWwindow*) window, xpos, ypos);
}

void _glfwInputCursorEnter(_GLFWwindow* window, GLFWbool entered)
{
    if (window->callbacks.cursorEnter)
        window->callbacks.cursorEnter((GLFWwindow*) window, entered);
}

void _glfwInputDrop(_GLFWwindow* window, int count, const char** paths)
{
    if (window->callbacks.drop)
        window->callbacks.drop((GLFWwindow*) window, count, paths);
}

void _glfwInputJoystickChange(int jid, int event)
{
    if (_glfw.callbacks.joystick)
        _glfw.callbacks.joystick(jid, event);
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

GLFWbool _glfwIsPrintable(int key)
{
    return (key >= GLFW_KEY_APOSTROPHE && key <= GLFW_KEY_WORLD_2) ||
           (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_ADD) ||
           key == GLFW_KEY_KP_EQUAL;
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI int glfwGetInputMode(GLFWwindow* handle, int mode)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(0);

    switch (mode)
    {
        case GLFW_CURSOR:
            return window->cursorMode;
        case GLFW_STICKY_KEYS:
            return window->stickyKeys;
        case GLFW_STICKY_MOUSE_BUTTONS:
            return window->stickyMouseButtons;
        default:
            _glfwInputError(GLFW_INVALID_ENUM, "Invalid input mode %i", mode);
            return 0;
    }
}

GLFWAPI void glfwSetInputMode(GLFWwindow* handle, int mode, int value)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    switch (mode)
    {
        case GLFW_CURSOR:
        {
            if (value != GLFW_CURSOR_NORMAL &&
                value != GLFW_CURSOR_HIDDEN &&
                value != GLFW_CURSOR_DISABLED)
            {
                _glfwInputError(GLFW_INVALID_ENUM,
                                "Invalid cursor mode %i",
                                value);
                return;
            }

            if (window->cursorMode == value)
                return;

            window->cursorMode = value;

            _glfwPlatformGetCursorPos(window,
                                      &window->virtualCursorPosX,
                                      &window->virtualCursorPosY);

            if (_glfwPlatformWindowFocused(window))
                _glfwPlatformSetCursorMode(window, value);

            return;
        }

        case GLFW_STICKY_KEYS:
        {
            if (window->stickyKeys == value)
                return;

            if (!value)
            {
                int i;

                // Release all sticky keys
                for (i = 0;  i <= GLFW_KEY_LAST;  i++)
                {
                    if (window->keys[i] == _GLFW_STICK)
                        window->keys[i] = GLFW_RELEASE;
                }
            }

            window->stickyKeys = value ? GLFW_TRUE : GLFW_FALSE;
            return;
        }

        case GLFW_STICKY_MOUSE_BUTTONS:
        {
            if (window->stickyMouseButtons == value)
                return;

            if (!value)
            {
                int i;

                // Release all sticky mouse buttons
                for (i = 0;  i <= GLFW_MOUSE_BUTTON_LAST;  i++)
                {
                    if (window->mouseButtons[i] == _GLFW_STICK)
                        window->mouseButtons[i] = GLFW_RELEASE;
                }
            }

            window->stickyMouseButtons = value ? GLFW_TRUE : GLFW_FALSE;
            return;
        }
    }

    _glfwInputError(GLFW_INVALID_ENUM, "Invalid input mode %i", mode);
}

GLFWAPI const char* glfwGetKeyName(int key, int scancode)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return _glfwPlatformGetKeyName(key, scancode);
}

GLFWAPI int glfwGetKeyScancode(int key)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(-1);

    if (key < GLFW_KEY_SPACE || key > GLFW_KEY_LAST)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid key %i", key);
        return GLFW_RELEASE;
    }

    return _glfwPlatformGetKeyScancode(key);
}

GLFWAPI int glfwGetKey(GLFWwindow* handle, int key)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(GLFW_RELEASE);

    if (key < GLFW_KEY_SPACE || key > GLFW_KEY_LAST)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid key %i", key);
        return GLFW_RELEASE;
    }

    if (window->keys[key] == _GLFW_STICK)
    {
        // Sticky mode: release key now
        window->keys[key] = GLFW_RELEASE;
        return GLFW_PRESS;
    }

    return (int) window->keys[key];
}

GLFWAPI int glfwGetMouseButton(GLFWwindow* handle, int button)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(GLFW_RELEASE);

    if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid mouse button %i", button);
        return GLFW_RELEASE;
    }

    if (window->mouseButtons[button] == _GLFW_STICK)
    {
        // Sticky mode: release mouse button now
        window->mouseButtons[button] = GLFW_RELEASE;
        return GLFW_PRESS;
    }

    return (int) window->mouseButtons[button];
}

GLFWAPI void glfwGetCursorPos(GLFWwindow* handle, double* xpos, double* ypos)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    if (xpos)
        *xpos = 0;
    if (ypos)
        *ypos = 0;

    _GLFW_REQUIRE_INIT();

    if (window->cursorMode == GLFW_CURSOR_DISABLED)
    {
        if (xpos)
            *xpos = window->virtualCursorPosX;
        if (ypos)
            *ypos = window->virtualCursorPosY;
    }
    else
        _glfwPlatformGetCursorPos(window, xpos, ypos);
}

GLFWAPI void glfwSetCursorPos(GLFWwindow* handle, double xpos, double ypos)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    if (xpos != xpos || xpos < -DBL_MAX || xpos > DBL_MAX ||
        ypos != ypos || ypos < -DBL_MAX || ypos > DBL_MAX)
    {
        _glfwInputError(GLFW_INVALID_VALUE,
                        "Invalid cursor position %f %f",
                        xpos, ypos);
        return;
    }

    if (!_glfwPlatformWindowFocused(window))
        return;

    if (window->cursorMode == GLFW_CURSOR_DISABLED)
    {
        // Only update the accumulated position if the cursor is disabled
        window->virtualCursorPosX = xpos;
        window->virtualCursorPosY = ypos;
    }
    else
    {
        // Update system cursor position
        _glfwPlatformSetCursorPos(window, xpos, ypos);
    }
}

GLFWAPI GLFWcursor* glfwCreateCursor(const GLFWimage* image, int xhot, int yhot)
{
    _GLFWcursor* cursor;

    assert(image != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    cursor = calloc(1, sizeof(_GLFWcursor));
    cursor->next = _glfw.cursorListHead;
    _glfw.cursorListHead = cursor;

    if (!_glfwPlatformCreateCursor(cursor, image, xhot, yhot))
    {
        glfwDestroyCursor((GLFWcursor*) cursor);
        return NULL;
    }

    return (GLFWcursor*) cursor;
}

GLFWAPI GLFWcursor* glfwCreateStandardCursor(int shape)
{
    _GLFWcursor* cursor;

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (shape != GLFW_ARROW_CURSOR &&
        shape != GLFW_IBEAM_CURSOR &&
        shape != GLFW_CROSSHAIR_CURSOR &&
        shape != GLFW_HAND_CURSOR &&
        shape != GLFW_HRESIZE_CURSOR &&
        shape != GLFW_VRESIZE_CURSOR)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid standard cursor %i", shape);
        return NULL;
    }

    cursor = calloc(1, sizeof(_GLFWcursor));
    cursor->next = _glfw.cursorListHead;
    _glfw.cursorListHead = cursor;

    if (!_glfwPlatformCreateStandardCursor(cursor, shape))
    {
        glfwDestroyCursor((GLFWcursor*) cursor);
        return NULL;
    }

    return (GLFWcursor*) cursor;
}

GLFWAPI void glfwDestroyCursor(GLFWcursor* handle)
{
    _GLFWcursor* cursor = (_GLFWcursor*) handle;

    _GLFW_REQUIRE_INIT();

    if (cursor == NULL)
        return;

    // Make sure the cursor is not being used by any window
    {
        _GLFWwindow* window;

        for (window = _glfw.windowListHead;  window;  window = window->next)
        {
            if (window->cursor == cursor)
                glfwSetCursor((GLFWwindow*) window, NULL);
        }
    }

    _glfwPlatformDestroyCursor(cursor);

    // Unlink cursor from global linked list
    {
        _GLFWcursor** prev = &_glfw.cursorListHead;

        while (*prev != cursor)
            prev = &((*prev)->next);

        *prev = cursor->next;
    }

    free(cursor);
}

GLFWAPI void glfwSetCursor(GLFWwindow* windowHandle, GLFWcursor* cursorHandle)
{
    _GLFWwindow* window = (_GLFWwindow*) windowHandle;
    _GLFWcursor* cursor = (_GLFWcursor*) cursorHandle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    window->cursor = cursor;

    _glfwPlatformSetCursor(window, cursor);
}

GLFWAPI GLFWkeyfun glfwSetKeyCallback(GLFWwindow* handle, GLFWkeyfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.key, cbfun);
    return cbfun;
}

GLFWAPI GLFWcharfun glfwSetCharCallback(GLFWwindow* handle, GLFWcharfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.character, cbfun);
    return cbfun;
}

GLFWAPI GLFWcharmodsfun glfwSetCharModsCallback(GLFWwindow* handle, GLFWcharmodsfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.charmods, cbfun);
    return cbfun;
}

GLFWAPI GLFWmousebuttonfun glfwSetMouseButtonCallback(GLFWwindow* handle,
                                                      GLFWmousebuttonfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.mouseButton, cbfun);
    return cbfun;
}

GLFWAPI GLFWcursorposfun glfwSetCursorPosCallback(GLFWwindow* handle,
                                                  GLFWcursorposfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.cursorPos, cbfun);
    return cbfun;
}

GLFWAPI GLFWcursorenterfun glfwSetCursorEnterCallback(GLFWwindow* handle,
                                                      GLFWcursorenterfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.cursorEnter, cbfun);
    return cbfun;
}

GLFWAPI GLFWscrollfun glfwSetScrollCallback(GLFWwindow* handle,
                                            GLFWscrollfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.scroll, cbfun);
    return cbfun;
}

GLFWAPI GLFWdropfun glfwSetDropCallback(GLFWwindow* handle, GLFWdropfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.drop, cbfun);
    return cbfun;
}

GLFWAPI int glfwJoystickPresent(int jid)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(0);

    if (jid < 0 || jid > GLFW_JOYSTICK_LAST)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid joystick %i", jid);
        return 0;
    }

    return _glfwPlatformJoystickPresent(jid);
}

GLFWAPI const float* glfwGetJoystickAxes(int jid, int* count)
{
    assert(count != NULL);
    *count = 0;

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (jid < 0 || jid > GLFW_JOYSTICK_LAST)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid joystick %i", jid);
        return NULL;
    }

    return _glfwPlatformGetJoystickAxes(jid, count);
}

GLFWAPI const unsigned char* glfwGetJoystickButtons(int jid, int* count)
{
    assert(count != NULL);
    *count = 0;

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (jid < 0 || jid > GLFW_JOYSTICK_LAST)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid joystick %i", jid);
        return NULL;
    }

    return _glfwPlatformGetJoystickButtons(jid, count);
}

GLFWAPI const char* glfwGetJoystickName(int jid)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (jid < 0 || jid > GLFW_JOYSTICK_LAST)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid joystick %i", jid);
        return NULL;
    }

    return _glfwPlatformGetJoystickName(jid);
}

GLFWAPI GLFWjoystickfun glfwSetJoystickCallback(GLFWjoystickfun cbfun)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(_glfw.callbacks.joystick, cbfun);
    return cbfun;
}

GLFWAPI void glfwSetClipboardString(GLFWwindow* handle, const char* string)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);
    assert(string != NULL);

    _GLFW_REQUIRE_INIT();
    _glfwPlatformSetClipboardString(window, string);
}

GLFWAPI const char* glfwGetClipboardString(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return _glfwPlatformGetClipboardString(window);
}

GLFWAPI double glfwGetTime(void)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(0.0);
    return (double) (_glfwPlatformGetTimerValue() - _glfw.timerOffset) /
        _glfwPlatformGetTimerFrequency();
}

GLFWAPI void glfwSetTime(double time)
{
    _GLFW_REQUIRE_INIT();

    if (time != time || time < 0.0 || time > 18446744073.0)
    {
        _glfwInputError(GLFW_INVALID_VALUE, "Invalid time %f", time);
        return;
    }

    _glfw.timerOffset = _glfwPlatformGetTimerValue() -
        (uint64_t) (time * _glfwPlatformGetTimerFrequency());
}

GLFWAPI uint64_t glfwGetTimerValue(void)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(0);
    return _glfwPlatformGetTimerValue();
}

GLFWAPI uint64_t glfwGetTimerFrequency(void)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(0);
    return _glfwPlatformGetTimerFrequency();
}

//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <assert.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>


// Lexically compare video modes, used by qsort
//
static int compareVideoModes(const void* fp, const void* sp)
{
    const GLFWvidmode* fm = fp;
    const GLFWvidmode* sm = sp;
    const int fbpp = fm->redBits + fm->greenBits + fm->blueBits;
    const int sbpp = sm->redBits + sm->greenBits + sm->blueBits;
    const int farea = fm->width * fm->height;
    const int sarea = sm->width * sm->height;

    // First sort on color bits per pixel
    if (fbpp != sbpp)
        return fbpp - sbpp;

    // Then sort on screen area
    if (farea != sarea)
        return farea - sarea;

    // Lastly sort on refresh rate
    return fm->refreshRate - sm->refreshRate;
}

// Retrieves the available modes for the specified monitor
//
static GLFWbool refreshVideoModes(_GLFWmonitor* monitor)
{
    int modeCount;
    GLFWvidmode* modes;

    if (monitor->modes)
        return GLFW_TRUE;

    modes = _glfwPlatformGetVideoModes(monitor, &modeCount);
    if (!modes)
        return GLFW_FALSE;

    qsort(modes, modeCount, sizeof(GLFWvidmode), compareVideoModes);

    free(monitor->modes);
    monitor->modes = modes;
    monitor->modeCount = modeCount;

    return GLFW_TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////                         GLFW event API                       //////
//////////////////////////////////////////////////////////////////////////

void _glfwInputMonitor(_GLFWmonitor* monitor, int action, int type)
{
    if (action == GLFW_CONNECTED)
    {
        _glfw.monitorCount++;
        _glfw.monitors =
            realloc(_glfw.monitors, sizeof(_GLFWmonitor*) * _glfw.monitorCount);

        if (type == _GLFW_INSERT_FIRST)
        {
            memmove(_glfw.monitors + 1,
                    _glfw.monitors,
                    (_glfw.monitorCount - 1) * sizeof(_GLFWmonitor*));
            _glfw.monitors[0] = monitor;
        }
        else
            _glfw.monitors[_glfw.monitorCount - 1] = monitor;
    }
    else if (action == GLFW_DISCONNECTED)
    {
        int i;

        for (i = 0;  i < _glfw.monitorCount;  i++)
        {
            if (_glfw.monitors[i] == monitor)
            {
                _glfw.monitorCount--;
                memmove(_glfw.monitors + i,
                        _glfw.monitors + i + 1,
                        (_glfw.monitorCount - i) * sizeof(_GLFWmonitor*));
                break;
            }
        }
    }

    if (_glfw.callbacks.monitor)
        _glfw.callbacks.monitor((GLFWmonitor*) monitor, action);

    if (action == GLFW_DISCONNECTED)
        _glfwFreeMonitor(monitor);
}

void _glfwInputMonitorWindowChange(_GLFWmonitor* monitor, _GLFWwindow* window)
{
    monitor->window = window;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

_GLFWmonitor* _glfwAllocMonitor(const char* name, int widthMM, int heightMM)
{
    _GLFWmonitor* monitor = calloc(1, sizeof(_GLFWmonitor));
    monitor->widthMM = widthMM;
    monitor->heightMM = heightMM;

    if (name)
        monitor->name = strdup(name);

    return monitor;
}

void _glfwFreeMonitor(_GLFWmonitor* monitor)
{
    if (monitor == NULL)
        return;

    _glfwFreeGammaArrays(&monitor->originalRamp);
    _glfwFreeGammaArrays(&monitor->currentRamp);

    free(monitor->modes);
    free(monitor->name);
    free(monitor);
}

void _glfwAllocGammaArrays(GLFWgammaramp* ramp, unsigned int size)
{
    ramp->red = calloc(size, sizeof(unsigned short));
    ramp->green = calloc(size, sizeof(unsigned short));
    ramp->blue = calloc(size, sizeof(unsigned short));
    ramp->size = size;
}

void _glfwFreeGammaArrays(GLFWgammaramp* ramp)
{
    free(ramp->red);
    free(ramp->green);
    free(ramp->blue);

    memset(ramp, 0, sizeof(GLFWgammaramp));
}

void _glfwFreeMonitors(_GLFWmonitor** monitors, int count)
{
    int i;

    for (i = 0;  i < count;  i++)
        _glfwFreeMonitor(monitors[i]);

    free(monitors);
}

const GLFWvidmode* _glfwChooseVideoMode(_GLFWmonitor* monitor,
                                        const GLFWvidmode* desired)
{
    int i;
    unsigned int sizeDiff, leastSizeDiff = UINT_MAX;
    unsigned int rateDiff, leastRateDiff = UINT_MAX;
    unsigned int colorDiff, leastColorDiff = UINT_MAX;
    const GLFWvidmode* current;
    const GLFWvidmode* closest = NULL;

    if (!refreshVideoModes(monitor))
        return NULL;

    for (i = 0;  i < monitor->modeCount;  i++)
    {
        current = monitor->modes + i;

        colorDiff = 0;

        if (desired->redBits != GLFW_DONT_CARE)
            colorDiff += abs(current->redBits - desired->redBits);
        if (desired->greenBits != GLFW_DONT_CARE)
            colorDiff += abs(current->greenBits - desired->greenBits);
        if (desired->blueBits != GLFW_DONT_CARE)
            colorDiff += abs(current->blueBits - desired->blueBits);

        sizeDiff = abs((current->width - desired->width) *
                       (current->width - desired->width) +
                       (current->height - desired->height) *
                       (current->height - desired->height));

        if (desired->refreshRate != GLFW_DONT_CARE)
            rateDiff = abs(current->refreshRate - desired->refreshRate);
        else
            rateDiff = UINT_MAX - current->refreshRate;

        if ((colorDiff < leastColorDiff) ||
            (colorDiff == leastColorDiff && sizeDiff < leastSizeDiff) ||
            (colorDiff == leastColorDiff && sizeDiff == leastSizeDiff && rateDiff < leastRateDiff))
        {
            closest = current;
            leastSizeDiff = sizeDiff;
            leastRateDiff = rateDiff;
            leastColorDiff = colorDiff;
        }
    }

    return closest;
}

int _glfwCompareVideoModes(const GLFWvidmode* fm, const GLFWvidmode* sm)
{
    return compareVideoModes(fm, sm);
}

void _glfwSplitBPP(int bpp, int* red, int* green, int* blue)
{
    int delta;

    // We assume that by 32 the user really meant 24
    if (bpp == 32)
        bpp = 24;

    // Convert "bits per pixel" to red, green & blue sizes

    *red = *green = *blue = bpp / 3;
    delta = bpp - (*red * 3);
    if (delta >= 1)
        *green = *green + 1;

    if (delta == 2)
        *red = *red + 1;
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI GLFWmonitor** glfwGetMonitors(int* count)
{
    assert(count != NULL);
    *count = 0;

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    *count = _glfw.monitorCount;
    return (GLFWmonitor**) _glfw.monitors;
}

GLFWAPI GLFWmonitor* glfwGetPrimaryMonitor(void)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (!_glfw.monitorCount)
        return NULL;

    return (GLFWmonitor*) _glfw.monitors[0];
}

GLFWAPI void glfwGetMonitorPos(GLFWmonitor* handle, int* xpos, int* ypos)
{
    _GLFWmonitor* monitor = (_GLFWmonitor*) handle;
    assert(monitor != NULL);

    if (xpos)
        *xpos = 0;
    if (ypos)
        *ypos = 0;

    _GLFW_REQUIRE_INIT();

    _glfwPlatformGetMonitorPos(monitor, xpos, ypos);
}

GLFWAPI void glfwGetMonitorPhysicalSize(GLFWmonitor* handle, int* widthMM, int* heightMM)
{
    _GLFWmonitor* monitor = (_GLFWmonitor*) handle;
    assert(monitor != NULL);

    if (widthMM)
        *widthMM = 0;
    if (heightMM)
        *heightMM = 0;

    _GLFW_REQUIRE_INIT();

    if (widthMM)
        *widthMM = monitor->widthMM;
    if (heightMM)
        *heightMM = monitor->heightMM;
}

GLFWAPI const char* glfwGetMonitorName(GLFWmonitor* handle)
{
    _GLFWmonitor* monitor = (_GLFWmonitor*) handle;
    assert(monitor != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return monitor->name;
}

GLFWAPI GLFWmonitorfun glfwSetMonitorCallback(GLFWmonitorfun cbfun)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(_glfw.callbacks.monitor, cbfun);
    return cbfun;
}

GLFWAPI const GLFWvidmode* glfwGetVideoModes(GLFWmonitor* handle, int* count)
{
    _GLFWmonitor* monitor = (_GLFWmonitor*) handle;
    assert(monitor != NULL);
    assert(count != NULL);

    *count = 0;

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (!refreshVideoModes(monitor))
        return NULL;

    *count = monitor->modeCount;
    return monitor->modes;
}

GLFWAPI const GLFWvidmode* glfwGetVideoMode(GLFWmonitor* handle)
{
    _GLFWmonitor* monitor = (_GLFWmonitor*) handle;
    assert(monitor != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    _glfwPlatformGetVideoMode(monitor, &monitor->currentMode);
    return &monitor->currentMode;
}

GLFWAPI void glfwSetGamma(GLFWmonitor* handle, float gamma)
{
    int i;
    unsigned short values[256];
    GLFWgammaramp ramp;

    _GLFW_REQUIRE_INIT();

    if (gamma != gamma || gamma <= 0.f || gamma > FLT_MAX)
    {
        _glfwInputError(GLFW_INVALID_VALUE, "Invalid gamma value %f", gamma);
        return;
    }

    for (i = 0;  i < 256;  i++)
    {
        double value;

        // Calculate intensity
        value = i / 255.0;
        // Apply gamma curve
        value = pow(value, 1.0 / gamma) * 65535.0 + 0.5;

        // Clamp to value range
        if (value > 65535.0)
            value = 65535.0;

        values[i] = (unsigned short) value;
    }

    ramp.red = values;
    ramp.green = values;
    ramp.blue = values;
    ramp.size = 256;

    glfwSetGammaRamp(handle, &ramp);
}

GLFWAPI const GLFWgammaramp* glfwGetGammaRamp(GLFWmonitor* handle)
{
    _GLFWmonitor* monitor = (_GLFWmonitor*) handle;
    assert(monitor != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    _glfwFreeGammaArrays(&monitor->currentRamp);
    _glfwPlatformGetGammaRamp(monitor, &monitor->currentRamp);

    return &monitor->currentRamp;
}

GLFWAPI void glfwSetGammaRamp(GLFWmonitor* handle, const GLFWgammaramp* ramp)
{
    _GLFWmonitor* monitor = (_GLFWmonitor*) handle;
    assert(monitor != NULL);
    assert(ramp != NULL);
    assert(ramp->red != NULL);
    assert(ramp->green != NULL);
    assert(ramp->blue != NULL);

    if (ramp->size <= 0)
    {
        _glfwInputError(GLFW_INVALID_VALUE,
                        "Invalid gamma ramp size %i",
                        ramp->size);
        return;
    }

    _GLFW_REQUIRE_INIT();

    if (!monitor->originalRamp.size)
        _glfwPlatformGetGammaRamp(monitor, &monitor->originalRamp);

    _glfwPlatformSetGammaRamp(monitor, ramp);
}

//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define _GLFW_FIND_LOADER    1
#define _GLFW_REQUIRE_LOADER 2


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

GLFWbool _glfwInitVulkan(int mode)
{
    VkResult err;
    VkExtensionProperties* ep;
    uint32_t i, count;

    if (_glfw.vk.available)
        return GLFW_TRUE;

#if !defined(_GLFW_VULKAN_STATIC)
#if defined(_GLFW_WIN32)
    _glfw.vk.handle = _glfw_dlopen("vulkan-1.dll");
#elif defined(_GLFW_COCOA)
    // NULL maps to RTLD_DEFAULT, which searches all loaded binaries
    _glfw.vk.handle = _glfw_dlopen(NULL);
#else
    _glfw.vk.handle = _glfw_dlopen("libvulkan.so.1");
#endif
    if (!_glfw.vk.handle)
    {
        if (mode == _GLFW_REQUIRE_LOADER)
            _glfwInputError(GLFW_API_UNAVAILABLE, "Vulkan: Loader not found");

        return GLFW_FALSE;
    }

    _glfw.vk.GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
        _glfw_dlsym(_glfw.vk.handle, "vkGetInstanceProcAddr");
    if (!_glfw.vk.GetInstanceProcAddr)
    {
#if defined(_GLFW_COCOA)
        if (mode == _GLFW_REQUIRE_LOADER)
        {
            _glfwInputError(GLFW_API_UNAVAILABLE,
                            "Vulkan: vkGetInstanceProcAddr not found in process");
        }
#else
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "Vulkan: Loader does not export vkGetInstanceProcAddr");
#endif

        _glfwTerminateVulkan();
        return GLFW_FALSE;
    }

    _glfw.vk.EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)
        vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
    if (!_glfw.vk.EnumerateInstanceExtensionProperties)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "Vulkan: Failed to retrieve vkEnumerateInstanceExtensionProperties");

        _glfwTerminateVulkan();
        return GLFW_FALSE;
    }
#endif // _GLFW_VULKAN_STATIC

    err = vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
    if (err)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Vulkan: Failed to query instance extension count: %s",
                        _glfwGetVulkanResultString(err));

        _glfwTerminateVulkan();
        return GLFW_FALSE;
    }

    ep = calloc(count, sizeof(VkExtensionProperties));

    err = vkEnumerateInstanceExtensionProperties(NULL, &count, ep);
    if (err)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Vulkan: Failed to query instance extensions: %s",
                        _glfwGetVulkanResultString(err));

        free(ep);
        _glfwTerminateVulkan();
        return GLFW_FALSE;
    }

    for (i = 0;  i < count;  i++)
    {
        if (strcmp(ep[i].extensionName, "VK_KHR_surface") == 0)
            _glfw.vk.KHR_surface = GLFW_TRUE;
#if defined(_GLFW_WIN32)
        else if (strcmp(ep[i].extensionName, "VK_KHR_win32_surface") == 0)
            _glfw.vk.KHR_win32_surface = GLFW_TRUE;
#elif defined(_GLFW_COCOA)
        else if (strcmp(ep[i].extensionName, "VK_MVK_macos_surface") == 0)
            _glfw.vk.MVK_macos_surface = GLFW_TRUE;
#elif defined(_GLFW_X11)
        else if (strcmp(ep[i].extensionName, "VK_KHR_xlib_surface") == 0)
            _glfw.vk.KHR_xlib_surface = GLFW_TRUE;
        else if (strcmp(ep[i].extensionName, "VK_KHR_xcb_surface") == 0)
            _glfw.vk.KHR_xcb_surface = GLFW_TRUE;
#elif defined(_GLFW_WAYLAND)
        else if (strcmp(ep[i].extensionName, "VK_KHR_wayland_surface") == 0)
            _glfw.vk.KHR_wayland_surface = GLFW_TRUE;
#elif defined(_GLFW_MIR)
        else if (strcmp(ep[i].extensionName, "VK_KHR_mir_surface") == 0)
            _glfw.vk.KHR_mir_surface = GLFW_TRUE;
#endif
    }

    free(ep);

    _glfw.vk.available = GLFW_TRUE;

    _glfwPlatformGetRequiredInstanceExtensions(_glfw.vk.extensions);

    return GLFW_TRUE;
}

void _glfwTerminateVulkan(void)
{
#if !defined(_GLFW_VULKAN_STATIC)
    if (_glfw.vk.handle)
        _glfw_dlclose(_glfw.vk.handle);
#endif
}

const char* _glfwGetVulkanResultString(VkResult result)
{
    switch (result)
    {
        case VK_SUCCESS:
            return "Success";
        case VK_NOT_READY:
            return "A fence or query has not yet completed";
        case VK_TIMEOUT:
            return "A wait operation has not completed in the specified time";
        case VK_EVENT_SET:
            return "An event is signaled";
        case VK_EVENT_RESET:
            return "An event is unsignaled";
        case VK_INCOMPLETE:
            return "A return array was too small for the result";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "A host memory allocation has failed";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "A device memory allocation has failed";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "Initialization of an object could not be completed for implementation-specific reasons";
        case VK_ERROR_DEVICE_LOST:
            return "The logical or physical device has been lost";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "Mapping of a memory object has failed";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "A requested layer is not present or could not be loaded";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "A requested extension is not supported";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "A requested feature is not supported";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "Too many objects of the type have already been created";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "A requested format is not supported on this device";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "A surface is no longer available";
        case VK_SUBOPTIMAL_KHR:
            return "A swapchain no longer matches the surface properties exactly, but can still be used";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "A surface has changed in such a way that it is no longer compatible with the swapchain";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "The display used by a swapchain does not use the same presentable image layout";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "A validation layer found an error";
        default:
            return "ERROR: UNKNOWN VULKAN ERROR";
    }
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI int glfwVulkanSupported(void)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(GLFW_FALSE);
    return _glfwInitVulkan(_GLFW_FIND_LOADER);
}

GLFWAPI const char** glfwGetRequiredInstanceExtensions(uint32_t* count)
{
    *count = 0;

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (!_glfwInitVulkan(_GLFW_REQUIRE_LOADER))
        return NULL;

    if (!_glfw.vk.extensions[0])
        return NULL;

    *count = 2;
    return (const char**) _glfw.vk.extensions;
}

GLFWAPI GLFWvkproc glfwGetInstanceProcAddress(VkInstance instance,
                                              const char* procname)
{
    GLFWvkproc proc;

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (!_glfwInitVulkan(_GLFW_REQUIRE_LOADER))
        return NULL;

    proc = (GLFWvkproc) vkGetInstanceProcAddr(instance, procname);
#if defined(_GLFW_VULKAN_STATIC)
    if (!proc)
    {
        if (strcmp(procname, "vkGetInstanceProcAddr") == 0)
            return (GLFWvkproc) vkGetInstanceProcAddr;
    }
#else
    if (!proc)
        proc = (GLFWvkproc) _glfw_dlsym(_glfw.vk.handle, procname);
#endif

    return proc;
}

GLFWAPI int glfwGetPhysicalDevicePresentationSupport(VkInstance instance,
                                                     VkPhysicalDevice device,
                                                     uint32_t queuefamily)
{
    _GLFW_REQUIRE_INIT_OR_RETURN(GLFW_FALSE);

    if (!_glfwInitVulkan(_GLFW_REQUIRE_LOADER))
        return GLFW_FALSE;

    if (!_glfw.vk.extensions[0])
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "Vulkan: Window surface creation extensions not found");
        return GLFW_FALSE;
    }

    return _glfwPlatformGetPhysicalDevicePresentationSupport(instance,
                                                             device,
                                                             queuefamily);
}

GLFWAPI VkResult glfwCreateWindowSurface(VkInstance instance,
                                         GLFWwindow* handle,
                                         const VkAllocationCallbacks* allocator,
                                         VkSurfaceKHR* surface)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);
    assert(surface != NULL);

    *surface = VK_NULL_HANDLE;

    _GLFW_REQUIRE_INIT_OR_RETURN(VK_ERROR_INITIALIZATION_FAILED);

    if (!_glfwInitVulkan(_GLFW_REQUIRE_LOADER))
        return VK_ERROR_INITIALIZATION_FAILED;

    if (!_glfw.vk.extensions[0])
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "Vulkan: Window surface creation extensions not found");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    return _glfwPlatformCreateWindowSurface(instance, window, allocator, surface);
}

//========================================================================
// GLFW 3.3 WGL - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>


// Returns the specified attribute of the specified pixel format
//
static int getPixelFormatAttrib(_GLFWwindow* window, int pixelFormat, int attrib)
{
    int value = 0;

    assert(_glfw.wgl.ARB_pixel_format);

    if (!_glfw.wgl.GetPixelFormatAttribivARB(window->context.wgl.dc,
                                             pixelFormat,
                                             0, 1, &attrib, &value))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "WGL: Failed to retrieve pixel format attribute %i",
                        attrib);
        return 0;
    }

    return value;
}

// Return a list of available and usable framebuffer configs
//
static int choosePixelFormat(_GLFWwindow* window, const _GLFWfbconfig* desired)
{
    _GLFWfbconfig* usableConfigs;
    const _GLFWfbconfig* closest;
    int i, pixelFormat, nativeCount, usableCount;

    if (_glfw.wgl.ARB_pixel_format)
    {
        nativeCount = getPixelFormatAttrib(window,
                                           1,
                                           WGL_NUMBER_PIXEL_FORMATS_ARB);
    }
    else
    {
        nativeCount = DescribePixelFormat(window->context.wgl.dc,
                                          1,
                                          sizeof(PIXELFORMATDESCRIPTOR),
                                          NULL);
    }

    usableConfigs = calloc(nativeCount, sizeof(_GLFWfbconfig));
    usableCount = 0;

    for (i = 0;  i < nativeCount;  i++)
    {
        const int n = i + 1;
        _GLFWfbconfig* u = usableConfigs + usableCount;

        if (_glfw.wgl.ARB_pixel_format)
        {
            // Get pixel format attributes through "modern" extension

            if (!getPixelFormatAttrib(window, n, WGL_SUPPORT_OPENGL_ARB) ||
                !getPixelFormatAttrib(window, n, WGL_DRAW_TO_WINDOW_ARB))
            {
                continue;
            }

            if (getPixelFormatAttrib(window, n, WGL_PIXEL_TYPE_ARB) !=
                WGL_TYPE_RGBA_ARB)
            {
                continue;
            }

            if (getPixelFormatAttrib(window, n, WGL_ACCELERATION_ARB) ==
                 WGL_NO_ACCELERATION_ARB)
            {
                continue;
            }

            u->redBits = getPixelFormatAttrib(window, n, WGL_RED_BITS_ARB);
            u->greenBits = getPixelFormatAttrib(window, n, WGL_GREEN_BITS_ARB);
            u->blueBits = getPixelFormatAttrib(window, n, WGL_BLUE_BITS_ARB);
            u->alphaBits = getPixelFormatAttrib(window, n, WGL_ALPHA_BITS_ARB);

            u->depthBits = getPixelFormatAttrib(window, n, WGL_DEPTH_BITS_ARB);
            u->stencilBits = getPixelFormatAttrib(window, n, WGL_STENCIL_BITS_ARB);

            u->accumRedBits = getPixelFormatAttrib(window, n, WGL_ACCUM_RED_BITS_ARB);
            u->accumGreenBits = getPixelFormatAttrib(window, n, WGL_ACCUM_GREEN_BITS_ARB);
            u->accumBlueBits = getPixelFormatAttrib(window, n, WGL_ACCUM_BLUE_BITS_ARB);
            u->accumAlphaBits = getPixelFormatAttrib(window, n, WGL_ACCUM_ALPHA_BITS_ARB);

            u->auxBuffers = getPixelFormatAttrib(window, n, WGL_AUX_BUFFERS_ARB);

            if (getPixelFormatAttrib(window, n, WGL_STEREO_ARB))
                u->stereo = GLFW_TRUE;
            if (getPixelFormatAttrib(window, n, WGL_DOUBLE_BUFFER_ARB))
                u->doublebuffer = GLFW_TRUE;

            if (_glfw.wgl.ARB_multisample)
                u->samples = getPixelFormatAttrib(window, n, WGL_SAMPLES_ARB);

            if (_glfw.wgl.ARB_framebuffer_sRGB ||
                _glfw.wgl.EXT_framebuffer_sRGB)
            {
                if (getPixelFormatAttrib(window, n, WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB))
                    u->sRGB = GLFW_TRUE;
            }
        }
        else
        {
            PIXELFORMATDESCRIPTOR pfd;

            // Get pixel format attributes through legacy PFDs

            if (!DescribePixelFormat(window->context.wgl.dc,
                                     n,
                                     sizeof(PIXELFORMATDESCRIPTOR),
                                     &pfd))
            {
                continue;
            }

            if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW) ||
                !(pfd.dwFlags & PFD_SUPPORT_OPENGL))
            {
                continue;
            }

            if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED) &&
                (pfd.dwFlags & PFD_GENERIC_FORMAT))
            {
                continue;
            }

            if (pfd.iPixelType != PFD_TYPE_RGBA)
                continue;

            u->redBits = pfd.cRedBits;
            u->greenBits = pfd.cGreenBits;
            u->blueBits = pfd.cBlueBits;
            u->alphaBits = pfd.cAlphaBits;

            u->depthBits = pfd.cDepthBits;
            u->stencilBits = pfd.cStencilBits;

            u->accumRedBits = pfd.cAccumRedBits;
            u->accumGreenBits = pfd.cAccumGreenBits;
            u->accumBlueBits = pfd.cAccumBlueBits;
            u->accumAlphaBits = pfd.cAccumAlphaBits;

            u->auxBuffers = pfd.cAuxBuffers;

            if (pfd.dwFlags & PFD_STEREO)
                u->stereo = GLFW_TRUE;
            if (pfd.dwFlags & PFD_DOUBLEBUFFER)
                u->doublebuffer = GLFW_TRUE;
        }

        u->handle = n;
        usableCount++;
    }

    if (!usableCount)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "WGL: The driver does not appear to support OpenGL");

        free(usableConfigs);
        return 0;
    }

    closest = _glfwChooseFBConfig(desired, usableConfigs, usableCount);
    if (!closest)
    {
        _glfwInputError(GLFW_FORMAT_UNAVAILABLE,
                        "WGL: Failed to find a suitable pixel format");

        free(usableConfigs);
        return 0;
    }

    pixelFormat = (int) closest->handle;
    free(usableConfigs);

    return pixelFormat;
}

// Returns whether desktop compositing is enabled
//
static GLFWbool isCompositionEnabled(void)
{
    BOOL enabled;

    if (!_glfw_DwmIsCompositionEnabled)
        return FALSE;

    if (_glfw_DwmIsCompositionEnabled(&enabled) != S_OK)
        return FALSE;

    return enabled;
}

static void makeContextCurrentWGL(_GLFWwindow* window)
{
    if (window)
    {
        if (wglMakeCurrent(window->context.wgl.dc, window->context.wgl.handle))
            _glfwPlatformSetCurrentContext(window);
        else
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "WGL: Failed to make context current");
            _glfwPlatformSetCurrentContext(NULL);
        }
    }
    else
    {
        if (!wglMakeCurrent(NULL, NULL))
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "WGL: Failed to clear current context");
        }

        _glfwPlatformSetCurrentContext(NULL);
    }
}

static void swapBuffersWGL(_GLFWwindow* window)
{
    // HACK: Use DwmFlush when desktop composition is enabled
    if (isCompositionEnabled() && !window->monitor)
    {
        int count = abs(window->context.wgl.interval);
        while (count--)
            _glfw_DwmFlush();
    }

    SwapBuffers(window->context.wgl.dc);
}

static void swapIntervalWGL(int interval)
{
    _GLFWwindow* window = _glfwPlatformGetCurrentContext();

    window->context.wgl.interval = interval;

    // HACK: Disable WGL swap interval when desktop composition is enabled to
    //       avoid interfering with DWM vsync
    if (isCompositionEnabled() && !window->monitor)
        interval = 0;

    if (_glfw.wgl.EXT_swap_control)
        _glfw.wgl.SwapIntervalEXT(interval);
}

static int extensionSupportedWGL(const char* extension)
{
    const char* extensions;

    if (_glfw.wgl.GetExtensionsStringEXT)
    {
        extensions = _glfw.wgl.GetExtensionsStringEXT();
        if (extensions)
        {
            if (_glfwStringInExtensionString(extension, extensions))
                return GLFW_TRUE;
        }
    }

    if (_glfw.wgl.GetExtensionsStringARB)
    {
        extensions = _glfw.wgl.GetExtensionsStringARB(wglGetCurrentDC());
        if (extensions)
        {
            if (_glfwStringInExtensionString(extension, extensions))
                return GLFW_TRUE;
        }
    }

    return GLFW_FALSE;
}

static GLFWglproc getProcAddressWGL(const char* procname)
{
    const GLFWglproc proc = (GLFWglproc) wglGetProcAddress(procname);
    if (proc)
        return proc;

    return (GLFWglproc) GetProcAddress(_glfw.wgl.instance, procname);
}

// Destroy the OpenGL context
//
static void destroyContextWGL(_GLFWwindow* window)
{
    if (window->context.wgl.handle)
    {
        wglDeleteContext(window->context.wgl.handle);
        window->context.wgl.handle = NULL;
    }
}

// Initialize WGL-specific extensions
//
static void loadWGLExtensions(void)
{
    PIXELFORMATDESCRIPTOR pfd;
    HGLRC rc;
    HDC dc = GetDC(_glfw.win32.helperWindowHandle);;

    _glfw.wgl.extensionsLoaded = GLFW_TRUE;

    // NOTE: A dummy context has to be created for opengl32.dll to load the
    //       OpenGL ICD, from which we can then query WGL extensions
    // NOTE: This code will accept the Microsoft GDI ICD; accelerated context
    //       creation failure occurs during manual pixel format enumeration

    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;

    if (!SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "WGL: Failed to set pixel format for dummy context");
        return;
    }

    rc = wglCreateContext(dc);
    if (!rc)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "WGL: Failed to create dummy context");
        return;
    }

    if (!wglMakeCurrent(dc, rc))
    {
        wglDeleteContext(rc);

        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "WGL: Failed to make dummy context current");
        return;
    }

    // NOTE: Functions must be loaded first as they're needed to retrieve the
    //       extension string that tells us whether the functions are supported
    _glfw.wgl.GetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)
        wglGetProcAddress("wglGetExtensionsStringEXT");
    _glfw.wgl.GetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
        wglGetProcAddress("wglGetExtensionsStringARB");
    _glfw.wgl.CreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
        wglGetProcAddress("wglCreateContextAttribsARB");
    _glfw.wgl.SwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)
        wglGetProcAddress("wglSwapIntervalEXT");
    _glfw.wgl.GetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
        wglGetProcAddress("wglGetPixelFormatAttribivARB");

    // NOTE: WGL_ARB_extensions_string and WGL_EXT_extensions_string are not
    //       checked below as we are already using them
    _glfw.wgl.ARB_multisample =
        extensionSupportedWGL("WGL_ARB_multisample");
    _glfw.wgl.ARB_framebuffer_sRGB =
        extensionSupportedWGL("WGL_ARB_framebuffer_sRGB");
    _glfw.wgl.EXT_framebuffer_sRGB =
        extensionSupportedWGL("WGL_EXT_framebuffer_sRGB");
    _glfw.wgl.ARB_create_context =
        extensionSupportedWGL("WGL_ARB_create_context");
    _glfw.wgl.ARB_create_context_profile =
        extensionSupportedWGL("WGL_ARB_create_context_profile");
    _glfw.wgl.EXT_create_context_es2_profile =
        extensionSupportedWGL("WGL_EXT_create_context_es2_profile");
    _glfw.wgl.ARB_create_context_robustness =
        extensionSupportedWGL("WGL_ARB_create_context_robustness");
    _glfw.wgl.EXT_swap_control =
        extensionSupportedWGL("WGL_EXT_swap_control");
    _glfw.wgl.ARB_pixel_format =
        extensionSupportedWGL("WGL_ARB_pixel_format");
    _glfw.wgl.ARB_context_flush_control =
        extensionSupportedWGL("WGL_ARB_context_flush_control");

    wglMakeCurrent(dc, NULL);
    wglDeleteContext(rc);
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

// Initialize WGL
//
GLFWbool _glfwInitWGL(void)
{
    if (_glfw.wgl.instance)
        return GLFW_TRUE;

    _glfw.wgl.instance = LoadLibraryA("opengl32.dll");
    if (!_glfw.wgl.instance)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "WGL: Failed to load opengl32.dll");
        return GLFW_FALSE;
    }

    _glfw.wgl.CreateContext = (PFN_wglCreateContext)
        GetProcAddress(_glfw.wgl.instance, "wglCreateContext");
    _glfw.wgl.DeleteContext = (PFN_wglDeleteContext)
        GetProcAddress(_glfw.wgl.instance, "wglDeleteContext");
    _glfw.wgl.GetProcAddress = (PFN_wglGetProcAddress)
        GetProcAddress(_glfw.wgl.instance, "wglGetProcAddress");
    _glfw.wgl.GetCurrentDC = (PFN_wglGetCurrentDC)
        GetProcAddress(_glfw.wgl.instance, "wglGetCurrentDC");
    _glfw.wgl.MakeCurrent = (PFN_wglMakeCurrent)
        GetProcAddress(_glfw.wgl.instance, "wglMakeCurrent");
    _glfw.wgl.ShareLists = (PFN_wglShareLists)
        GetProcAddress(_glfw.wgl.instance, "wglShareLists");

    return GLFW_TRUE;
}

// Terminate WGL
//
void _glfwTerminateWGL(void)
{
    if (_glfw.wgl.instance)
        FreeLibrary(_glfw.wgl.instance);
}

#define setWGLattrib(attribName, attribValue) \
{ \
    attribs[index++] = attribName; \
    attribs[index++] = attribValue; \
    assert((size_t) index < sizeof(attribs) / sizeof(attribs[0])); \
}

// Create the OpenGL or OpenGL ES context
//
GLFWbool _glfwCreateContextWGL(_GLFWwindow* window,
                               const _GLFWctxconfig* ctxconfig,
                               const _GLFWfbconfig* fbconfig)
{
    int attribs[40];
    int pixelFormat;
    PIXELFORMATDESCRIPTOR pfd;
    HGLRC share = NULL;

    if (!_glfw.wgl.extensionsLoaded)
        loadWGLExtensions();

    if (ctxconfig->share)
        share = ctxconfig->share->context.wgl.handle;

    window->context.wgl.dc = GetDC(window->win32.handle);
    if (!window->context.wgl.dc)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "WGL: Failed to retrieve DC for window");
        return GLFW_FALSE;
    }

    pixelFormat = choosePixelFormat(window, fbconfig);
    if (!pixelFormat)
        return GLFW_FALSE;

    if (!DescribePixelFormat(window->context.wgl.dc,
                             pixelFormat, sizeof(pfd), &pfd))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "WGL: Failed to retrieve PFD for selected pixel format");
        return GLFW_FALSE;
    }

    if (!SetPixelFormat(window->context.wgl.dc, pixelFormat, &pfd))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "WGL: Failed to set selected pixel format");
        return GLFW_FALSE;
    }

    if (ctxconfig->client == GLFW_OPENGL_API)
    {
        if (ctxconfig->forward)
        {
            if (!_glfw.wgl.ARB_create_context)
            {
                _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                                "WGL: A forward compatible OpenGL context requested but WGL_ARB_create_context is unavailable");
                return GLFW_FALSE;
            }
        }

        if (ctxconfig->profile)
        {
            if (!_glfw.wgl.ARB_create_context_profile)
            {
                _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                                "WGL: OpenGL profile requested but WGL_ARB_create_context_profile is unavailable");
                return GLFW_FALSE;
            }
        }
    }
    else
    {
        if (!_glfw.wgl.ARB_create_context ||
            !_glfw.wgl.ARB_create_context_profile ||
            !_glfw.wgl.EXT_create_context_es2_profile)
        {
            _glfwInputError(GLFW_API_UNAVAILABLE,
                            "WGL: OpenGL ES requested but WGL_ARB_create_context_es2_profile is unavailable");
            return GLFW_FALSE;
        }
    }

    if (_glfw.wgl.ARB_create_context)
    {
        int index = 0, mask = 0, flags = 0;

        if (ctxconfig->client == GLFW_OPENGL_API)
        {
            if (ctxconfig->forward)
                flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

            if (ctxconfig->profile == GLFW_OPENGL_CORE_PROFILE)
                mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            else if (ctxconfig->profile == GLFW_OPENGL_COMPAT_PROFILE)
                mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
        }
        else
            mask |= WGL_CONTEXT_ES2_PROFILE_BIT_EXT;

        if (ctxconfig->debug)
            flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
        if (ctxconfig->noerror)
            flags |= GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR;

        if (ctxconfig->robustness)
        {
            if (_glfw.wgl.ARB_create_context_robustness)
            {
                if (ctxconfig->robustness == GLFW_NO_RESET_NOTIFICATION)
                {
                    setWGLattrib(WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
                                 WGL_NO_RESET_NOTIFICATION_ARB);
                }
                else if (ctxconfig->robustness == GLFW_LOSE_CONTEXT_ON_RESET)
                {
                    setWGLattrib(WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
                                 WGL_LOSE_CONTEXT_ON_RESET_ARB);
                }

                flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
            }
        }

        if (ctxconfig->release)
        {
            if (_glfw.wgl.ARB_context_flush_control)
            {
                if (ctxconfig->release == GLFW_RELEASE_BEHAVIOR_NONE)
                {
                    setWGLattrib(WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
                                 WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB);
                }
                else if (ctxconfig->release == GLFW_RELEASE_BEHAVIOR_FLUSH)
                {
                    setWGLattrib(WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
                                 WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB);
                }
            }
        }

        // NOTE: Only request an explicitly versioned context when necessary, as
        //       explicitly requesting version 1.0 does not always return the
        //       highest version supported by the driver
        if (ctxconfig->major != 1 || ctxconfig->minor != 0)
        {
            setWGLattrib(WGL_CONTEXT_MAJOR_VERSION_ARB, ctxconfig->major);
            setWGLattrib(WGL_CONTEXT_MINOR_VERSION_ARB, ctxconfig->minor);
        }

        if (flags)
            setWGLattrib(WGL_CONTEXT_FLAGS_ARB, flags);

        if (mask)
            setWGLattrib(WGL_CONTEXT_PROFILE_MASK_ARB, mask);

        setWGLattrib(0, 0);

        window->context.wgl.handle =
            _glfw.wgl.CreateContextAttribsARB(window->context.wgl.dc,
                                              share, attribs);
        if (!window->context.wgl.handle)
        {
            const DWORD error = GetLastError();

            if (error == (0xc0070000 | ERROR_INVALID_VERSION_ARB))
            {
                if (ctxconfig->client == GLFW_OPENGL_API)
                {
                    _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                                    "WGL: Driver does not support OpenGL version %i.%i",
                                    ctxconfig->major,
                                    ctxconfig->minor);
                }
                else
                {
                    _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                                    "WGL: Driver does not support OpenGL ES version %i.%i",
                                    ctxconfig->major,
                                    ctxconfig->minor);
                }
            }
            else if (error == (0xc0070000 | ERROR_INVALID_PROFILE_ARB))
            {
                _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                                "WGL: Driver does not support the requested OpenGL profile");
            }
            else
            {
                if (ctxconfig->client == GLFW_OPENGL_API)
                {
                    _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                                    "WGL: Failed to create OpenGL context");
                }
                else
                {
                    _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                                    "WGL: Failed to create OpenGL ES context");
                }
            }

            return GLFW_FALSE;
        }
    }
    else
    {
        window->context.wgl.handle = wglCreateContext(window->context.wgl.dc);
        if (!window->context.wgl.handle)
        {
            _glfwInputError(GLFW_VERSION_UNAVAILABLE,
                            "WGL: Failed to create OpenGL context");
            return GLFW_FALSE;
        }

        if (share)
        {
            if (!wglShareLists(share, window->context.wgl.handle))
            {
                _glfwInputError(GLFW_PLATFORM_ERROR,
                                "WGL: Failed to enable sharing with specified OpenGL context");
                return GLFW_FALSE;
            }
        }
    }

    window->context.makeCurrent = makeContextCurrentWGL;
    window->context.swapBuffers = swapBuffersWGL;
    window->context.swapInterval = swapIntervalWGL;
    window->context.extensionSupported = extensionSupportedWGL;
    window->context.getProcAddress = getProcAddressWGL;
    window->context.destroy = destroyContextWGL;

    return GLFW_TRUE;
}

#undef setWGLattrib


//////////////////////////////////////////////////////////////////////////
//////                        GLFW native API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI HGLRC glfwGetWGLContext(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (window->context.client == GLFW_NO_API)
    {
        _glfwInputError(GLFW_NO_WINDOW_CONTEXT, NULL);
        return NULL;
    }

    return window->context.wgl.handle;
}

//========================================================================
// GLFW 3.3 Win32 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <stdlib.h>
#include <malloc.h>

#include <initguid.h>
DEFINE_GUID(GUID_DEVINTERFACE_HID,0x4d1e55b2,0xf16f,0x11cf,0x88,0xcb,0x00,0x11,0x11,0x00,0x00,0x30);

#if defined(_GLFW_USE_HYBRID_HPG) || defined(_GLFW_USE_OPTIMUS_HPG)

// Executables (but not DLLs) exporting this symbol with this value will be
// automatically directed to the high-performance GPU on Nvidia Optimus systems
// with up-to-date drivers
//
__declspec(dllexport) DWORD NvOptimusEnablement = 1;

// Executables (but not DLLs) exporting this symbol with this value will be
// automatically directed to the high-performance GPU on AMD PowerXpress systems
// with up-to-date drivers
//
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // _GLFW_USE_HYBRID_HPG

#if defined(_GLFW_BUILD_DLL)

// GLFW DLL entry point
//
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    return TRUE;
}

#endif // _GLFW_BUILD_DLL

// Load necessary libraries (DLLs)
//
static GLFWbool loadLibraries(void)
{
    _glfw.win32.winmm.instance = LoadLibraryA("winmm.dll");
    if (!_glfw.win32.winmm.instance)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to load winmm.dll");
        return GLFW_FALSE;
    }

    _glfw.win32.winmm.timeGetTime = (PFN_timeGetTime)
        GetProcAddress(_glfw.win32.winmm.instance, "timeGetTime");

    _glfw.win32.user32.instance = LoadLibraryA("user32.dll");
    if (!_glfw.win32.user32.instance)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to load user32.dll");
        return GLFW_FALSE;
    }

    _glfw.win32.user32.SetProcessDPIAware = (PFN_SetProcessDPIAware)
        GetProcAddress(_glfw.win32.user32.instance, "SetProcessDPIAware");
    _glfw.win32.user32.ChangeWindowMessageFilterEx = (PFN_ChangeWindowMessageFilterEx)
        GetProcAddress(_glfw.win32.user32.instance, "ChangeWindowMessageFilterEx");

    _glfw.win32.dinput8.instance = LoadLibraryA("dinput8.dll");
    if (_glfw.win32.dinput8.instance)
    {
        _glfw.win32.dinput8.DirectInput8Create = (PFN_DirectInput8Create)
            GetProcAddress(_glfw.win32.dinput8.instance, "DirectInput8Create");
    }

    {
        int i;
        const char* names[] =
        {
            "xinput1_4.dll",
            "xinput1_3.dll",
            "xinput9_1_0.dll",
            "xinput1_2.dll",
            "xinput1_1.dll",
            NULL
        };

        for (i = 0;  names[i];  i++)
        {
            _glfw.win32.xinput.instance = LoadLibraryA(names[i]);
            if (_glfw.win32.xinput.instance)
            {
                _glfw.win32.xinput.XInputGetCapabilities = (PFN_XInputGetCapabilities)
                    GetProcAddress(_glfw.win32.xinput.instance, "XInputGetCapabilities");
                _glfw.win32.xinput.XInputGetState = (PFN_XInputGetState)
                    GetProcAddress(_glfw.win32.xinput.instance, "XInputGetState");

                break;
            }
        }
    }

    _glfw.win32.dwmapi.instance = LoadLibraryA("dwmapi.dll");
    if (_glfw.win32.dwmapi.instance)
    {
        _glfw.win32.dwmapi.DwmIsCompositionEnabled = (PFN_DwmIsCompositionEnabled)
            GetProcAddress(_glfw.win32.dwmapi.instance, "DwmIsCompositionEnabled");
        _glfw.win32.dwmapi.DwmFlush = (PFN_DwmFlush)
            GetProcAddress(_glfw.win32.dwmapi.instance, "DwmFlush");
    }

    _glfw.win32.shcore.instance = LoadLibraryA("shcore.dll");
    if (_glfw.win32.shcore.instance)
    {
        _glfw.win32.shcore.SetProcessDpiAwareness = (PFN_SetProcessDpiAwareness)
            GetProcAddress(_glfw.win32.shcore.instance, "SetProcessDpiAwareness");
    }

    return GLFW_TRUE;
}

// Unload used libraries (DLLs)
//
static void freeLibraries(void)
{
    if (_glfw.win32.xinput.instance)
        FreeLibrary(_glfw.win32.xinput.instance);

    if (_glfw.win32.dinput8.instance)
        FreeLibrary(_glfw.win32.dinput8.instance);

    if (_glfw.win32.winmm.instance)
        FreeLibrary(_glfw.win32.winmm.instance);

    if (_glfw.win32.user32.instance)
        FreeLibrary(_glfw.win32.user32.instance);

    if (_glfw.win32.dwmapi.instance)
        FreeLibrary(_glfw.win32.dwmapi.instance);

    if (_glfw.win32.shcore.instance)
        FreeLibrary(_glfw.win32.shcore.instance);
}

// Create key code translation tables
//
static void createKeyTables(void)
{
    int scancode;

    memset(_glfw.win32.keycodes, -1, sizeof(_glfw.win32.keycodes));
    memset(_glfw.win32.scancodes, -1, sizeof(_glfw.win32.scancodes));

    _glfw.win32.keycodes[0x00B] = GLFW_KEY_0;
    _glfw.win32.keycodes[0x002] = GLFW_KEY_1;
    _glfw.win32.keycodes[0x003] = GLFW_KEY_2;
    _glfw.win32.keycodes[0x004] = GLFW_KEY_3;
    _glfw.win32.keycodes[0x005] = GLFW_KEY_4;
    _glfw.win32.keycodes[0x006] = GLFW_KEY_5;
    _glfw.win32.keycodes[0x007] = GLFW_KEY_6;
    _glfw.win32.keycodes[0x008] = GLFW_KEY_7;
    _glfw.win32.keycodes[0x009] = GLFW_KEY_8;
    _glfw.win32.keycodes[0x00A] = GLFW_KEY_9;
    _glfw.win32.keycodes[0x01E] = GLFW_KEY_A;
    _glfw.win32.keycodes[0x030] = GLFW_KEY_B;
    _glfw.win32.keycodes[0x02E] = GLFW_KEY_C;
    _glfw.win32.keycodes[0x020] = GLFW_KEY_D;
    _glfw.win32.keycodes[0x012] = GLFW_KEY_E;
    _glfw.win32.keycodes[0x021] = GLFW_KEY_F;
    _glfw.win32.keycodes[0x022] = GLFW_KEY_G;
    _glfw.win32.keycodes[0x023] = GLFW_KEY_H;
    _glfw.win32.keycodes[0x017] = GLFW_KEY_I;
    _glfw.win32.keycodes[0x024] = GLFW_KEY_J;
    _glfw.win32.keycodes[0x025] = GLFW_KEY_K;
    _glfw.win32.keycodes[0x026] = GLFW_KEY_L;
    _glfw.win32.keycodes[0x032] = GLFW_KEY_M;
    _glfw.win32.keycodes[0x031] = GLFW_KEY_N;
    _glfw.win32.keycodes[0x018] = GLFW_KEY_O;
    _glfw.win32.keycodes[0x019] = GLFW_KEY_P;
    _glfw.win32.keycodes[0x010] = GLFW_KEY_Q;
    _glfw.win32.keycodes[0x013] = GLFW_KEY_R;
    _glfw.win32.keycodes[0x01F] = GLFW_KEY_S;
    _glfw.win32.keycodes[0x014] = GLFW_KEY_T;
    _glfw.win32.keycodes[0x016] = GLFW_KEY_U;
    _glfw.win32.keycodes[0x02F] = GLFW_KEY_V;
    _glfw.win32.keycodes[0x011] = GLFW_KEY_W;
    _glfw.win32.keycodes[0x02D] = GLFW_KEY_X;
    _glfw.win32.keycodes[0x015] = GLFW_KEY_Y;
    _glfw.win32.keycodes[0x02C] = GLFW_KEY_Z;

    _glfw.win32.keycodes[0x028] = GLFW_KEY_APOSTROPHE;
    _glfw.win32.keycodes[0x02B] = GLFW_KEY_BACKSLASH;
    _glfw.win32.keycodes[0x033] = GLFW_KEY_COMMA;
    _glfw.win32.keycodes[0x00D] = GLFW_KEY_EQUAL;
    _glfw.win32.keycodes[0x029] = GLFW_KEY_GRAVE_ACCENT;
    _glfw.win32.keycodes[0x01A] = GLFW_KEY_LEFT_BRACKET;
    _glfw.win32.keycodes[0x00C] = GLFW_KEY_MINUS;
    _glfw.win32.keycodes[0x034] = GLFW_KEY_PERIOD;
    _glfw.win32.keycodes[0x01B] = GLFW_KEY_RIGHT_BRACKET;
    _glfw.win32.keycodes[0x027] = GLFW_KEY_SEMICOLON;
    _glfw.win32.keycodes[0x035] = GLFW_KEY_SLASH;
    _glfw.win32.keycodes[0x056] = GLFW_KEY_WORLD_2;

    _glfw.win32.keycodes[0x00E] = GLFW_KEY_BACKSPACE;
    _glfw.win32.keycodes[0x153] = GLFW_KEY_DELETE;
    _glfw.win32.keycodes[0x14F] = GLFW_KEY_END;
    _glfw.win32.keycodes[0x01C] = GLFW_KEY_ENTER;
    _glfw.win32.keycodes[0x001] = GLFW_KEY_ESCAPE;
    _glfw.win32.keycodes[0x147] = GLFW_KEY_HOME;
    _glfw.win32.keycodes[0x152] = GLFW_KEY_INSERT;
    _glfw.win32.keycodes[0x15D] = GLFW_KEY_MENU;
    _glfw.win32.keycodes[0x151] = GLFW_KEY_PAGE_DOWN;
    _glfw.win32.keycodes[0x149] = GLFW_KEY_PAGE_UP;
    _glfw.win32.keycodes[0x045] = GLFW_KEY_PAUSE;
    _glfw.win32.keycodes[0x146] = GLFW_KEY_PAUSE;
    _glfw.win32.keycodes[0x039] = GLFW_KEY_SPACE;
    _glfw.win32.keycodes[0x00F] = GLFW_KEY_TAB;
    _glfw.win32.keycodes[0x03A] = GLFW_KEY_CAPS_LOCK;
    _glfw.win32.keycodes[0x145] = GLFW_KEY_NUM_LOCK;
    _glfw.win32.keycodes[0x046] = GLFW_KEY_SCROLL_LOCK;
    _glfw.win32.keycodes[0x03B] = GLFW_KEY_F1;
    _glfw.win32.keycodes[0x03C] = GLFW_KEY_F2;
    _glfw.win32.keycodes[0x03D] = GLFW_KEY_F3;
    _glfw.win32.keycodes[0x03E] = GLFW_KEY_F4;
    _glfw.win32.keycodes[0x03F] = GLFW_KEY_F5;
    _glfw.win32.keycodes[0x040] = GLFW_KEY_F6;
    _glfw.win32.keycodes[0x041] = GLFW_KEY_F7;
    _glfw.win32.keycodes[0x042] = GLFW_KEY_F8;
    _glfw.win32.keycodes[0x043] = GLFW_KEY_F9;
    _glfw.win32.keycodes[0x044] = GLFW_KEY_F10;
    _glfw.win32.keycodes[0x057] = GLFW_KEY_F11;
    _glfw.win32.keycodes[0x058] = GLFW_KEY_F12;
    _glfw.win32.keycodes[0x064] = GLFW_KEY_F13;
    _glfw.win32.keycodes[0x065] = GLFW_KEY_F14;
    _glfw.win32.keycodes[0x066] = GLFW_KEY_F15;
    _glfw.win32.keycodes[0x067] = GLFW_KEY_F16;
    _glfw.win32.keycodes[0x068] = GLFW_KEY_F17;
    _glfw.win32.keycodes[0x069] = GLFW_KEY_F18;
    _glfw.win32.keycodes[0x06A] = GLFW_KEY_F19;
    _glfw.win32.keycodes[0x06B] = GLFW_KEY_F20;
    _glfw.win32.keycodes[0x06C] = GLFW_KEY_F21;
    _glfw.win32.keycodes[0x06D] = GLFW_KEY_F22;
    _glfw.win32.keycodes[0x06E] = GLFW_KEY_F23;
    _glfw.win32.keycodes[0x076] = GLFW_KEY_F24;
    _glfw.win32.keycodes[0x038] = GLFW_KEY_LEFT_ALT;
    _glfw.win32.keycodes[0x01D] = GLFW_KEY_LEFT_CONTROL;
    _glfw.win32.keycodes[0x02A] = GLFW_KEY_LEFT_SHIFT;
    _glfw.win32.keycodes[0x15B] = GLFW_KEY_LEFT_SUPER;
    _glfw.win32.keycodes[0x137] = GLFW_KEY_PRINT_SCREEN;
    _glfw.win32.keycodes[0x138] = GLFW_KEY_RIGHT_ALT;
    _glfw.win32.keycodes[0x11D] = GLFW_KEY_RIGHT_CONTROL;
    _glfw.win32.keycodes[0x036] = GLFW_KEY_RIGHT_SHIFT;
    _glfw.win32.keycodes[0x15C] = GLFW_KEY_RIGHT_SUPER;
    _glfw.win32.keycodes[0x150] = GLFW_KEY_DOWN;
    _glfw.win32.keycodes[0x14B] = GLFW_KEY_LEFT;
    _glfw.win32.keycodes[0x14D] = GLFW_KEY_RIGHT;
    _glfw.win32.keycodes[0x148] = GLFW_KEY_UP;

    _glfw.win32.keycodes[0x052] = GLFW_KEY_KP_0;
    _glfw.win32.keycodes[0x04F] = GLFW_KEY_KP_1;
    _glfw.win32.keycodes[0x050] = GLFW_KEY_KP_2;
    _glfw.win32.keycodes[0x051] = GLFW_KEY_KP_3;
    _glfw.win32.keycodes[0x04B] = GLFW_KEY_KP_4;
    _glfw.win32.keycodes[0x04C] = GLFW_KEY_KP_5;
    _glfw.win32.keycodes[0x04D] = GLFW_KEY_KP_6;
    _glfw.win32.keycodes[0x047] = GLFW_KEY_KP_7;
    _glfw.win32.keycodes[0x048] = GLFW_KEY_KP_8;
    _glfw.win32.keycodes[0x049] = GLFW_KEY_KP_9;
    _glfw.win32.keycodes[0x04E] = GLFW_KEY_KP_ADD;
    _glfw.win32.keycodes[0x053] = GLFW_KEY_KP_DECIMAL;
    _glfw.win32.keycodes[0x135] = GLFW_KEY_KP_DIVIDE;
    _glfw.win32.keycodes[0x11C] = GLFW_KEY_KP_ENTER;
    _glfw.win32.keycodes[0x037] = GLFW_KEY_KP_MULTIPLY;
    _glfw.win32.keycodes[0x04A] = GLFW_KEY_KP_SUBTRACT;

    for (scancode = 0;  scancode < 512;  scancode++)
    {
        if (_glfw.win32.keycodes[scancode] > 0)
            _glfw.win32.scancodes[_glfw.win32.keycodes[scancode]] = scancode;
    }
}

// Creates a dummy window for behind-the-scenes work
//
static HWND createHelperWindow(void)
{
    MSG msg;
    HWND window = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
                                  _GLFW_WNDCLASSNAME,
                                  L"GLFW helper window",
                                  WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                  0, 0, 1, 1,
                                  HWND_MESSAGE, NULL,
                                  GetModuleHandleW(NULL),
                                  NULL);
    if (!window)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to create helper window");
        return NULL;
    }

    // HACK: The first call to ShowWindow is ignored if the parent process
    //       passed along a STARTUPINFO, so clear that flag with a no-op call
    ShowWindow(window, SW_HIDE);

    // Register for HID device notifications
    {
        DEV_BROADCAST_DEVICEINTERFACE_W dbi;
        ZeroMemory(&dbi, sizeof(dbi));
        dbi.dbcc_size = sizeof(dbi);
        dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        dbi.dbcc_classguid = GUID_DEVINTERFACE_HID;

        RegisterDeviceNotificationW(window,
                                    (DEV_BROADCAST_HDR*) &dbi,
                                    DEVICE_NOTIFY_WINDOW_HANDLE);
    }

    while (PeekMessageW(&msg, _glfw.win32.helperWindowHandle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

   return window;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

// Returns a wide string version of the specified UTF-8 string
//
WCHAR* _glfwCreateWideStringFromUTF8Win32(const char* source)
{
    WCHAR* target;
    int length;

    length = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);
    if (!length)
        return NULL;

    target = calloc(length, sizeof(WCHAR));

    if (!MultiByteToWideChar(CP_UTF8, 0, source, -1, target, length))
    {
        free(target);
        return NULL;
    }

    return target;
}

// Returns a UTF-8 string version of the specified wide string
//
char* _glfwCreateUTF8FromWideStringWin32(const WCHAR* source)
{
    char* target;
    int length;

    length = WideCharToMultiByte(CP_UTF8, 0, source, -1, NULL, 0, NULL, NULL);
    if (!length)
        return NULL;

    target = calloc(length, 1);

    if (!WideCharToMultiByte(CP_UTF8, 0, source, -1, target, length, NULL, NULL))
    {
        free(target);
        return NULL;
    }

    return target;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformInit(void)
{
    if (!_glfwInitThreadLocalStorageWin32())
        return GLFW_FALSE;

    // To make SetForegroundWindow work as we want, we need to fiddle
    // with the FOREGROUNDLOCKTIMEOUT system setting (we do this as early
    // as possible in the hope of still being the foreground process)
    SystemParametersInfoW(SPI_GETFOREGROUNDLOCKTIMEOUT, 0,
                          &_glfw.win32.foregroundLockTimeout, 0);
    SystemParametersInfoW(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, UIntToPtr(0),
                          SPIF_SENDCHANGE);

    if (!loadLibraries())
        return GLFW_FALSE;

    createKeyTables();

    if (_glfw_SetProcessDpiAwareness)
        _glfw_SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    else if (_glfw_SetProcessDPIAware)
        _glfw_SetProcessDPIAware();

    if (!_glfwRegisterWindowClassWin32())
        return GLFW_FALSE;

    _glfw.win32.helperWindowHandle = createHelperWindow();
    if (!_glfw.win32.helperWindowHandle)
        return GLFW_FALSE;

    _glfwInitTimerWin32();
    _glfwInitJoysticksWin32();

    _glfwPollMonitorsWin32();
    return GLFW_TRUE;
}

void _glfwPlatformTerminate(void)
{
    if (_glfw.win32.helperWindowHandle)
        DestroyWindow(_glfw.win32.helperWindowHandle);

    _glfwUnregisterWindowClassWin32();

    // Restore previous foreground lock timeout system setting
    SystemParametersInfoW(SPI_SETFOREGROUNDLOCKTIMEOUT, 0,
                          UIntToPtr(_glfw.win32.foregroundLockTimeout),
                          SPIF_SENDCHANGE);

    free(_glfw.win32.clipboardString);

    _glfwTerminateWGL();
    _glfwTerminateEGL();

    _glfwTerminateJoysticksWin32();
    _glfwTerminateThreadLocalStorageWin32();

    freeLibraries();
}

const char* _glfwPlatformGetVersionString(void)
{
    return _GLFW_VERSION_NUMBER " Win32 WGL EGL"
#if defined(__MINGW32__)
        " MinGW"
#elif defined(_MSC_VER)
        " VisualC"
#endif
#if defined(_GLFW_USE_HYBRID_HPG) || defined(_GLFW_USE_OPTIMUS_HPG)
        " hybrid-GPU"
#endif
#if defined(_GLFW_BUILD_DLL)
        " DLL"
#endif
        ;
}

//========================================================================
// GLFW 3.1 Win32 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <math.h>

#include <initguid.h>

#define _GLFW_PRESENCE_ONLY 1
#define _GLFW_UPDATE_STATE  2

#define _GLFW_TYPE_AXIS     0
#define _GLFW_TYPE_SLIDER   1
#define _GLFW_TYPE_BUTTON   2
#define _GLFW_TYPE_POV      3

// Data produced with DirectInput device object enumeration
//
typedef struct _GLFWobjenumWin32
{
    IDirectInputDevice8W*   device;
    _GLFWjoyobjectWin32*    objects;
    int                     objectCount;
    int                     axisCount;
    int                     sliderCount;
    int                     buttonCount;
    int                     povCount;
} _GLFWobjenumWin32;

// Define only the necessary GUIDs (it's bad enough that we're exporting these)
//
DEFINE_GUID(IID_IDirectInput8W,0xbf798031,0x483a,0x4da2,0xaa,0x99,0x5d,0x64,0xed,0x36,0x97,0x00);
DEFINE_GUID(GUID_XAxis,0xa36d02e0,0xc9f3,0x11cf,0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_YAxis,0xa36d02e1,0xc9f3,0x11cf,0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_ZAxis,0xa36d02e2,0xc9f3,0x11cf,0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RxAxis,0xa36d02f4,0xc9f3,0x11cf,0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RyAxis,0xa36d02f5,0xc9f3,0x11cf,0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RzAxis,0xa36d02e3,0xc9f3,0x11cf,0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Slider,0xa36d02e4,0xc9f3,0x11cf,0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Button,0xa36d02f0,0xc9f3,0x11cf,0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_POV,0xa36d02f2,0xc9f3,0x11cf,0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00);

// Object data array for our clone of c_dfDIJoystick
// Generated with https://github.com/elmindreda/c_dfDIJoystick2
//
static DIOBJECTDATAFORMAT _glfwObjectDataFormats[] =
{
    { &GUID_XAxis,DIJOFS_X,DIDFT_AXIS|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
    { &GUID_YAxis,DIJOFS_Y,DIDFT_AXIS|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
    { &GUID_ZAxis,DIJOFS_Z,DIDFT_AXIS|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
    { &GUID_RxAxis,DIJOFS_RX,DIDFT_AXIS|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
    { &GUID_RyAxis,DIJOFS_RY,DIDFT_AXIS|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
    { &GUID_RzAxis,DIJOFS_RZ,DIDFT_AXIS|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
    { &GUID_Slider,DIJOFS_SLIDER(0),DIDFT_AXIS|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
    { &GUID_Slider,DIJOFS_SLIDER(1),DIDFT_AXIS|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
    { &GUID_POV,DIJOFS_POV(0),DIDFT_POV|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { &GUID_POV,DIJOFS_POV(1),DIDFT_POV|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { &GUID_POV,DIJOFS_POV(2),DIDFT_POV|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { &GUID_POV,DIJOFS_POV(3),DIDFT_POV|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(0),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(1),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(2),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(3),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(4),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(5),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(6),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(7),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(8),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(9),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(10),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(11),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(12),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(13),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(14),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(15),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(16),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(17),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(18),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(19),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(20),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(21),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(22),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(23),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(24),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(25),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(26),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(27),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(28),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(29),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(30),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
    { NULL,DIJOFS_BUTTON(31),DIDFT_BUTTON|DIDFT_OPTIONAL|DIDFT_ANYINSTANCE,0 },
};

// Our clone of c_dfDIJoystick
//
static const DIDATAFORMAT _glfwDataFormat =
{
    sizeof(DIDATAFORMAT),
    sizeof(DIOBJECTDATAFORMAT),
    DIDFT_ABSAXIS,
    sizeof(DIJOYSTATE),
    sizeof(_glfwObjectDataFormats) / sizeof(DIOBJECTDATAFORMAT),
    _glfwObjectDataFormats
};

// Returns a description fitting the specified XInput capabilities
//
static const char* getDeviceDescription(const XINPUT_CAPABILITIES* xic)
{
    switch (xic->SubType)
    {
        case XINPUT_DEVSUBTYPE_WHEEL:
            return "XInput Wheel";
        case XINPUT_DEVSUBTYPE_ARCADE_STICK:
            return "XInput Arcade Stick";
        case XINPUT_DEVSUBTYPE_FLIGHT_STICK:
            return "XInput Flight Stick";
        case XINPUT_DEVSUBTYPE_DANCE_PAD:
            return "XInput Dance Pad";
        case XINPUT_DEVSUBTYPE_GUITAR:
            return "XInput Guitar";
        case XINPUT_DEVSUBTYPE_DRUM_KIT:
            return "XInput Drum Kit";
        case XINPUT_DEVSUBTYPE_GAMEPAD:
        {
            if (xic->Flags & XINPUT_CAPS_WIRELESS)
                return "Wireless Xbox 360 Controller";
            else
                return "Xbox 360 Controller";
        }
    }

    return "Unknown XInput Device";
}

// Lexically compare device objects
//
static int compareJoystickObjects(const void* first, const void* second)
{
    const _GLFWjoyobjectWin32* fo = first;
    const _GLFWjoyobjectWin32* so = second;

    if (fo->type != so->type)
        return fo->type - so->type;

    return fo->offset - so->offset;
}

// Checks whether the specified device supports XInput
// Technique from FDInputJoystickManager::IsXInputDeviceFast in ZDoom
//
static GLFWbool supportsXInput(const GUID* guid)
{
    UINT i, count = 0;
    RAWINPUTDEVICELIST* ridl;
    GLFWbool result = GLFW_FALSE;

    if (GetRawInputDeviceList(NULL, &count, sizeof(RAWINPUTDEVICELIST)) != 0)
        return GLFW_FALSE;

    ridl = calloc(count, sizeof(RAWINPUTDEVICELIST));

    if (GetRawInputDeviceList(ridl, &count, sizeof(RAWINPUTDEVICELIST)) == (UINT) -1)
    {
        free(ridl);
        return GLFW_FALSE;
    }

    for (i = 0;  i < count;  i++)
    {
        RID_DEVICE_INFO rdi;
        char name[256];
        UINT size;

        if (ridl[i].dwType != RIM_TYPEHID)
            continue;

        ZeroMemory(&rdi, sizeof(rdi));
        rdi.cbSize = sizeof(rdi);
        size = sizeof(rdi);

        if ((INT) GetRawInputDeviceInfoA(ridl[i].hDevice,
                                         RIDI_DEVICEINFO,
                                         &rdi, &size) == -1)
        {
            continue;
        }

        if (MAKELONG(rdi.hid.dwVendorId, rdi.hid.dwProductId) != (LONG) guid->Data1)
            continue;

        memset(name, 0, sizeof(name));
        size = sizeof(name);

        if ((INT) GetRawInputDeviceInfoA(ridl[i].hDevice,
                                         RIDI_DEVICENAME,
                                         name, &size) == -1)
        {
            break;
        }

        name[sizeof(name) - 1] = '\0';
        if (strstr(name, "IG_"))
        {
            result = GLFW_TRUE;
            break;
        }
    }

    free(ridl);
    return result;
}

// Frees all resources associated with the specified joystick
//
static void closeJoystick(_GLFWjoystickWin32* js)
{
    if (js->device)
    {
        IDirectInputDevice8_Unacquire(js->device);
        IDirectInputDevice8_Release(js->device);
    }

    free(js->name);
    free(js->axes);
    free(js->buttons);
    free(js->objects);
    memset(js, 0, sizeof(_GLFWjoystickWin32));

    _glfwInputJoystickChange((int) (js - _glfw.win32_js), GLFW_DISCONNECTED);
}

// DirectInput device object enumeration callback
// Insights gleaned from SDL2
//
static BOOL CALLBACK deviceObjectCallback(const DIDEVICEOBJECTINSTANCEW* doi,
                                          void* user)
{
    _GLFWobjenumWin32* data = user;
    _GLFWjoyobjectWin32* object = data->objects + data->objectCount;

    if (DIDFT_GETTYPE(doi->dwType) & DIDFT_AXIS)
    {
        DIPROPRANGE dipr;

        if (memcmp(&doi->guidType, &GUID_Slider, sizeof(GUID)) == 0)
            object->offset = DIJOFS_SLIDER(data->sliderCount);
        else if (memcmp(&doi->guidType, &GUID_XAxis, sizeof(GUID)) == 0)
            object->offset = DIJOFS_X;
        else if (memcmp(&doi->guidType, &GUID_YAxis, sizeof(GUID)) == 0)
            object->offset = DIJOFS_Y;
        else if (memcmp(&doi->guidType, &GUID_ZAxis, sizeof(GUID)) == 0)
            object->offset = DIJOFS_Z;
        else if (memcmp(&doi->guidType, &GUID_RxAxis, sizeof(GUID)) == 0)
            object->offset = DIJOFS_RX;
        else if (memcmp(&doi->guidType, &GUID_RyAxis, sizeof(GUID)) == 0)
            object->offset = DIJOFS_RY;
        else if (memcmp(&doi->guidType, &GUID_RzAxis, sizeof(GUID)) == 0)
            object->offset = DIJOFS_RZ;
        else
            return DIENUM_CONTINUE;

        ZeroMemory(&dipr, sizeof(dipr));
        dipr.diph.dwSize = sizeof(dipr);
        dipr.diph.dwHeaderSize = sizeof(dipr.diph);
        dipr.diph.dwObj = doi->dwType;
        dipr.diph.dwHow = DIPH_BYID;
        dipr.lMin = -32768;
        dipr.lMax =  32767;

        if (FAILED(IDirectInputDevice8_SetProperty(data->device,
                                                   DIPROP_RANGE,
                                                   &dipr.diph)))
        {
            return DIENUM_CONTINUE;
        }

        if (memcmp(&doi->guidType, &GUID_Slider, sizeof(GUID)) == 0)
        {
            object->type = _GLFW_TYPE_SLIDER;
            data->sliderCount++;
        }
        else
        {
            object->type = _GLFW_TYPE_AXIS;
            data->axisCount++;
        }
    }
    else if (DIDFT_GETTYPE(doi->dwType) & DIDFT_BUTTON)
    {
        object->offset = DIJOFS_BUTTON(data->buttonCount);
        object->type = _GLFW_TYPE_BUTTON;
        data->buttonCount++;
    }
    else if (DIDFT_GETTYPE(doi->dwType) & DIDFT_POV)
    {
        object->offset = DIJOFS_POV(data->povCount);
        object->type = _GLFW_TYPE_POV;
        data->povCount++;
    }

    data->objectCount++;
    return DIENUM_CONTINUE;
}

// DirectInput device enumeration callback
//
static BOOL CALLBACK deviceCallback(const DIDEVICEINSTANCE* di, void* user)
{
    int jid = 0;
    DIDEVCAPS dc;
    DIPROPDWORD dipd;
    IDirectInputDevice8* device;
    _GLFWobjenumWin32 data;
    _GLFWjoystickWin32* js;

    for (jid = GLFW_JOYSTICK_1;  jid <= GLFW_JOYSTICK_LAST;  jid++)
    {
        if (memcmp(&_glfw.win32_js[jid].guid, &di->guidInstance, sizeof(GUID)) == 0)
            return DIENUM_CONTINUE;
    }

    for (jid = GLFW_JOYSTICK_1;  jid <= GLFW_JOYSTICK_LAST;  jid++)
    {
        if (!_glfw.win32_js[jid].present)
            break;
    }

    if (jid > GLFW_JOYSTICK_LAST)
        return DIENUM_STOP;

    if (supportsXInput(&di->guidProduct))
        return DIENUM_CONTINUE;

    if (FAILED(IDirectInput8_CreateDevice(_glfw.win32.dinput8.api,
                                          &di->guidInstance,
                                          &device,
                                          NULL)))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to create device");
        return DIENUM_CONTINUE;
    }

    if (FAILED(IDirectInputDevice8_SetDataFormat(device, &_glfwDataFormat)))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to set device data format");

        IDirectInputDevice8_Release(device);
        return DIENUM_CONTINUE;
    }

    ZeroMemory(&dc, sizeof(dc));
    dc.dwSize = sizeof(dc);

    if (FAILED(IDirectInputDevice8_GetCapabilities(device, &dc)))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to query device capabilities");

        IDirectInputDevice8_Release(device);
        return DIENUM_CONTINUE;
    }

    ZeroMemory(&dipd, sizeof(dipd));
    dipd.diph.dwSize = sizeof(dipd);
    dipd.diph.dwHeaderSize = sizeof(dipd.diph);
    dipd.diph.dwHow = DIPH_DEVICE;
    dipd.dwData = DIPROPAXISMODE_ABS;

    if (FAILED(IDirectInputDevice8_SetProperty(device,
                                               DIPROP_AXISMODE,
                                               &dipd.diph)))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to set device axis mode");

        IDirectInputDevice8_Release(device);
        return DIENUM_CONTINUE;
    }

    memset(&data, 0, sizeof(data));
    data.device = device;
    data.objects = calloc(dc.dwAxes + dc.dwButtons + dc.dwPOVs,
                          sizeof(_GLFWjoyobjectWin32));

    if (FAILED(IDirectInputDevice8_EnumObjects(device,
                                               deviceObjectCallback,
                                               &data,
                                               DIDFT_AXIS | DIDFT_BUTTON | DIDFT_POV)))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to enumerate device objects");

        IDirectInputDevice8_Release(device);
        free(data.objects);
        return DIENUM_CONTINUE;
    }

    qsort(data.objects, data.objectCount,
          sizeof(_GLFWjoyobjectWin32),
          compareJoystickObjects);

    js = _glfw.win32_js + jid;
    js->device = device;
    js->guid = di->guidInstance;
    js->axisCount = data.axisCount + data.sliderCount;
    js->axes = calloc(js->axisCount, sizeof(float));
    js->buttonCount += data.buttonCount + data.povCount * 4;
    js->buttons = calloc(js->buttonCount, 1);
    js->objects = data.objects;
    js->objectCount = data.objectCount;
    js->name = _glfwCreateUTF8FromWideStringWin32(di->tszInstanceName);
    js->present = GLFW_TRUE;

    _glfwInputJoystickChange(jid, GLFW_CONNECTED);
    return DIENUM_CONTINUE;
}

// Attempt to open the specified joystick device
// TODO: Pack state arrays for non-gamepad devices
//
static GLFWbool openXinputDevice(DWORD index)
{
    int jid;
    XINPUT_CAPABILITIES xic;
    _GLFWjoystickWin32* js;

    for (jid = GLFW_JOYSTICK_1;  jid <= GLFW_JOYSTICK_LAST;  jid++)
    {
        if (_glfw.win32_js[jid].present &&
            _glfw.win32_js[jid].device == NULL &&
            _glfw.win32_js[jid].index == index)
        {
            return GLFW_FALSE;
        }
    }

    for (jid = GLFW_JOYSTICK_1;  jid <= GLFW_JOYSTICK_LAST;  jid++)
    {
        if (!_glfw.win32_js[jid].present)
            break;
    }

    if (jid > GLFW_JOYSTICK_LAST)
        return GLFW_FALSE;

    if (_glfw_XInputGetCapabilities(index, 0, &xic) != ERROR_SUCCESS)
        return GLFW_FALSE;

    js = _glfw.win32_js + jid;
    js->axisCount = 6;
    js->axes = calloc(js->axisCount, sizeof(float));
    js->buttonCount = 14;
    js->buttons = calloc(js->buttonCount, 1);
    js->present = GLFW_TRUE;
    js->name = strdup(getDeviceDescription(&xic));
    js->index = index;

    _glfwInputJoystickChange(jid, GLFW_CONNECTED);

    return GLFW_TRUE;
}

// Polls for and processes events the specified joystick
//
static GLFWbool pollJoystickState(_GLFWjoystickWin32* js, int mode)
{
    if (!js->present)
        return GLFW_FALSE;

    if (js->device)
    {
        int i, j, ai = 0, bi = 0;
        HRESULT result;
        DIJOYSTATE state;

        IDirectInputDevice8_Poll(js->device);
        result = IDirectInputDevice8_GetDeviceState(js->device,
                                                    sizeof(state),
                                                    &state);
        if (result == DIERR_NOTACQUIRED || result == DIERR_INPUTLOST)
        {
            IDirectInputDevice8_Acquire(js->device);
            IDirectInputDevice8_Poll(js->device);
            result = IDirectInputDevice8_GetDeviceState(js->device,
                                                        sizeof(state),
                                                        &state);
        }

        if (FAILED(result))
        {
            closeJoystick(js);
            return GLFW_FALSE;
        }

        if (mode == _GLFW_PRESENCE_ONLY)
            return GLFW_TRUE;

        for (i = 0;  i < js->objectCount;  i++)
        {
            const void* data = (char*) &state + js->objects[i].offset;

            switch (js->objects[i].type)
            {
                case _GLFW_TYPE_AXIS:
                case _GLFW_TYPE_SLIDER:
                {
                    js->axes[ai++] = (*((LONG*) data) + 0.5f) / 32767.5f;
                    break;
                }

                case _GLFW_TYPE_BUTTON:
                {
                    if (*((BYTE*) data) & 0x80)
                        js->buttons[bi++] = GLFW_PRESS;
                    else
                        js->buttons[bi++] = GLFW_RELEASE;

                    break;
                }

                case _GLFW_TYPE_POV:
                {
                    const int directions[9] = { 1, 3, 2, 6, 4, 12, 8, 9, 0 };
                    // Screams of horror are appropriate at this point
                    int value = LOWORD(*(DWORD*) data) / (45 * DI_DEGREES);
                    if (value < 0 || value > 8)
                        value = 8;

                    for (j = 0;  j < 4;  j++)
                    {
                        if (directions[value] & (1 << j))
                            js->buttons[bi++] = GLFW_PRESS;
                        else
                            js->buttons[bi++] = GLFW_RELEASE;
                    }

                    break;
                }
            }
        }

        return GLFW_TRUE;
    }
    else
    {
        int i;
        DWORD result;
        XINPUT_STATE xis;
        const WORD buttons[14] =
        {
            XINPUT_GAMEPAD_A,
            XINPUT_GAMEPAD_B,
            XINPUT_GAMEPAD_X,
            XINPUT_GAMEPAD_Y,
            XINPUT_GAMEPAD_LEFT_SHOULDER,
            XINPUT_GAMEPAD_RIGHT_SHOULDER,
            XINPUT_GAMEPAD_BACK,
            XINPUT_GAMEPAD_START,
            XINPUT_GAMEPAD_LEFT_THUMB,
            XINPUT_GAMEPAD_RIGHT_THUMB,
            XINPUT_GAMEPAD_DPAD_UP,
            XINPUT_GAMEPAD_DPAD_RIGHT,
            XINPUT_GAMEPAD_DPAD_DOWN,
            XINPUT_GAMEPAD_DPAD_LEFT
        };

        result = _glfw_XInputGetState(js->index, &xis);
        if (result != ERROR_SUCCESS)
        {
            if (result == ERROR_DEVICE_NOT_CONNECTED)
                closeJoystick(js);

            return GLFW_FALSE;
        }

        if (mode == _GLFW_PRESENCE_ONLY)
            return GLFW_TRUE;

        if ((float) xis.Gamepad.sThumbLX * xis.Gamepad.sThumbLX +
            (float) xis.Gamepad.sThumbLY * xis.Gamepad.sThumbLY >
            (float) XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE *
                    XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
        {
            js->axes[0] = (xis.Gamepad.sThumbLX + 0.5f) / 32767.f;
            js->axes[1] = (xis.Gamepad.sThumbLY + 0.5f) / 32767.f;
        }
        else
        {
            js->axes[0] = 0.f;
            js->axes[1] = 0.f;
        }

        if ((float) xis.Gamepad.sThumbRX * xis.Gamepad.sThumbRX +
            (float) xis.Gamepad.sThumbRY * xis.Gamepad.sThumbRY >
            (float) XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE *
                    XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
        {
            js->axes[2] = (xis.Gamepad.sThumbRX + 0.5f) / 32767.f;
            js->axes[3] = (xis.Gamepad.sThumbRY + 0.5f) / 32767.f;
        }
        else
        {
            js->axes[2] = 0.f;
            js->axes[3] = 0.f;
        }

        if (xis.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
            js->axes[4] = xis.Gamepad.bLeftTrigger / 127.5f - 1.f;
        else
            js->axes[4] = -1.f;

        if (xis.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
            js->axes[5] = xis.Gamepad.bRightTrigger / 127.5f - 1.f;
        else
            js->axes[5] = -1.f;

        for (i = 0;  i < 14;  i++)
            js->buttons[i] = (xis.Gamepad.wButtons & buttons[i]) ? 1 : 0;

        return GLFW_TRUE;
    }
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

// Initialize joystick interface
//
void _glfwInitJoysticksWin32(void)
{
    if (_glfw.win32.dinput8.instance)
    {
        if (FAILED(_glfw_DirectInput8Create(GetModuleHandle(NULL),
                                            DIRECTINPUT_VERSION,
                                            &IID_IDirectInput8W,
                                            (void**) &_glfw.win32.dinput8.api,
                                            NULL)))
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "Win32: Failed to create interface");
        }
    }

    _glfwDetectJoystickConnectionWin32();
}

// Close all opened joystick handles
//
void _glfwTerminateJoysticksWin32(void)
{
    int jid;

    for (jid = GLFW_JOYSTICK_1;  jid <= GLFW_JOYSTICK_LAST;  jid++)
        closeJoystick(_glfw.win32_js + jid);

    if (_glfw.win32.dinput8.api)
        IDirectInput8_Release(_glfw.win32.dinput8.api);
}

// Checks for new joysticks after DBT_DEVICEARRIVAL
//
void _glfwDetectJoystickConnectionWin32(void)
{
    if (_glfw.win32.xinput.instance)
    {
        DWORD i;

        for (i = 0;  i < XUSER_MAX_COUNT;  i++)
            openXinputDevice(i);
    }

    if (_glfw.win32.dinput8.api)
    {
        if (FAILED(IDirectInput8_EnumDevices(_glfw.win32.dinput8.api,
                                             DI8DEVCLASS_GAMECTRL,
                                             deviceCallback,
                                             NULL,
                                             DIEDFL_ALLDEVICES)))
        {
            _glfwInputError(GLFW_PLATFORM_ERROR,
                            "Failed to enumerate DirectInput8 devices");
            return;
        }
    }
}

// Checks for joystick disconnection after DBT_DEVICEREMOVECOMPLETE
//
void _glfwDetectJoystickDisconnectionWin32(void)
{
    int jid;

    for (jid = GLFW_JOYSTICK_1;  jid <= GLFW_JOYSTICK_LAST;  jid++)
        pollJoystickState(_glfw.win32_js + jid, _GLFW_PRESENCE_ONLY);
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformJoystickPresent(int jid)
{
    _GLFWjoystickWin32* js = _glfw.win32_js + jid;
    return pollJoystickState(js, _GLFW_PRESENCE_ONLY);
}

const float* _glfwPlatformGetJoystickAxes(int jid, int* count)
{
    _GLFWjoystickWin32* js = _glfw.win32_js + jid;
    if (!pollJoystickState(js, _GLFW_UPDATE_STATE))
        return NULL;

    *count = js->axisCount;
    return js->axes;
}

const unsigned char* _glfwPlatformGetJoystickButtons(int jid, int* count)
{
    _GLFWjoystickWin32* js = _glfw.win32_js + jid;
    if (!pollJoystickState(js, _GLFW_UPDATE_STATE))
        return NULL;

    *count = js->buttonCount;
    return js->buttons;
}

const char* _glfwPlatformGetJoystickName(int jid)
{
    _GLFWjoystickWin32* js = _glfw.win32_js + jid;
    if (!pollJoystickState(js, _GLFW_PRESENCE_ONLY))
        return NULL;

    return js->name;
}

//========================================================================
// GLFW 3.3 Win32 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <malloc.h>


// Create monitor from an adapter and (optionally) a display
//
static _GLFWmonitor* createMonitor(DISPLAY_DEVICEW* adapter,
                                   DISPLAY_DEVICEW* display)
{
    _GLFWmonitor* monitor;
    char* name;
    HDC dc;

    if (display)
        name = _glfwCreateUTF8FromWideStringWin32(display->DeviceString);
    else
        name = _glfwCreateUTF8FromWideStringWin32(adapter->DeviceString);
    if (!name)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to convert string to UTF-8");
        return NULL;
    }

    dc = CreateDCW(L"DISPLAY", adapter->DeviceName, NULL, NULL);

    monitor = _glfwAllocMonitor(name,
                                GetDeviceCaps(dc, HORZSIZE),
                                GetDeviceCaps(dc, VERTSIZE));

    DeleteDC(dc);
    free(name);

    if (adapter->StateFlags & DISPLAY_DEVICE_MODESPRUNED)
        monitor->win32.modesPruned = GLFW_TRUE;

    wcscpy(monitor->win32.adapterName, adapter->DeviceName);
    WideCharToMultiByte(CP_UTF8, 0,
                        adapter->DeviceName, -1,
                        monitor->win32.publicAdapterName,
                        sizeof(monitor->win32.publicAdapterName),
                        NULL, NULL);

    if (display)
    {
        wcscpy(monitor->win32.displayName, display->DeviceName);
        WideCharToMultiByte(CP_UTF8, 0,
                            display->DeviceName, -1,
                            monitor->win32.publicDisplayName,
                            sizeof(monitor->win32.publicDisplayName),
                            NULL, NULL);
    }

    return monitor;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

// Poll for changes in the set of connected monitors
//
void _glfwPollMonitorsWin32(void)
{
    int i, disconnectedCount;
    _GLFWmonitor** disconnected;
    DWORD adapterIndex, displayIndex;
    DISPLAY_DEVICEW adapter, display;
    GLFWbool hasDisplays = GLFW_FALSE;

    disconnectedCount = _glfw.monitorCount;
    disconnected = calloc(_glfw.monitorCount, sizeof(_GLFWmonitor*));
    memcpy(disconnected,
           _glfw.monitors,
           _glfw.monitorCount * sizeof(_GLFWmonitor*));

    // HACK: Check if any active adapters have connected displays
    //       If not, this is a headless system or a VMware guest

    for (adapterIndex = 0;  ;  adapterIndex++)
    {
        ZeroMemory(&adapter, sizeof(DISPLAY_DEVICEW));
        adapter.cb = sizeof(DISPLAY_DEVICEW);

        if (!EnumDisplayDevicesW(NULL, adapterIndex, &adapter, 0))
            break;

        if (!(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE))
            continue;

        ZeroMemory(&display, sizeof(DISPLAY_DEVICEW));
        display.cb = sizeof(DISPLAY_DEVICEW);

        if (EnumDisplayDevicesW(adapter.DeviceName, 0, &display, 0))
        {
            hasDisplays = GLFW_TRUE;
            break;
        }
    }

    for (adapterIndex = 0;  ;  adapterIndex++)
    {
        int type = _GLFW_INSERT_LAST;

        ZeroMemory(&adapter, sizeof(DISPLAY_DEVICEW));
        adapter.cb = sizeof(DISPLAY_DEVICEW);

        if (!EnumDisplayDevicesW(NULL, adapterIndex, &adapter, 0))
            break;

        if (!(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE))
            continue;

        if (adapter.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
            type = _GLFW_INSERT_FIRST;

        if (hasDisplays)
        {
            for (displayIndex = 0;  ;  displayIndex++)
            {
                ZeroMemory(&display, sizeof(DISPLAY_DEVICEW));
                display.cb = sizeof(DISPLAY_DEVICEW);

                if (!EnumDisplayDevicesW(adapter.DeviceName, displayIndex, &display, 0))
                    break;

                for (i = 0;  i < disconnectedCount;  i++)
                {
                    if (disconnected[i] &&
                        wcscmp(disconnected[i]->win32.displayName,
                               display.DeviceName) == 0)
                    {
                        disconnected[i] = NULL;
                        break;
                    }
                }

                if (i < disconnectedCount)
                    continue;

                _glfwInputMonitor(createMonitor(&adapter, &display),
                                  GLFW_CONNECTED, type);

                type = _GLFW_INSERT_LAST;
            }
        }
        else
        {
            for (i = 0;  i < disconnectedCount;  i++)
            {
                if (disconnected[i] &&
                    wcscmp(disconnected[i]->win32.adapterName,
                           adapter.DeviceName) == 0)
                {
                    disconnected[i] = NULL;
                    break;
                }
            }

            if (i < disconnectedCount)
                continue;

            _glfwInputMonitor(createMonitor(&adapter, NULL),
                              GLFW_CONNECTED, type);
        }
    }

    for (i = 0;  i < disconnectedCount;  i++)
    {
        if (disconnected[i])
            _glfwInputMonitor(disconnected[i], GLFW_DISCONNECTED, 0);
    }

    free(disconnected);
}

// Change the current video mode
//
GLFWbool _glfwSetVideoModeWin32(_GLFWmonitor* monitor, const GLFWvidmode* desired)
{
    GLFWvidmode current;
    const GLFWvidmode* best;
    DEVMODEW dm;

    best = _glfwChooseVideoMode(monitor, desired);
    _glfwPlatformGetVideoMode(monitor, &current);
    if (_glfwCompareVideoModes(&current, best) == 0)
        return GLFW_TRUE;

    ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(DEVMODEW);
    dm.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL |
                            DM_DISPLAYFREQUENCY;
    dm.dmPelsWidth        = best->width;
    dm.dmPelsHeight       = best->height;
    dm.dmBitsPerPel       = best->redBits + best->greenBits + best->blueBits;
    dm.dmDisplayFrequency = best->refreshRate;

    if (dm.dmBitsPerPel < 15 || dm.dmBitsPerPel >= 24)
        dm.dmBitsPerPel = 32;

    if (ChangeDisplaySettingsExW(monitor->win32.adapterName,
                                 &dm,
                                 NULL,
                                 CDS_FULLSCREEN,
                                 NULL) != DISP_CHANGE_SUCCESSFUL)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to set video mode");
        return GLFW_FALSE;
    }

    monitor->win32.modeChanged = GLFW_TRUE;
    return GLFW_TRUE;
}

// Restore the previously saved (original) video mode
//
void _glfwRestoreVideoModeWin32(_GLFWmonitor* monitor)
{
    if (monitor->win32.modeChanged)
    {
        ChangeDisplaySettingsExW(monitor->win32.adapterName,
                                 NULL, NULL, CDS_FULLSCREEN, NULL);
        monitor->win32.modeChanged = GLFW_FALSE;
    }
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

void _glfwPlatformGetMonitorPos(_GLFWmonitor* monitor, int* xpos, int* ypos)
{
    DEVMODEW settings;
    ZeroMemory(&settings, sizeof(DEVMODEW));
    settings.dmSize = sizeof(DEVMODEW);

    EnumDisplaySettingsExW(monitor->win32.adapterName,
                           ENUM_CURRENT_SETTINGS,
                           &settings,
                           EDS_ROTATEDMODE);

    if (xpos)
        *xpos = settings.dmPosition.x;
    if (ypos)
        *ypos = settings.dmPosition.y;
}

GLFWvidmode* _glfwPlatformGetVideoModes(_GLFWmonitor* monitor, int* count)
{
    int modeIndex = 0, size = 0;
    GLFWvidmode* result = NULL;

    *count = 0;

    for (;;)
    {
        int i;
        GLFWvidmode mode;
        DEVMODEW dm;

        ZeroMemory(&dm, sizeof(DEVMODEW));
        dm.dmSize = sizeof(DEVMODEW);

        if (!EnumDisplaySettingsW(monitor->win32.adapterName, modeIndex, &dm))
            break;

        modeIndex++;

        // Skip modes with less than 15 BPP
        if (dm.dmBitsPerPel < 15)
            continue;

        mode.width  = dm.dmPelsWidth;
        mode.height = dm.dmPelsHeight;
        mode.refreshRate = dm.dmDisplayFrequency;
        _glfwSplitBPP(dm.dmBitsPerPel,
                      &mode.redBits,
                      &mode.greenBits,
                      &mode.blueBits);

        for (i = 0;  i < *count;  i++)
        {
            if (_glfwCompareVideoModes(result + i, &mode) == 0)
                break;
        }

        // Skip duplicate modes
        if (i < *count)
            continue;

        if (monitor->win32.modesPruned)
        {
            // Skip modes not supported by the connected displays
            if (ChangeDisplaySettingsExW(monitor->win32.adapterName,
                                         &dm,
                                         NULL,
                                         CDS_TEST,
                                         NULL) != DISP_CHANGE_SUCCESSFUL)
            {
                continue;
            }
        }

        if (*count == size)
        {
            size += 128;
            result = (GLFWvidmode*) realloc(result, size * sizeof(GLFWvidmode));
        }

        (*count)++;
        result[*count - 1] = mode;
    }

    if (!*count)
    {
        // HACK: Report the current mode if no valid modes were found
        result = calloc(1, sizeof(GLFWvidmode));
        _glfwPlatformGetVideoMode(monitor, result);
        *count = 1;
    }

    return result;
}

void _glfwPlatformGetVideoMode(_GLFWmonitor* monitor, GLFWvidmode* mode)
{
    DEVMODEW dm;

    ZeroMemory(&dm, sizeof(DEVMODEW));
    dm.dmSize = sizeof(DEVMODEW);

    EnumDisplaySettingsW(monitor->win32.adapterName, ENUM_CURRENT_SETTINGS, &dm);

    mode->width  = dm.dmPelsWidth;
    mode->height = dm.dmPelsHeight;
    mode->refreshRate = dm.dmDisplayFrequency;
    _glfwSplitBPP(dm.dmBitsPerPel,
                  &mode->redBits,
                  &mode->greenBits,
                  &mode->blueBits);
}

void _glfwPlatformGetGammaRamp(_GLFWmonitor* monitor, GLFWgammaramp* ramp)
{
    HDC dc;
    WORD values[768];

    dc = CreateDCW(L"DISPLAY", monitor->win32.adapterName, NULL, NULL);
    GetDeviceGammaRamp(dc, values);
    DeleteDC(dc);

    _glfwAllocGammaArrays(ramp, 256);

    memcpy(ramp->red,   values +   0, 256 * sizeof(unsigned short));
    memcpy(ramp->green, values + 256, 256 * sizeof(unsigned short));
    memcpy(ramp->blue,  values + 512, 256 * sizeof(unsigned short));
}

void _glfwPlatformSetGammaRamp(_GLFWmonitor* monitor, const GLFWgammaramp* ramp)
{
    HDC dc;
    WORD values[768];

    if (ramp->size != 256)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Gamma ramp size must be 256");
        return;
    }

    memcpy(values +   0, ramp->red,   256 * sizeof(unsigned short));
    memcpy(values + 256, ramp->green, 256 * sizeof(unsigned short));
    memcpy(values + 512, ramp->blue,  256 * sizeof(unsigned short));

    dc = CreateDCW(L"DISPLAY", monitor->win32.adapterName, NULL, NULL);
    SetDeviceGammaRamp(dc, values);
    DeleteDC(dc);
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW native API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI const char* glfwGetWin32Adapter(GLFWmonitor* handle)
{
    _GLFWmonitor* monitor = (_GLFWmonitor*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return monitor->win32.publicAdapterName;
}

GLFWAPI const char* glfwGetWin32Monitor(GLFWmonitor* handle)
{
    _GLFWmonitor* monitor = (_GLFWmonitor*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return monitor->win32.publicDisplayName;
}

//========================================================================
// GLFW 3.3 Win32 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

// Initialise timer
//
void _glfwInitTimerWin32(void)
{
    uint64_t frequency;

    if (QueryPerformanceFrequency((LARGE_INTEGER*) &frequency))
    {
        _glfw.win32_time.hasPC = GLFW_TRUE;
        _glfw.win32_time.frequency = frequency;
    }
    else
    {
        _glfw.win32_time.hasPC = GLFW_FALSE;
        _glfw.win32_time.frequency = 1000;
    }
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

uint64_t _glfwPlatformGetTimerValue(void)
{
    if (_glfw.win32_time.hasPC)
    {
        uint64_t value;
        QueryPerformanceCounter((LARGE_INTEGER*) &value);
        return value;
    }
    else
        return (uint64_t) _glfw_timeGetTime();
}

uint64_t _glfwPlatformGetTimerFrequency(void)
{
    return _glfw.win32_time.frequency;
}

//========================================================================
// GLFW 3.3 Win32 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

GLFWbool _glfwInitThreadLocalStorageWin32(void)
{
    _glfw.win32_tls.context = TlsAlloc();
    if (_glfw.win32_tls.context == TLS_OUT_OF_INDEXES)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to allocate TLS index");
        return GLFW_FALSE;
    }

    _glfw.win32_tls.allocated = GLFW_TRUE;
    return GLFW_TRUE;
}

void _glfwTerminateThreadLocalStorageWin32(void)
{
    if (_glfw.win32_tls.allocated)
        TlsFree(_glfw.win32_tls.context);
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

void _glfwPlatformSetCurrentContext(_GLFWwindow* context)
{
    TlsSetValue(_glfw.win32_tls.context, context);
}

_GLFWwindow* _glfwPlatformGetCurrentContext(void)
{
    return TlsGetValue(_glfw.win32_tls.context);
}

//========================================================================
// GLFW 3.3 Win32 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <limits.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <windowsx.h>
#include <shellapi.h>

#define _GLFW_KEY_INVALID -2

// Returns the window style for the specified window
//
static DWORD getWindowStyle(const _GLFWwindow* window)
{
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    if (window->monitor)
        style |= WS_POPUP;
    else
    {
        style |= WS_SYSMENU | WS_MINIMIZEBOX;

        if (window->decorated)
        {
            style |= WS_CAPTION;

            if (window->resizable)
                style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
        }
        else
            style |= WS_POPUP;
    }

    return style;
}

// Returns the extended window style for the specified window
//
static DWORD getWindowExStyle(const _GLFWwindow* window)
{
    DWORD style = WS_EX_APPWINDOW;

    if (window->monitor || window->floating)
        style |= WS_EX_TOPMOST;

    return style;
}

// Returns the image whose area most closely matches the desired one
//
static const GLFWimage* chooseImage(int count, const GLFWimage* images,
                                    int width, int height)
{
    int i, leastDiff = INT_MAX;
    const GLFWimage* closest = NULL;

    for (i = 0;  i < count;  i++)
    {
        const int currDiff = abs(images[i].width * images[i].height -
                                 width * height);
        if (currDiff < leastDiff)
        {
            closest = images + i;
            leastDiff = currDiff;
        }
    }

    return closest;
}

// Creates an RGBA icon or cursor
//
static HICON createIcon(const GLFWimage* image,
                        int xhot, int yhot, GLFWbool icon)
{
    int i;
    HDC dc;
    HICON handle;
    HBITMAP color, mask;
    BITMAPV5HEADER bi;
    ICONINFO ii;
    unsigned char* target = NULL;
    unsigned char* source = image->pixels;

    ZeroMemory(&bi, sizeof(bi));
    bi.bV5Size        = sizeof(BITMAPV5HEADER);
    bi.bV5Width       = image->width;
    bi.bV5Height      = -image->height;
    bi.bV5Planes      = 1;
    bi.bV5BitCount    = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask     = 0x00ff0000;
    bi.bV5GreenMask   = 0x0000ff00;
    bi.bV5BlueMask    = 0x000000ff;
    bi.bV5AlphaMask   = 0xff000000;

    dc = GetDC(NULL);
    color = CreateDIBSection(dc,
                             (BITMAPINFO*) &bi,
                             DIB_RGB_COLORS,
                             (void**) &target,
                             NULL,
                             (DWORD) 0);
    ReleaseDC(NULL, dc);

    if (!color)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to create RGBA bitmap");
        return NULL;
    }

    mask = CreateBitmap(image->width, image->height, 1, 1, NULL);
    if (!mask)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to create mask bitmap");
        DeleteObject(color);
        return NULL;
    }

    for (i = 0;  i < image->width * image->height;  i++)
    {
        target[0] = source[2];
        target[1] = source[1];
        target[2] = source[0];
        target[3] = source[3];
        target += 4;
        source += 4;
    }

    ZeroMemory(&ii, sizeof(ii));
    ii.fIcon    = icon;
    ii.xHotspot = xhot;
    ii.yHotspot = yhot;
    ii.hbmMask  = mask;
    ii.hbmColor = color;

    handle = CreateIconIndirect(&ii);

    DeleteObject(color);
    DeleteObject(mask);

    if (!handle)
    {
        if (icon)
            _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to create icon");
        else
            _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to create cursor");
    }

    return handle;
}

// Translate client window size to full window size according to styles
//
static void getFullWindowSize(DWORD style, DWORD exStyle,
                              int clientWidth, int clientHeight,
                              int* fullWidth, int* fullHeight)
{
    RECT rect = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);
    *fullWidth = rect.right - rect.left;
    *fullHeight = rect.bottom - rect.top;
}

// Enforce the client rect aspect ratio based on which edge is being dragged
//
static void applyAspectRatio(_GLFWwindow* window, int edge, RECT* area)
{
    int xoff, yoff;
    const float ratio = (float) window->numer / (float) window->denom;

    getFullWindowSize(getWindowStyle(window), getWindowExStyle(window),
                      0, 0, &xoff, &yoff);

    if (edge == WMSZ_LEFT  || edge == WMSZ_BOTTOMLEFT ||
        edge == WMSZ_RIGHT || edge == WMSZ_BOTTOMRIGHT)
    {
        area->bottom = area->top + yoff +
            (int) ((area->right - area->left - xoff) / ratio);
    }
    else if (edge == WMSZ_TOPLEFT || edge == WMSZ_TOPRIGHT)
    {
        area->top = area->bottom - yoff -
            (int) ((area->right - area->left - xoff) / ratio);
    }
    else if (edge == WMSZ_TOP || edge == WMSZ_BOTTOM)
    {
        area->right = area->left + xoff +
            (int) ((area->bottom - area->top - yoff) * ratio);
    }
}

// Centers the cursor over the window client area
//
static void centerCursor(_GLFWwindow* window)
{
    int width, height;
    _glfwPlatformGetWindowSize(window, &width, &height);
    _glfwPlatformSetCursorPos(window, width / 2.0, height / 2.0);
}

// Returns whether the cursor is in the client area of the specified window
//
static GLFWbool cursorInClientArea(_GLFWwindow* window)
{
    RECT area;
    POINT pos;

    if (!GetCursorPos(&pos))
        return GLFW_FALSE;

    if (WindowFromPoint(pos) != window->win32.handle)
        return GLFW_FALSE;

    GetClientRect(window->win32.handle, &area);
    ClientToScreen(window->win32.handle, (POINT*) &area.left);
    ClientToScreen(window->win32.handle, (POINT*) &area.right);

    return PtInRect(&area, pos);
}

// Updates the cursor image according to its cursor mode
//
static void updateCursorImage(_GLFWwindow* window)
{
    if (window->cursorMode == GLFW_CURSOR_NORMAL)
    {
        if (window->cursor)
            SetCursor(window->cursor->win32.handle);
        else
            SetCursor(LoadCursorW(NULL, IDC_ARROW));
    }
    else
        SetCursor(NULL);
}

// Updates the cursor clip rect
//
static void updateClipRect(_GLFWwindow* window)
{
    if (window)
    {
        RECT clipRect;
        GetClientRect(window->win32.handle, &clipRect);
        ClientToScreen(window->win32.handle, (POINT*) &clipRect.left);
        ClientToScreen(window->win32.handle, (POINT*) &clipRect.right);
        ClipCursor(&clipRect);
    }
    else
        ClipCursor(NULL);
}

// Update native window styles to match attributes
//
static void updateWindowStyles(const _GLFWwindow* window)
{
    RECT rect;
    DWORD style = GetWindowLongW(window->win32.handle, GWL_STYLE);
    style &= ~(WS_OVERLAPPEDWINDOW | WS_POPUP);
    style |= getWindowStyle(window);

    GetClientRect(window->win32.handle, &rect);
    AdjustWindowRectEx(&rect, style, FALSE, getWindowExStyle(window));
    ClientToScreen(window->win32.handle, (POINT*) &rect.left);
    ClientToScreen(window->win32.handle, (POINT*) &rect.right);
    SetWindowLongW(window->win32.handle, GWL_STYLE, style);
    SetWindowPos(window->win32.handle, HWND_TOP,
                 rect.left, rect.top,
                 rect.right - rect.left, rect.bottom - rect.top,
                 SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER);
}

// Translates a GLFW standard cursor to a resource ID
//
static LPWSTR translateCursorShape(int shape)
{
    switch (shape)
    {
        case GLFW_ARROW_CURSOR:
            return IDC_ARROW;
        case GLFW_IBEAM_CURSOR:
            return IDC_IBEAM;
        case GLFW_CROSSHAIR_CURSOR:
            return IDC_CROSS;
        case GLFW_HAND_CURSOR:
            return IDC_HAND;
        case GLFW_HRESIZE_CURSOR:
            return IDC_SIZEWE;
        case GLFW_VRESIZE_CURSOR:
            return IDC_SIZENS;
    }

    return NULL;
}

// Retrieves and translates modifier keys
//
static int getKeyMods(void)
{
    int mods = 0;

    if (GetKeyState(VK_SHIFT) & (1 << 31))
        mods |= GLFW_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & (1 << 31))
        mods |= GLFW_MOD_CONTROL;
    if (GetKeyState(VK_MENU) & (1 << 31))
        mods |= GLFW_MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & (1 << 31))
        mods |= GLFW_MOD_SUPER;

    return mods;
}

// Retrieves and translates modifier keys
//
static int getAsyncKeyMods(void)
{
    int mods = 0;

    if (GetAsyncKeyState(VK_SHIFT) & (1 << 31))
        mods |= GLFW_MOD_SHIFT;
    if (GetAsyncKeyState(VK_CONTROL) & (1 << 31))
        mods |= GLFW_MOD_CONTROL;
    if (GetAsyncKeyState(VK_MENU) & (1 << 31))
        mods |= GLFW_MOD_ALT;
    if ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & (1 << 31))
        mods |= GLFW_MOD_SUPER;

    return mods;
}

// Translates a Windows key to the corresponding GLFW key
//
static int translateKey(WPARAM wParam, LPARAM lParam)
{
    if (wParam == VK_CONTROL)
    {
        // The CTRL keys require special handling

        MSG next;
        DWORD time;

        // Is this an extended key (i.e. right key)?
        if (lParam & 0x01000000)
            return GLFW_KEY_RIGHT_CONTROL;

        // Here is a trick: "Alt Gr" sends LCTRL, then RALT. We only
        // want the RALT message, so we try to see if the next message
        // is a RALT message. In that case, this is a false LCTRL!
        time = GetMessageTime();

        if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
        {
            if (next.message == WM_KEYDOWN ||
                next.message == WM_SYSKEYDOWN ||
                next.message == WM_KEYUP ||
                next.message == WM_SYSKEYUP)
            {
                if (next.wParam == VK_MENU &&
                    (next.lParam & 0x01000000) &&
                    next.time == time)
                {
                    // Next message is a RALT down message, which
                    // means that this is not a proper LCTRL message
                    return _GLFW_KEY_INVALID;
                }
            }
        }

        return GLFW_KEY_LEFT_CONTROL;
    }

    if (wParam == VK_PROCESSKEY)
    {
        // IME notifies that keys have been filtered by setting the virtual
        // key-code to VK_PROCESSKEY
        return _GLFW_KEY_INVALID;
    }

    return _glfw.win32.keycodes[HIWORD(lParam) & 0x1FF];
}

// Make the specified window and its video mode active on its monitor
//
static GLFWbool acquireMonitor(_GLFWwindow* window)
{
    GLFWvidmode mode;
    GLFWbool status;
    int xpos, ypos;

    status = _glfwSetVideoModeWin32(window->monitor, &window->videoMode);

    _glfwPlatformGetVideoMode(window->monitor, &mode);
    _glfwPlatformGetMonitorPos(window->monitor, &xpos, &ypos);

    SetWindowPos(window->win32.handle, HWND_TOPMOST,
                 xpos, ypos, mode.width, mode.height,
                 SWP_NOACTIVATE | SWP_NOCOPYBITS);

    _glfwInputMonitorWindowChange(window->monitor, window);
    return status;
}

// Remove the window and restore the original video mode
//
static void releaseMonitor(_GLFWwindow* window)
{
    if (window->monitor->window != window)
        return;

    _glfwInputMonitorWindowChange(window->monitor, NULL);
    _glfwRestoreVideoModeWin32(window->monitor);
}

// Window callback function (handles window messages)
//
static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg,
                                   WPARAM wParam, LPARAM lParam)
{
    _GLFWwindow* window = GetPropW(hWnd, L"GLFW");
    if (!window)
    {
        // This is the message handling for the hidden helper window

        switch (uMsg)
        {
            case WM_DEVICECHANGE:
            {
                if (wParam == DBT_DEVNODES_CHANGED)
                {
                    _glfwPollMonitorsWin32();
                    return TRUE;
                }
                else if (wParam == DBT_DEVICEARRIVAL)
                {
                    DEV_BROADCAST_HDR* dbh = (DEV_BROADCAST_HDR*) lParam;
                    if (dbh)
                    {
                        if (dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                            _glfwDetectJoystickConnectionWin32();
                    }
                }
                else if (wParam == DBT_DEVICEREMOVECOMPLETE)
                {
                    DEV_BROADCAST_HDR* dbh = (DEV_BROADCAST_HDR*) lParam;
                    if (dbh)
                    {
                        if (dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                            _glfwDetectJoystickDisconnectionWin32();
                    }
                }

                break;
            }
        }

        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
        case WM_SETFOCUS:
        {
            _glfwInputWindowFocus(window, GLFW_TRUE);

            if (window->cursorMode == GLFW_CURSOR_DISABLED)
                _glfwPlatformSetCursorMode(window, GLFW_CURSOR_DISABLED);

            return 0;
        }

        case WM_KILLFOCUS:
        {
            if (window->cursorMode == GLFW_CURSOR_DISABLED)
                _glfwPlatformSetCursorMode(window, GLFW_CURSOR_NORMAL);

            if (window->monitor && window->autoIconify)
                _glfwPlatformIconifyWindow(window);

            _glfwInputWindowFocus(window, GLFW_FALSE);
            return 0;
        }

        case WM_SYSCOMMAND:
        {
            switch (wParam & 0xfff0)
            {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                {
                    if (window->monitor)
                    {
                        // We are running in full screen mode, so disallow
                        // screen saver and screen blanking
                        return 0;
                    }
                    else
                        break;
                }

                // User trying to access application menu using ALT?
                case SC_KEYMENU:
                    return 0;
            }
            break;
        }

        case WM_CLOSE:
        {
            _glfwInputWindowCloseRequest(window);
            return 0;
        }

        case WM_CHAR:
        case WM_SYSCHAR:
        case WM_UNICHAR:
        {
            const GLFWbool plain = (uMsg != WM_SYSCHAR);

            if (uMsg == WM_UNICHAR && wParam == UNICODE_NOCHAR)
            {
                // WM_UNICHAR is not sent by Windows, but is sent by some
                // third-party input method engine
                // Returning TRUE here announces support for this message
                return TRUE;
            }

            _glfwInputChar(window, (unsigned int) wParam, getKeyMods(), plain);
            return 0;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            const int key = translateKey(wParam, lParam);
            const int scancode = (lParam >> 16) & 0x1ff;
            const int action = ((lParam >> 31) & 1) ? GLFW_RELEASE : GLFW_PRESS;
            const int mods = getKeyMods();

            if (key == _GLFW_KEY_INVALID)
                break;

            if (action == GLFW_RELEASE && wParam == VK_SHIFT)
            {
                // Release both Shift keys on Shift up event, as only one event
                // is sent even if both keys are released
                _glfwInputKey(window, GLFW_KEY_LEFT_SHIFT, scancode, action, mods);
                _glfwInputKey(window, GLFW_KEY_RIGHT_SHIFT, scancode, action, mods);
            }
            else if (wParam == VK_SNAPSHOT)
            {
                // Key down is not reported for the Print Screen key
                _glfwInputKey(window, key, scancode, GLFW_PRESS, mods);
                _glfwInputKey(window, key, scancode, GLFW_RELEASE, mods);
            }
            else
                _glfwInputKey(window, key, scancode, action, mods);

            break;
        }

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            int button, action;

            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
                button = GLFW_MOUSE_BUTTON_LEFT;
            else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
                button = GLFW_MOUSE_BUTTON_RIGHT;
            else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
                button = GLFW_MOUSE_BUTTON_MIDDLE;
            else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                button = GLFW_MOUSE_BUTTON_4;
            else
                button = GLFW_MOUSE_BUTTON_5;

            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN ||
                uMsg == WM_MBUTTONDOWN || uMsg == WM_XBUTTONDOWN)
            {
                action = GLFW_PRESS;
                SetCapture(hWnd);
            }
            else
            {
                action = GLFW_RELEASE;
                ReleaseCapture();
            }

            _glfwInputMouseClick(window, button, action, getKeyMods());

            if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP)
                return TRUE;

            return 0;
        }

        case WM_MOUSEMOVE:
        {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            if (window->cursorMode == GLFW_CURSOR_DISABLED)
            {
                const int dx = x - window->win32.lastCursorPosX;
                const int dy = y - window->win32.lastCursorPosY;

                if (_glfw.win32.disabledCursorWindow != window)
                    break;

                _glfwInputCursorPos(window,
                                    window->virtualCursorPosX + dx,
                                    window->virtualCursorPosY + dy);
            }
            else
                _glfwInputCursorPos(window, x, y);

            window->win32.lastCursorPosX = x;
            window->win32.lastCursorPosY = y;

            if (!window->win32.cursorTracked)
            {
                TRACKMOUSEEVENT tme;
                ZeroMemory(&tme, sizeof(tme));
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = window->win32.handle;
                TrackMouseEvent(&tme);

                window->win32.cursorTracked = GLFW_TRUE;
                _glfwInputCursorEnter(window, GLFW_TRUE);
            }

            return 0;
        }

        case WM_MOUSELEAVE:
        {
            window->win32.cursorTracked = GLFW_FALSE;
            _glfwInputCursorEnter(window, GLFW_FALSE);
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            _glfwInputScroll(window, 0.0, (SHORT) HIWORD(wParam) / (double) WHEEL_DELTA);
            return 0;
        }

        case WM_MOUSEHWHEEL:
        {
            // This message is only sent on Windows Vista and later
            // NOTE: The X-axis is inverted for consistency with macOS and X11
            _glfwInputScroll(window, -((SHORT) HIWORD(wParam) / (double) WHEEL_DELTA), 0.0);
            return 0;
        }

        case WM_ENTERSIZEMOVE:
        case WM_ENTERMENULOOP:
        {
            if (window->cursorMode == GLFW_CURSOR_DISABLED)
                _glfwPlatformSetCursorMode(window, GLFW_CURSOR_NORMAL);

            break;
        }

        case WM_EXITSIZEMOVE:
        case WM_EXITMENULOOP:
        {
            if (window->cursorMode == GLFW_CURSOR_DISABLED)
                _glfwPlatformSetCursorMode(window, GLFW_CURSOR_DISABLED);

            break;
        }

        case WM_SIZE:
        {
            const GLFWbool iconified = wParam == SIZE_MINIMIZED;
            const GLFWbool maximized = wParam == SIZE_MAXIMIZED ||
                                       (window->win32.maximized &&
                                        wParam != SIZE_RESTORED);

            if (_glfw.win32.disabledCursorWindow == window)
                updateClipRect(window);

            if (window->win32.iconified != iconified)
                _glfwInputWindowIconify(window, iconified);

            if (window->win32.maximized != maximized)
                _glfwInputWindowMaximize(window, maximized);

            _glfwInputFramebufferSize(window, LOWORD(lParam), HIWORD(lParam));
            _glfwInputWindowSize(window, LOWORD(lParam), HIWORD(lParam));

            if (window->monitor && window->win32.iconified != iconified)
            {
                if (iconified)
                    releaseMonitor(window);
                else
                    acquireMonitor(window);
            }

            window->win32.iconified = iconified;
            window->win32.maximized = maximized;
            return 0;
        }

        case WM_MOVE:
        {
            if (_glfw.win32.disabledCursorWindow == window)
                updateClipRect(window);

            // NOTE: This cannot use LOWORD/HIWORD recommended by MSDN, as
            // those macros do not handle negative window positions correctly
            _glfwInputWindowPos(window,
                                GET_X_LPARAM(lParam),
                                GET_Y_LPARAM(lParam));
            return 0;
        }

        case WM_SIZING:
        {
            if (window->numer == GLFW_DONT_CARE ||
                window->denom == GLFW_DONT_CARE)
            {
                break;
            }

            applyAspectRatio(window, (int) wParam, (RECT*) lParam);
            return TRUE;
        }

        case WM_GETMINMAXINFO:
        {
            int xoff, yoff;
            MINMAXINFO* mmi = (MINMAXINFO*) lParam;

            if (window->monitor)
                break;

            getFullWindowSize(getWindowStyle(window), getWindowExStyle(window),
                              0, 0, &xoff, &yoff);

            if (window->minwidth != GLFW_DONT_CARE &&
                window->minheight != GLFW_DONT_CARE)
            {
                mmi->ptMinTrackSize.x = window->minwidth + xoff;
                mmi->ptMinTrackSize.y = window->minheight + yoff;
            }

            if (window->maxwidth != GLFW_DONT_CARE &&
                window->maxheight != GLFW_DONT_CARE)
            {
                mmi->ptMaxTrackSize.x = window->maxwidth + xoff;
                mmi->ptMaxTrackSize.y = window->maxheight + yoff;
            }

            return 0;
        }

        case WM_PAINT:
        {
            _glfwInputWindowDamage(window);
            break;
        }

        case WM_ERASEBKGND:
        {
            return TRUE;
        }

        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT)
            {
                updateCursorImage(window);
                return TRUE;
            }

            break;
        }

        case WM_DPICHANGED:
        {
            RECT* rect = (RECT*) lParam;
            SetWindowPos(window->win32.handle,
                         HWND_TOP,
                         rect->left,
                         rect->top,
                         rect->right - rect->left,
                         rect->bottom - rect->top,
                         SWP_NOACTIVATE | SWP_NOZORDER);
            break;
        }

        case WM_DROPFILES:
        {
            HDROP drop = (HDROP) wParam;
            POINT pt;
            int i;

            const int count = DragQueryFileW(drop, 0xffffffff, NULL, 0);
            char** paths = calloc(count, sizeof(char*));

            // Move the mouse to the position of the drop
            DragQueryPoint(drop, &pt);
            _glfwInputCursorPos(window, pt.x, pt.y);

            for (i = 0;  i < count;  i++)
            {
                const UINT length = DragQueryFileW(drop, i, NULL, 0);
                WCHAR* buffer = calloc(length + 1, sizeof(WCHAR));

                DragQueryFileW(drop, i, buffer, length + 1);
                paths[i] = _glfwCreateUTF8FromWideStringWin32(buffer);

                free(buffer);
            }

            _glfwInputDrop(window, count, (const char**) paths);

            for (i = 0;  i < count;  i++)
                free(paths[i]);
            free(paths);

            DragFinish(drop);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// Creates the GLFW window
//
static int createNativeWindow(_GLFWwindow* window,
                              const _GLFWwndconfig* wndconfig)
{
    int xpos, ypos, fullWidth, fullHeight;
    WCHAR* wideTitle;
    DWORD style = getWindowStyle(window);
    DWORD exStyle = getWindowExStyle(window);

    if (window->monitor)
    {
        GLFWvidmode mode;

        // NOTE: This window placement is temporary and approximate, as the
        //       correct position and size cannot be known until the monitor
        //       video mode has been set
        _glfwPlatformGetMonitorPos(window->monitor, &xpos, &ypos);
        _glfwPlatformGetVideoMode(window->monitor, &mode);
        fullWidth  = mode.width;
        fullHeight = mode.height;
    }
    else
    {
        xpos = CW_USEDEFAULT;
        ypos = CW_USEDEFAULT;

        if (wndconfig->maximized)
            style |= WS_MAXIMIZE;

        getFullWindowSize(style, exStyle,
                          wndconfig->width, wndconfig->height,
                          &fullWidth, &fullHeight);
    }

    wideTitle = _glfwCreateWideStringFromUTF8Win32(wndconfig->title);
    if (!wideTitle)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to convert window title to UTF-16");
        return GLFW_FALSE;
    }

    window->win32.handle = CreateWindowExW(exStyle,
                                           _GLFW_WNDCLASSNAME,
                                           wideTitle,
                                           style,
                                           xpos, ypos,
                                           fullWidth, fullHeight,
                                           NULL, // No parent window
                                           NULL, // No window menu
                                           GetModuleHandleW(NULL),
                                           NULL);

    free(wideTitle);

    if (!window->win32.handle)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to create window");
        return GLFW_FALSE;
    }

    SetPropW(window->win32.handle, L"GLFW", window);

    if (_glfw_ChangeWindowMessageFilterEx)
    {
        _glfw_ChangeWindowMessageFilterEx(window->win32.handle,
                                          WM_DROPFILES, MSGFLT_ALLOW, NULL);
        _glfw_ChangeWindowMessageFilterEx(window->win32.handle,
                                          WM_COPYDATA, MSGFLT_ALLOW, NULL);
        _glfw_ChangeWindowMessageFilterEx(window->win32.handle,
                                          WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL);
    }

    DragAcceptFiles(window->win32.handle, TRUE);

    return GLFW_TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

// Registers the GLFW window class
//
GLFWbool _glfwRegisterWindowClassWin32(void)
{
    WNDCLASSEXW wc;

    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = (WNDPROC) windowProc;
    wc.hInstance     = GetModuleHandleW(NULL);
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = _GLFW_WNDCLASSNAME;

    // Load user-provided icon if available
    wc.hIcon = LoadImageW(GetModuleHandleW(NULL),
                          L"GLFW_ICON", IMAGE_ICON,
                          0, 0, LR_DEFAULTSIZE | LR_SHARED);
    if (!wc.hIcon)
    {
        // No user-provided icon found, load default icon
        wc.hIcon = LoadImageW(NULL,
                              IDI_APPLICATION, IMAGE_ICON,
                              0, 0, LR_DEFAULTSIZE | LR_SHARED);
    }

    if (!RegisterClassExW(&wc))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to register window class");
        return GLFW_FALSE;
    }

    return GLFW_TRUE;
}

// Unregisters the GLFW window class
//
void _glfwUnregisterWindowClassWin32(void)
{
    UnregisterClassW(_GLFW_WNDCLASSNAME, GetModuleHandleW(NULL));
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformCreateWindow(_GLFWwindow* window,
                              const _GLFWwndconfig* wndconfig,
                              const _GLFWctxconfig* ctxconfig,
                              const _GLFWfbconfig* fbconfig)
{
    if (!createNativeWindow(window, wndconfig))
        return GLFW_FALSE;

    if (ctxconfig->client != GLFW_NO_API)
    {
        if (ctxconfig->source == GLFW_NATIVE_CONTEXT_API)
        {
            if (!_glfwInitWGL())
                return GLFW_FALSE;
            if (!_glfwCreateContextWGL(window, ctxconfig, fbconfig))
                return GLFW_FALSE;
        }
        else
        {
            if (!_glfwInitEGL())
                return GLFW_FALSE;
            if (!_glfwCreateContextEGL(window, ctxconfig, fbconfig))
                return GLFW_FALSE;
        }
    }

    if (window->monitor)
    {
        _glfwPlatformShowWindow(window);
        _glfwPlatformFocusWindow(window);
        if (!acquireMonitor(window))
            return GLFW_FALSE;

        centerCursor(window);
    }

    return GLFW_TRUE;
}

void _glfwPlatformDestroyWindow(_GLFWwindow* window)
{
    if (window->monitor)
        releaseMonitor(window);

    if (window->context.destroy)
        window->context.destroy(window);

    if (_glfw.win32.disabledCursorWindow == window)
        _glfw.win32.disabledCursorWindow = NULL;

    if (window->win32.handle)
    {
        RemovePropW(window->win32.handle, L"GLFW");
        DestroyWindow(window->win32.handle);
        window->win32.handle = NULL;
    }

    if (window->win32.bigIcon)
        DestroyIcon(window->win32.bigIcon);

    if (window->win32.smallIcon)
        DestroyIcon(window->win32.smallIcon);
}

void _glfwPlatformSetWindowTitle(_GLFWwindow* window, const char* title)
{
    WCHAR* wideTitle = _glfwCreateWideStringFromUTF8Win32(title);
    if (!wideTitle)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to convert window title to UTF-16");
        return;
    }

    SetWindowTextW(window->win32.handle, wideTitle);
    free(wideTitle);
}

void _glfwPlatformSetWindowIcon(_GLFWwindow* window,
                                int count, const GLFWimage* images)
{
    HICON bigIcon = NULL, smallIcon = NULL;

    if (count)
    {
        const GLFWimage* bigImage = chooseImage(count, images,
                                                GetSystemMetrics(SM_CXICON),
                                                GetSystemMetrics(SM_CYICON));
        const GLFWimage* smallImage = chooseImage(count, images,
                                                  GetSystemMetrics(SM_CXSMICON),
                                                  GetSystemMetrics(SM_CYSMICON));

        bigIcon = createIcon(bigImage, 0, 0, GLFW_TRUE);
        smallIcon = createIcon(smallImage, 0, 0, GLFW_TRUE);
    }
    else
    {
        bigIcon = (HICON) GetClassLongPtrW(window->win32.handle, GCLP_HICON);
        smallIcon = (HICON) GetClassLongPtrW(window->win32.handle, GCLP_HICONSM);
    }

    SendMessage(window->win32.handle, WM_SETICON, ICON_BIG, (LPARAM) bigIcon);
    SendMessage(window->win32.handle, WM_SETICON, ICON_SMALL, (LPARAM) smallIcon);

    if (window->win32.bigIcon)
        DestroyIcon(window->win32.bigIcon);

    if (window->win32.smallIcon)
        DestroyIcon(window->win32.smallIcon);

    if (count)
    {
        window->win32.bigIcon = bigIcon;
        window->win32.smallIcon = smallIcon;
    }
}

void _glfwPlatformGetWindowPos(_GLFWwindow* window, int* xpos, int* ypos)
{
    POINT pos = { 0, 0 };
    ClientToScreen(window->win32.handle, &pos);

    if (xpos)
        *xpos = pos.x;
    if (ypos)
        *ypos = pos.y;
}

void _glfwPlatformSetWindowPos(_GLFWwindow* window, int xpos, int ypos)
{
    RECT rect = { xpos, ypos, xpos, ypos };
    AdjustWindowRectEx(&rect, getWindowStyle(window),
                       FALSE, getWindowExStyle(window));
    SetWindowPos(window->win32.handle, NULL, rect.left, rect.top, 0, 0,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

void _glfwPlatformGetWindowSize(_GLFWwindow* window, int* width, int* height)
{
    RECT area;
    GetClientRect(window->win32.handle, &area);

    if (width)
        *width = area.right;
    if (height)
        *height = area.bottom;
}

void _glfwPlatformSetWindowSize(_GLFWwindow* window, int width, int height)
{
    if (window->monitor)
    {
        if (window->monitor->window == window)
            acquireMonitor(window);
    }
    else
    {
        RECT rect = { 0, 0, width, height };
        AdjustWindowRectEx(&rect, getWindowStyle(window),
                           FALSE, getWindowExStyle(window));
        SetWindowPos(window->win32.handle, HWND_TOP,
                     0, 0, rect.right - rect.left, rect.bottom - rect.top,
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
    }
}

void _glfwPlatformSetWindowSizeLimits(_GLFWwindow* window,
                                      int minwidth, int minheight,
                                      int maxwidth, int maxheight)
{
    RECT area;

    if ((minwidth == GLFW_DONT_CARE || minheight == GLFW_DONT_CARE) &&
        (maxwidth == GLFW_DONT_CARE || maxheight == GLFW_DONT_CARE))
    {
        return;
    }

    GetWindowRect(window->win32.handle, &area);
    MoveWindow(window->win32.handle,
               area.left, area.top,
               area.right - area.left,
               area.bottom - area.top, TRUE);
}

void _glfwPlatformSetWindowAspectRatio(_GLFWwindow* window, int numer, int denom)
{
    RECT area;

    if (numer == GLFW_DONT_CARE || denom == GLFW_DONT_CARE)
        return;

    GetWindowRect(window->win32.handle, &area);
    applyAspectRatio(window, WMSZ_BOTTOMRIGHT, &area);
    MoveWindow(window->win32.handle,
               area.left, area.top,
               area.right - area.left,
               area.bottom - area.top, TRUE);
}

void _glfwPlatformGetFramebufferSize(_GLFWwindow* window, int* width, int* height)
{
    _glfwPlatformGetWindowSize(window, width, height);
}

void _glfwPlatformGetWindowFrameSize(_GLFWwindow* window,
                                     int* left, int* top,
                                     int* right, int* bottom)
{
    RECT rect;
    int width, height;

    _glfwPlatformGetWindowSize(window, &width, &height);
    SetRect(&rect, 0, 0, width, height);
    AdjustWindowRectEx(&rect, getWindowStyle(window),
                       FALSE, getWindowExStyle(window));

    if (left)
        *left = -rect.left;
    if (top)
        *top = -rect.top;
    if (right)
        *right = rect.right - width;
    if (bottom)
        *bottom = rect.bottom - height;
}

void _glfwPlatformIconifyWindow(_GLFWwindow* window)
{
    ShowWindow(window->win32.handle, SW_MINIMIZE);
}

void _glfwPlatformRestoreWindow(_GLFWwindow* window)
{
    ShowWindow(window->win32.handle, SW_RESTORE);
}

void _glfwPlatformMaximizeWindow(_GLFWwindow* window)
{
    ShowWindow(window->win32.handle, SW_MAXIMIZE);
}

void _glfwPlatformShowWindow(_GLFWwindow* window)
{
    ShowWindow(window->win32.handle, SW_SHOW);
}

void _glfwPlatformHideWindow(_GLFWwindow* window)
{
    ShowWindow(window->win32.handle, SW_HIDE);
}

void _glfwPlatformFocusWindow(_GLFWwindow* window)
{
    BringWindowToTop(window->win32.handle);
    SetForegroundWindow(window->win32.handle);
    SetFocus(window->win32.handle);
}

void _glfwPlatformSetWindowMonitor(_GLFWwindow* window,
                                   _GLFWmonitor* monitor,
                                   int xpos, int ypos,
                                   int width, int height,
                                   int refreshRate)
{
    if (window->monitor == monitor)
    {
        if (monitor)
        {
            if (monitor->window == window)
                acquireMonitor(window);
        }
        else
        {
            RECT rect = { xpos, ypos, xpos + width, ypos + height };
            AdjustWindowRectEx(&rect, getWindowStyle(window),
                               FALSE, getWindowExStyle(window));
            SetWindowPos(window->win32.handle, HWND_TOP,
                         rect.left, rect.top,
                         rect.right - rect.left, rect.bottom - rect.top,
                         SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER);
        }

        return;
    }

    if (window->monitor)
        releaseMonitor(window);

    _glfwInputWindowMonitorChange(window, monitor);

    if (monitor)
    {
        GLFWvidmode mode;
        DWORD style = GetWindowLongW(window->win32.handle, GWL_STYLE);
        UINT flags = SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOCOPYBITS;

        if (window->decorated)
        {
            style &= ~WS_OVERLAPPEDWINDOW;
            style |= getWindowStyle(window);
            SetWindowLongW(window->win32.handle, GWL_STYLE, style);

            flags |= SWP_FRAMECHANGED;
        }

        _glfwPlatformGetVideoMode(monitor, &mode);
        _glfwPlatformGetMonitorPos(monitor, &xpos, &ypos);

        SetWindowPos(window->win32.handle, HWND_TOPMOST,
                     xpos, ypos, mode.width, mode.height,
                     flags);

        acquireMonitor(window);
    }
    else
    {
        HWND after;
        RECT rect = { xpos, ypos, xpos + width, ypos + height };
        DWORD style = GetWindowLongW(window->win32.handle, GWL_STYLE);
        UINT flags = SWP_NOACTIVATE | SWP_NOCOPYBITS;

        if (window->decorated)
        {
            style &= ~WS_POPUP;
            style |= getWindowStyle(window);
            SetWindowLongW(window->win32.handle, GWL_STYLE, style);

            flags |= SWP_FRAMECHANGED;
        }

        if (window->floating)
            after = HWND_TOPMOST;
        else
            after = HWND_NOTOPMOST;

        AdjustWindowRectEx(&rect, getWindowStyle(window),
                           FALSE, getWindowExStyle(window));
        SetWindowPos(window->win32.handle, after,
                     rect.left, rect.top,
                     rect.right - rect.left, rect.bottom - rect.top,
                     flags);
    }
}

int _glfwPlatformWindowFocused(_GLFWwindow* window)
{
    return window->win32.handle == GetActiveWindow();
}

int _glfwPlatformWindowIconified(_GLFWwindow* window)
{
    return IsIconic(window->win32.handle);
}

int _glfwPlatformWindowVisible(_GLFWwindow* window)
{
    return IsWindowVisible(window->win32.handle);
}

int _glfwPlatformWindowMaximized(_GLFWwindow* window)
{
    return IsZoomed(window->win32.handle);
}

void _glfwPlatformSetWindowResizable(_GLFWwindow* window, GLFWbool enabled)
{
    updateWindowStyles(window);
}

void _glfwPlatformSetWindowDecorated(_GLFWwindow* window, GLFWbool enabled)
{
    updateWindowStyles(window);
}

void _glfwPlatformSetWindowFloating(_GLFWwindow* window, GLFWbool enabled)
{
    const HWND after = enabled ? HWND_TOPMOST : HWND_NOTOPMOST;
    SetWindowPos(window->win32.handle, after, 0, 0, 0, 0,
                 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

void _glfwPlatformPollEvents(void)
{
    MSG msg;
    HWND handle;
    _GLFWwindow* window;

    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            // Treat WM_QUIT as a close on all windows
            // While GLFW does not itself post WM_QUIT, other processes may post
            // it to this one, for example Task Manager

            window = _glfw.windowListHead;
            while (window)
            {
                _glfwInputWindowCloseRequest(window);
                window = window->next;
            }
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    handle = GetActiveWindow();
    if (handle)
    {
        // LSHIFT/RSHIFT fixup (keys tend to "stick" without this fix)
        // This is the only async event handling in GLFW, but it solves some
        // nasty problems
        window = GetPropW(handle, L"GLFW");
        if (window)
        {
            const int mods = getAsyncKeyMods();

            // Get current state of left and right shift keys
            const int lshiftDown = (GetAsyncKeyState(VK_LSHIFT) >> 15) & 1;
            const int rshiftDown = (GetAsyncKeyState(VK_RSHIFT) >> 15) & 1;

            // See if this differs from our belief of what has happened
            // (we only have to check for lost key up events)
            if (!lshiftDown && window->keys[GLFW_KEY_LEFT_SHIFT] == 1)
                _glfwInputKey(window, GLFW_KEY_LEFT_SHIFT, 0, GLFW_RELEASE, mods);

            if (!rshiftDown && window->keys[GLFW_KEY_RIGHT_SHIFT] == 1)
                _glfwInputKey(window, GLFW_KEY_RIGHT_SHIFT, 0, GLFW_RELEASE, mods);
        }
    }

    window = _glfw.win32.disabledCursorWindow;
    if (window)
    {
        int width, height;
        _glfwPlatformGetWindowSize(window, &width, &height);

        // NOTE: Re-center the cursor only if it has moved since the last call,
        //       to avoid breaking glfwWaitEvents with WM_MOUSEMOVE
        if (window->win32.lastCursorPosX != width / 2 ||
            window->win32.lastCursorPosY != height / 2)
        {
            _glfwPlatformSetCursorPos(window, width / 2, height / 2);
        }
    }
}

void _glfwPlatformWaitEvents(void)
{
    WaitMessage();

    _glfwPlatformPollEvents();
}

void _glfwPlatformWaitEventsTimeout(double timeout)
{
    MsgWaitForMultipleObjects(0, NULL, FALSE, (DWORD) (timeout * 1e3), QS_ALLEVENTS);

    _glfwPlatformPollEvents();
}

void _glfwPlatformPostEmptyEvent(void)
{
    PostMessage(_glfw.win32.helperWindowHandle, WM_NULL, 0, 0);
}

void _glfwPlatformGetCursorPos(_GLFWwindow* window, double* xpos, double* ypos)
{
    POINT pos;

    if (GetCursorPos(&pos))
    {
        ScreenToClient(window->win32.handle, &pos);

        if (xpos)
            *xpos = pos.x;
        if (ypos)
            *ypos = pos.y;
    }
}

void _glfwPlatformSetCursorPos(_GLFWwindow* window, double xpos, double ypos)
{
    POINT pos = { (int) xpos, (int) ypos };

    // Store the new position so it can be recognized later
    window->win32.lastCursorPosX = pos.x;
    window->win32.lastCursorPosY = pos.y;

    ClientToScreen(window->win32.handle, &pos);
    SetCursorPos(pos.x, pos.y);
}

void _glfwPlatformSetCursorMode(_GLFWwindow* window, int mode)
{
    if (mode == GLFW_CURSOR_DISABLED)
    {
        _glfw.win32.disabledCursorWindow = window;
        _glfwPlatformGetCursorPos(window,
                                  &_glfw.win32.restoreCursorPosX,
                                  &_glfw.win32.restoreCursorPosY);
        centerCursor(window);
        updateClipRect(window);
    }
    else if (_glfw.win32.disabledCursorWindow == window)
    {
        _glfw.win32.disabledCursorWindow = NULL;
        updateClipRect(NULL);
        _glfwPlatformSetCursorPos(window,
                                  _glfw.win32.restoreCursorPosX,
                                  _glfw.win32.restoreCursorPosY);
    }

    if (cursorInClientArea(window))
        updateCursorImage(window);
}

const char* _glfwPlatformGetKeyName(int key, int scancode)
{
    WCHAR name[16];

    if (key != GLFW_KEY_UNKNOWN)
        scancode = _glfw.win32.scancodes[key];

    if (!_glfwIsPrintable(_glfw.win32.keycodes[scancode]))
        return NULL;

    if (!GetKeyNameTextW(scancode << 16, name, sizeof(name) / sizeof(WCHAR)))
        return NULL;

    if (!WideCharToMultiByte(CP_UTF8, 0, name, -1,
                             _glfw.win32.keyName,
                             sizeof(_glfw.win32.keyName),
                             NULL, NULL))
    {
        return NULL;
    }

    return _glfw.win32.keyName;
}

int _glfwPlatformGetKeyScancode(int key)
{
    return _glfw.win32.scancodes[key];
}

int _glfwPlatformCreateCursor(_GLFWcursor* cursor,
                              const GLFWimage* image,
                              int xhot, int yhot)
{
    cursor->win32.handle = (HCURSOR) createIcon(image, xhot, yhot, GLFW_FALSE);
    if (!cursor->win32.handle)
        return GLFW_FALSE;

    return GLFW_TRUE;
}

int _glfwPlatformCreateStandardCursor(_GLFWcursor* cursor, int shape)
{
    cursor->win32.handle =
        CopyCursor(LoadCursorW(NULL, translateCursorShape(shape)));
    if (!cursor->win32.handle)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to create standard cursor");
        return GLFW_FALSE;
    }

    return GLFW_TRUE;
}

void _glfwPlatformDestroyCursor(_GLFWcursor* cursor)
{
    if (cursor->win32.handle)
        DestroyIcon((HICON) cursor->win32.handle);
}

void _glfwPlatformSetCursor(_GLFWwindow* window, _GLFWcursor* cursor)
{
    if (cursorInClientArea(window))
        updateCursorImage(window);
}

void _glfwPlatformSetClipboardString(_GLFWwindow* window, const char* string)
{
    int characterCount;
    HANDLE object;
    WCHAR* buffer;

    characterCount = MultiByteToWideChar(CP_UTF8, 0, string, -1, NULL, 0);
    if (!characterCount)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to convert clipboard string to UTF-16");
        return;
    }

    object = GlobalAlloc(GMEM_MOVEABLE, characterCount * sizeof(WCHAR));
    if (!object)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to allocate global handle for clipboard");
        return;
    }

    buffer = GlobalLock(object);
    if (!buffer)
    {
        GlobalFree(object);

        _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to lock global handle");
        return;
    }

    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, characterCount);
    GlobalUnlock(object);

    if (!OpenClipboard(_glfw.win32.helperWindowHandle))
    {
        GlobalFree(object);

        _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to open clipboard");
        return;
    }

    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, object);
    CloseClipboard();
}

const char* _glfwPlatformGetClipboardString(_GLFWwindow* window)
{
    HANDLE object;
    WCHAR* buffer;

    if (!OpenClipboard(_glfw.win32.helperWindowHandle))
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to open clipboard");
        return NULL;
    }

    object = GetClipboardData(CF_UNICODETEXT);
    if (!object)
    {
        CloseClipboard();

        _glfwInputError(GLFW_FORMAT_UNAVAILABLE,
                        "Win32: Failed to convert clipboard to string");
        return NULL;
    }

    buffer = GlobalLock(object);
    if (!buffer)
    {
        CloseClipboard();

        _glfwInputError(GLFW_PLATFORM_ERROR, "Win32: Failed to lock global handle");
        return NULL;
    }

    free(_glfw.win32.clipboardString);
    _glfw.win32.clipboardString =
        _glfwCreateUTF8FromWideStringWin32(buffer);

    GlobalUnlock(object);
    CloseClipboard();

    if (!_glfw.win32.clipboardString)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to convert wide string to UTF-8");
        return NULL;
    }

    return _glfw.win32.clipboardString;
}

void _glfwPlatformGetRequiredInstanceExtensions(char** extensions)
{
    if (!_glfw.vk.KHR_surface || !_glfw.vk.KHR_win32_surface)
        return;

    extensions[0] = "VK_KHR_surface";
    extensions[1] = "VK_KHR_win32_surface";
}

int _glfwPlatformGetPhysicalDevicePresentationSupport(VkInstance instance,
                                                      VkPhysicalDevice device,
                                                      uint32_t queuefamily)
{
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)
        vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
    if (!vkGetPhysicalDeviceWin32PresentationSupportKHR)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "Win32: Vulkan instance missing VK_KHR_win32_surface extension");
        return GLFW_FALSE;
    }

    return vkGetPhysicalDeviceWin32PresentationSupportKHR(device, queuefamily);
}

VkResult _glfwPlatformCreateWindowSurface(VkInstance instance,
                                          _GLFWwindow* window,
                                          const VkAllocationCallbacks* allocator,
                                          VkSurfaceKHR* surface)
{
    VkResult err;
    VkWin32SurfaceCreateInfoKHR sci;
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;

    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)
        vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
    if (!vkCreateWin32SurfaceKHR)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE,
                        "Win32: Vulkan instance missing VK_KHR_win32_surface extension");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    memset(&sci, 0, sizeof(sci));
    sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    sci.hinstance = GetModuleHandle(NULL);
    sci.hwnd = window->win32.handle;

    err = vkCreateWin32SurfaceKHR(instance, &sci, allocator, surface);
    if (err)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR,
                        "Win32: Failed to create Vulkan surface: %s",
                        _glfwGetVulkanResultString(err));
    }

    return err;
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW native API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI HWND glfwGetWin32Window(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return window->win32.handle;
}

//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
// Copyright (c) 2012 Torsten Walluhn <tw@mad-cad.net>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//#include "internal.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>


//////////////////////////////////////////////////////////////////////////
//////                         GLFW event API                       //////
//////////////////////////////////////////////////////////////////////////

void _glfwInputWindowFocus(_GLFWwindow* window, GLFWbool focused)
{
    if (focused)
    {
        if (window->callbacks.focus)
            window->callbacks.focus((GLFWwindow*) window, focused);
    }
    else
    {
        int i;

        if (window->callbacks.focus)
            window->callbacks.focus((GLFWwindow*) window, focused);

        // Release all pressed keyboard keys
        for (i = 0;  i <= GLFW_KEY_LAST;  i++)
        {
            if (window->keys[i] == GLFW_PRESS)
                _glfwInputKey(window, i, 0, GLFW_RELEASE, 0);
        }

        // Release all pressed mouse buttons
        for (i = 0;  i <= GLFW_MOUSE_BUTTON_LAST;  i++)
        {
            if (window->mouseButtons[i] == GLFW_PRESS)
                _glfwInputMouseClick(window, i, GLFW_RELEASE, 0);
        }
    }
}

void _glfwInputWindowPos(_GLFWwindow* window, int x, int y)
{
    if (window->callbacks.pos)
        window->callbacks.pos((GLFWwindow*) window, x, y);
}

void _glfwInputWindowSize(_GLFWwindow* window, int width, int height)
{
    if (window->callbacks.size)
        window->callbacks.size((GLFWwindow*) window, width, height);
}

void _glfwInputWindowIconify(_GLFWwindow* window, GLFWbool iconified)
{
    if (window->callbacks.iconify)
        window->callbacks.iconify((GLFWwindow*) window, iconified);
}

void _glfwInputWindowMaximize(_GLFWwindow* window, GLFWbool maximized)
{
    if (window->callbacks.maximize)
        window->callbacks.maximize((GLFWwindow*) window, maximized);
}

void _glfwInputFramebufferSize(_GLFWwindow* window, int width, int height)
{
    if (window->callbacks.fbsize)
        window->callbacks.fbsize((GLFWwindow*) window, width, height);
}

void _glfwInputWindowDamage(_GLFWwindow* window)
{
    if (window->callbacks.refresh)
        window->callbacks.refresh((GLFWwindow*) window);
}

void _glfwInputWindowCloseRequest(_GLFWwindow* window)
{
    window->shouldClose = GLFW_TRUE;

    if (window->callbacks.close)
        window->callbacks.close((GLFWwindow*) window);
}

void _glfwInputWindowMonitorChange(_GLFWwindow* window, _GLFWmonitor* monitor)
{
    window->monitor = monitor;
}


//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI GLFWwindow* glfwCreateWindow(int width, int height,
                                     const char* title,
                                     GLFWmonitor* monitor,
                                     GLFWwindow* share)
{
    _GLFWfbconfig fbconfig;
    _GLFWctxconfig ctxconfig;
    _GLFWwndconfig wndconfig;
    _GLFWwindow* window;
    _GLFWwindow* previous;

    assert(title != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);

    if (width <= 0 || height <= 0)
    {
        _glfwInputError(GLFW_INVALID_VALUE,
                        "Invalid window size %ix%i",
                        width, height);

        return NULL;
    }

    fbconfig  = _glfw.hints.framebuffer;
    ctxconfig = _glfw.hints.context;
    wndconfig = _glfw.hints.window;

    wndconfig.width   = width;
    wndconfig.height  = height;
    wndconfig.title   = title;
    ctxconfig.share   = (_GLFWwindow*) share;

    if (ctxconfig.share)
    {
        if (ctxconfig.client == GLFW_NO_API ||
            ctxconfig.share->context.client == GLFW_NO_API)
        {
            _glfwInputError(GLFW_NO_WINDOW_CONTEXT, NULL);
            return NULL;
        }
    }

    if (!_glfwIsValidContextConfig(&ctxconfig))
        return NULL;

    window = calloc(1, sizeof(_GLFWwindow));
    window->next = _glfw.windowListHead;
    _glfw.windowListHead = window;

    window->videoMode.width       = width;
    window->videoMode.height      = height;
    window->videoMode.redBits     = fbconfig.redBits;
    window->videoMode.greenBits   = fbconfig.greenBits;
    window->videoMode.blueBits    = fbconfig.blueBits;
    window->videoMode.refreshRate = _glfw.hints.refreshRate;

    window->monitor     = (_GLFWmonitor*) monitor;
    window->resizable   = wndconfig.resizable;
    window->decorated   = wndconfig.decorated;
    window->autoIconify = wndconfig.autoIconify;
    window->floating    = wndconfig.floating;
    window->cursorMode  = GLFW_CURSOR_NORMAL;

    window->minwidth    = GLFW_DONT_CARE;
    window->minheight   = GLFW_DONT_CARE;
    window->maxwidth    = GLFW_DONT_CARE;
    window->maxheight   = GLFW_DONT_CARE;
    window->numer       = GLFW_DONT_CARE;
    window->denom       = GLFW_DONT_CARE;

    // Save the currently current context so it can be restored later
    previous = _glfwPlatformGetCurrentContext();
    if (ctxconfig.client != GLFW_NO_API)
        glfwMakeContextCurrent(NULL);

    // Open the actual window and create its context
    if (!_glfwPlatformCreateWindow(window, &wndconfig, &ctxconfig, &fbconfig))
    {
        glfwMakeContextCurrent((GLFWwindow*) previous);
        glfwDestroyWindow((GLFWwindow*) window);
        return NULL;
    }

    if (ctxconfig.client != GLFW_NO_API)
    {
        window->context.makeCurrent(window);

        // Retrieve the actual (as opposed to requested) context attributes
        if (!_glfwRefreshContextAttribs(&ctxconfig))
        {
            glfwMakeContextCurrent((GLFWwindow*) previous);
            glfwDestroyWindow((GLFWwindow*) window);
            return NULL;
        }

        // Restore the previously current context (or NULL)
        glfwMakeContextCurrent((GLFWwindow*) previous);
    }

    if (!window->monitor)
    {
        if (wndconfig.visible)
        {
            _glfwPlatformShowWindow(window);
            if (wndconfig.focused)
                _glfwPlatformFocusWindow(window);
        }
    }

    return (GLFWwindow*) window;
}

void glfwDefaultWindowHints(void)
{
    _GLFW_REQUIRE_INIT();

    memset(&_glfw.hints, 0, sizeof(_glfw.hints));

    // The default is OpenGL with minimum version 1.0
    _glfw.hints.context.client = GLFW_OPENGL_API;
    _glfw.hints.context.source = GLFW_NATIVE_CONTEXT_API;
    _glfw.hints.context.major  = 1;
    _glfw.hints.context.minor  = 0;

    // The default is a focused, visible, resizable window with decorations
    _glfw.hints.window.resizable   = GLFW_TRUE;
    _glfw.hints.window.visible     = GLFW_TRUE;
    _glfw.hints.window.decorated   = GLFW_TRUE;
    _glfw.hints.window.focused     = GLFW_TRUE;
    _glfw.hints.window.autoIconify = GLFW_TRUE;

    // The default is 24 bits of color, 24 bits of depth and 8 bits of stencil,
    // double buffered
    _glfw.hints.framebuffer.redBits      = 8;
    _glfw.hints.framebuffer.greenBits    = 8;
    _glfw.hints.framebuffer.blueBits     = 8;
    _glfw.hints.framebuffer.alphaBits    = 8;
    _glfw.hints.framebuffer.depthBits    = 24;
    _glfw.hints.framebuffer.stencilBits  = 8;
    _glfw.hints.framebuffer.doublebuffer = GLFW_TRUE;

    // The default is to select the highest available refresh rate
    _glfw.hints.refreshRate = GLFW_DONT_CARE;

    // The default is to use full Retina resolution framebuffers
    _glfw.hints.window.ns.retina = GLFW_TRUE;
}

GLFWAPI void glfwWindowHint(int hint, int value)
{
    _GLFW_REQUIRE_INIT();

    switch (hint)
    {
        case GLFW_RED_BITS:
            _glfw.hints.framebuffer.redBits = value;
            break;
        case GLFW_GREEN_BITS:
            _glfw.hints.framebuffer.greenBits = value;
            break;
        case GLFW_BLUE_BITS:
            _glfw.hints.framebuffer.blueBits = value;
            break;
        case GLFW_ALPHA_BITS:
            _glfw.hints.framebuffer.alphaBits = value;
            break;
        case GLFW_DEPTH_BITS:
            _glfw.hints.framebuffer.depthBits = value;
            break;
        case GLFW_STENCIL_BITS:
            _glfw.hints.framebuffer.stencilBits = value;
            break;
        case GLFW_ACCUM_RED_BITS:
            _glfw.hints.framebuffer.accumRedBits = value;
            break;
        case GLFW_ACCUM_GREEN_BITS:
            _glfw.hints.framebuffer.accumGreenBits = value;
            break;
        case GLFW_ACCUM_BLUE_BITS:
            _glfw.hints.framebuffer.accumBlueBits = value;
            break;
        case GLFW_ACCUM_ALPHA_BITS:
            _glfw.hints.framebuffer.accumAlphaBits = value;
            break;
        case GLFW_AUX_BUFFERS:
            _glfw.hints.framebuffer.auxBuffers = value;
            break;
        case GLFW_STEREO:
            _glfw.hints.framebuffer.stereo = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_DOUBLEBUFFER:
            _glfw.hints.framebuffer.doublebuffer = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_SAMPLES:
            _glfw.hints.framebuffer.samples = value;
            break;
        case GLFW_SRGB_CAPABLE:
            _glfw.hints.framebuffer.sRGB = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_RESIZABLE:
            _glfw.hints.window.resizable = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_DECORATED:
            _glfw.hints.window.decorated = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_FOCUSED:
            _glfw.hints.window.focused = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_AUTO_ICONIFY:
            _glfw.hints.window.autoIconify = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_FLOATING:
            _glfw.hints.window.floating = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_MAXIMIZED:
            _glfw.hints.window.maximized = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_VISIBLE:
            _glfw.hints.window.visible = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_COCOA_RETINA_FRAMEBUFFER:
            _glfw.hints.window.ns.retina = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_COCOA_FRAME_AUTOSAVE:
            _glfw.hints.window.ns.frame = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_CLIENT_API:
            _glfw.hints.context.client = value;
            break;
        case GLFW_CONTEXT_CREATION_API:
            _glfw.hints.context.source = value;
            break;
        case GLFW_CONTEXT_VERSION_MAJOR:
            _glfw.hints.context.major = value;
            break;
        case GLFW_CONTEXT_VERSION_MINOR:
            _glfw.hints.context.minor = value;
            break;
        case GLFW_CONTEXT_ROBUSTNESS:
            _glfw.hints.context.robustness = value;
            break;
        case GLFW_OPENGL_FORWARD_COMPAT:
            _glfw.hints.context.forward = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_OPENGL_DEBUG_CONTEXT:
            _glfw.hints.context.debug = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_CONTEXT_NO_ERROR:
            _glfw.hints.context.noerror = value ? GLFW_TRUE : GLFW_FALSE;
            break;
        case GLFW_OPENGL_PROFILE:
            _glfw.hints.context.profile = value;
            break;
        case GLFW_CONTEXT_RELEASE_BEHAVIOR:
            _glfw.hints.context.release = value;
            break;
        case GLFW_REFRESH_RATE:
            _glfw.hints.refreshRate = value;
            break;
        default:
            _glfwInputError(GLFW_INVALID_ENUM, "Invalid window hint %i", hint);
            break;
    }
}

GLFWAPI void glfwDestroyWindow(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;

    _GLFW_REQUIRE_INIT();

    // Allow closing of NULL (to match the behavior of free)
    if (window == NULL)
        return;

    // Clear all callbacks to avoid exposing a half torn-down window object
    memset(&window->callbacks, 0, sizeof(window->callbacks));

    // The window's context must not be current on another thread when the
    // window is destroyed
    if (window == _glfwPlatformGetCurrentContext())
        glfwMakeContextCurrent(NULL);

    _glfwPlatformDestroyWindow(window);

    // Unlink window from global linked list
    {
        _GLFWwindow** prev = &_glfw.windowListHead;

        while (*prev != window)
            prev = &((*prev)->next);

        *prev = window->next;
    }

    free(window);
}

GLFWAPI int glfwWindowShouldClose(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(0);
    return window->shouldClose;
}

GLFWAPI void glfwSetWindowShouldClose(GLFWwindow* handle, int value)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();
    window->shouldClose = value;
}

GLFWAPI void glfwSetWindowTitle(GLFWwindow* handle, const char* title)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    assert(title != NULL);

    _GLFW_REQUIRE_INIT();
    _glfwPlatformSetWindowTitle(window, title);
}

GLFWAPI void glfwSetWindowIcon(GLFWwindow* handle,
                               int count, const GLFWimage* images)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);
    assert(count >= 0);
    assert(count == 0 || images != NULL);

    _GLFW_REQUIRE_INIT();
    _glfwPlatformSetWindowIcon(window, count, images);
}

GLFWAPI void glfwGetWindowPos(GLFWwindow* handle, int* xpos, int* ypos)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    if (xpos)
        *xpos = 0;
    if (ypos)
        *ypos = 0;

    _GLFW_REQUIRE_INIT();
    _glfwPlatformGetWindowPos(window, xpos, ypos);
}

GLFWAPI void glfwSetWindowPos(GLFWwindow* handle, int xpos, int ypos)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    if (window->monitor)
        return;

    _glfwPlatformSetWindowPos(window, xpos, ypos);
}

GLFWAPI void glfwGetWindowSize(GLFWwindow* handle, int* width, int* height)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    if (width)
        *width = 0;
    if (height)
        *height = 0;

    _GLFW_REQUIRE_INIT();
    _glfwPlatformGetWindowSize(window, width, height);
}

GLFWAPI void glfwSetWindowSize(GLFWwindow* handle, int width, int height)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    window->videoMode.width  = width;
    window->videoMode.height = height;

    _glfwPlatformSetWindowSize(window, width, height);
}

GLFWAPI void glfwSetWindowSizeLimits(GLFWwindow* handle,
                                     int minwidth, int minheight,
                                     int maxwidth, int maxheight)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    if (minwidth != GLFW_DONT_CARE && minheight != GLFW_DONT_CARE)
    {
        if (minwidth < 0 || minheight < 0)
        {
            _glfwInputError(GLFW_INVALID_VALUE,
                            "Invalid window minimum size %ix%i",
                            minwidth, minheight);
            return;
        }
    }

    if (maxwidth != GLFW_DONT_CARE && maxheight != GLFW_DONT_CARE)
    {
        if (maxwidth < 0 || maxheight < 0 ||
            maxwidth < minwidth || maxheight < minheight)
        {
            _glfwInputError(GLFW_INVALID_VALUE,
                            "Invalid window maximum size %ix%i",
                            maxwidth, maxheight);
            return;
        }
    }

    window->minwidth  = minwidth;
    window->minheight = minheight;
    window->maxwidth  = maxwidth;
    window->maxheight = maxheight;

    if (window->monitor || !window->resizable)
        return;

    _glfwPlatformSetWindowSizeLimits(window,
                                     minwidth, minheight,
                                     maxwidth, maxheight);
}

GLFWAPI void glfwSetWindowAspectRatio(GLFWwindow* handle, int numer, int denom)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    if (numer != GLFW_DONT_CARE && denom != GLFW_DONT_CARE)
    {
        if (numer <= 0 || denom <= 0)
        {
            _glfwInputError(GLFW_INVALID_VALUE,
                            "Invalid window aspect ratio %i:%i",
                            numer, denom);
            return;
        }
    }

    window->numer = numer;
    window->denom = denom;

    if (window->monitor || !window->resizable)
        return;

    _glfwPlatformSetWindowAspectRatio(window, numer, denom);
}

GLFWAPI void glfwGetFramebufferSize(GLFWwindow* handle, int* width, int* height)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    if (width)
        *width = 0;
    if (height)
        *height = 0;

    _GLFW_REQUIRE_INIT();
    _glfwPlatformGetFramebufferSize(window, width, height);
}

GLFWAPI void glfwGetWindowFrameSize(GLFWwindow* handle,
                                    int* left, int* top,
                                    int* right, int* bottom)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    if (left)
        *left = 0;
    if (top)
        *top = 0;
    if (right)
        *right = 0;
    if (bottom)
        *bottom = 0;

    _GLFW_REQUIRE_INIT();
    _glfwPlatformGetWindowFrameSize(window, left, top, right, bottom);
}

GLFWAPI void glfwIconifyWindow(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();
    _glfwPlatformIconifyWindow(window);
}

GLFWAPI void glfwRestoreWindow(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();
    _glfwPlatformRestoreWindow(window);
}

GLFWAPI void glfwMaximizeWindow(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    if (window->monitor)
        return;

    _glfwPlatformMaximizeWindow(window);
}

GLFWAPI void glfwShowWindow(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    if (window->monitor)
        return;

    _glfwPlatformShowWindow(window);
    _glfwPlatformFocusWindow(window);
}

GLFWAPI void glfwHideWindow(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    if (window->monitor)
        return;

    _glfwPlatformHideWindow(window);
}

GLFWAPI void glfwFocusWindow(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    _glfwPlatformFocusWindow(window);
}

GLFWAPI int glfwGetWindowAttrib(GLFWwindow* handle, int attrib)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(0);

    switch (attrib)
    {
        case GLFW_FOCUSED:
            return _glfwPlatformWindowFocused(window);
        case GLFW_ICONIFIED:
            return _glfwPlatformWindowIconified(window);
        case GLFW_VISIBLE:
            return _glfwPlatformWindowVisible(window);
        case GLFW_MAXIMIZED:
            return _glfwPlatformWindowMaximized(window);
        case GLFW_RESIZABLE:
            return window->resizable;
        case GLFW_DECORATED:
            return window->decorated;
        case GLFW_FLOATING:
            return window->floating;
        case GLFW_AUTO_ICONIFY:
            return window->autoIconify;
        case GLFW_CLIENT_API:
            return window->context.client;
        case GLFW_CONTEXT_CREATION_API:
            return window->context.source;
        case GLFW_CONTEXT_VERSION_MAJOR:
            return window->context.major;
        case GLFW_CONTEXT_VERSION_MINOR:
            return window->context.minor;
        case GLFW_CONTEXT_REVISION:
            return window->context.revision;
        case GLFW_CONTEXT_ROBUSTNESS:
            return window->context.robustness;
        case GLFW_OPENGL_FORWARD_COMPAT:
            return window->context.forward;
        case GLFW_OPENGL_DEBUG_CONTEXT:
            return window->context.debug;
        case GLFW_OPENGL_PROFILE:
            return window->context.profile;
        case GLFW_CONTEXT_RELEASE_BEHAVIOR:
            return window->context.release;
        case GLFW_CONTEXT_NO_ERROR:
            return window->context.noerror;
    }

    _glfwInputError(GLFW_INVALID_ENUM, "Invalid window attribute %i", attrib);
    return 0;
}

GLFWAPI void glfwSetWindowAttrib(GLFWwindow* handle, int attrib, int value)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    value = value ? GLFW_TRUE : GLFW_FALSE;

    switch (attrib)
    {
        case GLFW_RESIZABLE:
            if (window->resizable != value)
            {
                window->resizable = value;
                if (!window->monitor)
                    _glfwPlatformSetWindowResizable(window, value);
            }
            return;

        case GLFW_DECORATED:
            if (window->decorated != value)
            {
                window->decorated = value;
                if (!window->monitor)
                    _glfwPlatformSetWindowDecorated(window, value);
            }
            return;

        case GLFW_FLOATING:
            if (window->floating != value)
            {
                window->floating = value;
                if (!window->monitor)
                    _glfwPlatformSetWindowFloating(window, value);
            }
            return;

        case GLFW_AUTO_ICONIFY:
            window->autoIconify = value;
            return;
    }

    _glfwInputError(GLFW_INVALID_ENUM, "Invalid window attribute %i", attrib);
}

GLFWAPI GLFWmonitor* glfwGetWindowMonitor(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return (GLFWmonitor*) window->monitor;
}

GLFWAPI void glfwSetWindowMonitor(GLFWwindow* wh,
                                  GLFWmonitor* mh,
                                  int xpos, int ypos,
                                  int width, int height,
                                  int refreshRate)
{
    _GLFWwindow* window = (_GLFWwindow*) wh;
    _GLFWmonitor* monitor = (_GLFWmonitor*) mh;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();

    if (width <= 0 || height <= 0)
    {
        _glfwInputError(GLFW_INVALID_VALUE,
                        "Invalid window size %ix%i",
                        width, height);
        return;
    }

    if (refreshRate < 0 && refreshRate != GLFW_DONT_CARE)
    {
        _glfwInputError(GLFW_INVALID_VALUE,
                        "Invalid refresh rate %i",
                        refreshRate);
        return;
    }

    window->videoMode.width       = width;
    window->videoMode.height      = height;
    window->videoMode.refreshRate = refreshRate;

    _glfwPlatformSetWindowMonitor(window, monitor,
                                  xpos, ypos, width, height,
                                  refreshRate);
}

GLFWAPI void glfwSetWindowUserPointer(GLFWwindow* handle, void* pointer)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT();
    window->userPointer = pointer;
}

GLFWAPI void* glfwGetWindowUserPointer(GLFWwindow* handle)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    return window->userPointer;
}

GLFWAPI GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* handle,
                                                  GLFWwindowposfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.pos, cbfun);
    return cbfun;
}

GLFWAPI GLFWwindowsizefun glfwSetWindowSizeCallback(GLFWwindow* handle,
                                                    GLFWwindowsizefun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.size, cbfun);
    return cbfun;
}

GLFWAPI GLFWwindowclosefun glfwSetWindowCloseCallback(GLFWwindow* handle,
                                                      GLFWwindowclosefun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.close, cbfun);
    return cbfun;
}

GLFWAPI GLFWwindowrefreshfun glfwSetWindowRefreshCallback(GLFWwindow* handle,
                                                          GLFWwindowrefreshfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.refresh, cbfun);
    return cbfun;
}

GLFWAPI GLFWwindowfocusfun glfwSetWindowFocusCallback(GLFWwindow* handle,
                                                      GLFWwindowfocusfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.focus, cbfun);
    return cbfun;
}

GLFWAPI GLFWwindowiconifyfun glfwSetWindowIconifyCallback(GLFWwindow* handle,
                                                          GLFWwindowiconifyfun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.iconify, cbfun);
    return cbfun;
}

GLFWAPI GLFWwindowmaximizefun glfwSetWindowMaximizeCallback(GLFWwindow* handle,
                                                            GLFWwindowmaximizefun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.maximize, cbfun);
    return cbfun;
}

GLFWAPI GLFWframebuffersizefun glfwSetFramebufferSizeCallback(GLFWwindow* handle,
                                                              GLFWframebuffersizefun cbfun)
{
    _GLFWwindow* window = (_GLFWwindow*) handle;
    assert(window != NULL);

    _GLFW_REQUIRE_INIT_OR_RETURN(NULL);
    _GLFW_SWAP_POINTERS(window->callbacks.fbsize, cbfun);
    return cbfun;
}

GLFWAPI void glfwPollEvents(void)
{
    _GLFW_REQUIRE_INIT();
    _glfwPlatformPollEvents();
}

GLFWAPI void glfwWaitEvents(void)
{
    _GLFW_REQUIRE_INIT();

    if (!_glfw.windowListHead)
        return;

    _glfwPlatformWaitEvents();
}

GLFWAPI void glfwWaitEventsTimeout(double timeout)
{
    _GLFW_REQUIRE_INIT();

    if (timeout != timeout || timeout < 0.0 || timeout > DBL_MAX)
    {
        _glfwInputError(GLFW_INVALID_VALUE, "Invalid time %f", timeout);
        return;
    }

    _glfwPlatformWaitEventsTimeout(timeout);
}

GLFWAPI void glfwPostEmptyEvent(void)
{
    _GLFW_REQUIRE_INIT();

    if (!_glfw.windowListHead)
        return;

    _glfwPlatformPostEmptyEvent();
}


#endif

