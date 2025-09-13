#ifndef GAME_H
#define GAME_H

// This is a 2D game using raylib, written in C.
// Files already included in main.c: raylib.h, raymath.h, stdio.h, stdlib.h, string.h, assert.h, sys/stat.h, errno.h, stdint.h, stdbool.h, ctype.h, time.h, limits.h, math.h
// Please use only raylib and included files in main.c functions, and your own functions also use raymath functions for mathematical operations.
// Use C11 and always call calloc to allocate memory, do not use malloc!

// This is a 2D Battle City - style game with a top - down view.
// Don't rewrite the entire code, just add new parts. 
// Also, specify which functions need to be changed, if necessary, and which code should be inserted instead of the old one.

// This is the main game header file that includes all components
// Include order is important for dependencies

#include "game_constants.h"
#include "game_types.h"
#include "game_globals.h"
#include "game_audio.h"
#include "game_math.h"
#include "game_levels.h"
#include "game_map.h"
#include "game_explosions.h"
#include "game_damagefx.h"
#include "game_powerups.h"
#include "game_obstacles.h"
#include "game_bullets.h"
#include "game_tanks.h"
#include "game_enemies.h"
#include "game_splash.h"
#include "game_main.h"

#endif // GAME_H