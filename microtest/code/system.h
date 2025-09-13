typedef void (*update_draw_frame)(void);
update_draw_frame gudf = NULL;

void system_update_draw_frame(void); // Update and Draw one frame

void system_init(void)
{
#if defined(PLATFORM_DESKTOP)
	setup_fs_once();
#endif

#if defined(PLATFORM_WEB)
	emscripten_async_call(ems_startup_callback, NULL, 0);
#endif
}

void system_run(void)
{
	SetTargetFPS(60);
#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(system_update_draw_frame, 0, 1);
#else
	// Main game loop
	while(!WindowShouldClose())    // Detect window close button or ESC key
		system_update_draw_frame();
#endif
}

bool is_system_initialized(void)
{
	return fs_initialized;
}

void draw_system_not_initialized(void)
{
	printf("system not initialized!\n");

	BeginDrawing();

	ClearBackground(RAYWHITE);

	char* system_text = "SYSTEM NOT INITIALIZED!";
	DrawText(system_text, GetRenderWidth() / 2 - MeasureText(system_text, 20) / 2, GetRenderHeight() / 2 - 20, 20, LIGHTGRAY);

	DrawFPS(0, 0);

	EndDrawing();
}

void system_update_draw_frame(void)
{
	if(!is_system_initialized())
		draw_system_not_initialized();
	else
		gudf();
}
