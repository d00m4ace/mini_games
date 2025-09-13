#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define APP_NAME "AI GAME EXAMPLE"

// Define custom colors (only if they are not already defined in raylib)
#ifndef DARKRED
#define DARKRED (Color){139, 0, 0, 255}
#endif

// Tank level constants
#define INITIAL_TANK_LEVEL 3
#define MAX_TANK_LEVEL 99

// Enemy tank constants
#define MAX_ENEMY_TANKS 20
#define ENEMY_SPAWN_TIME 3.0f
#define ENEMY_MOVE_TIME 1.0f
#define ENEMY_POINTS 100

// Bullet constants
#define BULLET_SIZE 4
#define BULLET_SPEED 300.0f
#define TANK_RELOAD_TIME 0.5f
#define MAX_BULLETS 300
#define BULLET_LIFETIME 5.0f

// Obstacle constants
#define BLOCK_SIZE 32
#define MAX_OBSTACLES 500

// Mine regeneration constants
#define MINE_RESPAWN_TIME_MIN 5.0f
#define MINE_RESPAWN_TIME_MAX 9.0f

// Mine trigger: how deep (in pixels) a tank must enter the mine cell to trigger it
#define MINE_TRIGGER_INSET 6

#define MAX_FLOATING_DAMAGE 50
#define MAX_HIT_PARTICLES 200

// Steel wall respawn constants
#define STEEL_WALL_RESPAWN_TIME 15.0f
#define STEEL_WALL_RESPAWN_DAMAGE 5

// Map system constants
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define MAP_OFFSET_X ((SCREEN_WIDTH - MAP_WIDTH * BLOCK_SIZE) / 2)
#define MAP_OFFSET_Y ((SCREEN_HEIGHT - MAP_HEIGHT * BLOCK_SIZE) / 2)

// Power-up respawn constants
#define POWER_UP_RESPAWN_TIME_MIN 15.0f
#define POWER_UP_RESPAWN_TIME_MAX 25.0f
#define MAX_POWER_UPS_PER_TYPE 2
#define POWER_UP_SPAWN_CHECK_INTERVAL 5.0f

// Tank constants
#define TANK_SIZE 28
#define TANK_SPEED 100.0f
#define CANNON_LENGTH 20
#define CANNON_WIDTH 4

// Explosion effect constants
#define MAX_EXPLOSIONS 50
#define EXPLOSION_DURATION 1.0f
#define EXPLOSION_PARTICLES 12
#define EXPLOSION_SIZE 40

// Splash screen constants
#define SPLASH_DURATION 2.0f
#define FADE_DURATION 0.5f

#endif // GAME_CONSTANTS_H