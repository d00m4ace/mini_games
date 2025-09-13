#ifndef GAME_SPLASH_H
#define GAME_SPLASH_H

// Function declarations
void init_splash_screen(SplashType type, int level);
void update_splash_screen(float delta_time);
void draw_splash_screen(void);
void start_new_game(void);

// Implementation
void init_splash_screen(SplashType type, int level)
{
	extern SplashScreen splash_screen;

	splash_screen.type = type;
	splash_screen.active = true;
	splash_screen.fade_alpha = 1.0f;
	splash_screen.level_number = level;
	splash_screen.input_cooldown = 0.5f;  // NEW: 0.5 second cooldown

	switch(type) {
	case SPLASH_MAIN_MENU:  // NEW: Main menu case
		strcpy(splash_screen.title_text, "BATTLE TANKS");
		strcpy(splash_screen.subtitle_text, "PRESS SPACE TO START GAME");
		splash_screen.title_color = YELLOW;
		splash_screen.subtitle_color = WHITE;
		splash_screen.timer = -1.0f;
		splash_screen.show_continue_prompt = true;
		break;

	case SPLASH_GAME_START:
		strcpy(splash_screen.title_text, "BATTLE TANKS");
		strcpy(splash_screen.subtitle_text, "PRESS ANY KEY TO START");
		splash_screen.title_color = YELLOW;
		splash_screen.subtitle_color = WHITE;
		splash_screen.timer = -1.0f;
		splash_screen.show_continue_prompt = true;
		break;

	case SPLASH_LEVEL_COMPLETE:
		sprintf(splash_screen.title_text, "LEVEL %d COMPLETE!", level);
		strcpy(splash_screen.subtitle_text, "PRESS ANY KEY TO CONTINUE");
		splash_screen.title_color = GREEN;
		splash_screen.subtitle_color = LIME;
		splash_screen.timer = -1.0f;
		splash_screen.show_continue_prompt = true;
		break;

	case SPLASH_GAME_OVER:
		strcpy(splash_screen.title_text, "GAME OVER");
		strcpy(splash_screen.subtitle_text, "PRESS SPACE TO CONTINUE");  // Changed text
		splash_screen.title_color = RED;
		splash_screen.subtitle_color = WHITE;
		splash_screen.timer = -1.0f;
		splash_screen.show_continue_prompt = true;
		break;

	case SPLASH_PAUSE:
		strcpy(splash_screen.title_text, "PAUSED");
		strcpy(splash_screen.subtitle_text, "PRESS P TO CONTINUE");
		splash_screen.title_color = GOLD;
		splash_screen.subtitle_color = WHITE;
		splash_screen.timer = -1.0f;
		splash_screen.show_continue_prompt = true;
		break;

	default:
		splash_screen.active = false;
		break;
	}
}

void update_splash_screen(float delta_time)
{
	extern SplashScreen splash_screen;
	extern float global_input_cooldown;  // Add this extern

	if(!splash_screen.active) return;

	// Update cooldowns
	splash_screen.input_cooldown -= delta_time;
	if(splash_screen.input_cooldown < 0) splash_screen.input_cooldown = 0;

	global_input_cooldown -= delta_time;
	if(global_input_cooldown < 0) global_input_cooldown = 0;

	splash_screen.fade_alpha = 1.0f;

	// Only process input if cooldown has expired
	bool can_process_input = (splash_screen.input_cooldown <= 0 && global_input_cooldown <= 0);

	if(splash_screen.show_continue_prompt && can_process_input) {
		if(splash_screen.type == SPLASH_MAIN_MENU) {
			// Handle START button click
			int button_width = 200;
			int button_height = 80;
			int button_x = SCREEN_WIDTH / 2 - button_width / 2;
			int button_y = SCREEN_HEIGHT / 2 - 20;

			Rectangle start_button = { button_x, button_y, button_width, button_height };
			Vector2 mouse_pos = GetMousePosition();

			if((CheckCollisionPointRec(mouse_pos, start_button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
				splash_screen.active = false;
				global_input_cooldown = 0.3f;

				splash_screen.active = false;
				global_input_cooldown = 0.3f;  // Set cooldown
				start_new_game();
				init_splash_screen(SPLASH_GAME_START, 1);
			}
		}
		else if(splash_screen.type == SPLASH_GAME_OVER && IsKeyPressed(KEY_SPACE)) {
			// Don't handle here - let handle_game_over_input handle it
			// But we can set the cooldown here
			global_input_cooldown = 0.3f;
		}
		else if(splash_screen.type == SPLASH_PAUSE && IsKeyPressed(KEY_P)) {
			// Don't handle unpause here - let the main game loop handle it
			global_input_cooldown = 0.3f;
		}
		else if(splash_screen.type == SPLASH_LEVEL_COMPLETE) {
			int key = GetKeyPressed();
			if(key != 0 || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) ||
				IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				splash_screen.active = false;
				global_input_cooldown = 0.3f;  // Set global cooldown
			}
		}
		else if(splash_screen.type == SPLASH_GAME_START) {
			int key = GetKeyPressed();
			if(key != 0 || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) ||
				IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				splash_screen.active = false;
				global_input_cooldown = 0.3f;  // Set global cooldown
			}
		}
	}
}

void draw_splash_screen(void)
{
	extern SplashScreen splash_screen;
	extern Tank player1_tank, player2_tank;
	extern GameState game_state;
	extern Font title_font, subtitle_font, ui_font, button_font;

	if(!splash_screen.active) return;

	DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color) { 0, 0, 0, 200 });

	// Choose fonts based on availability
	Font current_title_font = (title_font.texture.id > 0) ? title_font : GetFontDefault();
	Font current_subtitle_font = (subtitle_font.texture.id > 0) ? subtitle_font : GetFontDefault();
	Font current_ui_font = (ui_font.texture.id > 0) ? ui_font : GetFontDefault();
	Font current_button_font = (button_font.texture.id > 0) ? button_font : GetFontDefault();

	int title_size = (current_title_font.baseSize > 0) ? current_title_font.baseSize : 48;
	int subtitle_size = (current_subtitle_font.baseSize > 0) ? current_subtitle_font.baseSize : 24;
	int ui_size = (current_ui_font.baseSize > 0) ? current_ui_font.baseSize : 18;
	int button_size = (current_button_font.baseSize > 0) ? current_button_font.baseSize : 32;

	Vector2 title_text_size = MeasureTextEx(current_title_font, splash_screen.title_text, title_size, 2);
	Vector2 subtitle_text_size = MeasureTextEx(current_subtitle_font, splash_screen.subtitle_text, subtitle_size, 1);

	Vector2 title_pos = {
		SCREEN_WIDTH / 2 - title_text_size.x / 2,
		SCREEN_HEIGHT / 2 - 150
	};

	Vector2 subtitle_pos = {
		SCREEN_WIDTH / 2 - subtitle_text_size.x / 2,
		SCREEN_HEIGHT / 2 - 60
	};

	// Draw title with enhanced glow effect using fonts
	for(int i = 4; i > 0; i--) {
		Color glow_color = Fade(splash_screen.title_color, 0.2f / i);
		Vector2 glow_pos = { title_pos.x - i * 2, title_pos.y - i * 2 };
		DrawTextEx(current_title_font, splash_screen.title_text, glow_pos, title_size, 2, glow_color);
		glow_pos = (Vector2){ title_pos.x + i * 2, title_pos.y + i * 2 };
		DrawTextEx(current_title_font, splash_screen.title_text, glow_pos, title_size, 2, glow_color);
	}
	DrawTextEx(current_title_font, splash_screen.title_text, title_pos, title_size, 2, splash_screen.title_color);

	// Handle different splash screen types
	if(splash_screen.type == SPLASH_MAIN_MENU) {
		// Draw START button
		int button_width = 200;
		int button_height = 80;
		int button_x = SCREEN_WIDTH / 2 - button_width / 2;
		int button_y = SCREEN_HEIGHT / 2 - 20;

		Rectangle start_button = { button_x, button_y, button_width, button_height };
		Vector2 mouse_pos = GetMousePosition();
		bool is_mouse_over = CheckCollisionPointRec(mouse_pos, start_button);

		// Flashing frame effect
		float flash_intensity = 0.7f + 0.3f * sinf(GetTime() * 6.0f);
		Color frame_color = is_mouse_over ?
			(Color) {
			255, 255, 255, (int)(255 * flash_intensity)
		} :
			(Color) {
			255, 255, 0, (int)(200 * flash_intensity)
		};

			// Draw button background
			Color bg_color = is_mouse_over ? (Color) { 40, 40, 40, 200 } : (Color) { 20, 20, 20, 180 };
			DrawRectangleRec(start_button, bg_color);

			// Draw flashing frame
			DrawRectangleLinesEx(start_button, 4, frame_color);
			if(is_mouse_over) {
				DrawRectangleLinesEx((Rectangle) { button_x - 2, button_y - 2, button_width + 4, button_height + 4 }, 2, YELLOW);
			}

			// Draw START text
			const char* start_text = "START";
			Vector2 start_text_size = MeasureTextEx(current_button_font, start_text, button_size, 2);
			Vector2 start_pos = {
				button_x + button_width / 2 - start_text_size.x / 2,
				button_y + button_height / 2 - start_text_size.y / 2
			};

			Color start_text_color = is_mouse_over ? WHITE : YELLOW;
			DrawTextEx(current_button_font, start_text, start_pos, button_size, 2, start_text_color);

			// Additional main menu content
			char* option1 = "Click START to begin your battle";
			char* option2 = "ESC - Exit Game";

			int y_offset = button_y + button_height + 40;
			Vector2 option1_size = MeasureTextEx(current_ui_font, option1, ui_size, 1);
			Vector2 option2_size = MeasureTextEx(current_ui_font, option2, ui_size, 1);

			DrawTextEx(current_ui_font, option1,
				(Vector2) {
				SCREEN_WIDTH / 2 - option1_size.x / 2, y_offset
			}, ui_size, 1, GREEN);
			DrawTextEx(current_ui_font, option2,
				(Vector2) {
				SCREEN_WIDTH / 2 - option2_size.x / 2, y_offset + 30
			}, ui_size, 1, RED);

			// Game info
			char* info1 = "2-Player Tank Battle Game";
			char* info2 = "Defeat enemies to advance levels";
			char* info3 = "Collect power-ups to gain advantages";

			int info_y = y_offset + 80;
			Vector2 info1_size = MeasureTextEx(current_ui_font, info1, 16, 1);
			Vector2 info2_size = MeasureTextEx(current_ui_font, info2, 16, 1);
			Vector2 info3_size = MeasureTextEx(current_ui_font, info3, 16, 1);

			DrawTextEx(current_ui_font, info1, (Vector2) { SCREEN_WIDTH / 2 - info1_size.x / 2, info_y }, 16, 1, LIGHTGRAY);
			DrawTextEx(current_ui_font, info2, (Vector2) { SCREEN_WIDTH / 2 - info2_size.x / 2, info_y + 20 }, 16, 1, LIGHTGRAY);
			DrawTextEx(current_ui_font, info3, (Vector2) { SCREEN_WIDTH / 2 - info3_size.x / 2, info_y + 40 }, 16, 1, LIGHTGRAY);

			// Copyright
			char* copyright = "(C) d00m4ace.com";
			Vector2 copyright_size = MeasureTextEx(current_ui_font, copyright, 32, 1);
			int copyright_y = SCREEN_HEIGHT - 50;
			DrawTextEx(current_ui_font, copyright,
				(Vector2) {
				SCREEN_WIDTH / 2 - copyright_size.x / 2, copyright_y
			}, 32, 1, (Color) { 128, 128, 128, 255 });
	}
	else if(splash_screen.type == SPLASH_GAME_START) {
		// Draw subtitle with pulsing effect
		if(splash_screen.show_continue_prompt) {
			float pulse = 0.7f + 0.3f * sinf(GetTime() * 4.0f);
			Color pulsing_subtitle = Fade(splash_screen.subtitle_color, pulse);
			DrawTextEx(current_subtitle_font, splash_screen.subtitle_text, subtitle_pos, subtitle_size, 1, pulsing_subtitle);
		}
		else {
			DrawTextEx(current_subtitle_font, splash_screen.subtitle_text, subtitle_pos, subtitle_size, 1, splash_screen.subtitle_color);
		}

		char* controls1 = "Player 1: WASD + SPACE";
		char* controls2 = "Player 2: ARROWS + ENTER";
		char* controls3 = "Press P to PAUSE";

		int y_offset = subtitle_pos.y + 80;
		Vector2 c1_size = MeasureTextEx(current_ui_font, controls1, ui_size, 1);
		Vector2 c2_size = MeasureTextEx(current_ui_font, controls2, ui_size, 1);
		Vector2 c3_size = MeasureTextEx(current_ui_font, controls3, ui_size, 1);

		DrawTextEx(current_ui_font, controls1, (Vector2) { SCREEN_WIDTH / 2 - c1_size.x / 2, y_offset }, ui_size, 1, GREEN);
		DrawTextEx(current_ui_font, controls2, (Vector2) { SCREEN_WIDTH / 2 - c2_size.x / 2, y_offset + 25 }, ui_size, 1, BLUE);
		DrawTextEx(current_ui_font, controls3, (Vector2) { SCREEN_WIDTH / 2 - c3_size.x / 2, y_offset + 55 }, ui_size, 1, YELLOW);

		char* copyright = "(C) d00m4ace.com";
		Vector2 copyright_size = MeasureTextEx(current_ui_font, copyright, 32, 1);
		int copyright_y = SCREEN_HEIGHT - 50;
		DrawTextEx(current_ui_font, copyright,
			(Vector2) {
			SCREEN_WIDTH / 2 - copyright_size.x / 2, copyright_y
		}, 32, 1, (Color) { 128, 128, 128, 255 });
	}
	else if(splash_screen.type == SPLASH_LEVEL_COMPLETE) {
		// Draw subtitle with pulsing effect
		if(splash_screen.show_continue_prompt) {
			float pulse = 0.7f + 0.3f * sinf(GetTime() * 4.0f);
			Color pulsing_subtitle = Fade(splash_screen.subtitle_color, pulse);
			DrawTextEx(current_subtitle_font, splash_screen.subtitle_text, subtitle_pos, subtitle_size, 1, pulsing_subtitle);
		}
		else {
			DrawTextEx(current_subtitle_font, splash_screen.subtitle_text, subtitle_pos, subtitle_size, 1, splash_screen.subtitle_color);
		}

		// Show player tank status during level complete
		int tank_display_y = SCREEN_HEIGHT / 2 + 20;
		int tank_spacing = 200;
		int tank_start_x = SCREEN_WIDTH / 2 - tank_spacing;

		// Player 1 status
		char* p1_status = player1_tank.active ? "ALIVE" : "DEAD";
		Color p1_status_color = player1_tank.active ? GREEN : RED;
		char* p1_info = TextFormat("PLAYER 1: %s", p1_status);
		char* p1_level = TextFormat("Level: %d", player1_tank.level);
		char* p1_score = TextFormat("Score: %d", game_state.player1_score);

		Vector2 p1_info_size = MeasureTextEx(current_ui_font, p1_info, 20, 1);
		Vector2 p1_level_size = MeasureTextEx(current_ui_font, p1_level, 16, 1);
		Vector2 p1_score_size = MeasureTextEx(current_ui_font, p1_score, 16, 1);

		DrawTextEx(current_ui_font, p1_info, (Vector2) { tank_start_x - p1_info_size.x / 2, tank_display_y }, 20, 1, p1_status_color);
		DrawTextEx(current_ui_font, p1_level, (Vector2) { tank_start_x - p1_level_size.x / 2, tank_display_y + 25 }, 16, 1, GREEN);
		DrawTextEx(current_ui_font, p1_score, (Vector2) { tank_start_x - p1_score_size.x / 2, tank_display_y + 45 }, 16, 1, GREEN);

		if(player1_tank.active) {
			Rectangle mini_tank1 = { tank_start_x - 10, tank_display_y + 70, 20, 20 };
			DrawRectangleRec(mini_tank1, GREEN);
			DrawRectangleLinesEx(mini_tank1, 1, DARKGREEN);
		}

		// Player 2 status
		char* p2_status = player2_tank.active ? "ALIVE" : "DEAD";
		Color p2_status_color = player2_tank.active ? BLUE : RED;
		char* p2_info = TextFormat("PLAYER 2: %s", p2_status);
		char* p2_level = TextFormat("Level: %d", player2_tank.level);
		char* p2_score = TextFormat("Score: %d", game_state.player2_score);

		int tank2_x = tank_start_x + tank_spacing * 2;
		Vector2 p2_info_size = MeasureTextEx(current_ui_font, p2_info, 20, 1);
		Vector2 p2_level_size = MeasureTextEx(current_ui_font, p2_level, 16, 1);
		Vector2 p2_score_size = MeasureTextEx(current_ui_font, p2_score, 16, 1);

		DrawTextEx(current_ui_font, p2_info, (Vector2) { tank2_x - p2_info_size.x / 2, tank_display_y }, 20, 1, p2_status_color);
		DrawTextEx(current_ui_font, p2_level, (Vector2) { tank2_x - p2_level_size.x / 2, tank_display_y + 25 }, 16, 1, BLUE);
		DrawTextEx(current_ui_font, p2_score, (Vector2) { tank2_x - p2_score_size.x / 2, tank_display_y + 45 }, 16, 1, BLUE);

		if(player2_tank.active) {
			Rectangle mini_tank2 = { tank2_x - 10, tank_display_y + 70, 20, 20 };
			DrawRectangleRec(mini_tank2, BLUE);
			DrawRectangleLinesEx(mini_tank2, 1, DARKBLUE);
		}

		char* level_progress = TextFormat("Enemies Defeated: %d", game_state.enemies_killed_this_level);
		Vector2 level_progress_size = MeasureTextEx(current_ui_font, level_progress, ui_size, 1);
		DrawTextEx(current_ui_font, level_progress,
			(Vector2) {
			SCREEN_WIDTH / 2 - level_progress_size.x / 2, tank_display_y + 120
		}, ui_size, 1, YELLOW);
	}
	else if(splash_screen.type == SPLASH_GAME_OVER) {
		// Draw subtitle with pulsing effect
		if(splash_screen.show_continue_prompt) {
			float pulse_restart = 0.7f + 0.3f * sinf(GetTime() * 4.0f);
			DrawTextEx(current_subtitle_font, splash_screen.subtitle_text,
				(Vector2) {
				subtitle_pos.x, subtitle_pos.y + 180
			}, subtitle_size, 1,
				Fade(splash_screen.subtitle_color, pulse_restart));
		}

		char* p1_final = TextFormat("Player 1: %d points", game_state.player1_score);
		char* p2_final = TextFormat("Player 2: %d points", game_state.player2_score);

		Vector2 p1_final_size = MeasureTextEx(current_ui_font, p1_final, 20, 1);
		Vector2 p2_final_size = MeasureTextEx(current_ui_font, p2_final, 20, 1);

		DrawTextEx(current_ui_font, p1_final,
			(Vector2) {
			SCREEN_WIDTH / 2 - p1_final_size.x / 2, subtitle_pos.y + 60
		}, 20, 1, GREEN);
		DrawTextEx(current_ui_font, p2_final,
			(Vector2) {
			SCREEN_WIDTH / 2 - p2_final_size.x / 2, subtitle_pos.y + 85
		}, 20, 1, BLUE);

		char* winner_text = "";
		Color winner_color = WHITE;
		if(game_state.player1_score > game_state.player2_score) {
			winner_text = "PLAYER 1 WINS!";
			winner_color = GREEN;
		}
		else if(game_state.player2_score > game_state.player1_score) {
			winner_text = "PLAYER 2 WINS!";
			winner_color = BLUE;
		}
		else {
			winner_text = "IT'S A TIE!";
			winner_color = YELLOW;
		}

		Vector2 winner_text_size = MeasureTextEx(current_title_font, winner_text, 32, 2);
		DrawTextEx(current_title_font, winner_text,
			(Vector2) {
			SCREEN_WIDTH / 2 - winner_text_size.x / 2, subtitle_pos.y + 120
		}, 32, 2, winner_color);
	}
	else if(splash_screen.type == SPLASH_PAUSE) {
		// Draw subtitle with pulsing effect
		if(splash_screen.show_continue_prompt) {
			float pulse = 0.7f + 0.3f * sinf(GetTime() * 4.0f);
			Color pulsing_subtitle = Fade(splash_screen.subtitle_color, pulse);
			DrawTextEx(current_subtitle_font, splash_screen.subtitle_text, subtitle_pos, subtitle_size, 1, pulsing_subtitle);
		}
		else {
			DrawTextEx(current_subtitle_font, splash_screen.subtitle_text, subtitle_pos, subtitle_size, 1, splash_screen.subtitle_color);
		}

		// Additional pause screen info
		char* pause_info = "Game is paused";
		Vector2 pause_info_size = MeasureTextEx(current_ui_font, pause_info, ui_size, 1);
		DrawTextEx(current_ui_font, pause_info,
			(Vector2) {
			SCREEN_WIDTH / 2 - pause_info_size.x / 2, subtitle_pos.y + 50
		}, ui_size, 1, LIGHTGRAY);
	}
	else {
		// Default handling for any other splash screen types
		if(splash_screen.show_continue_prompt) {
			float pulse = 0.7f + 0.3f * sinf(GetTime() * 4.0f);
			Color pulsing_subtitle = Fade(splash_screen.subtitle_color, pulse);
			DrawTextEx(current_subtitle_font, splash_screen.subtitle_text, subtitle_pos, subtitle_size, 1, pulsing_subtitle);
		}
		else {
			DrawTextEx(current_subtitle_font, splash_screen.subtitle_text, subtitle_pos, subtitle_size, 1, splash_screen.subtitle_color);
		}
	}
}

#endif // GAME_SPLASH_H