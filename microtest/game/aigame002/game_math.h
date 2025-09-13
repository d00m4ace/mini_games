#ifndef GAME_MATH_H
#define GAME_MATH_H

// Function declarations
float get_tank_speed_multiplier(int level);
int get_max_tank_level_for_game_level(int game_level);
float get_bullet_speed_multiplier(int level);
int calculate_damage(int tank_level);
int get_enemies_per_level(int level);
int get_max_active_enemies(int level);
Vector2 align_to_grid(Vector2 position);
Vector2 get_grid_position(int grid_x, int grid_y);
bool is_position_free_for_tank(Vector2 position);

// Implementation
float get_tank_speed_multiplier(int level)
{
	float base_multiplier = 1.0f + (level - 1) * 0.015f;
	float log_component = logf(1.0f + (level - 1) * 0.1f) * 0.3f;
	float multiplier = base_multiplier + log_component;
	return (multiplier > 2.5f) ? 2.5f : multiplier;
}

int get_max_tank_level_for_game_level(int game_level)
{
	int max_level = game_level + 10;
	return (max_level > MAX_TANK_LEVEL) ? MAX_TANK_LEVEL : max_level;
}

float get_bullet_speed_multiplier(int level)
{
	float base_multiplier = 1.0f + (level - 1) * 0.02f;
	float log_component = logf(1.0f + (level - 1) * 0.15f) * 0.2f;
	float multiplier = base_multiplier + log_component;
	return (multiplier > 3.0f) ? 3.0f : multiplier;
}

int calculate_damage(int tank_level)
{
	int damage = tank_level / 3;
	return (damage < 1) ? 1 : damage;
}

int get_enemies_per_level(int level)
{
	if(level <= 1) return 8;
	float scaled = 8.0f * powf(1.3f, (float)(level - 1));
	return (int)roundf(scaled);
}

int get_max_active_enemies(int level)
{
	int enemies_per_level = get_enemies_per_level(level);
	return (enemies_per_level < MAX_ENEMY_TANKS) ? enemies_per_level : MAX_ENEMY_TANKS;
}

Vector2 align_to_grid(Vector2 position)
{
	return (Vector2) {
		floorf(position.x / BLOCK_SIZE) * BLOCK_SIZE,
		floorf(position.y / BLOCK_SIZE) * BLOCK_SIZE
	};
}

Vector2 get_grid_position(int grid_x, int grid_y)
{
	return (Vector2) { grid_x * BLOCK_SIZE, grid_y * BLOCK_SIZE };
}

bool is_position_free_for_tank(Vector2 position)
{
	Rectangle tank_rect = { position.x, position.y, TANK_SIZE, TANK_SIZE };
	// This will be defined in game_obstacles.h
	extern bool check_obstacle_collision(Rectangle rect, bool is_tank);
	return !check_obstacle_collision(tank_rect, true);
}

#endif // GAME_MATH_H