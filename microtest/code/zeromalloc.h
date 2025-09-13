#define malloc(size) calloc(1, (size))

// Helper function to allocate and copy a string
char* create_string(const char* src)
{
	if(!src) return NULL;
	size_t len = strlen(src);
	char* result = (char*)malloc(len + 1);
	if(result)
		memcpy(result, src, len + 1);
	return result;
}
