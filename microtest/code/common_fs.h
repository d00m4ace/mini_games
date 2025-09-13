int fs_iter = -1; // Инициализируем переменную итерации

void initialize_iter_from_file()
{
	FILE* file = fopen("persistent/any_file.txt", "r");

	if(file != NULL)
	{
		char line[256];
		char last_line[256] = { 0 };

		while(fgets(line, sizeof(line), file))
			strcpy(last_line, line);

		fclose(file);

		if(strlen(last_line) > 0)
		{
			fs_iter = atoi(last_line);
			printf("Initialized fs_iter from a file: %d\n", fs_iter);
		}
		else
		{
			printf("The file is empty, iter = 0\n");
			fs_iter = 0;
		}
	}
	else
	{
		printf("The file doesn't exist, iter = 0\n");
		fs_iter = 0;
	}
}

void append_iter_to_file(int iter_value)
{
	FILE* file = fopen("persistent/any_file.txt", "a");
	assert(file != NULL);
	fprintf(file, "%d\n", iter_value);
	fclose(file);
}

void print_file_content()
{
	FILE* file = fopen("persistent/any_file.txt", "r");
	if(file != NULL)
	{
		char line[256];
		printf("File contents:\n");

		while(fgets(line, sizeof(line), file))
			printf("%s", line);

		fclose(file);
	}
	else
	{
		printf("File not found or empty.\n");
	}
}

void hello_test_write()
{
	// Открываем файл для записи (режим "w" - write)
	FILE* off_file = fopen("resources/offline_file.txt", "w");
	if(off_file == NULL)
	{
		perror("Error opening file");
		return;
	}
	// Записываем строку в файл
	fprintf(off_file, "Hello, offline file!\n");
	// Закрываем файл
	if(fclose(off_file) != 0)
	{
		perror("Error closing file");
		return;
	}
}

void hello_test_read()
{
	// Открываем файл для чтения (режим "r" - read)
	FILE* off_file = fopen("resources/offline_file.txt", "r");
	if(off_file == NULL)
	{
		perror("Error opening file");
		return;
	}
	// Читаем и выводим содержимое файла построчно
	char buffer[256];
	while(fgets(buffer, sizeof(buffer), off_file) != NULL)
	{
		printf("%s", buffer);
	}
	// Закрываем файл
	if(fclose(off_file) != 0)
	{
		perror("Error closing file");
		return;
	}
}

int is_file_loaded = 0;
unsigned char* load_file_data = NULL;
int load_file_data_size = 0;

void LoadFileCallback(unsigned char* data, int size) {
	printf("File loaded size: %d\n", size);

	if(size <= 0) {
		printf("Error loading file\n");
		is_file_loaded = -1;
		return;
	}

	if(load_file_data != NULL) {
		free(load_file_data);
		load_file_data = NULL;
	}

	load_file_data = (unsigned char*)malloc(size + 1);
	memcpy(load_file_data, data, size);
	load_file_data[size] = '\0'; 	 	
	is_file_loaded = 1;
}

void open_file_dialog_load()
{
	if(load_file_data != NULL) {
		free(load_file_data);
		load_file_data = NULL;
	}

	is_file_loaded = 0;

#if defined(PLATFORM_WEB)
	open_file_dialog();
#endif

#if defined(PLATFORM_DESKTOP)
	char* loadedFileName = NULL;
	loadedFileName = open_file_dialog_load_file();

	if(loadedFileName != NULL)
	{
		printf("Loaded file name: %s\n", loadedFileName);
		FILE* loadedFile = fopen(loadedFileName, "rb");
		if(loadedFile != NULL)
		{
			fseek(loadedFile, 0, SEEK_END);
			long fileSize = ftell(loadedFile);
			fseek(loadedFile, 0, SEEK_SET);
			load_file_data = (unsigned char*)malloc(fileSize + 1);
			if(load_file_data != NULL)
			{
				fread(load_file_data, 1, fileSize, loadedFile);
				load_file_data[fileSize] = '\0'; // Null-terminate the string
				load_file_data_size = fileSize;
				is_file_loaded = 1;
				printf("File loaded successfully!\n");
			}
			fclose(loadedFile);
		}
		else
		{
			printf("Error opening loaded file: %s\n", strerror(errno));
			is_file_loaded = -1;
		}
	}
	else
	{
		printf("No file selected.\n");
		is_file_loaded = -1;
	}
#endif
}

// Open file dialog for saving a file
void open_file_dialog_save(const char* source_file_path, const char* destination_file_name)
{
	if(load_file_data != NULL) {
		free(load_file_data);
		load_file_data = NULL;
	}

	is_file_loaded = 0;

	// Save the file to the local file system
#if defined(PLATFORM_WEB)
	save_file_from_memfs_to_disk(source_file_path, destination_file_name);
#endif

#if defined(PLATFORM_DESKTOP)

	FILE* loadedFile = fopen(source_file_path, "rb");
	if(loadedFile != NULL)
	{
		fseek(loadedFile, 0, SEEK_END);
		long fileSize = ftell(loadedFile);
		fseek(loadedFile, 0, SEEK_SET);
		load_file_data = (unsigned char*)malloc(fileSize + 1);
		if(load_file_data != NULL)
		{
			fread(load_file_data, 1, fileSize, loadedFile);
			load_file_data[fileSize] = '\0'; // Null-terminate the string
			load_file_data_size = fileSize;
			is_file_loaded = 1;
			printf("File loaded successfully!\n");
		}
		fclose(loadedFile);
	}
	else
	{
		printf("Error opening loaded file: %s\n", strerror(errno));
		is_file_loaded = -1;
	}

	if(is_file_loaded == -1)
	{
		printf("No file selected.\n");
		return;
	}

	char selected_destination_file_name[512] = { 0 }; // Ensure null-termination
	char selected_destination_file_name_dst[512] = "downloads/"; // Ensure null-termination

	strcat(selected_destination_file_name_dst, destination_file_name);

	const char* selectedFile = tinyfd_saveFileDialog("Save File", selected_destination_file_name_dst, 0, NULL, "All Files");

	if(selectedFile)
	{
		strncpy(selected_destination_file_name, selectedFile, 511);
		selected_destination_file_name[511] = 0;
	}

	// Save the file to the local file system
	FILE* savedFile = fopen(selected_destination_file_name, "wb");
	if(savedFile != NULL)
	{
		fwrite(load_file_data, 1, load_file_data_size, savedFile);
		fclose(savedFile);
		printf("File saved successfully!\n");
	}
	else
	{
		printf("Error saving file: %s\n", strerror(errno));
	}
	// Free the allocated memory
	if(load_file_data != NULL)
	{
		free(load_file_data);
		load_file_data = NULL;
	}
	load_file_data_size = 0;
	is_file_loaded = 0;
#endif
}

int is_clipboard_pasted = 0;
char* clipboard_paste = NULL;

void request_clipboard_paste_text()
{
	is_clipboard_pasted = 0; // Сбрасываем флаг перед вставкой текста

	if(clipboard_paste != NULL)
	{
		free(clipboard_paste);
		clipboard_paste = NULL;
	}

#if defined(PLATFORM_WEB)
	request_clipboard_paste();
#endif

#if defined(PLATFORM_DESKTOP)

	// Вставляем текст из буфера обмена
	// Используем функцию raylib для получения текста из буфера обмена
	// Вставляем текст из буфера обмена
	const char* clipboard_paste_text = GetClipboardText();  // Using raylib's built-in function
	if(clipboard_paste_text != NULL)
	{
		clipboard_paste = (char*)malloc(strlen(clipboard_paste_text) + 1);
		strcpy(clipboard_paste, clipboard_paste_text);
		// Выводим текст в консоль
		printf("Clipboard Paste text: %s\n", clipboard_paste);
		is_clipboard_pasted = 1;
	}
	else
	{
		printf("Clipboard is empty or not available.\n");
	}
#endif
}

void OnClipboardPaste(const char* text)
{
	clipboard_paste = (char*)malloc(strlen(text) + 1);
	strcpy(clipboard_paste, text);
	// Выводим текст в консоль
	printf("Clipboard Paste text: %s\n", clipboard_paste);
	is_clipboard_pasted = 1;
}

void request_clipboard_copy_text(const char* text)
{
#if defined(PLATFORM_WEB)
	copy_to_clipboard(text);
#endif
#if defined(PLATFORM_DESKTOP)
	// Копируем текст в буфер обмена
	SetClipboardText(text);  // Using raylib's built-in function
	printf("Text copied to clipboard: %s\n", text);
#endif
}