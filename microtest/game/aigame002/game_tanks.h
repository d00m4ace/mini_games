#ifndef GAME_TANKS_H
#define GAME_TANKS_H

// Function declarations
void draw_tank_level(Tank tank);
void draw_tank(Tank tank);
Tank create_ai_tank(Vector2 position, Color body_color, Color cannon_color);
void update_tank(Tank* tank, float delta_time);
void update_tank_with_ai_movement(Tank* tank, float delta_time, Vector2 ai_movement, bool is_ai_controlled);

// Implementation
void draw_tank_level(Tank tank)
{
	if(!tank.active) return;

	Vector2 level_pos = {
		tank.position.x + TANK_SIZE - 12,
		tank.position.y + TANK_SIZE - 12
	};

	char level_text[4];
	sprintf(level_text, "%d", tank.level);

	int text_width = MeasureText(level_text, 10);
	DrawCircleV((Vector2) { level_pos.x + text_width / 2 + 2, level_pos.y + 6 }, 9, BLACK);
	DrawCircleV((Vector2) { level_pos.x + text_width / 2 + 2, level_pos.y + 6 }, 8, WHITE);

	DrawText(level_text, level_pos.x, level_pos.y, 10, BLACK);
}

void draw_tank(Tank tank)
{
	Vector2 tank_center = { tank.position.x + TANK_SIZE / 2, tank.position.y + TANK_SIZE / 2 };

	// Draw shield effect first (behind tank)
	if(tank.has_shield) {
		float shield_pulse = 0.7f + 0.3f * sinf(GetTime() * 8);
		DrawCircleLines(tank_center.x, tank_center.y, TANK_SIZE / 2 + 4, Fade(SKYBLUE, shield_pulse));
		DrawCircleLines(tank_center.x, tank_center.y, TANK_SIZE / 2 + 6, Fade(WHITE, shield_pulse * 0.5f));
	}

	// Draw tank body
	DrawRectangle(tank.position.x, tank.position.y, TANK_SIZE, TANK_SIZE, tank.body_color);
	DrawRectangleLines(tank.position.x, tank.position.y, TANK_SIZE, TANK_SIZE, BLACK);
	DrawRectangleLines(tank.position.x + 1, tank.position.y + 1, TANK_SIZE - 2, TANK_SIZE - 2, BLACK);

	// Draw tank tracks based on direction
	if(tank.rotation == 0.0f || tank.rotation == 180.0f) {
		DrawRectangle(tank.position.x + 3, tank.position.y - 2, TANK_SIZE - 6, 3, DARKGRAY);
		DrawRectangle(tank.position.x + 3, tank.position.y + TANK_SIZE - 1, TANK_SIZE - 6, 3, DARKGRAY);

		for(int i = 0; i < 4; i++) {
			DrawRectangle(tank.position.x + 5 + i * 5, tank.position.y - 1, 2, 1, BLACK);
			DrawRectangle(tank.position.x + 5 + i * 5, tank.position.y + TANK_SIZE, 2, 1, BLACK);
		}
	}
	else {
		DrawRectangle(tank.position.x - 2, tank.position.y + 3, 3, TANK_SIZE - 6, DARKGRAY);
		DrawRectangle(tank.position.x + TANK_SIZE - 1, tank.position.y + 3, 3, TANK_SIZE - 6, DARKGRAY);

		for(int i = 0; i < 4; i++) {
			DrawRectangle(tank.position.x - 1, tank.position.y + 5 + i * 5, 1, 2, BLACK);
			DrawRectangle(tank.position.x + TANK_SIZE, tank.position.y + 5 + i * 5, 1, 2, BLACK);
		}
	}

	// Draw cannon barrel
	float cannon_rad = tank.cannon_rotation * DEG2RAD;
	Vector2 cannon_direction = { cosf(cannon_rad), sinf(cannon_rad) };
	Vector2 cannon_end = Vector2Add(tank_center, Vector2Scale(cannon_direction, CANNON_LENGTH - 2));

	// Rapid fire effect
	Color cannon_color = tank.cannon_color;
	if(tank.rapid_fire_timer > 0) {
		float pulse = sinf(GetTime() * 12);
		cannon_color = pulse > 0 ? YELLOW : tank.cannon_color;
	}

	DrawLineEx(tank_center, cannon_end, CANNON_WIDTH - 1, cannon_color);

	// Draw turret in center
	DrawCircleV(tank_center, 6, tank.body_color);
	DrawCircleLinesV(tank_center, 6, BLACK);

	// Draw cannon tip
	DrawCircleV(cannon_end, 1, BLACK);

	// Draw tank center
	DrawCircleV(tank_center, 2, DARKGRAY);

	// Draw status effects
	if(tank.healing_timer > 0) {
		for(int p = 0; p < 3; p++) {
			float angle = GetTime() * 6 + p * 2;
			int x = tank_center.x + cosf(angle) * (8 + p * 2);
			int y = tank_center.y + sinf(angle) * (8 + p * 2);
			DrawPixel(x, y, GREEN);
		}
	}

	if(tank.on_ice) {
		for(int p = 0; p < 5; p++) {
			int x = tank.position.x + (rand() % TANK_SIZE);
			int y = tank.position.y + TANK_SIZE + p;
			if(rand() % 3 == 0) {
				DrawPixel(x, y, SKYBLUE);
			}
		}
	}

	if(tank.on_conveyor) {
		for(int p = 0; p < 3; p++) {
			float particle_time = GetTime() * 8 + p * 0.5f;
			int offset_x = (int)(sinf(particle_time) * 4);
			int offset_y = (int)(cosf(particle_time) * 4);
			int x = tank.position.x + TANK_SIZE / 2 + offset_x;
			int y = tank.position.y + TANK_SIZE + 2 + offset_y;
			DrawPixel(x, y, YELLOW);
		}
	}
}

Tank create_ai_tank(Vector2 position, Color body_color, Color cannon_color)
{
	Tank ai_tank = { 0 };
	ai_tank.position = position;
	ai_tank.rotation = 0.0f;
	ai_tank.cannon_rotation = 0.0f;
	ai_tank.body_color = body_color;
	ai_tank.cannon_color = cannon_color;
	ai_tank.is_player_controlled = false;
	ai_tank.key_forward = 0;
	ai_tank.key_backward = 0;
	ai_tank.key_turn_left = 0;
	ai_tank.key_turn_right = 0;
	ai_tank.key_shoot = 0;
	return ai_tank;
}

void update_tank(Tank* tank, float delta_time)
{
    update_tank_with_ai_movement(tank, delta_time, (Vector2) { 0, 0 }, false);
}

bool test_collision_at_position(Vector2 test_pos)
{
    // Check map boundaries first - ensure tank stays completely within map
    if(test_pos.x < MAP_OFFSET_X ||
        test_pos.y < MAP_OFFSET_Y ||
        test_pos.x + TANK_SIZE > MAP_OFFSET_X + MAP_WIDTH * BLOCK_SIZE ||
        test_pos.y + TANK_SIZE > MAP_OFFSET_Y + MAP_HEIGHT * BLOCK_SIZE) {
        return true; // Collision with boundary
    }

    // Check obstacle collision
    Rectangle test_rect = { test_pos.x, test_pos.y, TANK_SIZE, TANK_SIZE };
    return check_obstacle_collision_enhanced(test_rect, true, NULL);
}

void update_tank_with_ai_movement(Tank* tank, float delta_time, Vector2 ai_movement, bool is_ai_controlled)
{
    extern GameState game_state;

    Vector2 movement = { 0, 0 };
    Vector2 old_position = tank->position;

    // Reset per-frame effects
    tank->on_ice = false;
    tank->on_conveyor = false;
    tank->conveyor_force = (Vector2){ 0, 0 };
    tank->speed_multiplier = 1.0f;
    tank->slide_friction = 1.0f;

    // Update timers
    if(tank->reload_timer > 0) {
        tank->reload_timer -= delta_time;
    }
    if(tank->shield_timer > 0) {
        tank->shield_timer -= delta_time;
        if(tank->shield_timer <= 0) {
            tank->has_shield = false;
        }
    }
    if(tank->rapid_fire_timer > 0) {
        tank->rapid_fire_timer -= delta_time;
    }
    if(tank->healing_timer > 0) {
        float previous_timer = tank->healing_timer;
        tank->healing_timer -= delta_time;

        int max_allowed_level = get_max_tank_level_for_game_level(game_state.current_level);

        if((int)previous_timer != (int)tank->healing_timer && tank->level < max_allowed_level) {
            tank->level++;
        }

        if(tank->healing_timer <= 0.0f || tank->level >= max_allowed_level) {
            tank->healing_timer = 0.0f;
        }
    }

    // Apply obstacle effects at current position BEFORE movement
    Rectangle current_tank_rect = { tank->position.x, tank->position.y, TANK_SIZE, TANK_SIZE };
    check_obstacle_collision_enhanced(current_tank_rect, true, tank);

    // Calculate speed multiplier based on tank level and effects
    float base_speed_multiplier = get_tank_speed_multiplier(tank->level);
    float final_speed_multiplier = base_speed_multiplier * tank->speed_multiplier;

    // Handle input and calculate movement
    if(is_ai_controlled) {
        // Use AI-provided movement
        movement = ai_movement;
    }
    else if(tank->is_player_controlled) {
        // Player controlled tank - only 4 directions
        if(tank->key_forward && IsKeyPressed(tank->key_forward)) {
            tank->rotation = 270.0f;
        }
        if(tank->key_backward && IsKeyPressed(tank->key_backward)) {
            tank->rotation = 90.0f;
        }
        if(tank->key_turn_left && IsKeyPressed(tank->key_turn_left)) {
            tank->rotation = 180.0f;
        }
        if(tank->key_turn_right && IsKeyPressed(tank->key_turn_right)) {
            tank->rotation = 0.0f;
        }

        // Movement in the direction tank is facing
        if((tank->key_forward && IsKeyDown(tank->key_forward)) ||
            (tank->key_backward && IsKeyDown(tank->key_backward)) ||
            (tank->key_turn_left && IsKeyDown(tank->key_turn_left)) ||
            (tank->key_turn_right && IsKeyDown(tank->key_turn_right))) {

            float rad = tank->rotation * DEG2RAD;
            movement.x = cosf(rad) * TANK_SPEED * final_speed_multiplier * delta_time;
            movement.y = sinf(rad) * TANK_SPEED * final_speed_multiplier * delta_time;
        }

        // Shooting with rapid fire consideration
        float current_reload_time = (tank->rapid_fire_timer > 0) ? TANK_RELOAD_TIME * 0.3f : TANK_RELOAD_TIME;
        if(tank->key_shoot && IsKeyPressed(tank->key_shoot) && tank->reload_timer <= 0) {
            Vector2 tank_center = { tank->position.x + TANK_SIZE / 2, tank->position.y + TANK_SIZE / 2 };
            float cannon_rad = tank->cannon_rotation * DEG2RAD;
            Vector2 bullet_start = Vector2Add(tank_center, Vector2Scale((Vector2) { cosf(cannon_rad), sinf(cannon_rad) }, CANNON_LENGTH - 2));

            int damage = calculate_damage(tank->level);
            add_bullet_with_team_and_level(bullet_start, tank->cannon_rotation, tank->cannon_color, tank->team, damage, tank->level);
            tank->reload_timer = current_reload_time;
        }
    }

    // Apply conveyor belt force AFTER player input but BEFORE ice physics
    if(tank->on_conveyor) {
        float conveyor_resistance = get_tank_speed_multiplier(tank->level) * tank->speed_multiplier;
        float resistance_factor = conveyor_resistance / 2.0f;
        if(resistance_factor > 0.5f) resistance_factor = 0.5f;

        Vector2 effective_conveyor_force = Vector2Scale(tank->conveyor_force, 1.0f - resistance_factor);
        Vector2 conveyor_movement = Vector2Scale(effective_conveyor_force, delta_time);

        if(Vector2Length(movement) > 0) {
            Vector2 normalized_movement = Vector2Normalize(movement);
            Vector2 normalized_conveyor = Vector2Normalize(tank->conveyor_force);
            float dot_product = Vector2DotProduct(normalized_movement, normalized_conveyor);

            if(dot_product < -0.5f) {
                conveyor_movement = Vector2Scale(conveyor_movement, 0.6f);
            }
        }

        movement = Vector2Add(movement, conveyor_movement);
    }

    // Apply ice sliding physics
    if(tank->on_ice && tank->slide_friction < 1.0f) {
        tank->velocity = Vector2Scale(tank->velocity, tank->slide_friction);
        movement = Vector2Add(movement, Vector2Scale(tank->velocity, delta_time));

        Vector2 new_velocity = Vector2Scale(movement, 1.0f / delta_time);
        tank->velocity = Vector2Lerp(tank->velocity, new_velocity, 0.3f);
    }
    else {
        tank->velocity = (Vector2){ 0, 0 };
        tank->slide_friction = 1.0f;
    }

    // Apply movement
    Vector2 new_position = Vector2Add(tank->position, movement);
    tank->position = new_position;

    // Check collision at new position using simplified system
    Rectangle new_tank_rect = { tank->position.x, tank->position.y, TANK_SIZE, TANK_SIZE };
    bool collision_detected = check_obstacle_collision_enhanced(new_tank_rect, true, tank);

    // Handle collision with sliding
    if(collision_detected) {
        // Reset to old position
        tank->position = old_position;

        Vector2 original_movement = Vector2Subtract(new_position, old_position);
        bool movement_resolved = false;

        // Try sliding with offsets
        float offsets[] = { 1.0f, 2.0f, -1.0f, -2.0f };

        // Calculate sliding direction preference based on tank center relative to tile center
        Vector2 tank_center = { tank->position.x + TANK_SIZE / 2, tank->position.y + TANK_SIZE / 2 };
        Vector2 tank_map_coords = world_to_map_grid(tank_center);
        int tile_x = (int)tank_map_coords.x;
        int tile_y = (int)tank_map_coords.y;

        Vector2 tile_world_pos = map_grid_to_world(tile_x, tile_y);
        Vector2 tile_center = { tile_world_pos.x + BLOCK_SIZE / 2, tile_world_pos.y + BLOCK_SIZE / 2 };
        Vector2 offset_from_tile_center = Vector2Subtract(tank_center, tile_center);

        // If moving horizontally, try vertical sliding
        if(!movement_resolved && fabsf(original_movement.x) > 0.1f) {
            bool prefer_up = offset_from_tile_center.y > 0;

            // Try preferred direction first
            int start_idx = prefer_up ? 2 : 0;  // 2,3 for up (-), 0,1 for down (+)
            int end_idx = prefer_up ? 4 : 2;

            for(int i = start_idx; i < end_idx && !movement_resolved; i++) {
                Vector2 test_pos = (Vector2){
                    old_position.x + original_movement.x * 0.5f,
                    old_position.y + offsets[i]
                };

                Rectangle test_rect = { test_pos.x, test_pos.y, TANK_SIZE, TANK_SIZE };
                if(!check_obstacle_collision_enhanced(test_rect, true, NULL)) {
                    tank->position = test_pos;
                    movement_resolved = true;
                }
            }

            // Try just vertical sliding if mixed didn't work
            if(!movement_resolved) {
                for(int i = start_idx; i < end_idx && !movement_resolved; i++) {
                    Vector2 test_pos = (Vector2){ old_position.x, old_position.y + offsets[i] };
                    Rectangle test_rect = { test_pos.x, test_pos.y, TANK_SIZE, TANK_SIZE };
                    if(!check_obstacle_collision_enhanced(test_rect, true, NULL)) {
                        tank->position = test_pos;
                        movement_resolved = true;
                    }
                }
            }

            // Try opposite direction
            if(!movement_resolved) {
                int opp_start = prefer_up ? 0 : 2;
                int opp_end = prefer_up ? 2 : 4;

                for(int i = opp_start; i < opp_end && !movement_resolved; i++) {
                    Vector2 test_pos = (Vector2){
                        old_position.x + original_movement.x * 0.5f,
                        old_position.y + offsets[i]
                    };

                    Rectangle test_rect = { test_pos.x, test_pos.y, TANK_SIZE, TANK_SIZE };
                    if(!check_obstacle_collision_enhanced(test_rect, true, NULL)) {
                        tank->position = test_pos;
                        movement_resolved = true;
                    }
                }
            }
        }

        // If moving vertically, try horizontal sliding
        if(!movement_resolved && fabsf(original_movement.y) > 0.1f) {
            bool prefer_left = offset_from_tile_center.x > 0;

            int start_idx = prefer_left ? 2 : 0;  // 2,3 for left (-), 0,1 for right (+)
            int end_idx = prefer_left ? 4 : 2;

            for(int i = start_idx; i < end_idx && !movement_resolved; i++) {
                Vector2 test_pos = (Vector2){
                    old_position.x + offsets[i],
                    old_position.y + original_movement.y * 0.5f
                };

                Rectangle test_rect = { test_pos.x, test_pos.y, TANK_SIZE, TANK_SIZE };
                if(!check_obstacle_collision_enhanced(test_rect, true, NULL)) {
                    tank->position = test_pos;
                    movement_resolved = true;
                }
            }

            // Try just horizontal sliding if mixed didn't work
            if(!movement_resolved) {
                for(int i = start_idx; i < end_idx && !movement_resolved; i++) {
                    Vector2 test_pos = (Vector2){ old_position.x + offsets[i], old_position.y };
                    Rectangle test_rect = { test_pos.x, test_pos.y, TANK_SIZE, TANK_SIZE };
                    if(!check_obstacle_collision_enhanced(test_rect, true, NULL)) {
                        tank->position = test_pos;
                        movement_resolved = true;
                    }
                }
            }

            // Try opposite direction
            if(!movement_resolved) {
                int opp_start = prefer_left ? 0 : 2;
                int opp_end = prefer_left ? 2 : 4;

                for(int i = opp_start; i < opp_end && !movement_resolved; i++) {
                    Vector2 test_pos = (Vector2){
                        old_position.x + offsets[i],
                        old_position.y + original_movement.y * 0.5f
                    };

                    Rectangle test_rect = { test_pos.x, test_pos.y, TANK_SIZE, TANK_SIZE };
                    if(!check_obstacle_collision_enhanced(test_rect, true, NULL)) {
                        tank->position = test_pos;
                        movement_resolved = true;
                    }
                }
            }
        }

        // Adjust velocity based on collision resolution
        if(!movement_resolved) {
            tank->velocity = Vector2Scale(tank->velocity, 0.5f);
        }
        else {
            tank->velocity = Vector2Scale(tank->velocity, 0.8f);
        }
    }

    // Cannon always points in same direction as tank body
    tank->cannon_rotation = tank->rotation;

    // Keep tank within map boundaries - CORRECTED VERSION
    Vector2 tank_center = { tank->position.x + TANK_SIZE / 2, tank->position.y + TANK_SIZE / 2 };
    Vector2 map_coords = world_to_map_grid(tank_center);
    int tank_map_x = (int)map_coords.x;
    int tank_map_y = (int)map_coords.y;

    // Calculate world boundaries more precisely
    float min_x = MAP_OFFSET_X;
    float min_y = MAP_OFFSET_Y;
    float max_x = MAP_OFFSET_X + (MAP_WIDTH * BLOCK_SIZE) - TANK_SIZE;
    float max_y = MAP_OFFSET_Y + (MAP_HEIGHT * BLOCK_SIZE) - TANK_SIZE;

    // Apply strict boundary constraints
    if(tank->position.x < min_x) {
        tank->position.x = min_x;
        tank->velocity.x = 0;
    }
    if(tank->position.y < min_y) {
        tank->position.y = min_y;
        tank->velocity.y = 0;
    }
    if(tank->position.x > max_x) {
        tank->position.x = max_x;
        tank->velocity.x = 0;
    }
    if(tank->position.y > max_y) {
        tank->position.y = max_y;
        tank->velocity.y = 0;
    }

    // Additional safety check - ensure tank never goes completely outside the game area
    if(tank->position.x + TANK_SIZE < MAP_OFFSET_X ||
        tank->position.x > MAP_OFFSET_X + MAP_WIDTH * BLOCK_SIZE ||
        tank->position.y + TANK_SIZE < MAP_OFFSET_Y ||
        tank->position.y > MAP_OFFSET_Y + MAP_HEIGHT * BLOCK_SIZE) {

        // Emergency repositioning - place tank back in safe area
        Vector2 emergency_pos = find_safe_spawn_position();
        tank->position = emergency_pos;
        tank->velocity = (Vector2){ 0, 0 };
    }
}

#endif // GAME_TANKS_H