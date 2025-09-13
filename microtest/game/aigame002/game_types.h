#ifndef GAME_TYPES_H
#define GAME_TYPES_H

// Obstacle types
typedef enum {
	OBSTACLE_EMPTY,
	OBSTACLE_WATER,
	OBSTACLE_FOREST,
	OBSTACLE_STEEL_WALL,
	OBSTACLE_ICE_BLOCK,
	OBSTACLE_QUICKSAND,
	OBSTACLE_TELEPORTER,
	OBSTACLE_CONVEYOR_BELT,
	OBSTACLE_MINE_FIELD,
	OBSTACLE_HEALING_STATION,
	OBSTACLE_AMMO_DEPOT,
	OBSTACLE_SHIELD_GENERATOR,
	OBSTACLE_BRIDGE,
	OBSTACLE_GATE,
	OBSTACLE_REFLECTOR
} ObstacleType;

// Splash screen types
typedef enum {
	SPLASH_NONE,
	SPLASH_MAIN_MENU,
	SPLASH_GAME_START,
	SPLASH_LEVEL_COMPLETE,
	SPLASH_GAME_OVER,
	SPLASH_PAUSE
} SplashType;

// ASCII to obstacle type mapping
typedef struct {
	char ascii_char;
	ObstacleType type;
} MapLegend;

// Floating damage numbers
typedef struct {
	Vector2 position;
	int damage;
	float lifetime;
	float alpha;
	bool active;
} FloatingDamage;

// Simple particle for hit effects
typedef struct {
	Vector2 position;
	Vector2 velocity;
	float lifetime;
	Color color;
	bool active;
} HitParticle;

// Explosion effect structure
typedef struct {
	Vector2 position;
	float timer;
	bool active;
	Color color;
	float particles[EXPLOSION_PARTICLES][4]; // x, y, vel_x, vel_y for each particle
} Explosion;

// Obstacle structure
typedef struct {
	Vector2 position;
	ObstacleType type;
	bool active;
	Color color;
	int health;
	int max_health;
	float timer;
	int direction;
	bool triggered;
	Vector2 linked_pos;
	float effect_radius;
	bool state;
	float effect_timer;
	int uses_remaining;
	bool is_block_tanks;    // NEW: Does this obstacle block tank movement?
	bool is_block_bullets;  // NEW: Does this obstacle block bullets?
} Obstacle;

// Bullet structure
typedef struct {
	Vector2 position;
	Vector2 velocity;
	bool active;
	Color color;
	int team; // 0 = player1, 1 = player2, 2 = enemy
	int damage;
	float lifetime;
	float last_teleport_time;
} Bullet;

// Tank structure
typedef struct {
	Vector2 position;
	float rotation;
	float cannon_rotation;
	Color body_color;
	Color cannon_color;

	int key_forward;
	int key_backward;
	int key_turn_left;
	int key_turn_right;
	int key_shoot;

	bool is_player_controlled;
	float reload_timer;
	int team;
	bool active;
	int level;
	float ai_timer;
	int ai_direction;

	// Enhanced fields
	Vector2 velocity;
	float slide_friction;
	bool on_ice;
	bool on_conveyor;
	Vector2 conveyor_force;
	float speed_multiplier;
	bool has_shield;
	float shield_timer;
	float rapid_fire_timer;
	float healing_timer;
	Vector2 last_safe_position;
	float last_teleport_time;
} Tank;

// Game state structure
typedef struct {
	int player1_score;
	int player2_score;
	float enemy_spawn_timer;
	bool game_over;
	bool both_players_dead;
	int current_level;
	int enemies_killed_this_level;
	int enemy_base_level;
	bool level_complete;
	float level_complete_timer;

	// Power-up management
	float power_up_spawn_timer;
	float healing_respawn_timer;
	float ammo_respawn_timer;
	float shield_respawn_timer;
	int active_healing_stations;
	int active_ammo_depots;
	int active_shield_generators;
} GameState;

// Splash screen structure
typedef struct {
	SplashType type;
	bool active;
	float timer;
	float fade_alpha;
	int level_number;
	char title_text[64];
	char subtitle_text[64];
	Color title_color;
	Color subtitle_color;
	bool show_continue_prompt;
	float input_cooldown;
} SplashScreen;

#endif // GAME_TYPES_H