#ifndef GAME_POWERUPS_H
#define GAME_POWERUPS_H

// Function declarations
Vector2 find_empty_cell_for_power_up(void);
int count_active_power_ups(ObstacleType type);
void spawn_random_power_up(ObstacleType type);
void update_power_up_spawning(float delta_time);

// Implementation
Vector2 find_empty_cell_for_power_up(void)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];
	extern Tank player1_tank, player2_tank;

	for(int attempts = 0; attempts < 100; attempts++) {
		int x = 2 + (rand() % (MAP_WIDTH - 4));
		int y = 2 + (rand() % (MAP_HEIGHT - 4));
		Vector2 world_pos = map_grid_to_world(x, y);

		Rectangle test_rect = { world_pos.x, world_pos.y, BLOCK_SIZE, BLOCK_SIZE };
		bool is_empty = true;

		if(map_obstacles[x][y].active) {
			is_empty = false;
		}
		else {
			if(map_obstacles[x][y].type == OBSTACLE_STEEL_WALL && map_obstacles[x][y].timer > 0) {
				is_empty = false;
			}
		}

		if(is_empty) {
			Vector2 test_center = { world_pos.x + BLOCK_SIZE / 2, world_pos.y + BLOCK_SIZE / 2 };
			bool too_close = false;

			if(player1_tank.active) {
				Vector2 p1_center = { player1_tank.position.x + TANK_SIZE / 2, player1_tank.position.y + TANK_SIZE / 2 };
				if(Vector2Distance(test_center, p1_center) < BLOCK_SIZE * 2) {
					too_close = true;
				}
			}

			if(!too_close && player2_tank.active) {
				Vector2 p2_center = { player2_tank.position.x + TANK_SIZE / 2, player2_tank.position.y + TANK_SIZE / 2 };
				if(Vector2Distance(test_center, p2_center) < BLOCK_SIZE * 2) {
					too_close = true;
				}
			}

			if(!too_close) {
				return world_pos;
			}
		}
	}

	return map_grid_to_world(MAP_WIDTH / 2, MAP_HEIGHT / 2);
}

int count_active_power_ups(ObstacleType type)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];
	int count = 0;
	for(int x = 0; x < MAP_WIDTH; x++) {
		for(int y = 0; y < MAP_HEIGHT; y++) {
			if(map_obstacles[x][y].active && map_obstacles[x][y].type == type) {
				count++;
			}
		}
	}
	return count;
}

void spawn_random_power_up(ObstacleType type)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];
	Vector2 spawn_pos = find_empty_cell_for_power_up();
	Vector2 map_coords = world_to_map_grid(spawn_pos);
	int map_x = (int)map_coords.x;
	int map_y = (int)map_coords.y;

	if(map_x >= 0 && map_x < MAP_WIDTH && map_y >= 0 && map_y < MAP_HEIGHT) {
		map_obstacles[map_x][map_y].position = spawn_pos;
		map_obstacles[map_x][map_y].type = type;
		map_obstacles[map_x][map_y].active = true;
		map_obstacles[map_x][map_y].triggered = false;
		map_obstacles[map_x][map_y].timer = 0;
		map_obstacles[map_x][map_y].effect_timer = 0;
		map_obstacles[map_x][map_y].state = false;
		map_obstacles[map_x][map_y].linked_pos = (Vector2){ 0, 0 };
		map_obstacles[map_x][map_y].direction = rand() % 4;

		switch(type) {
		case OBSTACLE_HEALING_STATION:
			map_obstacles[map_x][map_y].color = WHITE;
			map_obstacles[map_x][map_y].health = 3;
			map_obstacles[map_x][map_y].max_health = 3;
			map_obstacles[map_x][map_y].uses_remaining = 1;
			map_obstacles[map_x][map_y].effect_radius = BLOCK_SIZE;
			map_obstacles[map_x][map_y].is_block_tanks = false;
			map_obstacles[map_x][map_y].is_block_bullets = false;
			break;

		case OBSTACLE_AMMO_DEPOT:
			map_obstacles[map_x][map_y].color = (Color){ 139, 69, 19, 255 };
			map_obstacles[map_x][map_y].health = 2;
			map_obstacles[map_x][map_y].max_health = 2;
			map_obstacles[map_x][map_y].uses_remaining = 1;
			map_obstacles[map_x][map_y].effect_radius = BLOCK_SIZE;
			map_obstacles[map_x][map_y].is_block_tanks = false;
			map_obstacles[map_x][map_y].is_block_bullets = false;
			break;

		case OBSTACLE_SHIELD_GENERATOR:
			map_obstacles[map_x][map_y].color = (Color){ 100, 100, 150, 255 };
			map_obstacles[map_x][map_y].health = 4;
			map_obstacles[map_x][map_y].max_health = 4;
			map_obstacles[map_x][map_y].uses_remaining = 1;
			map_obstacles[map_x][map_y].effect_radius = BLOCK_SIZE;
			map_obstacles[map_x][map_y].is_block_tanks = false;
			map_obstacles[map_x][map_y].is_block_bullets = false;
			break;
		}
	}
}

void update_power_up_spawning(float delta_time)
{
	extern GameState game_state;

	game_state.power_up_spawn_timer -= delta_time;

	if(game_state.healing_respawn_timer > 0) {
		game_state.healing_respawn_timer -= delta_time;
	}
	if(game_state.ammo_respawn_timer > 0) {
		game_state.ammo_respawn_timer -= delta_time;
	}
	if(game_state.shield_respawn_timer > 0) {
		game_state.shield_respawn_timer -= delta_time;
	}

	if(game_state.power_up_spawn_timer <= 0) {
		game_state.power_up_spawn_timer = POWER_UP_SPAWN_CHECK_INTERVAL;

		game_state.active_healing_stations = count_active_power_ups(OBSTACLE_HEALING_STATION);
		game_state.active_ammo_depots = count_active_power_ups(OBSTACLE_AMMO_DEPOT);
		game_state.active_shield_generators = count_active_power_ups(OBSTACLE_SHIELD_GENERATOR);

		if(game_state.active_healing_stations < MAX_POWER_UPS_PER_TYPE &&
			game_state.healing_respawn_timer <= 0) {
			spawn_random_power_up(OBSTACLE_HEALING_STATION);
			game_state.healing_respawn_timer = POWER_UP_RESPAWN_TIME_MIN +
				(rand() % (int)(POWER_UP_RESPAWN_TIME_MAX - POWER_UP_RESPAWN_TIME_MIN));
		}

		if(game_state.active_ammo_depots < MAX_POWER_UPS_PER_TYPE &&
			game_state.ammo_respawn_timer <= 0) {
			spawn_random_power_up(OBSTACLE_AMMO_DEPOT);
			game_state.ammo_respawn_timer = POWER_UP_RESPAWN_TIME_MIN +
				(rand() % (int)(POWER_UP_RESPAWN_TIME_MAX - POWER_UP_RESPAWN_TIME_MIN));
		}

		if(game_state.active_shield_generators < MAX_POWER_UPS_PER_TYPE &&
			game_state.shield_respawn_timer <= 0) {
			spawn_random_power_up(OBSTACLE_SHIELD_GENERATOR);
			game_state.shield_respawn_timer = POWER_UP_RESPAWN_TIME_MIN +
				(rand() % (int)(POWER_UP_RESPAWN_TIME_MAX - POWER_UP_RESPAWN_TIME_MIN));
		}
	}
}

#endif // GAME_POWERUPS_H