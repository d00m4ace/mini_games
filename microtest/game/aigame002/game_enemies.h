#ifndef GAME_ENEMIES_H
#define GAME_ENEMIES_H

// Function declarations
Vector2 get_enemy_spawn_position(void);
void spawn_enemy_tank(void);
int count_active_enemies(void);
void update_enemy_tank(Tank* tank, float delta_time);

// Implementation
Vector2 get_enemy_spawn_position(void)
{
	// Preferred spawn areas (corners and edges with some margin)
	int spawn_areas[12][2] = {
		{2, 2}, {3, 2}, {2, 3},
		{MAP_WIDTH - 3, 2}, {MAP_WIDTH - 4, 2}, {MAP_WIDTH - 3, 3},
		{2, MAP_HEIGHT - 3}, {2, MAP_HEIGHT - 4}, {3, MAP_HEIGHT - 3},
		{MAP_WIDTH - 3, MAP_HEIGHT - 3}, {MAP_WIDTH - 4, MAP_HEIGHT - 3}, {MAP_WIDTH - 3, MAP_HEIGHT - 4}
	};

	// Try preferred spawn areas first
	for(int attempts = 0; attempts < 50; attempts++) {
		int area_index = rand() % 12;
		int spawn_x = spawn_areas[area_index][0];
		int spawn_y = spawn_areas[area_index][1];

		if(is_spawn_position_safe(spawn_x, spawn_y)) {
			Vector2 world_pos = map_grid_to_world(spawn_x, spawn_y);
			return (Vector2) { world_pos.x + (BLOCK_SIZE - TANK_SIZE) / 2, world_pos.y + (BLOCK_SIZE - TANK_SIZE) / 2 };
		}
	}

	// Try edge positions
	for(int attempts = 0; attempts < 100; attempts++) {
		int edge = rand() % 4;
		int spawn_x, spawn_y;

		switch(edge) {
		case 0:
			spawn_x = 2 + (rand() % (MAP_WIDTH - 4));
			spawn_y = 1 + (rand() % 3);
			break;
		case 1:
			spawn_x = MAP_WIDTH - 3 + (rand() % 2);
			spawn_y = 2 + (rand() % (MAP_HEIGHT - 4));
			break;
		case 2:
			spawn_x = 2 + (rand() % (MAP_WIDTH - 4));
			spawn_y = MAP_HEIGHT - 3 + (rand() % 2);
			break;
		case 3:
			spawn_x = 1 + (rand() % 3);
			spawn_y = 2 + (rand() % (MAP_HEIGHT - 4));
			break;
		}

		if(spawn_x >= 0 && spawn_x < MAP_WIDTH && spawn_y >= 0 && spawn_y < MAP_HEIGHT) {
			if(is_spawn_position_safe(spawn_x, spawn_y)) {
				Vector2 world_pos = map_grid_to_world(spawn_x, spawn_y);
				return (Vector2) { world_pos.x + (BLOCK_SIZE - TANK_SIZE) / 2, world_pos.y + (BLOCK_SIZE - TANK_SIZE) / 2 };
			}
		}
	}

	// Try any safe position within map boundaries
	for(int attempts = 0; attempts < 200; attempts++) {
		int spawn_x = 1 + (rand() % (MAP_WIDTH - 2));
		int spawn_y = 1 + (rand() % (MAP_HEIGHT - 2));

		if(is_spawn_position_safe(spawn_x, spawn_y)) {
			Vector2 world_pos = map_grid_to_world(spawn_x, spawn_y);
			return (Vector2) { world_pos.x + (BLOCK_SIZE - TANK_SIZE) / 2, world_pos.y + (BLOCK_SIZE - TANK_SIZE) / 2 };
		}
	}

	// Emergency fallback
	for(int y = 1; y < MAP_HEIGHT - 1; y++) {
		for(int x = 1; x < MAP_WIDTH - 1; x++) {
			if(is_spawn_position_safe(x, y)) {
				Vector2 world_pos = map_grid_to_world(x, y);
				return (Vector2) { world_pos.x + (BLOCK_SIZE - TANK_SIZE) / 2, world_pos.y + (BLOCK_SIZE - TANK_SIZE) / 2 };
			}
		}
	}

	// Ultimate emergency fallback
	Vector2 center_world = map_grid_to_world(MAP_WIDTH / 2, MAP_HEIGHT / 2);
	return (Vector2) { center_world.x + (BLOCK_SIZE - TANK_SIZE) / 2, center_world.y + (BLOCK_SIZE - TANK_SIZE) / 2 };
}

void spawn_enemy_tank(void)
{
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];
	extern GameState game_state;

	for(int i = 0; i < MAX_ENEMY_TANKS; i++) {
		if(!enemy_tanks[i].active) {
			enemy_tanks[i].position = get_enemy_spawn_position();
			enemy_tanks[i].rotation = 90.0f * (rand() % 4);
			enemy_tanks[i].cannon_rotation = enemy_tanks[i].rotation;
			enemy_tanks[i].body_color = RED;
			enemy_tanks[i].cannon_color = DARKRED;
			enemy_tanks[i].is_player_controlled = false;
			enemy_tanks[i].reload_timer = 0.0f;
			enemy_tanks[i].team = 2;
			enemy_tanks[i].active = true;
			enemy_tanks[i].level = game_state.current_level + (rand() % 3);
			enemy_tanks[i].ai_timer = 0.0f;
			enemy_tanks[i].ai_direction = rand() % 4;

			enemy_tanks[i].velocity = (Vector2){ 0, 0 };
			enemy_tanks[i].slide_friction = 1.0f;
			enemy_tanks[i].on_ice = false;
			enemy_tanks[i].on_conveyor = false;
			enemy_tanks[i].conveyor_force = (Vector2){ 0, 0 };
			enemy_tanks[i].speed_multiplier = 1.0f;
			enemy_tanks[i].has_shield = false;
			enemy_tanks[i].shield_timer = 0.0f;
			enemy_tanks[i].rapid_fire_timer = 0.0f;
			enemy_tanks[i].healing_timer = 0.0f;
			enemy_tanks[i].last_safe_position = enemy_tanks[i].position;
			enemy_tanks[i].last_teleport_time = 0.0f;

			break;
		}
	}
}

int count_active_enemies(void)
{
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];
	int count = 0;
	for(int i = 0; i < MAX_ENEMY_TANKS; i++) {
		if(enemy_tanks[i].active) count++;
	}
	return count;
}

void update_enemy_tank(Tank* tank, float delta_time)
{
    extern Tank player1_tank, player2_tank;
    extern GameState game_state;

    if(!tank->active) return;

    // All the AI logic for determining movement and shooting
    Vector2 movement = { 0, 0 };
    Vector2 enemy_center = { tank->position.x + TANK_SIZE / 2, tank->position.y + TANK_SIZE / 2 };

    // Update AI timer
    tank->ai_timer += delta_time;

    // Find closest active player
    Tank* target_player = NULL;
    float min_distance = INFINITY;

    if(player1_tank.active) {
        Vector2 p1_center = { player1_tank.position.x + TANK_SIZE / 2, player1_tank.position.y + TANK_SIZE / 2 };
        float dist = Vector2Distance(enemy_center, p1_center);
        if(dist < min_distance) {
            min_distance = dist;
            target_player = &player1_tank;
        }
    }

    if(player2_tank.active) {
        Vector2 p2_center = { player2_tank.position.x + TANK_SIZE / 2, player2_tank.position.y + TANK_SIZE / 2 };
        float dist = Vector2Distance(enemy_center, p2_center);
        if(dist < min_distance) {
            min_distance = dist;
            target_player = &player2_tank;
        }
    }

    bool player_in_range = (target_player != NULL && min_distance <= 192.0f);
    bool path_blocked = false;

    if(player_in_range) {
        Vector2 target_center = { target_player->position.x + TANK_SIZE / 2, target_player->position.y + TANK_SIZE / 2 };
        Vector2 direction = Vector2Subtract(target_center, enemy_center);

        int steps = (int)(min_distance / 8);
        for(int i = 1; i < steps; i++) {
            Vector2 check_pos = Vector2Add(enemy_center, Vector2Scale(Vector2Normalize(direction), i * 8));
            Vector2 map_coords = world_to_map_grid(check_pos);
            int map_x = (int)map_coords.x;
            int map_y = (int)map_coords.y;

            if(map_x >= 0 && map_x < MAP_WIDTH && map_y >= 0 && map_y < MAP_HEIGHT) {
                extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];
                if(map_obstacles[map_x][map_y].active &&
                    map_obstacles[map_x][map_y].type == OBSTACLE_STEEL_WALL) {
                    path_blocked = true;
                    break;
                }
            }
        }

        if(!path_blocked) {
            float dx = target_center.x - enemy_center.x;
            float dy = target_center.y - enemy_center.y;
            float threshold = 16.0f;

            if(tank->ai_timer >= ENEMY_MOVE_TIME * 0.8f) {
                if(fabsf(dx) > fabsf(dy) + threshold) {
                    if(dx > 0) {
                        tank->rotation = 0.0f;
                        tank->ai_direction = 1;
                    }
                    else {
                        tank->rotation = 180.0f;
                        tank->ai_direction = 3;
                    }
                }
                else if(fabsf(dy) > fabsf(dx) + threshold) {
                    if(dy > 0) {
                        tank->rotation = 90.0f;
                        tank->ai_direction = 2;
                    }
                    else {
                        tank->rotation = 270.0f;
                        tank->ai_direction = 0;
                    }
                }

                tank->ai_timer = 0.0f;
            }

            // AI shooting logic
            if(tank->reload_timer <= 0 && rand() % 100 < 8) {
                Vector2 tank_center = { tank->position.x + TANK_SIZE / 2, tank->position.y + TANK_SIZE / 2 };
                float cannon_rad = tank->cannon_rotation * DEG2RAD;
                Vector2 bullet_start = Vector2Add(tank_center, Vector2Scale((Vector2) { cosf(cannon_rad), sinf(cannon_rad) }, CANNON_LENGTH - 2));

                int damage = calculate_damage(tank->level);
                add_bullet_with_team_and_level(bullet_start, tank->cannon_rotation, tank->cannon_color, 2, damage, tank->level);
                tank->reload_timer = TANK_RELOAD_TIME * 1.5f;
            }
        }
        else {
            if(tank->ai_timer >= ENEMY_MOVE_TIME * 0.5f) {
                tank->ai_direction = rand() % 4;
                tank->ai_timer = 0.0f;
            }
        }
    }
    else {
        if(tank->ai_timer >= ENEMY_MOVE_TIME) {
            tank->ai_direction = rand() % 4;
            tank->ai_timer = 0.0f;
        }

        if(tank->reload_timer <= 0 && rand() % 100 < 1) {
            Vector2 tank_center = { tank->position.x + TANK_SIZE / 2, tank->position.y + TANK_SIZE / 2 };
            float cannon_rad = tank->cannon_rotation * DEG2RAD;
            Vector2 bullet_start = Vector2Add(tank_center, Vector2Scale((Vector2) { cosf(cannon_rad), sinf(cannon_rad) }, CANNON_LENGTH - 2));

            int damage = calculate_damage(tank->level);
            add_bullet_with_team_and_level(bullet_start, tank->cannon_rotation, tank->cannon_color, 2, damage, tank->level);
            tank->reload_timer = TANK_RELOAD_TIME * 2;
        }
    }

    if(!player_in_range || path_blocked) {
        switch(tank->ai_direction) {
        case 0: tank->rotation = 270.0f; break;
        case 1: tank->rotation = 0.0f; break;
        case 2: tank->rotation = 90.0f; break;
        case 3: tank->rotation = 180.0f; break;
        }
    }

    // Calculate AI movement based on current direction and speed
    float rad = tank->rotation * DEG2RAD;
    float base_speed_multiplier = player_in_range && !path_blocked ? 0.7f : 0.5f;
    float level_speed_multiplier = get_tank_speed_multiplier(tank->level);
    float final_speed_multiplier = base_speed_multiplier * level_speed_multiplier;

    movement.x = cosf(rad) * TANK_SPEED * final_speed_multiplier * delta_time;
    movement.y = sinf(rad) * TANK_SPEED * final_speed_multiplier * delta_time;

    // Store old position for collision handling in the shared update function
    Vector2 old_position = tank->position;

    // Use the shared tank update function with AI movement
    update_tank_with_ai_movement(tank, delta_time, movement, true);

    // AI-specific collision response (change direction if stuck)
    if(Vector2Distance(tank->position, old_position) < 1.0f && Vector2Length(movement) > 1.0f) {
        if(player_in_range && !path_blocked && target_player != NULL) {
            Vector2 target_center = { target_player->position.x + TANK_SIZE / 2, target_player->position.y + TANK_SIZE / 2 };
            float dx = target_center.x - enemy_center.x;
            float dy = target_center.y - enemy_center.y;

            if(tank->ai_direction == 1 || tank->ai_direction == 3) {
                tank->ai_direction = (dy > 0) ? 2 : 0;
            }
            else {
                tank->ai_direction = (dx > 0) ? 1 : 3;
            }
        }
        else {
            tank->ai_direction = rand() % 4;
        }
        tank->ai_timer = 0.0f;
    }

    // Additional boundary safety check for AI tanks
    float min_x = MAP_OFFSET_X;
    float min_y = MAP_OFFSET_Y;
    float max_x = MAP_OFFSET_X + (MAP_WIDTH * BLOCK_SIZE) - TANK_SIZE;
    float max_y = MAP_OFFSET_Y + (MAP_HEIGHT * BLOCK_SIZE) - TANK_SIZE;

    if(tank->position.x < min_x || tank->position.x > max_x ||
        tank->position.y < min_y || tank->position.y > max_y) {

        // AI tank went out of bounds - respawn it
        tank->position = get_enemy_spawn_position();
        tank->velocity = (Vector2){ 0, 0 };
        tank->ai_direction = rand() % 4;
        tank->ai_timer = 0.0f;
    }
}

#endif // GAME_ENEMIES_H