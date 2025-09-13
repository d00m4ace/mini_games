#ifndef GAME_OBSTACLES_H
#define GAME_OBSTACLES_H

// Function declarations
void apply_obstacle_effect(Obstacle* obs, Tank* tank);
void trigger_mine(Obstacle* mine, Tank* tank);
void use_teleporter(Obstacle* teleporter, Tank* tank);
Vector2 reflect_bullet(Vector2 bullet_pos, Vector2 bullet_velocity, Obstacle* reflector);
bool check_obstacle_collision_enhanced(Rectangle rect, bool is_tank, Tank* tank);
bool check_obstacle_collision(Rectangle rect, bool is_tank);
void respawn_steel_wall(Obstacle* wall);
bool check_bullet_obstacle_collision(Vector2 bullet_pos, Bullet* bullet);
void update_obstacles(float delta_time);
void draw_obstacles_background(void);
void draw_obstacles_foreground(void);

// Individual obstacle rendering functions
void draw_steel_wall(Obstacle* obs);
void draw_ice_block(Obstacle* obs);
void draw_quicksand(Obstacle* obs);
void draw_conveyor_belt(Obstacle* obs);
void draw_water(Obstacle* obs);
void draw_bridge(Obstacle* obs);
void draw_mine_field(Obstacle* obs);
void draw_healing_station(Obstacle* obs);
void draw_ammo_depot(Obstacle* obs);
void draw_shield_generator(Obstacle* obs);
void draw_gate(Obstacle* obs);
void draw_reflector(Obstacle* obs);
void draw_teleporter(Obstacle* obs);
void draw_forest(Obstacle* obs);
void draw_steel_wall_respawn_preview(Obstacle* obs);

// Implementation
void apply_obstacle_effect(Obstacle* obs, Tank* tank)
{
	extern GameState game_state;
	extern Sound sound_hit;
	extern bool sounds_loaded;

	Vector2 tank_center = { tank->position.x + TANK_SIZE / 2, tank->position.y + TANK_SIZE / 2 };
	Vector2 obs_center = { obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2 };

	switch(obs->type) {
	case OBSTACLE_ICE_BLOCK:
		tank->on_ice = true;
		tank->slide_friction = 0.95f;
		break;

	case OBSTACLE_QUICKSAND:
		tank->speed_multiplier = 0.3f;
		break;

	case OBSTACLE_CONVEYOR_BELT:
		tank->on_conveyor = true;
		float conveyor_strength = TANK_SPEED * 1.8f;
		switch(obs->direction) {
		case 0: tank->conveyor_force = (Vector2){ 0, -conveyor_strength }; break;
		case 1: tank->conveyor_force = (Vector2){ conveyor_strength, 0 }; break;
		case 2: tank->conveyor_force = (Vector2){ 0, conveyor_strength }; break;
		case 3: tank->conveyor_force = (Vector2){ -conveyor_strength, 0 }; break;
		}
		break;

	case OBSTACLE_HEALING_STATION:
		if(obs->uses_remaining > 0) {
			int max_allowed_level = get_max_tank_level_for_game_level(game_state.current_level);
			if(tank->level < max_allowed_level) {
				if(tank->healing_timer <= 0) {
					int levels_to_heal = max_allowed_level - tank->level;
					if(levels_to_heal > 5) levels_to_heal = 5;
					if(levels_to_heal < 1) levels_to_heal = 1;

					tank->healing_timer = (float)levels_to_heal;
					obs->uses_remaining--;

					if(obs->uses_remaining <= 0) {
						obs->active = false;
						Vector2 station_center = { obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2 };
						create_explosion(station_center, WHITE);
						if(sounds_loaded) PlaySound(sound_hit);
					}
				}
			}
		}
		break;

	case OBSTACLE_AMMO_DEPOT:
		if(obs->uses_remaining > 0) {
			tank->rapid_fire_timer = 5.0f;
			obs->uses_remaining--;

			if(obs->uses_remaining <= 0) {
				obs->active = false;
				Vector2 depot_center = { obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2 };
				create_explosion(depot_center, YELLOW);
				if(sounds_loaded) PlaySound(sound_hit);
			}
		}
		break;

	case OBSTACLE_SHIELD_GENERATOR:
		if(obs->uses_remaining > 0) {
			tank->has_shield = true;
			tank->shield_timer = 5.0f;
			obs->uses_remaining--;

			if(obs->uses_remaining <= 0) {
				obs->active = false;
				Vector2 gen_center = { obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2 };
				create_explosion(gen_center, SKYBLUE);
				if(sounds_loaded) PlaySound(sound_hit);
			}
		}
		break;
	}
}

void trigger_mine(Obstacle* mine, Tank* tank)
{
	extern GameState game_state;
	extern Tank player1_tank, player2_tank;
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];
	extern Sound sound_explosion, sound_hit;
	extern bool sounds_loaded;

	if(mine->triggered) return;

	mine->triggered = true;
	mine->timer = MINE_RESPAWN_TIME_MIN +
		((float)(rand() % (int)((MINE_RESPAWN_TIME_MAX - MINE_RESPAWN_TIME_MIN) * 100))) / 100.0f;

	Vector2 mine_center = { mine->position.x + BLOCK_SIZE / 2, mine->position.y + BLOCK_SIZE / 2 };
	create_explosion(mine_center, RED);

	Vector2 tank_center = { tank->position.x + TANK_SIZE / 2, tank->position.y + TANK_SIZE / 2 };
	float distance = Vector2Distance(mine_center, tank_center);

	if(distance < mine->effect_radius) {
		if(!tank->has_shield) {
			int damage = 2 + (game_state.current_level / 3);
			tank->level -= damage;

			if(sounds_loaded) PlaySound(sound_explosion);

			Vector2 hit_pos = (Vector2){
				tank->position.x + TANK_SIZE / 2,
				tank->position.y - 5
			};

			spawn_floating_damage(hit_pos, damage);
			spawn_hit_particles(hit_pos, ORANGE, 12);

			if(tank->level <= 0) {
				create_explosion(tank_center, tank->body_color);
				tank->active = false;
			}
		}
		else {
			create_explosion(tank_center, SKYBLUE);
			if(sounds_loaded) PlaySound(sound_hit);
		}
	}

	// Damage other nearby tanks
	Tank* all_tanks[] = { &player1_tank, &player2_tank };
	for(int i = 0; i < 2; i++) {
		if(all_tanks[i]->active && all_tanks[i] != tank) {
			Vector2 other_center = { all_tanks[i]->position.x + TANK_SIZE / 2, all_tanks[i]->position.y + TANK_SIZE / 2 };
			float other_distance = Vector2Distance(mine_center, other_center);

			if(other_distance < mine->effect_radius && !all_tanks[i]->has_shield) {
				int damage = 2 + (game_state.current_level / 3);
				all_tanks[i]->level -= damage;

				Vector2 hit_pos = (Vector2){
				tank->position.x + TANK_SIZE / 2,
				tank->position.y - 5
				};

				spawn_floating_damage(hit_pos, damage);
				spawn_hit_particles(hit_pos, ORANGE, 12);

				if(all_tanks[i]->level <= 0) {
					create_explosion(other_center, all_tanks[i]->body_color);
					all_tanks[i]->active = false;
				}
			}
		}
	}

	// Damage nearby enemies
	for(int i = 0; i < MAX_ENEMY_TANKS; i++) {
		if(enemy_tanks[i].active) {
			Vector2 enemy_center = { enemy_tanks[i].position.x + TANK_SIZE / 2, enemy_tanks[i].position.y + TANK_SIZE / 2 };
			float enemy_distance = Vector2Distance(mine_center, enemy_center);

			if(enemy_distance < mine->effect_radius) {
				int damage = 2 + (game_state.current_level / 3);
				enemy_tanks[i].level -= damage;

				Vector2 hit_pos = (Vector2){
				enemy_tanks[i].position.x + TANK_SIZE / 2,
				enemy_tanks[i].position.y - 5
				};

				spawn_floating_damage(hit_pos, damage);
				spawn_hit_particles(hit_pos, ORANGE, 12);

				if(enemy_tanks[i].level <= 0) {
					create_explosion(enemy_center, RED);
					enemy_tanks[i].active = false;
					game_state.enemies_killed_this_level++;

					if(tank->team == 0) game_state.player1_score += ENEMY_POINTS;
					else if(tank->team == 1) game_state.player2_score += ENEMY_POINTS;
				}
			}
		}
	}
}

void use_teleporter(Obstacle* teleporter, Tank* tank)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];
	extern Sound sound_hit;
	extern bool sounds_loaded;

	float current_time = GetTime();
	if(current_time - tank->last_teleport_time < 2.0f) {
		return;
	}

	Vector2 available_teleporters[MAP_WIDTH * MAP_HEIGHT];
	int teleporter_count = 0;

	for(int x = 0; x < MAP_WIDTH; x++) {
		for(int y = 0; y < MAP_HEIGHT; y++) {
			if(map_obstacles[x][y].active &&
				map_obstacles[x][y].type == OBSTACLE_TELEPORTER &&
				&map_obstacles[x][y] != teleporter) {
				available_teleporters[teleporter_count] = map_obstacles[x][y].position;
				teleporter_count++;
			}
		}
	}

	if(teleporter_count > 0) {
		int chosen_index = rand() % teleporter_count;
		Vector2 destination_pos = available_teleporters[chosen_index];

		tank->last_teleport_time = current_time;

		Vector2 dest_center = { destination_pos.x + BLOCK_SIZE / 2, destination_pos.y + BLOCK_SIZE / 2 };
		tank->position = (Vector2){ dest_center.x - TANK_SIZE / 2, dest_center.y - TANK_SIZE / 2 };

		tank->velocity = (Vector2){ 0, 0 };

		create_teleport_effect(teleporter->position);
		create_teleport_effect(destination_pos);

		if(sounds_loaded) PlaySound(sound_hit);
	}
}

Vector2 reflect_bullet(Vector2 bullet_pos, Vector2 bullet_velocity, Obstacle* reflector)
{
	Vector2 reflected = bullet_velocity;
	Vector2 rel_pos = Vector2Subtract(bullet_pos, reflector->position);

	if(rel_pos.x < BLOCK_SIZE / 3) {
		reflected.x = -fabsf(reflected.x);
	}
	else if(rel_pos.x > BLOCK_SIZE * 2 / 3) {
		reflected.x = fabsf(reflected.x);
	}

	if(rel_pos.y < BLOCK_SIZE / 3) {
		reflected.y = -fabsf(reflected.y);
	}
	else if(rel_pos.y > BLOCK_SIZE * 2 / 3) {
		reflected.y = fabsf(reflected.y);
	}

	return reflected;
}

bool check_obstacle_collision_enhanced(Rectangle rect, bool is_tank, Tank* tank)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];

	int start_x = (int)((rect.x - MAP_OFFSET_X) / BLOCK_SIZE);
	int start_y = (int)((rect.y - MAP_OFFSET_Y) / BLOCK_SIZE);
	int end_x = (int)((rect.x + rect.width - MAP_OFFSET_X) / BLOCK_SIZE);
	int end_y = (int)((rect.y + rect.height - MAP_OFFSET_Y) / BLOCK_SIZE);

	if(start_x < 0) start_x = 0;
	if(start_y < 0) start_y = 0;
	if(end_x >= MAP_WIDTH) end_x = MAP_WIDTH - 1;
	if(end_y >= MAP_HEIGHT) end_y = MAP_HEIGHT - 1;

	for(int x = start_x; x <= end_x; x++) {
		for(int y = start_y; y <= end_y; y++) {
			if(map_obstacles[x][y].active) {
				Rectangle obs_rect = { map_obstacles[x][y].position.x, map_obstacles[x][y].position.y, BLOCK_SIZE, BLOCK_SIZE };

				if(CheckCollisionRecs(rect, obs_rect)) {
					// Apply effects for tanks on passable obstacles
					if(is_tank && tank && !map_obstacles[x][y].is_block_tanks) {
						apply_obstacle_effect(&map_obstacles[x][y], tank);

						// Handle special trigger-based interactions
						if(map_obstacles[x][y].type == OBSTACLE_MINE_FIELD && !map_obstacles[x][y].triggered) {
							Rectangle inner = (Rectangle){
								obs_rect.x + MINE_TRIGGER_INSET,
								obs_rect.y + MINE_TRIGGER_INSET,
								BLOCK_SIZE - 2 * MINE_TRIGGER_INSET,
								BLOCK_SIZE - 2 * MINE_TRIGGER_INSET
							};
							if(CheckCollisionRecs(rect, inner)) {
								trigger_mine(&map_obstacles[x][y], tank);
							}
						}

						if(map_obstacles[x][y].type == OBSTACLE_TELEPORTER) {
							Vector2 tank_center = { tank->position.x + TANK_SIZE / 2, tank->position.y + TANK_SIZE / 2 };
							Vector2 teleporter_center = { map_obstacles[x][y].position.x + BLOCK_SIZE / 2, map_obstacles[x][y].position.y + BLOCK_SIZE / 2 };
							float distance = Vector2Distance(tank_center, teleporter_center);

							if(distance < BLOCK_SIZE / 4) {
								use_teleporter(&map_obstacles[x][y], tank);
							}
						}
					}

					// Standard blocking check using the new flags
					if(is_tank && map_obstacles[x][y].is_block_tanks) {
						return true;
					}

					if(!is_tank && map_obstacles[x][y].is_block_bullets) {
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool check_obstacle_collision(Rectangle rect, bool is_tank)
{
	return check_obstacle_collision_enhanced(rect, is_tank, NULL);
}

void respawn_steel_wall(Obstacle* wall)
{
	extern Tank player1_tank, player2_tank;
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];
	extern GameState game_state;
	extern Sound sound_explosion, sound_hit;
	extern bool sounds_loaded;

	if(wall->type != OBSTACLE_STEEL_WALL || wall->active) return;

	Vector2 respawn_pos = wall->linked_pos;
	Rectangle wall_rect = { respawn_pos.x, respawn_pos.y, BLOCK_SIZE, BLOCK_SIZE };

	Tank* all_tanks[] = { &player1_tank, &player2_tank };
	int tank_count = 2;

	for(int i = 0; i < tank_count; i++) {
		if(all_tanks[i]->active) {
			Rectangle tank_rect = { all_tanks[i]->position.x, all_tanks[i]->position.y, TANK_SIZE, TANK_SIZE };
			if(CheckCollisionRecs(wall_rect, tank_rect)) {
				if(!all_tanks[i]->has_shield) {
					all_tanks[i]->level -= STEEL_WALL_RESPAWN_DAMAGE;

					Vector2 tank_center = { all_tanks[i]->position.x + TANK_SIZE / 2, all_tanks[i]->position.y + TANK_SIZE / 2 };
					create_explosion(tank_center, DARKGRAY);

					if(sounds_loaded) PlaySound(sound_explosion);

					if(all_tanks[i]->level <= 0) {
						create_explosion(tank_center, all_tanks[i]->body_color);
						all_tanks[i]->active = false;
					}
				}
				else {
					Vector2 tank_center = { all_tanks[i]->position.x + TANK_SIZE / 2, all_tanks[i]->position.y + TANK_SIZE / 2 };
					create_explosion(tank_center, SKYBLUE);
					all_tanks[i]->has_shield = false;
					all_tanks[i]->shield_timer = 0.0f;
					if(sounds_loaded) PlaySound(sound_hit);
				}
			}
		}
	}

	for(int i = 0; i < MAX_ENEMY_TANKS; i++) {
		if(enemy_tanks[i].active) {
			Rectangle tank_rect = { enemy_tanks[i].position.x, enemy_tanks[i].position.y, TANK_SIZE, TANK_SIZE };
			if(CheckCollisionRecs(wall_rect, tank_rect)) {
				enemy_tanks[i].level -= STEEL_WALL_RESPAWN_DAMAGE;

				Vector2 tank_center = { enemy_tanks[i].position.x + TANK_SIZE / 2, enemy_tanks[i].position.y + TANK_SIZE / 2 };
				create_explosion(tank_center, DARKGRAY);

				if(sounds_loaded) PlaySound(sound_explosion);

				if(enemy_tanks[i].level <= 0) {
					create_explosion(tank_center, RED);
					enemy_tanks[i].active = false;
					game_state.enemies_killed_this_level++;
				}
			}
		}
	}

	wall->active = true;
	wall->position = respawn_pos;
	wall->health = 5 + (game_state.current_level * 1.5f);
	wall->max_health = wall->health;
	wall->color = (Color){ 169, 169, 169, 255 };
	wall->timer = 0.0f;
	wall->is_block_tanks = true;   // Restored wall blocks tanks
	wall->is_block_bullets = true; // Restored wall blocks bullets

	Vector2 wall_center = { wall->position.x + BLOCK_SIZE / 2, wall->position.y + BLOCK_SIZE / 2 };
	create_explosion(wall_center, GRAY);

	if(sounds_loaded) PlaySound(sound_hit);
}

bool check_bullet_obstacle_collision(Vector2 bullet_pos, Bullet* bullet)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];
	extern Sound sound_hit, sound_explosion;
	extern bool sounds_loaded;

	Rectangle bullet_rect = { bullet_pos.x - BULLET_SIZE, bullet_pos.y - BULLET_SIZE, BULLET_SIZE * 2, BULLET_SIZE * 2 };

	Vector2 map_coords = world_to_map_grid(bullet_pos);
	int center_x = (int)map_coords.x;
	int center_y = (int)map_coords.y;

	for(int dx = -1; dx <= 1; dx++) {
		for(int dy = -1; dy <= 1; dy++) {
			int check_x = center_x + dx;
			int check_y = center_y + dy;

			if(check_x < 0 || check_x >= MAP_WIDTH || check_y < 0 || check_y >= MAP_HEIGHT) {
				continue;
			}

			if(map_obstacles[check_x][check_y].active) {
				Rectangle obs_rect = { map_obstacles[check_x][check_y].position.x, map_obstacles[check_x][check_y].position.y, BLOCK_SIZE, BLOCK_SIZE };

				if(CheckCollisionRecs(bullet_rect, obs_rect)) {
					// Handle special bullet interactions
					if(map_obstacles[check_x][check_y].type == OBSTACLE_TELEPORTER) {
						extern void teleport_bullet(Bullet * bullet, Obstacle * source_teleporter);
						teleport_bullet(bullet, &map_obstacles[check_x][check_y]);
						return false;
					}

					if(map_obstacles[check_x][check_y].type == OBSTACLE_REFLECTOR) {
						bullet->velocity = reflect_bullet(bullet_pos, bullet->velocity, &map_obstacles[check_x][check_y]);
						if(sounds_loaded && bullet->team < 2) PlaySound(sound_hit);
						return false;
					}

					// Handle destructible steel walls
					if(map_obstacles[check_x][check_y].type == OBSTACLE_STEEL_WALL &&
						map_obstacles[check_x][check_y].is_block_bullets) {
						if(sounds_loaded && bullet->team < 2) PlaySound(sound_hit);
						map_obstacles[check_x][check_y].health -= bullet->damage;

						Vector2 wall_center = (Vector2){
							map_obstacles[check_x][check_y].position.x + BLOCK_SIZE / 2,
							map_obstacles[check_x][check_y].position.y - 5 };
						spawn_floating_damage(wall_center, bullet->damage);
						spawn_hit_particles(wall_center, GRAY, 6);

						if(map_obstacles[check_x][check_y].health <= 0) {
							map_obstacles[check_x][check_y].active = false;
							map_obstacles[check_x][check_y].is_block_tanks = false;
							map_obstacles[check_x][check_y].is_block_bullets = false;
							map_obstacles[check_x][check_y].timer = STEEL_WALL_RESPAWN_TIME;
							create_explosion(Vector2Add(map_obstacles[check_x][check_y].position, (Vector2) { BLOCK_SIZE / 2, BLOCK_SIZE / 2 }), GRAY);
							if(sounds_loaded && bullet->team < 2) PlaySound(sound_explosion);
						}
						else {
							float health_ratio = (float)map_obstacles[check_x][check_y].health / (float)map_obstacles[check_x][check_y].max_health;
							map_obstacles[check_x][check_y].color = (Color){
								(int)(169 * health_ratio),
								(int)(169 * health_ratio),
								(int)(169 * health_ratio),
								255
							};
						}
						return true;
					}

					// Simple bullet blocking check
					if(map_obstacles[check_x][check_y].is_block_bullets) {
						if(sounds_loaded && bullet->team < 2) PlaySound(sound_hit);
						return true;
					}
				}
			}
		}
	}

	return false;
}

void update_obstacles(float delta_time)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];

	for(int x = 0; x < MAP_WIDTH; x++) {
		for(int y = 0; y < MAP_HEIGHT; y++) {
			if(map_obstacles[x][y].active) {
				if(map_obstacles[x][y].timer > 0) {
					map_obstacles[x][y].timer -= delta_time;
				}

				switch(map_obstacles[x][y].type) {
				case OBSTACLE_GATE:
					if(map_obstacles[x][y].timer <= 0) {
						map_obstacles[x][y].state = !map_obstacles[x][y].state;
						map_obstacles[x][y].timer = 3.0f + (rand() % 4);
						// Update blocking properties when gate state changes
						map_obstacles[x][y].is_block_tanks = !map_obstacles[x][y].state;
						map_obstacles[x][y].is_block_bullets = !map_obstacles[x][y].state;
					}
					break;

				case OBSTACLE_MINE_FIELD:
					if(map_obstacles[x][y].triggered && map_obstacles[x][y].timer <= 0) {
						map_obstacles[x][y].triggered = false;
						map_obstacles[x][y].timer = 0;
					}
					break;

				case OBSTACLE_HEALING_STATION:
				case OBSTACLE_AMMO_DEPOT:
				case OBSTACLE_SHIELD_GENERATOR:
					if(map_obstacles[x][y].uses_remaining <= 0 && map_obstacles[x][y].timer <= 0) {
						map_obstacles[x][y].uses_remaining = 1;
						map_obstacles[x][y].timer = 0;
					}
					break;
				}
			}
			else {
				if(map_obstacles[x][y].type == OBSTACLE_STEEL_WALL) {
					if(map_obstacles[x][y].timer > 0) {
						map_obstacles[x][y].timer -= delta_time;
						if(map_obstacles[x][y].timer <= 0) {
							respawn_steel_wall(&map_obstacles[x][y]);
						}
					}
				}
			}
		}
	}
}

// Individual obstacle rendering functions
void draw_steel_wall(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, obs->color);

	// Draw rivets
	for(int row = 0; row < 3; row++) {
		for(int col = 0; col < 3; col++) {
			int rivet_x = obs->position.x + 6 + col * 10;
			int rivet_y = obs->position.y + 6 + row * 10;
			DrawCircle(rivet_x, rivet_y, 2, BLACK);
			DrawCircle(rivet_x, rivet_y, 1, WHITE);
		}
	}

	// Draw damage cracks
	if(obs->health < obs->max_health) {
		DrawLine(obs->position.x + 5, obs->position.y + 10,
			obs->position.x + 15, obs->position.y + 20, BLACK);
		if(obs->health < obs->max_health / 2) {
			DrawLine(obs->position.x + 20, obs->position.y + 5,
				obs->position.x + 28, obs->position.y + 25, BLACK);
		}
	}
}

void draw_ice_block(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 200, 230, 255, 200 });

	// Draw ice crystals
	for(int j = 0; j < 8; j++) {
		int crystal_x = obs->position.x + 4 + (j % 4) * 8;
		int crystal_y = obs->position.y + 4 + (j / 4) * 16;
		DrawLine(crystal_x - 2, crystal_y, crystal_x + 2, crystal_y, WHITE);
		DrawLine(crystal_x, crystal_y - 2, crystal_x, crystal_y + 2, WHITE);
	}
}

void draw_quicksand(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 194, 154, 108, 255 });

	// Draw swirling pattern
	Vector2 center = { obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2 };
	for(int r = 4; r < 16; r += 4) {
		float angle = GetTime() * 2 + r * 0.5f;
		int swirl_x = center.x + cosf(angle) * r;
		int swirl_y = center.y + sinf(angle) * r;
		DrawCircle(swirl_x, swirl_y, 1, (Color) { 160, 120, 80, 255 });
	}
}

void draw_conveyor_belt(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 60, 60, 60, 255 });
	DrawRectangle(obs->position.x + 2, obs->position.y + 2, BLOCK_SIZE - 4, BLOCK_SIZE - 4, (Color) { 80, 80, 80, 255 });

	// Draw animated arrows
	float arrow_offset = fmodf(GetTime() * 80, 12);
	Color arrow_color = YELLOW;

	for(int row = 0; row < 3; row++) {
		for(int col = 0; col < 3; col++) {
			int base_x = obs->position.x + 6 + col * 8;
			int base_y = obs->position.y + 6 + row * 8;

			switch(obs->direction) {
			case 0: // Up
			{
				int ay = base_y - (int)arrow_offset;
				if(ay > obs->position.y && ay < obs->position.y + BLOCK_SIZE - 6) {
					DrawLine(base_x - 2, ay + 3, base_x, ay, arrow_color);
					DrawLine(base_x + 2, ay + 3, base_x, ay, arrow_color);
					DrawLine(base_x, ay, base_x, ay + 6, arrow_color);
				}
			}
			break;
			case 1: // Right
			{
				int ax = base_x + (int)arrow_offset;
				if(ax > obs->position.x && ax < obs->position.x + BLOCK_SIZE - 6) {
					DrawLine(ax - 3, base_y - 2, ax, base_y, arrow_color);
					DrawLine(ax - 3, base_y + 2, ax, base_y, arrow_color);
					DrawLine(ax - 6, base_y, ax, base_y, arrow_color);
				}
			}
			break;
			case 2: // Down
			{
				int ay = base_y + (int)arrow_offset;
				if(ay > obs->position.y && ay < obs->position.y + BLOCK_SIZE - 6) {
					DrawLine(base_x - 2, ay - 3, base_x, ay, arrow_color);
					DrawLine(base_x + 2, ay - 3, base_x, ay, arrow_color);
					DrawLine(base_x, ay - 6, base_x, ay, arrow_color);
				}
			}
			break;
			case 3: // Left
			{
				int ax = base_x - (int)arrow_offset;
				if(ax > obs->position.x && ax < obs->position.x + BLOCK_SIZE - 6) {
					DrawLine(ax + 3, base_y - 2, ax, base_y, arrow_color);
					DrawLine(ax + 3, base_y + 2, ax, base_y, arrow_color);
					DrawLine(ax, base_y, ax + 6, base_y, arrow_color);
				}
			}
			break;
			}
		}
	}
	DrawRectangleLinesEx((Rectangle) { obs->position.x, obs->position.y, BLOCK_SIZE, BLOCK_SIZE }, 2, YELLOW);
}

void draw_water(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 0, 119, 190, 255 });

	// Draw water surface patterns
	for(int wave_y = 0; wave_y < 4; wave_y++) {
		for(int wave_x = 0; wave_x < 4; wave_x++) {
			int wave_pos_x = obs->position.x + wave_x * 8 + 2;
			int wave_pos_y = obs->position.y + wave_y * 8 + 2;
			DrawRectangle(wave_pos_x, wave_pos_y, 4, 4, SKYBLUE);
			if((wave_x + wave_y) % 2 == 0) {
				DrawPixel(wave_pos_x + 1, wave_pos_y + 1, WHITE);
			}
		}
	}
}

void draw_bridge(Obstacle* obs)
{
	// Draw water underneath
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 0, 119, 190, 255 });
	// Draw bridge planks
	DrawRectangle(obs->position.x + 4, obs->position.y + 2, BLOCK_SIZE - 8, BLOCK_SIZE - 4, (Color) { 139, 69, 19, 255 });
	for(int p = 0; p < 4; p++) {
		int plank_y = obs->position.y + 4 + p * 6;
		DrawLine(obs->position.x + 4, plank_y, obs->position.x + BLOCK_SIZE - 4, plank_y, DARKBROWN);
	}
}

void draw_mine_field(Obstacle* obs)
{
	extern Tank player1_tank, player2_tank;
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];

	bool should_reveal = false;
	Rectangle obs_rect = (Rectangle){
		obs->position.x,
		obs->position.y,
		BLOCK_SIZE, BLOCK_SIZE
	};

	// Check if any tank is close enough to reveal mine
	if(!should_reveal && player1_tank.active) {
		Rectangle p1_rect = (Rectangle){ player1_tank.position.x, player1_tank.position.y, TANK_SIZE, TANK_SIZE };
		Rectangle r = GetCollisionRec(p1_rect, obs_rect);
		if(r.width > 0 && r.height > 0) should_reveal = true;
	}

	if(!should_reveal && player2_tank.active) {
		Rectangle p2_rect = (Rectangle){ player2_tank.position.x, player2_tank.position.y, TANK_SIZE, TANK_SIZE };
		Rectangle r = GetCollisionRec(p2_rect, obs_rect);
		if(r.width > 0 && r.height > 0) should_reveal = true;
	}

	if(!should_reveal) {
		for(int e = 0; e < MAX_ENEMY_TANKS; e++) {
			if(!enemy_tanks[e].active) continue;
			Rectangle er = (Rectangle){ enemy_tanks[e].position.x, enemy_tanks[e].position.y, TANK_SIZE, TANK_SIZE };
			Rectangle r = GetCollisionRec(er, obs_rect);
			if(r.width > 0 && r.height > 0) { should_reveal = true; break; }
		}
	}

	if(should_reveal && !obs->triggered) {
		// Draw revealed mine
		DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 101, 67, 33, 255 });
		DrawCircle(obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2, 6, DARKGRAY);
		DrawCircle(obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2, 4, RED);

		// Draw mine spikes
		Vector2 center_mine = { obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2 };
		for(int spike = 0; spike < 8; spike++) {
			float angle = spike * 45.0f * DEG2RAD;
			int spike_x = center_mine.x + cosf(angle) * 8;
			int spike_y = center_mine.y + sinf(angle) * 8;
			DrawPixel(spike_x, spike_y, RED);
		}

		// Draw pulsing warning light
		float pulse = 0.5f + 0.5f * sinf(GetTime() * 6);
		DrawPixel(obs->position.x + BLOCK_SIZE / 2 - 1, obs->position.y + BLOCK_SIZE / 2 - 1, Fade(YELLOW, pulse));
		DrawPixel(obs->position.x + BLOCK_SIZE / 2 + 1, obs->position.y + BLOCK_SIZE / 2 - 1, Fade(YELLOW, pulse));
	}
	else if(obs->triggered) {
		// Draw exploded mine crater
		DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 60, 40, 20, 255 });
		DrawCircle(obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2, 10, (Color) { 40, 25, 15, 255 });
	}
}

void draw_healing_station(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 255, 255, 255, 255 });

	// Draw red cross
	DrawRectangle(obs->position.x + 12, obs->position.y + 4, 8, 24, RED);
	DrawRectangle(obs->position.x + 4, obs->position.y + 12, 24, 8, RED);

	if(obs->uses_remaining > 0) {
		// Draw pulsing effect when active
		float pulse = 0.5f + 0.5f * sinf(GetTime() * 4);
		DrawRectangle(obs->position.x + 14, obs->position.y + 6, 4, 20, Fade(WHITE, pulse));
		DrawRectangle(obs->position.x + 6, obs->position.y + 14, 20, 4, Fade(WHITE, pulse));
	}
	else if(obs->timer > 0) {
		// Draw recharge progress bar
		float recharge_progress = 1.0f - (obs->timer / 15.0f);
		int bar_width = (int)(20 * recharge_progress);
		DrawRectangle(obs->position.x + 6, obs->position.y + 2, bar_width, 2, BLUE);
		DrawRectangleLines(obs->position.x + 6, obs->position.y + 2, 20, 2, DARKBLUE);
	}
	else {
		// Draw as inactive
		DrawRectangle(obs->position.x, obs->position.y, BLOCK_SIZE, BLOCK_SIZE, Fade(DARKGRAY, 0.7f));
	}
}

void draw_ammo_depot(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 139, 69, 19, 255 });
	DrawRectangleLinesEx((Rectangle) { obs->position.x, obs->position.y, BLOCK_SIZE, BLOCK_SIZE }, 1, BLACK);

	// Draw ammunition
	for(int a = 0; a < 3; a++) {
		int ammo_x = obs->position.x + 8 + a * 6;
		int ammo_y = obs->position.y + 6;
		DrawRectangle(ammo_x, ammo_y, 2, 20, YELLOW);
		DrawRectangle(ammo_x, ammo_y, 2, 8, GOLD);
	}

	if(obs->uses_remaining <= 0) {
		DrawRectangle(obs->position.x, obs->position.y, BLOCK_SIZE, BLOCK_SIZE, Fade(DARKGRAY, 0.7f));
	}
}

void draw_shield_generator(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 100, 100, 150, 255 });
	Vector2 gen_center = { obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2 };

	if(obs->uses_remaining > 0) {
		// Draw pulsing shield rings
		float ring_pulse = sinf(GetTime() * 6) * 3;
		for(int r = 8; r < 16; r += 3) {
			DrawCircleLines(gen_center.x, gen_center.y, r + ring_pulse, SKYBLUE);
		}
	}
	DrawCircle(gen_center.x, gen_center.y, 4, obs->uses_remaining > 0 ? BLUE : DARKGRAY);
}

void draw_gate(Obstacle* obs)
{
	if(!obs->state) {
		// Draw closed gate
		DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 139, 69, 19, 255 });
		for(int b = 0; b < 4; b++) {
			int bar_x = obs->position.x + 4 + b * 6;
			DrawRectangle(bar_x, obs->position.y, 2, BLOCK_SIZE, (Color) { 101, 67, 33, 255 });
		}
	}
	else {
		// Draw open gate
		DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 50, 50, 50, 100 });
		DrawText("OPEN", obs->position.x + 2, obs->position.y + 12, 8, GREEN);
	}
}

void draw_reflector(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 200, 200, 255, 255 });

	// Draw reflective surface pattern
	for(int l = 0; l < 4; l++) {
		DrawLine(obs->position.x + l * 8, obs->position.y,
			obs->position.x + BLOCK_SIZE, obs->position.y + l * 8, WHITE);
		DrawLine(obs->position.x, obs->position.y + l * 8,
			obs->position.x + BLOCK_SIZE - l * 8, obs->position.y + BLOCK_SIZE, WHITE);
	}
}

void draw_teleporter(Obstacle* obs)
{
	DrawRectangleV(obs->position, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, (Color) { 50, 0, 100, 255 });
	Vector2 tp_center = { obs->position.x + BLOCK_SIZE / 2, obs->position.y + BLOCK_SIZE / 2 };

	// Draw animated spiral effect
	float spiral_time = GetTime() * 4;
	for(int s = 0; s < 20; s++) {
		float angle = spiral_time + s * 0.3f;
		float radius = 2 + (s % 8);
		int spiral_x = tp_center.x + cosf(angle) * radius;
		int spiral_y = tp_center.y + sinf(angle) * radius;
		DrawPixel(spiral_x, spiral_y, PURPLE);
	}
	DrawCircleLines(tp_center.x, tp_center.y, 12, MAGENTA);
}

void draw_forest(Obstacle* obs)
{
	Color forest_color = DARKGREEN;
	Color light_forest = GREEN;

	// Draw forest canopy
	DrawCircle(obs->position.x + 12, obs->position.y + 12, 14, forest_color);
	DrawCircle(obs->position.x + 20, obs->position.y + 12, 14, forest_color);
	DrawCircle(obs->position.x + 12, obs->position.y + 20, 14, forest_color);
	DrawCircle(obs->position.x + 20, obs->position.y + 20, 14, forest_color);
	DrawCircle(obs->position.x + 16, obs->position.y + 16, 12, forest_color);

	// Draw light spots
	DrawCircle(obs->position.x + 6, obs->position.y + 8, 3, light_forest);
	DrawCircle(obs->position.x + 26, obs->position.y + 6, 3, light_forest);
	DrawCircle(obs->position.x + 8, obs->position.y + 26, 3, light_forest);
	DrawCircle(obs->position.x + 24, obs->position.y + 24, 3, light_forest);
	DrawCircle(obs->position.x + 4, obs->position.y + 18, 2, light_forest);
	DrawCircle(obs->position.x + 28, obs->position.y + 14, 2, light_forest);
}

void draw_steel_wall_respawn_preview(Obstacle* obs)
{
	Vector2 respawn_pos = obs->linked_pos;
	float time_ratio = obs->timer / STEEL_WALL_RESPAWN_TIME;
	float warning_alpha = 0.3f + 0.7f * (1.0f - time_ratio);
	float pulse = 0.5f + 0.5f * sinf(GetTime() * (6.0f + (1.0f - time_ratio) * 6.0f));

	// Draw warning overlay
	Color warning_color = (Color){ 255, 100, 100, (int)(100 * warning_alpha * pulse) };
	DrawRectangleV(respawn_pos, (Vector2) { BLOCK_SIZE, BLOCK_SIZE }, warning_color);
	DrawRectangleLinesEx((Rectangle) { respawn_pos.x, respawn_pos.y, BLOCK_SIZE, BLOCK_SIZE }, 2,
		(Color) {
		255, 0, 0, (int)(255 * warning_alpha * pulse)
	});

	// Draw countdown timer
	if(obs->timer <= 3.0f) {
		char countdown_text[8];
		sprintf(countdown_text, "%.1f", obs->timer);
		int text_width = MeasureText(countdown_text, 12);
		int text_x = respawn_pos.x + BLOCK_SIZE / 2 - text_width / 2;
		int text_y = respawn_pos.y + BLOCK_SIZE / 2 - 6;

		// Draw text with outline
		DrawText(countdown_text, text_x - 1, text_y, 12, BLACK);
		DrawText(countdown_text, text_x + 1, text_y, 12, BLACK);
		DrawText(countdown_text, text_x, text_y - 1, 12, BLACK);
		DrawText(countdown_text, text_x, text_y + 1, 12, BLACK);
		DrawText(countdown_text, text_x, text_y, 12, WHITE);
	}

	// Draw progress bar
	float progress = 1.0f - time_ratio;
	int bar_width = (int)(BLOCK_SIZE * 0.8f * progress);
	int bar_x = respawn_pos.x + (BLOCK_SIZE - (int)(BLOCK_SIZE * 0.8f)) / 2;
	int bar_y = respawn_pos.y + BLOCK_SIZE - 6;

	DrawRectangle(bar_x, bar_y, (int)(BLOCK_SIZE * 0.8f), 4, (Color) { 50, 50, 50, 200 });
	DrawRectangle(bar_x, bar_y, bar_width, 4, (Color) { 255, 150, 0, (int)(255 * warning_alpha) });
}

void draw_obstacles_background(void)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];

	for(int x = 0; x < MAP_WIDTH; x++) {
		for(int y = 0; y < MAP_HEIGHT; y++) {
			if(map_obstacles[x][y].active && (map_obstacles[x][y].type != OBSTACLE_FOREST)) {
				switch(map_obstacles[x][y].type) {
				case OBSTACLE_EMPTY:
					continue;
				case OBSTACLE_STEEL_WALL:
					draw_steel_wall(&map_obstacles[x][y]);
					break;
				case OBSTACLE_ICE_BLOCK:
					draw_ice_block(&map_obstacles[x][y]);
					break;
				case OBSTACLE_QUICKSAND:
					draw_quicksand(&map_obstacles[x][y]);
					break;
				case OBSTACLE_CONVEYOR_BELT:
					draw_conveyor_belt(&map_obstacles[x][y]);
					break;
				case OBSTACLE_WATER:
					draw_water(&map_obstacles[x][y]);
					break;
				case OBSTACLE_BRIDGE:
					draw_bridge(&map_obstacles[x][y]);
					break;
				case OBSTACLE_MINE_FIELD:
					draw_mine_field(&map_obstacles[x][y]);
					break;
				case OBSTACLE_HEALING_STATION:
					draw_healing_station(&map_obstacles[x][y]);
					break;
				case OBSTACLE_AMMO_DEPOT:
					draw_ammo_depot(&map_obstacles[x][y]);
					break;
				case OBSTACLE_SHIELD_GENERATOR:
					draw_shield_generator(&map_obstacles[x][y]);
					break;
				case OBSTACLE_GATE:
					draw_gate(&map_obstacles[x][y]);
					break;
				case OBSTACLE_REFLECTOR:
					draw_reflector(&map_obstacles[x][y]);
					break;
				case OBSTACLE_TELEPORTER:
					draw_teleporter(&map_obstacles[x][y]);
					break;
				}

				// Draw border for most obstacles (except water and gates)
				if(map_obstacles[x][y].type != OBSTACLE_WATER && map_obstacles[x][y].type != OBSTACLE_GATE) {
					DrawRectangleLinesEx((Rectangle) { map_obstacles[x][y].position.x, map_obstacles[x][y].position.y, BLOCK_SIZE, BLOCK_SIZE }, 1, BLACK);
				}
			}
			else if(map_obstacles[x][y].type == OBSTACLE_STEEL_WALL && map_obstacles[x][y].timer > 0) {
				draw_steel_wall_respawn_preview(&map_obstacles[x][y]);
			}
		}
	}
}

void draw_obstacles_foreground(void)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];

	for(int x = 0; x < MAP_WIDTH; x++) {
		for(int y = 0; y < MAP_HEIGHT; y++) {
			if(map_obstacles[x][y].active && map_obstacles[x][y].type == OBSTACLE_FOREST) {
				draw_forest(&map_obstacles[x][y]);
			}
		}
	}
}

#endif // GAME_OBSTACLES_H