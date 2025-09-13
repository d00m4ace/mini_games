#ifndef GAME_BULLETS_H
#define GAME_BULLETS_H

// Function declarations
void teleport_bullet(Bullet* bullet, Obstacle* source_teleporter);
void add_bullet_with_team_and_level(Vector2 position, float rotation, Color color, int team, int damage, int shooter_level);
void add_bullet_with_team(Vector2 position, float rotation, Color color, int team, int damage);
void update_bullets(float delta_time);
void draw_bullets(void);
void check_bullet_tank_collisions(void);

// Implementation
void teleport_bullet(Bullet* bullet, Obstacle* source_teleporter)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];
	extern Sound sound_hit;
	extern bool sounds_loaded;

	float current_time = GetTime();
	if(current_time - bullet->last_teleport_time < 0.5f) {
		return;
	}

	Vector2 available_teleporters[MAP_WIDTH * MAP_HEIGHT];
	int teleporter_count = 0;

	for(int x = 0; x < MAP_WIDTH; x++) {
		for(int y = 0; y < MAP_HEIGHT; y++) {
			if(map_obstacles[x][y].active &&
				map_obstacles[x][y].type == OBSTACLE_TELEPORTER &&
				&map_obstacles[x][y] != source_teleporter) {
				available_teleporters[teleporter_count] = map_obstacles[x][y].position;
				teleporter_count++;
			}
		}
	}

	if(teleporter_count > 0) {
		int chosen_index = rand() % teleporter_count;
		Vector2 destination_pos = available_teleporters[chosen_index];

		bullet->last_teleport_time = current_time;

		Vector2 dest_center = { destination_pos.x + BLOCK_SIZE / 2, destination_pos.y + BLOCK_SIZE / 2 };
		Vector2 velocity_normalized = Vector2Normalize(bullet->velocity);
		Vector2 exit_offset = Vector2Scale(velocity_normalized, BLOCK_SIZE / 2 + BULLET_SIZE * 3);
		bullet->position = Vector2Add(dest_center, exit_offset);

		create_teleport_effect(source_teleporter->position);
		create_teleport_effect(destination_pos);

		if(sounds_loaded) PlaySound(sound_hit);
	}
	else {
		bullet->active = false;
	}
}

void add_bullet_with_team_and_level(Vector2 position, float rotation, Color color, int team, int damage, int shooter_level)
{
	extern Bullet bullets[MAX_BULLETS];
	extern Sound sound_shoot;
	extern bool sounds_loaded;

	for(int i = 0; i < MAX_BULLETS; i++) {
		if(!bullets[i].active) {
			bullets[i].position = position;
			float rad = rotation * DEG2RAD;

			float speed_multiplier = get_bullet_speed_multiplier(shooter_level);
			float final_bullet_speed = BULLET_SPEED * speed_multiplier;

			bullets[i].velocity = (Vector2){ cosf(rad) * final_bullet_speed, sinf(rad) * final_bullet_speed };
			bullets[i].active = true;
			bullets[i].color = color;
			bullets[i].team = team;
			bullets[i].damage = damage;
			bullets[i].lifetime = 0.0f;
			bullets[i].last_teleport_time = 0.0f;

			if(sounds_loaded && team < 2) {
				PlaySound(sound_shoot);
			}
			break;
		}
	}
}

void add_bullet_with_team(Vector2 position, float rotation, Color color, int team, int damage)
{
	add_bullet_with_team_and_level(position, rotation, color, team, damage, 1);
}

void update_bullets(float delta_time)
{
	extern Bullet bullets[MAX_BULLETS];
	// check_bullet_obstacle_collision is defined in game_obstacles.h
	extern bool check_bullet_obstacle_collision(Vector2 bullet_pos, Bullet* bullet);

	for(int i = 0; i < MAX_BULLETS; i++) {
		if(bullets[i].active) {
			bullets[i].lifetime += delta_time;

			if(bullets[i].lifetime >= BULLET_LIFETIME) {
				bullets[i].active = false;
				continue;
			}

			bullets[i].position = Vector2Add(bullets[i].position, Vector2Scale(bullets[i].velocity, delta_time));

			if(check_bullet_obstacle_collision(bullets[i].position, &bullets[i])) {
				bullets[i].active = false;
				continue;
			}

			if(bullets[i].position.x < 0 || bullets[i].position.x > SCREEN_WIDTH ||
				bullets[i].position.y < 0 || bullets[i].position.y > SCREEN_HEIGHT) {
				bullets[i].active = false;
			}
		}
	}
}

void draw_bullets(void)
{
	extern Bullet bullets[MAX_BULLETS];

	for(int i = 0; i < MAX_BULLETS; i++) {
		if(bullets[i].active) {
			DrawCircleV(bullets[i].position, BULLET_SIZE, bullets[i].color);
			DrawCircleLinesV(bullets[i].position, BULLET_SIZE, BLACK);
		}
	}
}

void check_bullet_tank_collisions(void)
{
	extern Bullet bullets[MAX_BULLETS];
	extern Tank player1_tank, player2_tank;
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];
	extern GameState game_state;
	extern Sound sound_hit, sound_explosion;
	extern bool sounds_loaded;

	for(int b = 0; b < MAX_BULLETS; b++) {
		if(!bullets[b].active) continue;

		Rectangle bullet_rect = { bullets[b].position.x - BULLET_SIZE, bullets[b].position.y - BULLET_SIZE,
								BULLET_SIZE * 2, BULLET_SIZE * 2 };

		// Check vs player 1
		if(player1_tank.active && bullets[b].team != 0) {
			Rectangle tank_rect = { player1_tank.position.x, player1_tank.position.y, TANK_SIZE, TANK_SIZE };
			if(CheckCollisionRecs(bullet_rect, tank_rect)) {
				bullets[b].active = false;

				if(player1_tank.has_shield) {
					Vector2 tank_center = { player1_tank.position.x + TANK_SIZE / 2, player1_tank.position.y + TANK_SIZE / 2 };
					create_explosion(tank_center, SKYBLUE);
					if(sounds_loaded) PlaySound(sound_hit);
					continue;
				}

				player1_tank.level -= bullets[b].damage;
				Vector2 hit_pos = (Vector2){ player1_tank.position.x + TANK_SIZE / 2, player1_tank.position.y - 5 };
				spawn_floating_damage(hit_pos, bullets[b].damage);
				spawn_hit_particles(hit_pos, YELLOW, 8);

				if(sounds_loaded) {
					PlaySound(sound_hit);
				}

				if(player1_tank.level <= 0) {
					Vector2 tank_center = {
						player1_tank.position.x + TANK_SIZE / 2,
						player1_tank.position.y + TANK_SIZE / 2
					};
					create_explosion(tank_center, GREEN);
					player1_tank.active = false;

					if(sounds_loaded) {
						PlaySound(sound_explosion);
					}

					if(bullets[b].team == 1) game_state.player2_score += 200;
					else if(bullets[b].team == 2) game_state.player2_score += 50;
				}
			}
		}

		// Check vs player 2
		if(player2_tank.active && bullets[b].team != 1) {
			Rectangle tank_rect = { player2_tank.position.x, player2_tank.position.y, TANK_SIZE, TANK_SIZE };
			if(CheckCollisionRecs(bullet_rect, tank_rect)) {
				bullets[b].active = false;

				if(player2_tank.has_shield) {
					Vector2 tank_center = { player2_tank.position.x + TANK_SIZE / 2, player2_tank.position.y + TANK_SIZE / 2 };
					create_explosion(tank_center, SKYBLUE);
					if(sounds_loaded) PlaySound(sound_hit);
					continue;
				}

				player2_tank.level -= bullets[b].damage;
				Vector2 hit_pos = (Vector2){ player2_tank.position.x + TANK_SIZE / 2, player2_tank.position.y - 5 };
				spawn_floating_damage(hit_pos, bullets[b].damage);
				spawn_hit_particles(hit_pos, YELLOW, 8);

				if(sounds_loaded) {
					PlaySound(sound_hit);
				}

				if(player2_tank.level <= 0) {
					Vector2 tank_center = {
						player2_tank.position.x + TANK_SIZE / 2,
						player2_tank.position.y + TANK_SIZE / 2
					};
					create_explosion(tank_center, BLUE);
					player2_tank.active = false;

					if(sounds_loaded) {
						PlaySound(sound_explosion);
					}

					if(bullets[b].team == 0) game_state.player1_score += 200;
					else if(bullets[b].team == 2) game_state.player1_score += 50;
				}
			}
		}

		// Check vs enemies
		if(bullets[b].team != 2) {
			for(int e = 0; e < MAX_ENEMY_TANKS; e++) {
				if(!enemy_tanks[e].active) continue;

				Rectangle enemy_rect = { enemy_tanks[e].position.x, enemy_tanks[e].position.y, TANK_SIZE, TANK_SIZE };
				if(CheckCollisionRecs(bullet_rect, enemy_rect)) {
					bullets[b].active = false;
					enemy_tanks[e].level -= bullets[b].damage;
					Vector2 hit_pos = (Vector2){ enemy_tanks[e].position.x + TANK_SIZE / 2, enemy_tanks[e].position.y - 5 };
					spawn_floating_damage(hit_pos, bullets[b].damage);
					spawn_hit_particles(hit_pos, YELLOW, 8);


					if(sounds_loaded) {
						PlaySound(sound_hit);
					}

					if(enemy_tanks[e].level <= 0) {
						Vector2 tank_center = {
							enemy_tanks[e].position.x + TANK_SIZE / 2,
							enemy_tanks[e].position.y + TANK_SIZE / 2
						};
						create_explosion(tank_center, RED);
						enemy_tanks[e].active = false;

						if(sounds_loaded) {
							PlaySound(sound_explosion);
						}

						if(bullets[b].team == 0) {
							game_state.player1_score += ENEMY_POINTS;
							if(player1_tank.active) {
								int max_allowed_level = get_max_tank_level_for_game_level(game_state.current_level);
								if(player1_tank.level < max_allowed_level) {
									player1_tank.level++;
								}
							}
						}
						else if(bullets[b].team == 1) {
							game_state.player2_score += ENEMY_POINTS;
							if(player2_tank.active) {
								int max_allowed_level = get_max_tank_level_for_game_level(game_state.current_level);
								if(player2_tank.level < max_allowed_level) {
									player2_tank.level++;
								}
							}
						}

						game_state.enemies_killed_this_level++;
					}
				}
			}
		}
	}
}

#endif // GAME_BULLETS_H