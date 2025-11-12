/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* Email:
 *		<nguyenthaithuanalg@gmail.com> - Nguyễn Thái Thuận
 * Mobile:
 *		<+084.799.324.179>
 * Skype:
 *		<nguyenthaithuanalg>
 * Date:
 *		<2024-July-14>
 * The lasted modified date:
 *		<2024-Sep-14>
 *		<2024-Dec-18>
 *		<2024-Dec-20>
 *		<2024-Dec-22>
 *		<2024-Dec-23>
 *		<2024-Dec-30>
 *		<2024-Jan-06>
 *		<2025-Jan-08>
 *		<2025-Jan-10>
 *		<2025-Feb-01>
 *		<2025-Feb-16>
 *		<2025-Apr-11>
 *		<2025-Apr-22>
 *		<2025-Jun-01>
 *		<2025-Jun-11>
 * Decription:
 *		The (only) main header file to export 4 APIs: [FFWR_init_log_ext, spllog, spllogtopic, FFWR_finish_log].
 */
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef ___FFWR_RENDER_LOG__
#define ___FFWR_RENDER_LOG__                 
#include <stdio.h>
#include <string.h>
/*strrchr*/

#define FFWR_MIN_AB(a, b) ((a) < (b)) ? (a) : (b)
#define FFWR_MAX_AB(a, b) ((a) > (b)) ? (a) : (b)

#if 1
#ifndef UNIX_LINUX
#define UNIX_LINUX                      
#endif
#endif

#if 0
#ifndef __MACH__
#define __MACH__                        
#endif
#endif

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LLU
	#define LLU                             unsigned long long
#endif

#define FFWR_uchar                       	unsigned char
#define FFWR_uint                        	unsigned int
#define FFWR_InitFlags						FFWR_uint



///#ifndef Uint8
///typedef Uint8 unsigned char
///#endif 	

#ifndef UNIX_LINUX
#ifndef __FFWR_RENDER_STATIC_LOG__
#ifdef EXPORT_DLL_API_FFWR_RENDER
#define DLL_API_FFWR_RENDER              __declspec(dllexport)
#else
#define DLL_API_FFWR_RENDER              __declspec(dllimport)
#endif
#else
#define DLL_API_FFWR_RENDER              
#endif
#else
#define DLL_API_FFWR_RENDER              
#endif /*! UNIX_LINUX */

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#define FFWR_INIT_TIMER          0x00000001u
#define FFWR_INIT_AUDIO          0x00000010u
#define FFWR_INIT_VIDEO          0x00000020u  /**< FFWR_INIT_VIDEO implies FFWR_INIT_EVENTS */
#define FFWR_INIT_JOYSTICK       0x00000200u  /**< FFWR_INIT_JOYSTICK implies FFWR_INIT_EVENTS */
#define FFWR_INIT_HAPTIC         0x00001000u
#define FFWR_INIT_GAMECONTROLLER 0x00002000u  /**< FFWR_INIT_GAMECONTROLLER implies FFWR_INIT_JOYSTICK */
#define FFWR_INIT_EVENTS         0x00004000u
#define FFWR_INIT_SENSOR         0x00008000u
#define FFWR_INIT_NOPARACHUTE    0x00100000u  /**< compatibility; this flag is ignored. */
#define FFWR_INIT_EVERYTHING ( \
                FFWR_INIT_TIMER | FFWR_INIT_AUDIO | FFWR_INIT_VIDEO | FFWR_INIT_EVENTS | \
                FFWR_INIT_JOYSTICK | FFWR_INIT_HAPTIC | FFWR_INIT_GAMECONTROLLER | FFWR_INIT_SENSOR \
            )

#define FFWR_NUM_DATA_POINTERS 8

/**
 * Flags used when creating a rendering context
 */
typedef enum FFWR_RendererFlags
{
    FFWR_RENDERER_SOFTWARE = 0x00000001,         /**< The renderer is a software fallback */
    FFWR_RENDERER_ACCELERATED = 0x00000002,      /**< The renderer uses hardware
                                                     acceleration */
    FFWR_RENDERER_PRESENTVSYNC = 0x00000004,     /**< Present is synchronized
                                                     with the refresh rate */
    FFWR_RENDERER_TARGETTEXTURE = 0x00000008     /**< The renderer supports
                                                     rendering to texture */
} FFWR_RendererFlags;

typedef enum FFWR_PixelFormat
{
    FFWR_PIXELFORMAT_UNKNOWN = 0,
    FFWR_PIXELFORMAT_INDEX1LSB = 0x11100100u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_INDEX1, FFWR_BITMAPORDER_4321, 0, 1, 0), */
    FFWR_PIXELFORMAT_INDEX1MSB = 0x11200100u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_INDEX1, FFWR_BITMAPORDER_1234, 0, 1, 0), */
    FFWR_PIXELFORMAT_INDEX2LSB = 0x1c100200u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_INDEX2, FFWR_BITMAPORDER_4321, 0, 2, 0), */
    FFWR_PIXELFORMAT_INDEX2MSB = 0x1c200200u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_INDEX2, FFWR_BITMAPORDER_1234, 0, 2, 0), */
    FFWR_PIXELFORMAT_INDEX4LSB = 0x12100400u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_INDEX4, FFWR_BITMAPORDER_4321, 0, 4, 0), */
    FFWR_PIXELFORMAT_INDEX4MSB = 0x12200400u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_INDEX4, FFWR_BITMAPORDER_1234, 0, 4, 0), */
    FFWR_PIXELFORMAT_INDEX8 = 0x13000801u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_INDEX8, 0, 0, 8, 1), */
    FFWR_PIXELFORMAT_RGB332 = 0x14110801u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED8, FFWR_PACKEDORDER_XRGB, FFWR_PACKEDLAYOUT_332, 8, 1), */
    FFWR_PIXELFORMAT_XRGB4444 = 0x15120c02u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_XRGB, FFWR_PACKEDLAYOUT_4444, 12, 2), */
    FFWR_PIXELFORMAT_XBGR4444 = 0x15520c02u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_XBGR, FFWR_PACKEDLAYOUT_4444, 12, 2), */
    FFWR_PIXELFORMAT_XRGB1555 = 0x15130f02u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_XRGB, FFWR_PACKEDLAYOUT_1555, 15, 2), */
    FFWR_PIXELFORMAT_XBGR1555 = 0x15530f02u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_XBGR, FFWR_PACKEDLAYOUT_1555, 15, 2), */
    FFWR_PIXELFORMAT_ARGB4444 = 0x15321002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_ARGB, FFWR_PACKEDLAYOUT_4444, 16, 2), */
    FFWR_PIXELFORMAT_RGBA4444 = 0x15421002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_RGBA, FFWR_PACKEDLAYOUT_4444, 16, 2), */
    FFWR_PIXELFORMAT_ABGR4444 = 0x15721002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_ABGR, FFWR_PACKEDLAYOUT_4444, 16, 2), */
    FFWR_PIXELFORMAT_BGRA4444 = 0x15821002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_BGRA, FFWR_PACKEDLAYOUT_4444, 16, 2), */
    FFWR_PIXELFORMAT_ARGB1555 = 0x15331002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_ARGB, FFWR_PACKEDLAYOUT_1555, 16, 2), */
    FFWR_PIXELFORMAT_RGBA5551 = 0x15441002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_RGBA, FFWR_PACKEDLAYOUT_5551, 16, 2), */
    FFWR_PIXELFORMAT_ABGR1555 = 0x15731002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_ABGR, FFWR_PACKEDLAYOUT_1555, 16, 2), */
    FFWR_PIXELFORMAT_BGRA5551 = 0x15841002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_BGRA, FFWR_PACKEDLAYOUT_5551, 16, 2), */
    FFWR_PIXELFORMAT_RGB565 = 0x15151002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_XRGB, FFWR_PACKEDLAYOUT_565, 16, 2), */
    FFWR_PIXELFORMAT_BGR565 = 0x15551002u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED16, FFWR_PACKEDORDER_XBGR, FFWR_PACKEDLAYOUT_565, 16, 2), */
    FFWR_PIXELFORMAT_RGB24 = 0x17101803u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYU8, FFWR_ARRAYORDER_RGB, 0, 24, 3), */
    FFWR_PIXELFORMAT_BGR24 = 0x17401803u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYU8, FFWR_ARRAYORDER_BGR, 0, 24, 3), */
    FFWR_PIXELFORMAT_XRGB8888 = 0x16161804u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_XRGB, FFWR_PACKEDLAYOUT_8888, 24, 4), */
    FFWR_PIXELFORMAT_RGBX8888 = 0x16261804u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_RGBX, FFWR_PACKEDLAYOUT_8888, 24, 4), */
    FFWR_PIXELFORMAT_XBGR8888 = 0x16561804u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_XBGR, FFWR_PACKEDLAYOUT_8888, 24, 4), */
    FFWR_PIXELFORMAT_BGRX8888 = 0x16661804u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_BGRX, FFWR_PACKEDLAYOUT_8888, 24, 4), */
    FFWR_PIXELFORMAT_ARGB8888 = 0x16362004u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_ARGB, FFWR_PACKEDLAYOUT_8888, 32, 4), */
    FFWR_PIXELFORMAT_RGBA8888 = 0x16462004u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_RGBA, FFWR_PACKEDLAYOUT_8888, 32, 4), */
    FFWR_PIXELFORMAT_ABGR8888 = 0x16762004u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_ABGR, FFWR_PACKEDLAYOUT_8888, 32, 4), */
    FFWR_PIXELFORMAT_BGRA8888 = 0x16862004u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_BGRA, FFWR_PACKEDLAYOUT_8888, 32, 4), */
    FFWR_PIXELFORMAT_XRGB2101010 = 0x16172004u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_XRGB, FFWR_PACKEDLAYOUT_2101010, 32, 4), */
    FFWR_PIXELFORMAT_XBGR2101010 = 0x16572004u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_XBGR, FFWR_PACKEDLAYOUT_2101010, 32, 4), */
    FFWR_PIXELFORMAT_ARGB2101010 = 0x16372004u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_ARGB, FFWR_PACKEDLAYOUT_2101010, 32, 4), */
    FFWR_PIXELFORMAT_ABGR2101010 = 0x16772004u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_PACKED32, FFWR_PACKEDORDER_ABGR, FFWR_PACKEDLAYOUT_2101010, 32, 4), */
    FFWR_PIXELFORMAT_RGB48 = 0x18103006u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYU16, FFWR_ARRAYORDER_RGB, 0, 48, 6), */
    FFWR_PIXELFORMAT_BGR48 = 0x18403006u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYU16, FFWR_ARRAYORDER_BGR, 0, 48, 6), */
    FFWR_PIXELFORMAT_RGBA64 = 0x18204008u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYU16, FFWR_ARRAYORDER_RGBA, 0, 64, 8), */
    FFWR_PIXELFORMAT_ARGB64 = 0x18304008u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYU16, FFWR_ARRAYORDER_ARGB, 0, 64, 8), */
    FFWR_PIXELFORMAT_BGRA64 = 0x18504008u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYU16, FFWR_ARRAYORDER_BGRA, 0, 64, 8), */
    FFWR_PIXELFORMAT_ABGR64 = 0x18604008u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYU16, FFWR_ARRAYORDER_ABGR, 0, 64, 8), */
    FFWR_PIXELFORMAT_RGB48_FLOAT = 0x1a103006u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF16, FFWR_ARRAYORDER_RGB, 0, 48, 6), */
    FFWR_PIXELFORMAT_BGR48_FLOAT = 0x1a403006u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF16, FFWR_ARRAYORDER_BGR, 0, 48, 6), */
    FFWR_PIXELFORMAT_RGBA64_FLOAT = 0x1a204008u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF16, FFWR_ARRAYORDER_RGBA, 0, 64, 8), */
    FFWR_PIXELFORMAT_ARGB64_FLOAT = 0x1a304008u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF16, FFWR_ARRAYORDER_ARGB, 0, 64, 8), */
    FFWR_PIXELFORMAT_BGRA64_FLOAT = 0x1a504008u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF16, FFWR_ARRAYORDER_BGRA, 0, 64, 8), */
    FFWR_PIXELFORMAT_ABGR64_FLOAT = 0x1a604008u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF16, FFWR_ARRAYORDER_ABGR, 0, 64, 8), */
    FFWR_PIXELFORMAT_RGB96_FLOAT = 0x1b10600cu,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF32, FFWR_ARRAYORDER_RGB, 0, 96, 12), */
    FFWR_PIXELFORMAT_BGR96_FLOAT = 0x1b40600cu,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF32, FFWR_ARRAYORDER_BGR, 0, 96, 12), */
    FFWR_PIXELFORMAT_RGBA128_FLOAT = 0x1b208010u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF32, FFWR_ARRAYORDER_RGBA, 0, 128, 16), */
    FFWR_PIXELFORMAT_ARGB128_FLOAT = 0x1b308010u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF32, FFWR_ARRAYORDER_ARGB, 0, 128, 16), */
    FFWR_PIXELFORMAT_BGRA128_FLOAT = 0x1b508010u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF32, FFWR_ARRAYORDER_BGRA, 0, 128, 16), */
    FFWR_PIXELFORMAT_ABGR128_FLOAT = 0x1b608010u,
        /* FFWR_DEFINE_PIXELFORMAT(FFWR_PIXELTYPE_ARRAYF32, FFWR_ARRAYORDER_ABGR, 0, 128, 16), */

    FFWR_PIXELFORMAT_YV12 = 0x32315659u,      /**< Planar mode: Y + V + U  (3 planes) */
        /* FFWR_DEFINE_PIXELFOURCC('Y', 'V', '1', '2'), */
    FFWR_PIXELFORMAT_IYUV = 0x56555949u,      /**< Planar mode: Y + U + V  (3 planes) */
        /* FFWR_DEFINE_PIXELFOURCC('I', 'Y', 'U', 'V'), */
    FFWR_PIXELFORMAT_YUY2 = 0x32595559u,      /**< Packed mode: Y0+U0+Y1+V0 (1 plane) */
        /* FFWR_DEFINE_PIXELFOURCC('Y', 'U', 'Y', '2'), */
    FFWR_PIXELFORMAT_UYVY = 0x59565955u,      /**< Packed mode: U0+Y0+V0+Y1 (1 plane) */
        /* FFWR_DEFINE_PIXELFOURCC('U', 'Y', 'V', 'Y'), */
    FFWR_PIXELFORMAT_YVYU = 0x55595659u,      /**< Packed mode: Y0+V0+Y1+U0 (1 plane) */
        /* FFWR_DEFINE_PIXELFOURCC('Y', 'V', 'Y', 'U'), */
    FFWR_PIXELFORMAT_NV12 = 0x3231564eu,      /**< Planar mode: Y + U/V interleaved  (2 planes) */
        /* FFWR_DEFINE_PIXELFOURCC('N', 'V', '1', '2'), */
    FFWR_PIXELFORMAT_NV21 = 0x3132564eu,      /**< Planar mode: Y + V/U interleaved  (2 planes) */
        /* FFWR_DEFINE_PIXELFOURCC('N', 'V', '2', '1'), */
    FFWR_PIXELFORMAT_P010 = 0x30313050u,      /**< Planar mode: Y + U/V interleaved  (2 planes) */
        /* FFWR_DEFINE_PIXELFOURCC('P', '0', '1', '0'), */
    FFWR_PIXELFORMAT_EXTERNAL_OES = 0x2053454fu,     /**< Android video texture format */
        /* FFWR_DEFINE_PIXELFOURCC('O', 'E', 'S', ' ') */

    FFWR_PIXELFORMAT_MJPG = 0x47504a4du,     /**< Motion JPEG */
        /* FFWR_DEFINE_PIXELFOURCC('M', 'J', 'P', 'G') */

    /* Aliases for RGBA byte arrays of color data, for the current platform */
    #if FFWR_BYTEORDER == FFWR_BIG_ENDIAN
    FFWR_PIXELFORMAT_RGBA32 = FFWR_PIXELFORMAT_RGBA8888,
    FFWR_PIXELFORMAT_ARGB32 = FFWR_PIXELFORMAT_ARGB8888,
    FFWR_PIXELFORMAT_BGRA32 = FFWR_PIXELFORMAT_BGRA8888,
    FFWR_PIXELFORMAT_ABGR32 = FFWR_PIXELFORMAT_ABGR8888,
    FFWR_PIXELFORMAT_RGBX32 = FFWR_PIXELFORMAT_RGBX8888,
    FFWR_PIXELFORMAT_XRGB32 = FFWR_PIXELFORMAT_XRGB8888,
    FFWR_PIXELFORMAT_BGRX32 = FFWR_PIXELFORMAT_BGRX8888,
    FFWR_PIXELFORMAT_XBGR32 = FFWR_PIXELFORMAT_XBGR8888
    #else
    FFWR_PIXELFORMAT_RGBA32 = FFWR_PIXELFORMAT_ABGR8888,
    FFWR_PIXELFORMAT_ARGB32 = FFWR_PIXELFORMAT_BGRA8888,
    FFWR_PIXELFORMAT_BGRA32 = FFWR_PIXELFORMAT_ARGB8888,
    FFWR_PIXELFORMAT_ABGR32 = FFWR_PIXELFORMAT_RGBA8888,
    FFWR_PIXELFORMAT_RGBX32 = FFWR_PIXELFORMAT_XBGR8888,
    FFWR_PIXELFORMAT_XRGB32 = FFWR_PIXELFORMAT_BGRX8888,
    FFWR_PIXELFORMAT_BGRX32 = FFWR_PIXELFORMAT_XRGB8888,
    FFWR_PIXELFORMAT_XBGR32 = FFWR_PIXELFORMAT_RGBX8888
    #endif
} FFWR_PixelFormat;

typedef enum FFWR_TextureAccess
{
    FFWR_TEXTUREACCESS_STATIC,    /**< Changes rarely, not lockable */
    FFWR_TEXTUREACCESS_STREAMING, /**< Changes frequently, lockable */
    FFWR_TEXTUREACCESS_TARGET     /**< Texture can be used as a render target */
} FFWR_TextureAccess;

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
typedef enum __FFWR_LOG_ERR_CODE__ {
	FFWR_OK,
	FFWR_INIT_ERR,
	FFWR_NULL_HWND_ERR, 
	FFWR_NULL_INPUT_ERR, 
	FFWR_CANNOT_CREATE_WIN_ERR, 
	FFWR_NULL_WINDOW_ERR, 
	FFWR_NULL_RENDER_OUTPUT_ERR, 
	FFWR_CREATERENDERER_ERR, 
	FFWR_NULL_TEXTURE_OUTPUT_ERR, 
	FFWR_NULL_RENDER_INPUT_ERR, 
	FFWR_UPDATE_YUV_TEXTURE_ERR, 
	FFWR_RENDERCLEAR_ERR, 
	FFWR_NULL_TEXTURE_INPUT_ERR, 
	FFWR_RENDERCOPY_ERR, 
	FFWR_NULL_SDL_WIN_INPUT_ERR, 
	FFWR_NETWORK_INIT_ERR, 
	FFWR_SDL_INIT_ERR, 
	FFWR_NULL_FFWR_INPUT_ST_ERR, 
	FFWR_MEMORY_ERR, 
	FFWR_AV_OPEN_INPUT_ERR, 
	FFWR_AV_FIND_STREAM_INFO_ERR, 
	FFWR_AV_NB_STREAMS_ERR, 
	FFWR_NO_VSTREAMS_ERR, 
	FFWR_NO_VCODEC_ERR, 
	FFWR_NO_VCONTEXT_ERR, 
	FFWR_PARAMETERS_TO_CONTEXT_ERR, 
	FFWR_OPEN_VCODEC_ERR, 
	FFWR_VFRAME_ALLOC_ERR, 
	FFWR_NO_VSTREAM_ERR, 
	FFWR_NO_ACONTEXT_ERR, 
	FFWR_ALLOC_ACONTEX_ERR, 
	FFWR_PARAMETERS_TO_ACONTEXT_ERR, 
	FFWR_OPEN_ACODEC_ERR, 
	FFWR_AFRAME_ALLOC_ERR, 
	FFWR_WIN_CREATE_MUTEX_ERR, 
	FFWR_MUTEX_NULL, 
	FFWR_WIN32_WAIT_OBJECT, 
	FFWR_WIN32_MUTEX_RELEASE, 
	FFWR_WIN32_RENDER_OBJECTS_NULL_ERR, 
	FFWR_DEMUX_OBJS_NULL_ERR, 
	FFWR_DEMUX_DATA_OUT_NULL_ERR, 
	FFWR_INSTREAM_NULL_ERR,
	FFWR_MALLOC_ERR,
	FFWR_NATIVE_WINDOW_NULL_ERR, 
	FFWR_CREATE_MUTEX_NULL_ERR, 
	FFWR_GEN_DATA_NULL_ERR, 
	FFWR_SEM_NULL_ERR, 
	FFWR_WIN_CREATE_SEM_ERR, 
	FFWR_OUTSEM_NULL_ERR,
	FFWR_WIN_CLOSE_SEM_ERR,
	FFWR_WIN_OUTMUTEX_NULL_ERR,
	FFWR_SEM_RELEASE_ERR,
	FFWR_SEM_WAIT_ERR,
	FFWR_LINUX_CREATE_MUTEX_ERR, 
    FFWR_LINUX_INIT_MUTEX_ERR, 
    FFWR_WIN_MUTEX_CLOSE,
	FFWR_LINUX_MUTEX_CLOSE, 
    FFWR_LINUX_CREATE_SEM_ERR, 
    FFWR_LINUX_INIT_SEM_ERR, 
    FFWR_UNIX_SEM_POST_ERR, 
    FFWR_UNIX_SEM_WAIT_ERR, FFWR_UNIX_PTHREAD_CREATE_ERR, 

	
	
	


	FFWR_END_ERR,
} FFWR_LOG_ERR_CODE;
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define MEMORY_PADDING      2
#define FFWR_BUFF_SIZE      12000000
#define FFWR_OUTPUT_ARATE   48000
#define FFWR_AUDIO_BUF          (1024 * 1024 * 2)
#define ffwr_malloc(__nn__, __obj__, __type__)                                 \
	{                                                                      \
		(__obj__) = (__type__ *)malloc(__nn__);                        \
		if (__obj__) {                                                 \
			spllog(1, "[ffwr-MEM] Malloc: 0x%p.", (__obj__));           \
			memset((__obj__), 0, (__nn__));                        \
		} else {                                                       \
			spllog(5, "Malloc: error.");                           \
		}                                                              \
	}
#define FFWR_MIN(__a__, __b__)  ((__a__) < (__b__)) ? (__a__) : (__b__)
#define ffwr_free(__obj__)                                                     \
	{                                                                      \
		if (__obj__) {                                                 \
			spllog(1, "[ffwr-MEM] Free: 0x%p.", (__obj__));             \
			free(__obj__);                                         \
			(__obj__) = 0;                                         \
		}                                                              \
	}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
typedef int (*FFWR_CALLBACL_FN)(void *);

typedef struct FFWR_Rect
{
    int x, y;
    int w, h;
} FFWR_Rect;


typedef struct __FFWR_RENDER_OBJECTS__
{	
	unsigned int ren_flags;
	FFWR_PixelFormat format;
	FFWR_TextureAccess access;
	int w; 
	int h;	
	
	void *sdl_window;
	void *sdl_render;
	void *sdl_texture;
	void *native_window;
} FFWR_RENDER_OBJECTS;


typedef enum __FFWR_DATA_TYPE_E__{
    FFWR_DTYPE_VFRAME,
    FFWR_DTYPE_PACKET,
    FFWR_DEMUX_THREAD_EXIT,

    FFWR_DTYPE_END
} FFWR_DATA_TYPE_E;

typedef struct __FFWR_GENERIC_DATA__ {
	int total; /*Total size*/
	int range; /*data range*/
	int pc; /*Point to the current*/
	int pl; /*Point to the last*/
	char data[0]; /*Generic data */
} ffwr_gen_data_st;

#define ffwr_araw_stream                ffwr_gen_data_st

typedef struct __FFWR_DEMUX_DATA__
{
	int vbuf_size;
    ffwr_gen_data_st * vbuf; /*video buffer*/
    ffwr_gen_data_st * shared_vbuf; /*video shared buffer*/
    void *mtx_vbuf; /*Mutex shared buffer*/
    void *sem_vbuf; /*sem shared buffer*/
    int vwait;
	
	int abuf_size;
    ffwr_gen_data_st * abuf; /*audio buffer*/
    ffwr_gen_data_st * shared_abuf; /*audio shared buffer*/
    void *mtx_abuf; /*Mutex shared audio buffer*/
    void *sem_abuf; /*sem shared audio buffer*/
    int await;
	
} FFWR_DEMUX_DATA;

typedef struct {
    int total;
    int type;    
} FFWR_SIZE_TYPE;

typedef struct __FFWR_VFrame__ {
    FFWR_SIZE_TYPE tt_sz;
    int w;
    int h;
    int fmt;
    int pts;
    int linesize[FFWR_NUM_DATA_POINTERS];
    int pos[FFWR_NUM_DATA_POINTERS + 1];   
    int len[FFWR_NUM_DATA_POINTERS + 1]; 
    unsigned char data[0];
} FFWR_VFrame;

typedef struct {
	char name[1024];
	int mode;
	FFWR_CALLBACL_FN cb;
	FFWR_SIZE_TYPE sz_type;
	char data[0];
} FFWR_INPUT_ST;

typedef struct __FFWR_DEMUX_OBJS__ {
	int isstop; 		/*Keep for whole life-cycle.*/
	FFWR_RENDER_OBJECTS render_objects; /*Keep for whole life-cycle.*/
	void *inner_demux; /*Must be cleared for each session.*/
	FFWR_DEMUX_DATA buffer;  /*Keep for whole life-cycle.*/
	FFWR_INPUT_ST input; /*From UI/User, may be changed for whole life-cycle.*/
} FFWR_DEMUX_OBJS;

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

DLL_API_FFWR_RENDER int
ffwr_hello();

DLL_API_FFWR_RENDER int
ffwr_init(FFWR_InitFlags flags);

DLL_API_FFWR_RENDER int	
ffwr_Quit();





DLL_API_FFWR_RENDER int	
ffwr_UpdateYUVTexture(
		void *texture, const FFWR_Rect *rect,
        const FFWR_uchar *Yplane, int Ypitch,
        const FFWR_uchar *Uplane, int Upitch,
        const FFWR_uchar *Vplane, int Vpitch
	);	
	
DLL_API_FFWR_RENDER int 
ffwr_RenderClear(void *renderer);

DLL_API_FFWR_RENDER int	 
ffwr_RenderCopy(void *renderer, void* texture,
    const FFWR_Rect *srcrect, const FFWR_Rect *dstrect);
	
DLL_API_FFWR_RENDER int	 
ffwr_RenderPresent(void *renderer);








DLL_API_FFWR_RENDER int 
ffwr_open_audio_output(FFWR_DEMUX_OBJS *, int sz);











DLL_API_FFWR_RENDER int
ffwr_create_mutex(void **outmutex, char *name);

DLL_API_FFWR_RENDER int
ffwr_destroy_mutex(void *obj);


DLL_API_FFWR_RENDER int
ffwr_create_semaphore(void **outsem, char *name);

DLL_API_FFWR_RENDER int
ffwr_destroy_semaphore(void *obj);

DLL_API_FFWR_RENDER int
ffwr_semaphore_post(void *obj);

DLL_API_FFWR_RENDER int
ffwr_semaphore_wait(void *obj);





DLL_API_FFWR_RENDER int 
ffwr_create_demux_objects(FFWR_DEMUX_OBJS *);

DLL_API_FFWR_RENDER int 
ffwr_init_demux_objects(FFWR_DEMUX_OBJS *);

DLL_API_FFWR_RENDER int 
ffwr_destroy_demux_objects(FFWR_DEMUX_OBJS *);

DLL_API_FFWR_RENDER int 
ffwr_get_stopping(FFWR_DEMUX_OBJS *obj);

DLL_API_FFWR_RENDER int 
ffwr_set_stopping(FFWR_DEMUX_OBJS *obj, int v);

DLL_API_FFWR_RENDER int 
ffwr_destroy_render_objects(FFWR_RENDER_OBJECTS *);

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef __cplusplus
}
#endif
#endif
