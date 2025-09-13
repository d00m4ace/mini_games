#ifndef GAME_GLOBALS_H
#define GAME_GLOBALS_H

// Define all global variables
Tank player1_tank = { 0 };
Tank player2_tank = { 0 };
Tank enemy_tanks[MAX_ENEMY_TANKS] = { 0 };
Bullet bullets[MAX_BULLETS] = { 0 };

FloatingDamage floating_damages[MAX_FLOATING_DAMAGE] = { 0 };
HitParticle hit_particles[MAX_HIT_PARTICLES] = { 0 };
Explosion explosions[MAX_EXPLOSIONS] = { 0 };

Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT] = { 0 };

SplashScreen splash_screen = { 0 };

GameState game_state = { 0 };
bool game_initialized = false;

bool game_paused = false;

float global_input_cooldown = 0.0f;  // Global input cooldown

Font title_font;
Font subtitle_font;
Font ui_font;
Font button_font;

// Sound system globals
Sound sound_shoot = { 0 };
Sound sound_hit = { 0 };
Sound sound_explosion = { 0 };
bool sounds_loaded = false;


// Map legend - defines what each ASCII character represents
static MapLegend map_legend[] = {
	{' ', OBSTACLE_EMPTY},          // Empty space (everyone can pass)
	{'#', OBSTACLE_STEEL_WALL},     // Steel wall (block tanks and bullets, can also be destroyed, reappear after a while)
	{'~', OBSTACLE_WATER},          // Water (block tanks, bullets can pass)	
	{'=', OBSTACLE_BRIDGE},         // Bridge (allows tanks to cross water, don't block bullets)
	{'T', OBSTACLE_FOREST},         // Trees/Forest (everyone can pass, hides tanks and bullets)
	{'I', OBSTACLE_ICE_BLOCK},      // Ice (slippery, tanks slide when moving on it, bullets can pass)
	{'Q', OBSTACLE_QUICKSAND},      // Quicksand (slows down tanks significantly, bullets can pass)
	{'>', OBSTACLE_CONVEYOR_BELT},  // Conveyor belt (move tanks right, bullets can pass)
	{'<', OBSTACLE_CONVEYOR_BELT},  // Conveyor belt (move tanks left, bullets can pass)
	{'v', OBSTACLE_CONVEYOR_BELT},  // Conveyor belt (move tanks down, bullets can pass)
	{'^', OBSTACLE_CONVEYOR_BELT},  // Conveyor belt (move tanks up, bullets can pass)
	{'M', OBSTACLE_MINE_FIELD},     // Mine field (explodes when tanks move over it, bullets can pass, respawn after a while)	
	{'H', OBSTACLE_HEALING_STATION},// Healing station (restores tank health/level over time, power up, respawn after some time in a new empty space)
	{'A', OBSTACLE_AMMO_DEPOT},     // Ammo depot (reduces reload time temporarily, power up, respawn after some time in a new empty space)
	{'S', OBSTACLE_SHIELD_GENERATOR},// Shield generator (provides temporary invincibility, power up, respawn after some time in a new empty space)
	{'G', OBSTACLE_GATE},           // Gate (opens/closes periodically, when closed block tanks and bullets)
	{'R', OBSTACLE_REFLECTOR},      // Reflector (bounces bullets back, tanks can pass)
	{'@', OBSTACLE_TELEPORTER}      // Teleporter (teleports tanks and bullets to another random teleport)
};

// Sample map
char game_map[MAP_HEIGHT][MAP_WIDTH + 1] = {
	"########################",
	"#                      #",
	"#  ##    H      ##     #",
	"#  ##           ##     #",
	"#       ~~~~           #",
	"#       ====      A    #",
	"#       ~~~~   <v      #",
	"#              >v      #",
	"#   @     TTT      @   #",
	"#        TTTTT         #",
	"#         TTT          #",
	"#                      #",
	"#  III        QQQ      #",
	"#  III   S    QQQ      #",
	"#  III        QQQ  ^   #",
	"#                  ^   #",
	"#    >>>>      R   ^   #",
	"#              R   ^   #",
	"#         M            #",
	"#       M M M          #",
	"#   #     M        #   #",
	"#   ########G#######   #",
	"#                      #",
	"########################"
};

#endif // GAME_GLOBALS_H