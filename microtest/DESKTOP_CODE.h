int fs_initialized = 0; // Флаг для инициализации только один раз

// Однократная инициализация
void setup_fs_once()
{
	if(!fs_initialized)
	{
		// Проверяем существование директории persistent и создаем ее при необходимости
#if defined(PLATFORM_DESKTOP)
		struct stat st = { 0 };

		// Проверяем, существует ли директория
		if(stat("persistent", &st) == -1)
		{
			// Директория не существует, создаем ее
#if defined(_WIN32)
	// Для Windows (используя _mkdir вместо CreateDirectory)
			if(_mkdir("persistent") == 0)
			{
				printf("The persistent directory was created successfully\n");
			}
			else
			{
				printf("Error when creating persistent directory: %s\n", strerror(errno));
			}
#else
	// Для Linux и других POSIX-систем
			if(mkdir("persistent", 0777) == 0)
			{
				printf("The persistent directory was created successfully\n");
			}
			else
			{
				printf("Error when creating persistent directory: %s\n", strerror(errno));
			}
#endif
		}
		else
		{
			printf("The persistent directory already exists\n");
		}
#endif
		fs_initialized = 1;
	}

	printf("setup_fs_once() completed!\n");
}

// Open file dialog for loading a file
char* open_file_dialog_load_file(void) {
	static char filename[512] = { 0 };

	const char* filters[] = { "*.*", "*.txt", "*.c", "*.h" };
	const char* selectedFile = tinyfd_openFileDialog(
		"Open File",    // Title
		"",             // Default path
		4,              // Number of filter patterns
		filters,        // Filter patterns
		"All Files",    // Filter description
		0               // Allow multiple selections (0 = no)
	);

	if(selectedFile) {
		strncpy(filename, selectedFile, 511);
		filename[511] = '\0';
		return filename;
	}

	return NULL;
}





