int fs_initialized = 0; // Флаг для инициализации только один раз

// Однократная инициализация
void setup_fs_once()
{
	if(!fs_initialized)
	{
		init_offline_fs();
		fs_initialized = 1;
	}
}

// Функция, которая вызывается многократно
void ems_startup_callback(void* arg)
{	
	setup_fs_once(); // Гарантированная однократная инициализация
	printf("ems_setup_fs_once() completed!\n");
}
