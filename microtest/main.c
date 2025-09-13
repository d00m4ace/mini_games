#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>      // для FILE, fopen, fclose, fprintf, printf, fgets
#include <stdlib.h>     // для malloc, free, atoi
#include <string.h>     // для strcpy, strlen, memcpy, strerror
#include <assert.h>     // для assert
#include <sys/stat.h>   // для struct stat, stat
#include <errno.h>      // для errno
#include <stdint.h>		// для uint8_t, int64_t 
#include <stdbool.h>	// для bool, true, false
#include <ctype.h> 
#include <time.h>
#include <limits.h>

#if defined(_WIN32)
#include <direct.h>  // для _mkdir в Windows
#endif

#include "raylib.h"
#include "raymath.h"

#if defined(_WIN32)
#define PLATFORM_DESKTOP
#elif defined(__linux__)
#define PLATFORM_DESKTOP
#elif defined(__APPLE__)
#define PLATFORM_APPLE
#elif defined(__ANDROID__)
#define PLATFORM_ANDROID
#elif defined(__EMSCRIPTEN__)
#define PLATFORM_HTML5
#else
#define PLATFORM_UNKNOWN
#endif

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
#if defined(PLATFORM_WEB)
#include "JS_CODE.h"
#include "EMS_CODE.h"
#endif

#if defined(PLATFORM_DESKTOP)
#include "misc/tinyfiledialogs.h"
#include "DESKTOP_CODE.h"
#endif

#include "code/zeromalloc.h"
#include "code/common_fs.h"
#include "code/system.h"
#include "code/graphics.h"

#include "game/aigame002/game.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	system_init();

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, APP_NAME);

	gudf = game_update_draw_frame;

	system_run();

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}