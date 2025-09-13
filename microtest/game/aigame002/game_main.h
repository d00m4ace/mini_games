#ifndef GAME_MAIN_H
#define GAME_MAIN_H

// =====================================================
// GAME STATE MANAGEMENT
// =====================================================

// Function declarations for game state management
void initialize_game_state(void);
void reset_game_state(void);
void initialize_player_tank(Tank* tank, int team, Vector2 position);
void reset_player_tank(Tank* tank);
void clear_all_entities(void);

// =====================================================
// LEVEL MANAGEMENT
// =====================================================

// Function declarations for level management
void check_game_over(void);
void check_level_progression(void);
void advance_to_next_level(void);

// =====================================================
// INPUT HANDLING
// =====================================================

// Function declarations for input handling
void handle_pause_input(void);
void handle_restart_input(void);

// =====================================================
// GAME UPDATE LOGIC
// =====================================================

// Function declarations for game updates
void update_game_entities(float delta_time);
void handle_enemy_spawning(float delta_time);

// =====================================================
// RENDERING SYSTEM
// =====================================================

// Function declarations for rendering
void render_game_field(float dim_factor);
void render_background(void);
void render_game_entities(void);
void render_ui(float dim_factor);
void render_player_status_indicators(float dim_factor);

// =====================================================
// MAIN GAME LOOP
// =====================================================

void game_update_draw_frame(void);

// =====================================================
// IMPLEMENTATION
// =====================================================

// Initialize game state
void initialize_game_state(void)
{
	extern GameState game_state;

	game_state.player1_score = 0;
	game_state.player2_score = 0;
	game_state.enemy_spawn_timer = ENEMY_SPAWN_TIME;
	game_state.game_over = false;
	game_state.both_players_dead = false;
	game_state.current_level = 1;
	game_state.enemies_killed_this_level = 0;
	game_state.enemy_base_level = 1;
	game_state.level_complete = false;
	game_state.level_complete_timer = 0.0f;

	// Initialize power-up spawning system
	game_state.power_up_spawn_timer = POWER_UP_SPAWN_CHECK_INTERVAL;
	game_state.healing_respawn_timer = 5.0f;
	game_state.ammo_respawn_timer = 8.0f;
	game_state.shield_respawn_timer = 12.0f;
	game_state.active_healing_stations = 0;
	game_state.active_ammo_depots = 0;
	game_state.active_shield_generators = 0;
}

// Reset game state for restart
void reset_game_state(void)
{
	extern GameState game_state;
	extern bool game_paused;

	game_state.player1_score = 0;
	game_state.player2_score = 0;
	game_state.enemy_spawn_timer = ENEMY_SPAWN_TIME;
	game_state.game_over = false;
	game_state.both_players_dead = false;
	game_state.current_level = 1;
	game_state.enemies_killed_this_level = 0;
	game_state.enemy_base_level = 1;
	game_state.level_complete = false;
	game_state.level_complete_timer = 0.0f;
	game_paused = false;

	// Reset power-up spawning system
	game_state.power_up_spawn_timer = POWER_UP_SPAWN_CHECK_INTERVAL;
	game_state.healing_respawn_timer = 5.0f;
	game_state.ammo_respawn_timer = 8.0f;
	game_state.shield_respawn_timer = 12.0f;
	game_state.active_healing_stations = 0;
	game_state.active_ammo_depots = 0;
	game_state.active_shield_generators = 0;
}

// Initialize a player tank with default values
void initialize_player_tank(Tank* tank, int team, Vector2 position)
{
	tank->position = position;
	tank->rotation = 270.0f;
	tank->cannon_rotation = 270.0f;
	tank->reload_timer = 0.0f;
	tank->team = team;
	tank->active = true;
	tank->level = INITIAL_TANK_LEVEL;
	tank->is_player_controlled = true;

	// Set colors based on team
	if(team == 0) { // Player 1
		tank->body_color = GREEN;
		tank->cannon_color = DARKGREEN;
		tank->key_forward = KEY_W;
		tank->key_backward = KEY_S;
		tank->key_turn_left = KEY_A;
		tank->key_turn_right = KEY_D;
		tank->key_shoot = KEY_SPACE;
	}
	else { // Player 2
		tank->body_color = BLUE;
		tank->cannon_color = DARKBLUE;
		tank->key_forward = KEY_UP;
		tank->key_backward = KEY_DOWN;
		tank->key_turn_left = KEY_LEFT;
		tank->key_turn_right = KEY_RIGHT;
		tank->key_shoot = KEY_ENTER;
	}

	// Initialize enhanced fields
	tank->velocity = (Vector2){ 0, 0 };
	tank->slide_friction = 1.0f;
	tank->on_ice = false;
	tank->on_conveyor = false;
	tank->conveyor_force = (Vector2){ 0, 0 };
	tank->speed_multiplier = 1.0f;
	tank->has_shield = false;
	tank->shield_timer = 0.0f;
	tank->rapid_fire_timer = 0.0f;
	tank->healing_timer = 0.0f;
	tank->last_safe_position = position;
	tank->last_teleport_time = 0.0f;
}

// Reset player tank to default state
void reset_player_tank(Tank* tank)
{
	tank->level = INITIAL_TANK_LEVEL;
	tank->reload_timer = 0.0f;
	tank->velocity = (Vector2){ 0, 0 };
	tank->slide_friction = 1.0f;
	tank->on_ice = false;
	tank->on_conveyor = false;
	tank->conveyor_force = (Vector2){ 0, 0 };
	tank->speed_multiplier = 1.0f;
	tank->has_shield = false;
	tank->shield_timer = 0.0f;
	tank->rapid_fire_timer = 0.0f;
	tank->healing_timer = 0.0f;
	tank->last_teleport_time = 0.0f;
}

// Clear all game entities
void clear_all_entities(void)
{
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];
	extern Explosion explosions[MAX_EXPLOSIONS];
	extern Bullet bullets[MAX_BULLETS];

	// Clear all entities
	for(int i = 0; i < MAX_ENEMY_TANKS; i++) {
		enemy_tanks[i].active = false;
	}

	for(int i = 0; i < MAX_EXPLOSIONS; i++) {
		explosions[i].active = false;
	}

	for(int i = 0; i < MAX_BULLETS; i++) {
		bullets[i].active = false;
		bullets[i].lifetime = 0.0f;
	}

	for(int i=0; i< MAX_FLOATING_DAMAGE; i++) {
		floating_damages[i].active = false;
		floating_damages[i].lifetime = 0.0f;
	}

	for(int i=0; i< MAX_HIT_PARTICLES; i++) {
		hit_particles[i].active = false;
		hit_particles[i].lifetime = 0.0f;
	}
}

// Check if game is over
void check_game_over(void)
{
	extern Tank player1_tank, player2_tank;
	extern GameState game_state;
	extern SplashScreen splash_screen;

	if(!player1_tank.active && !player2_tank.active && !splash_screen.active) {
		game_state.game_over = true;
		game_state.both_players_dead = true;
		init_splash_screen(SPLASH_GAME_OVER, game_state.current_level);
	}
}

// Check if current level is complete
void check_level_progression(void)
{
	extern GameState game_state;
	extern SplashScreen splash_screen;

	int required_kills = get_enemies_per_level(game_state.current_level);

	if(game_state.enemies_killed_this_level >= required_kills &&
		!game_state.level_complete && !splash_screen.active) {
		init_splash_screen(SPLASH_LEVEL_COMPLETE, game_state.current_level);
		game_state.level_complete = true;
		game_state.level_complete_timer = 1.0f;
	}
}

// Advance to the next level
void advance_to_next_level(void)
{
	extern GameState game_state;
	extern Tank player1_tank, player2_tank;

	// Update level progression
	game_state.current_level++;
	game_state.enemies_killed_this_level = 0;
	game_state.enemy_base_level = game_state.current_level;
	game_state.level_complete = false;
	game_state.level_complete_timer = 0.0f;
	game_state.enemy_spawn_timer = ENEMY_SPAWN_TIME;

	// Reset power-up spawning system
	game_state.power_up_spawn_timer = POWER_UP_SPAWN_CHECK_INTERVAL;
	game_state.healing_respawn_timer = 5.0f;
	game_state.ammo_respawn_timer = 8.0f;
	game_state.shield_respawn_timer = 12.0f;
	game_state.active_healing_stations = 0;
	game_state.active_ammo_depots = 0;
	game_state.active_shield_generators = 0;

	// Respawn dead players
	if(!player1_tank.active) {
		player1_tank.active = true;
		player1_tank.level = 3;
		reset_player_tank(&player1_tank);
	}

	if(!player2_tank.active) {
		player2_tank.active = true;
		player2_tank.level = 3;
		reset_player_tank(&player2_tank);
	}

	// Ensure players don't exceed level limits
	int max_allowed_level = get_max_tank_level_for_game_level(game_state.current_level);

	if(player1_tank.active && player1_tank.level > max_allowed_level) {
		player1_tank.level = max_allowed_level;
	}

	if(player2_tank.active && player2_tank.level > max_allowed_level) {
		player2_tank.level = max_allowed_level;
	}

	// Clear entities and reload map
	clear_all_entities();
	load_level(game_state.current_level);
	load_map_from_ascii();

	// Find new safe spawn positions for both players after loading the map
	if(player1_tank.active) {
		Vector2 safe_position1 = find_safe_spawn_position();
		player1_tank.position = safe_position1;
		player1_tank.last_safe_position = safe_position1;
	}

	if(player2_tank.active) {
		Vector2 safe_position2;
		int spawn_attempts = 0;

		// Try to spawn player 2 away from player 1
		do {
			safe_position2 = find_safe_spawn_position();
			spawn_attempts++;

			// Check distance from player 1 if both are active
			if(player1_tank.active && spawn_attempts < 50) {
				float distance = Vector2Distance(player1_tank.position, safe_position2);
				if(distance > TANK_SIZE * 3) { // At least 3 tank sizes apart
					break;
				}
			}
			else {
				break; // Accept position after max attempts or if player 1 inactive
			}
		} while(spawn_attempts < 100); // Increased max attempts for better positioning

		player2_tank.position = safe_position2;
		player2_tank.last_safe_position = safe_position2;
	}

	// Award level completion bonus
	if(player1_tank.active) game_state.player1_score += game_state.current_level * 50;
	if(player2_tank.active) game_state.player2_score += game_state.current_level * 50;
}

void start_new_game(void)
{
	extern Tank player1_tank, player2_tank;
	extern GameState game_state;

	// Reset game state
	reset_game_state();
	clear_all_entities();
	load_level(game_state.current_level);
	load_map_from_ascii();

	// Initialize players
	Vector2 safe_pos1 = find_safe_spawn_position();
	Vector2 safe_pos2 = find_safe_spawn_position();

	initialize_player_tank(&player1_tank, 0, safe_pos1);
	initialize_player_tank(&player2_tank, 1, safe_pos2);

	// Initialize enemy tanks as inactive
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];
	for(int i = 0; i < MAX_ENEMY_TANKS; i++) {
		enemy_tanks[i].active = false;
	}
}

void handle_game_over_input(void)
{
	extern GameState game_state;
	extern SplashScreen splash_screen;
	extern float global_input_cooldown;

	if(game_state.game_over && IsKeyPressed(KEY_SPACE) &&
		splash_screen.type == SPLASH_GAME_OVER && splash_screen.active &&
		global_input_cooldown <= 0) {  // NEW: Check cooldown

		game_state.game_over = false;
		global_input_cooldown = 0.3f;  // Set cooldown
		start_new_game();
		init_splash_screen(SPLASH_GAME_START, 1);
	}
}

void handle_pause_input(void)
{
	extern GameState game_state;
	extern SplashScreen splash_screen;
	extern bool game_paused;
	extern float global_input_cooldown;

	if(IsKeyPressed(KEY_P) && !game_state.game_over && !splash_screen.active)
	{
		game_paused = true;
		init_splash_screen(SPLASH_PAUSE, 0);
	}
	else if(IsKeyPressed(KEY_P) && !game_state.game_over && game_paused)
	{
		game_paused = false;
		splash_screen.active = false;
	}
}

// Handle restart input
void handle_restart_input(void)
{
	extern GameState game_state;
	extern SplashScreen splash_screen;
	extern Tank player1_tank, player2_tank;

	if(game_state.game_over && IsKeyPressed(KEY_SPACE) &&
		splash_screen.type == SPLASH_GAME_OVER && !splash_screen.active) {

		// Reset game state
		reset_game_state();
		clear_all_entities();
		load_level(game_state.current_level);
		load_map_from_ascii();

		// Reset players
		Vector2 safe_pos1 = find_safe_spawn_position();
		Vector2 safe_pos2 = find_safe_spawn_position();

		initialize_player_tank(&player1_tank, 0, safe_pos1);
		initialize_player_tank(&player2_tank, 1, safe_pos2);

		init_splash_screen(SPLASH_GAME_START, 1);
	}
}

// Update all game entities
void update_game_entities(float delta_time)
{
	extern Tank player1_tank, player2_tank;
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];

	// Update players
	if(player1_tank.active) update_tank(&player1_tank, delta_time);
	if(player2_tank.active) update_tank(&player2_tank, delta_time);

	// Update enemies
	for(int i = 0; i < MAX_ENEMY_TANKS; i++) {
		update_enemy_tank(&enemy_tanks[i], delta_time);
	}

	// Update bullets
	update_bullets(delta_time);

	// Check collisions
	check_bullet_tank_collisions();

	// Update explosions
	update_explosions(delta_time);

	// Update damage particles
	update_damage_fx(delta_time);

	// Update obstacles
	update_obstacles(delta_time);

	// Update power-up spawning
	update_power_up_spawning(delta_time);
}

// Handle enemy spawning logic
void handle_enemy_spawning(float delta_time)
{
	extern GameState game_state;

	game_state.enemy_spawn_timer -= delta_time;
	int max_active = get_max_active_enemies(game_state.current_level);

	if(game_state.enemy_spawn_timer <= 0 && count_active_enemies() < max_active) {
		spawn_enemy_tank();
		game_state.enemy_spawn_timer = ENEMY_SPAWN_TIME;
	}
}

// Render the background grid and border
void render_background(void)
{
	// Draw background grid
	Color grid_color = (Color){ 20, 20, 20, 255 };

	for(int x = 0; x <= MAP_WIDTH; x++) {
		int line_x = MAP_OFFSET_X + x * BLOCK_SIZE;
		DrawLine(line_x, MAP_OFFSET_Y, line_x, MAP_OFFSET_Y + MAP_HEIGHT * BLOCK_SIZE, grid_color);
	}
	for(int y = 0; y <= MAP_HEIGHT; y++) {
		int line_y = MAP_OFFSET_Y + y * BLOCK_SIZE;
		DrawLine(MAP_OFFSET_X, line_y, MAP_OFFSET_X + MAP_WIDTH * BLOCK_SIZE, line_y, grid_color);
	}

	// Draw map border
	DrawRectangleLines(MAP_OFFSET_X - 2, MAP_OFFSET_Y - 2,
		MAP_WIDTH * BLOCK_SIZE + 4, MAP_HEIGHT * BLOCK_SIZE + 4, WHITE);
}

// Render all game entities
void render_game_entities(void)
{
	extern Tank player1_tank, player2_tank;
	extern Tank enemy_tanks[MAX_ENEMY_TANKS];

	// Draw background obstacles
	draw_obstacles_background();

	// Draw tanks with levels
	if(player1_tank.active) {
		draw_tank(player1_tank);
		draw_tank_level(player1_tank);
	}
	if(player2_tank.active) {
		draw_tank(player2_tank);
		draw_tank_level(player2_tank);
	}

	for(int i = 0; i < MAX_ENEMY_TANKS; i++) {
		if(enemy_tanks[i].active) {
			draw_tank(enemy_tanks[i]);
			draw_tank_level(enemy_tanks[i]);
		}
	}

	// Draw bullets
	draw_bullets();

	// Draw explosions
	draw_explosions();

	// Draw damage particles
	draw_damage_fx();

	// Draw foreground obstacles
	draw_obstacles_foreground();
}

// Render player status indicators
void render_player_status_indicators(float dim_factor)
{
	extern Tank player1_tank, player2_tank;
	extern Font ui_font;

	// Choose font based on availability
	Font current_ui_font = (ui_font.texture.id > 0) ? ui_font : GetFontDefault();
	int ui_size = (current_ui_font.baseSize > 0) ? (int)(current_ui_font.baseSize * 0.6f) : 14;

	// Player 1 status indicators
	if(player1_tank.active) {
		int ui_y = 85; // Start below the level info

		if(player1_tank.has_shield) {
			Color shield_color = dim_factor < 1.0f ?
				(Color) {
				(int)(135 * dim_factor), (int)(206 * dim_factor), (int)(235 * dim_factor), 255
			} : SKYBLUE;
			DrawTextEx(current_ui_font, "P1: SHIELD", (Vector2) { 10, ui_y }, ui_size, 1, shield_color);
			ui_y += ui_size + 2;
		}
		if(player1_tank.rapid_fire_timer > 0) {
			Color rapid_color = dim_factor < 1.0f ?
				(Color) {
				(int)(255 * dim_factor), (int)(255 * dim_factor), 0, 255
			} : YELLOW;
			DrawTextEx(current_ui_font, "P1: RAPID FIRE", (Vector2) { 10, ui_y }, ui_size, 1, rapid_color);
			ui_y += ui_size + 2;
		}
		if(player1_tank.healing_timer > 0) {
			int heals_completed = (int)(10.0f - player1_tank.healing_timer);
			char* heal_text = TextFormat("P1: HEALING %d/10", heals_completed);
			Color heal_color = dim_factor < 1.0f ?
				(Color) {
				0, (int)(255 * dim_factor), 0, 255
			} : GREEN;
			DrawTextEx(current_ui_font, heal_text, (Vector2) { 10, ui_y }, ui_size, 1, heal_color);
			ui_y += ui_size + 2;
		}
		if(player1_tank.on_ice) {
			Color ice_color = dim_factor < 1.0f ?
				(Color) {
				(int)(135 * dim_factor), (int)(206 * dim_factor), (int)(235 * dim_factor), 255
			} : SKYBLUE;
			DrawTextEx(current_ui_font, "P1: ON ICE", (Vector2) { 10, ui_y }, ui_size, 1, ice_color);
			ui_y += ui_size + 2;
		}
		if(player1_tank.speed_multiplier < 1.0f) {
			Color slow_color = dim_factor < 1.0f ?
				(Color) {
				(int)(255 * dim_factor), (int)(165 * dim_factor), 0, 255
			} : ORANGE;
			DrawTextEx(current_ui_font, "P1: SLOWED", (Vector2) { 10, ui_y }, ui_size, 1, slow_color);
			ui_y += ui_size + 2;
		}
	}

	// Player 2 status indicators
	if(player2_tank.active) {
		int ui_y = 85; // Start below the level info
		int ui_x = SCREEN_WIDTH - 150; // Adjusted for font width

		if(player2_tank.has_shield) {
			Color shield_color = dim_factor < 1.0f ?
				(Color) {
				(int)(135 * dim_factor), (int)(206 * dim_factor), (int)(235 * dim_factor), 255
			} : SKYBLUE;
			DrawTextEx(current_ui_font, "P2: SHIELD", (Vector2) { ui_x, ui_y }, ui_size, 1, shield_color);
			ui_y += ui_size + 2;
		}
		if(player2_tank.rapid_fire_timer > 0) {
			Color rapid_color = dim_factor < 1.0f ?
				(Color) {
				(int)(255 * dim_factor), (int)(255 * dim_factor), 0, 255
			} : YELLOW;
			DrawTextEx(current_ui_font, "P2: RAPID FIRE", (Vector2) { ui_x, ui_y }, ui_size, 1, rapid_color);
			ui_y += ui_size + 2;
		}
		if(player2_tank.healing_timer > 0) {
			int heals_completed = (int)(10.0f - player2_tank.healing_timer);
			char* heal_text = TextFormat("P2: HEALING %d/10", heals_completed);
			Color heal_color = dim_factor < 1.0f ?
				(Color) {
				0, (int)(255 * dim_factor), 0, 255
			} : GREEN;
			DrawTextEx(current_ui_font, heal_text, (Vector2) { ui_x, ui_y }, ui_size, 1, heal_color);
			ui_y += ui_size + 2;
		}
		if(player2_tank.on_ice) {
			Color ice_color = dim_factor < 1.0f ?
				(Color) {
				(int)(135 * dim_factor), (int)(206 * dim_factor), (int)(235 * dim_factor), 255
			} : SKYBLUE;
			DrawTextEx(current_ui_font, "P2: ON ICE", (Vector2) { ui_x, ui_y }, ui_size, 1, ice_color);
			ui_y += ui_size + 2;
		}
		if(player2_tank.speed_multiplier < 1.0f) {
			Color slow_color = dim_factor < 1.0f ?
				(Color) {
				(int)(255 * dim_factor), (int)(165 * dim_factor), 0, 255
			} : ORANGE;
			DrawTextEx(current_ui_font, "P2: SLOWED", (Vector2) { ui_x, ui_y }, ui_size, 1, slow_color);
			ui_y += ui_size + 2;
		}
	}
}

// Render main game UI
void render_ui(float dim_factor)
{
	extern GameState game_state;
	extern SplashScreen splash_screen;
	extern bool game_paused;
	extern Font ui_font;

	if(!splash_screen.active || splash_screen.type == SPLASH_PAUSE) {
		// Choose font based on availability
		Font current_ui_font = (ui_font.texture.id > 0) ? ui_font : GetFontDefault();
		int ui_size = (current_ui_font.baseSize > 0) ? (int)(current_ui_font.baseSize * 0.7f) : 16;
		int small_ui_size = (current_ui_font.baseSize > 0) ? (int)(current_ui_font.baseSize * 0.6f) : 14;

		// Calculate dimmed colors
		Color green_ui = dim_factor < 1.0f ?
			(Color) {
			0, (int)(255 * dim_factor), 0, 255
		} : GREEN;
		Color blue_ui = dim_factor < 1.0f ?
			(Color) {
			0, 0, (int)(255 * dim_factor), 255
		} : BLUE;
		Color yellow_ui = dim_factor < 1.0f ?
			(Color) {
			(int)(255 * dim_factor), (int)(255 * dim_factor), 0, 255
		} : YELLOW;
		Color gold_ui = dim_factor < 1.0f ?
			(Color) {
			(int)(255 * dim_factor), (int)(215 * dim_factor), 0, 255
		} : GOLD;

		// Draw main UI - Player scores
		char* p1_info = TextFormat("P1: %d pts", game_state.player1_score);
		char* p2_info = TextFormat("P2: %d pts", game_state.player2_score);
		DrawTextEx(current_ui_font, p1_info, (Vector2) { 10, 10 }, ui_size, 1, green_ui);
		DrawTextEx(current_ui_font, p2_info, (Vector2) { 10, 10 + ui_size + 2 }, ui_size, 1, blue_ui);

		// Draw level info - split into two lines
		char* level_info = TextFormat("Level: %d", game_state.current_level);
		char* enemies_info = TextFormat("Enemies: %d/%d",
			game_state.enemies_killed_this_level,
			get_enemies_per_level(game_state.current_level));

		int level_y_start = 10 + (ui_size + 2) * 2;
		DrawTextEx(current_ui_font, level_info, (Vector2) { 10, level_y_start }, ui_size, 1, yellow_ui);
		DrawTextEx(current_ui_font, enemies_info, (Vector2) { 10, level_y_start + ui_size + 2 }, ui_size, 1, yellow_ui);

		// Draw player status indicators
		render_player_status_indicators(dim_factor);

		// Draw pause indicator
		if(game_paused && splash_screen.type == SPLASH_PAUSE) {
			int pause_y = 220;
			DrawTextEx(current_ui_font, "PAUSED - Press P to continue",
				(Vector2) {
				10, pause_y
			}, small_ui_size, 1, gold_ui);
		}
	}
}

// Render the game field with optional dimming
void render_game_field(float dim_factor)
{
	extern GameState game_state;

	if(dim_factor < 1.0f) {
		// Render dimmed background for pause
		Color grid_color = (Color){
			(int)(20 * dim_factor), (int)(20 * dim_factor), (int)(20 * dim_factor), 255
		};

		for(int x = 0; x <= MAP_WIDTH; x++) {
			int line_x = MAP_OFFSET_X + x * BLOCK_SIZE;
			DrawLine(line_x, MAP_OFFSET_Y, line_x, MAP_OFFSET_Y + MAP_HEIGHT * BLOCK_SIZE, grid_color);
		}
		for(int y = 0; y <= MAP_HEIGHT; y++) {
			int line_y = MAP_OFFSET_Y + y * BLOCK_SIZE;
			DrawLine(MAP_OFFSET_X, line_y, MAP_OFFSET_X + MAP_WIDTH * BLOCK_SIZE, line_y, grid_color);
		}

		DrawRectangleLines(MAP_OFFSET_X - 2, MAP_OFFSET_Y - 2,
			MAP_WIDTH * BLOCK_SIZE + 4, MAP_HEIGHT * BLOCK_SIZE + 4, WHITE);
	}
	else {
		render_background();
	}

	render_game_entities();
	render_ui(dim_factor);
}

// Main game loop function
void game_update_draw_frame(void)
{
	extern Tank player1_tank, player2_tank;
	extern GameState game_state;
	extern bool game_initialized;
	extern SplashScreen splash_screen;
	extern bool game_paused;

	static bool is_first_frame = true;

	// First-time initialization - start with main menu
	if(is_first_frame) {
		load_game_sounds();
		initialize_game_state();

		// Load fonts anonymous_pro_bold pixantiqua
		title_font = LoadFontEx("resources/anonymous_pro_bold.ttf", 72, 0, 250);
		subtitle_font = LoadFontEx("resources/anonymous_pro_bold.ttf", 32, 0, 250);
		ui_font = LoadFontEx("resources/anonymous_pro_bold.ttf", 20, 0, 250);
		button_font = LoadFontEx("resources/anonymous_pro_bold.ttf", 36, 0, 250);

		// Set texture filter for better scaling
		SetTextureFilter(title_font.texture, TEXTURE_FILTER_BILINEAR);
		SetTextureFilter(subtitle_font.texture, TEXTURE_FILTER_BILINEAR);
		SetTextureFilter(ui_font.texture, TEXTURE_FILTER_BILINEAR);
		SetTextureFilter(button_font.texture, TEXTURE_FILTER_BILINEAR);

		// Start with main menu
		init_splash_screen(SPLASH_MAIN_MENU, 0);

		game_initialized = true;
		is_first_frame = false;
	}

	// Handle input in correct order
	handle_pause_input();
	handle_game_over_input();

	// Update splash screen
	update_splash_screen(GetFrameTime());

	// Determine if game should update
	bool should_update_game = !game_state.game_over && !game_paused;

	if(splash_screen.active) {
		switch(splash_screen.type) {
		case SPLASH_MAIN_MENU:
		case SPLASH_GAME_START:
		case SPLASH_LEVEL_COMPLETE:
		case SPLASH_GAME_OVER:
		case SPLASH_PAUSE:
			should_update_game = false;
			break;
		default:
			break;
		}
	}

	// Check if we have players initialized (for game field rendering)
	bool players_exist = (player1_tank.active || player2_tank.active);

	// Update game logic 
	if(!game_state.game_over && should_update_game && players_exist) {
		float delta_time = GetFrameTime();

		update_game_entities(delta_time);
		check_level_progression();

		// Handle level completion
		if(game_state.level_complete && !splash_screen.active) {
			advance_to_next_level();
		}

		if(IsKeyPressed(KEY_N) && !game_paused && !splash_screen.active) {
			advance_to_next_level();
		}

		check_game_over();
		handle_enemy_spawning(delta_time);
	}	

	// Rendering
	BeginDrawing();
	ClearBackground(BLACK);

	// Determine what to show
	bool show_game_field = false;  // Default to false

	if(!splash_screen.active) {
		// No splash screen active
		if(players_exist && !game_state.game_over) {
			show_game_field = true;
		}
	}
	else {
		// Splash screen is active
		switch(splash_screen.type) {
		case SPLASH_PAUSE:
			show_game_field = players_exist && !game_state.game_over;
			break;
		case SPLASH_MAIN_MENU:
		case SPLASH_GAME_START:
		case SPLASH_LEVEL_COMPLETE:
		case SPLASH_GAME_OVER:
		default:
			show_game_field = false;
			break;
		}
	}

	if(show_game_field) {
		// Determine dimming factor
		float dim_factor = (splash_screen.active && splash_screen.type == SPLASH_PAUSE) ? 0.3f : 1.0f;
		render_game_field(dim_factor);
	}
	else {
		// Show clean background for full-screen splashes or when no game is active
		DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
			(Color) {
			10, 10, 20, 255
		}, (Color) { 5, 5, 15, 255 });

		// Add decorative elements for main menu and game start screen
		if(splash_screen.active &&
			(splash_screen.type == SPLASH_MAIN_MENU || splash_screen.type == SPLASH_GAME_START)) {
			for(int i = 0; i < 5; i++) {
				int x = (i * SCREEN_WIDTH / 5) + 50;
				int y = SCREEN_HEIGHT - 100 - (i % 2) * 50;
				DrawRectangle(x, y, 20, 20, (Color) { 30, 30, 30, 100 });
				DrawRectangle(x + 8, y - 8, 4, 15, (Color) { 25, 25, 25, 100 });
			}
		}
	}

	// Always draw splash screen on top if active
	if(splash_screen.active) {
		draw_splash_screen();
	}

	// Draw FPS counter
	DrawFPS(SCREEN_WIDTH - 80, 10);
	EndDrawing();
}

#endif // GAME_MAIN_H