#define DRAW_FLAG_FLIP_X 1
#define DRAW_FLAG_FLIP_Y 2

void draw_texture2d(Texture2D* texture, int x, int y, uint32_t flags, Color tint)
{
	Rectangle source = { 0, 0, texture->width, texture->height };

	if(flags & DRAW_FLAG_FLIP_X)
		source.width = -texture->width;

	if(flags & DRAW_FLAG_FLIP_Y)
		source.height = -texture->height;

	Vector2 position = { x, y };

	DrawTextureRec(*texture, source, position, tint);
}