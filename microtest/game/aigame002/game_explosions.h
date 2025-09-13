#ifndef GAME_EXPLOSIONS_H
#define GAME_EXPLOSIONS_H

// Function declarations
void create_explosion(Vector2 position, Color explosion_color);
void create_teleport_effect(Vector2 position);
void update_explosions(float delta_time);
void draw_explosions(void);

// Implementation
void create_explosion(Vector2 position, Color explosion_color)
{
	extern Explosion explosions[MAX_EXPLOSIONS];

	for(int i = 0; i < MAX_EXPLOSIONS; i++) {
		if(!explosions[i].active) {
			explosions[i].position = position;
			explosions[i].timer = EXPLOSION_DURATION;
			explosions[i].active = true;
			explosions[i].color = explosion_color;

			for(int p = 0; p < EXPLOSION_PARTICLES; p++) {
				float angle = (float)p * (360.0f / EXPLOSION_PARTICLES) + (rand() % 60 - 30);
				float velocity = 50.0f + (rand() % 100);

				explosions[i].particles[p][0] = 0;
				explosions[i].particles[p][1] = 0;
				explosions[i].particles[p][2] = cosf(angle * DEG2RAD) * velocity;
				explosions[i].particles[p][3] = sinf(angle * DEG2RAD) * velocity;
			}
			break;
		}
	}
}

void create_teleport_effect(Vector2 position)
{
	extern Explosion explosions[MAX_EXPLOSIONS];

	for(int i = 0; i < MAX_EXPLOSIONS; i++) {
		if(!explosions[i].active) {
			explosions[i].position = (Vector2){ position.x + BLOCK_SIZE / 2, position.y + BLOCK_SIZE / 2 };
			explosions[i].timer = 0.5f;
			explosions[i].active = true;
			explosions[i].color = PURPLE;

			for(int p = 0; p < EXPLOSION_PARTICLES; p++) {
				float angle = (float)p * (360.0f / EXPLOSION_PARTICLES);
				float start_radius = 30.0f;
				float velocity = -80.0f;

				explosions[i].particles[p][0] = cosf(angle * DEG2RAD) * start_radius;
				explosions[i].particles[p][1] = sinf(angle * DEG2RAD) * start_radius;
				explosions[i].particles[p][2] = cosf(angle * DEG2RAD) * velocity;
				explosions[i].particles[p][3] = sinf(angle * DEG2RAD) * velocity;
			}
			break;
		}
	}
}

void update_explosions(float delta_time)
{
	extern Explosion explosions[MAX_EXPLOSIONS];

	for(int i = 0; i < MAX_EXPLOSIONS; i++) {
		if(explosions[i].active) {
			explosions[i].timer -= delta_time;

			if(explosions[i].timer <= 0) {
				explosions[i].active = false;
				continue;
			}

			for(int p = 0; p < EXPLOSION_PARTICLES; p++) {
				explosions[i].particles[p][0] += explosions[i].particles[p][2] * delta_time;
				explosions[i].particles[p][1] += explosions[i].particles[p][3] * delta_time;
				explosions[i].particles[p][2] *= 0.98f;
				explosions[i].particles[p][3] *= 0.98f;
			}
		}
	}
}

void draw_explosions(void)
{
	extern Explosion explosions[MAX_EXPLOSIONS];

	for(int i = 0; i < MAX_EXPLOSIONS; i++) {
		if(explosions[i].active) {
			float life_ratio = explosions[i].timer / EXPLOSION_DURATION;

			if(explosions[i].color.r == PURPLE.r && explosions[i].color.g == PURPLE.g && explosions[i].color.b == PURPLE.b) {
				// Teleport effect
				float teleport_life_ratio = explosions[i].timer / 0.5f;
				Vector2 center = explosions[i].position;
				
				for(int ring = 1; ring <= 3; ring++) {
					float ring_radius = ring * 8 * (1.0f - teleport_life_ratio);
					DrawCircleLinesV(center, ring_radius, Fade(PURPLE, teleport_life_ratio));
				}

				for(int p = 0; p < EXPLOSION_PARTICLES; p++) {
					Vector2 particle_pos = {
						explosions[i].position.x + explosions[i].particles[p][0],
						explosions[i].position.y + explosions[i].particles[p][1]
					};

					float particle_alpha = teleport_life_ratio * 0.9f;
					Color particle_color = Fade(MAGENTA, particle_alpha);
					float particle_size = 2.0f * teleport_life_ratio;
					DrawCircleV(particle_pos, particle_size, particle_color);

					if(teleport_life_ratio > 0.3f) {
						DrawPixelV(particle_pos, WHITE);
					}
				}

				DrawCircleV(center, 4.0f * teleport_life_ratio, Fade(WHITE, teleport_life_ratio));
			}
			else {
				// Regular explosion effect
				float flash_size = EXPLOSION_SIZE * (1.0f - life_ratio * 0.5f);
				Color flash_color = Fade(explosions[i].color, life_ratio * 0.8f);
				DrawCircleV(explosions[i].position, flash_size, flash_color);

				Color center_color = Fade(WHITE, life_ratio);
				DrawCircleV(explosions[i].position, flash_size * 0.3f, center_color);

				for(int p = 0; p < EXPLOSION_PARTICLES; p++) {
					Vector2 particle_pos = {
						explosions[i].position.x + explosions[i].particles[p][0],
						explosions[i].position.y + explosions[i].particles[p][1]
					};

					float particle_alpha = life_ratio * 0.9f;
					Color particle_color = Fade(explosions[i].color, particle_alpha);
					float particle_size = 3.0f * life_ratio;
					DrawCircleV(particle_pos, particle_size, particle_color);

					if(life_ratio > 0.5f) {
						DrawPixelV(particle_pos, WHITE);
					}
				}

				if(life_ratio > 0.7f) {
					float ring_radius = EXPLOSION_SIZE * (1.0f - life_ratio) * 3.0f;
					Color ring_color = Fade(explosions[i].color, (life_ratio - 0.7f) * 3.33f);
					DrawCircleLinesV(explosions[i].position, ring_radius, ring_color);
				}
			}
		}
	}
}

#endif // GAME_EXPLOSIONS_H