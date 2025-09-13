#ifndef GAME_MAP_H
#define GAME_MAP_H

// Function declarations
Vector2 map_grid_to_world(int map_x, int map_y);
Vector2 world_to_map_grid(Vector2 world_pos);
ObstacleType get_obstacle_type_from_char(char c);
int get_conveyor_direction_from_char(char c);
void load_map_from_ascii(void);
Vector2 find_safe_spawn_position(void);
bool is_spawn_position_safe(int map_x, int map_y);

void load_level(int level_index)
{
	int lvl_indx = (level_index - 1) % NUM_LEVELS;

	for(int row = 0; row < MAP_HEIGHT; row++) {
		strcpy(game_map[row], levels[lvl_indx][row]);
	}
}

// Implementation
Vector2 map_grid_to_world(int map_x, int map_y)
{
	return (Vector2) {
		MAP_OFFSET_X + map_x * BLOCK_SIZE,
			MAP_OFFSET_Y + map_y * BLOCK_SIZE
	};
}

Vector2 world_to_map_grid(Vector2 world_pos)
{
	return (Vector2) {
		(world_pos.x - MAP_OFFSET_X) / BLOCK_SIZE,
			(world_pos.y - MAP_OFFSET_Y) / BLOCK_SIZE
	};
}

ObstacleType get_obstacle_type_from_char(char c)
{
	extern MapLegend map_legend[];
	int legend_size = sizeof(map_legend) / sizeof(map_legend[0]);
	for(int i = 0; i < legend_size; i++) {
		if(map_legend[i].ascii_char == c) {
			return map_legend[i].type;
		}
	}
	return OBSTACLE_EMPTY;
}

int get_conveyor_direction_from_char(char c)
{
	switch(c) {
	case '^': return 0; // Up
	case '>': return 1; // Right
	case 'v': return 2; // Down
	case '<': return 3; // Left
	default: return 0;
	}
}

void load_map_from_ascii(void)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];
	extern char game_map[MAP_HEIGHT][MAP_WIDTH + 1];
	extern GameState game_state;

	// Clear existing obstacles
	for(int x = 0; x < MAP_WIDTH; x++) {
		for(int y = 0; y < MAP_HEIGHT; y++) {
			map_obstacles[x][y].active = false;
			map_obstacles[x][y].type = OBSTACLE_EMPTY;
		}
	}

	// Load from ASCII map
	for(int y = 0; y < MAP_HEIGHT; y++) {
		for(int x = 0; x < MAP_WIDTH; x++) {
			char map_char = game_map[y][x];
			ObstacleType type = get_obstacle_type_from_char(map_char);

			if(type != OBSTACLE_EMPTY) {
				Vector2 world_pos = map_grid_to_world(x, y);

				map_obstacles[x][y].position = world_pos;
				map_obstacles[x][y].type = type;
				map_obstacles[x][y].active = true;
				map_obstacles[x][y].triggered = false;
				map_obstacles[x][y].timer = 0;
				map_obstacles[x][y].effect_timer = 0;
				map_obstacles[x][y].state = false;
				map_obstacles[x][y].linked_pos = (Vector2){ 0, 0 };
				map_obstacles[x][y].direction = 0;

				// Set type-specific properties
				switch(type) {
				case OBSTACLE_EMPTY:
					map_obstacles[x][y].color = (Color){ 0, 0, 0, 0 };
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].active = false;
					map_obstacles[x][y].is_block_tanks = false;
					map_obstacles[x][y].is_block_bullets = false;
					break;

				case OBSTACLE_STEEL_WALL:
					map_obstacles[x][y].color = (Color){ 169, 169, 169, 255 };
					map_obstacles[x][y].health = 5 + (game_state.current_level * 1.5f);
					map_obstacles[x][y].max_health = map_obstacles[x][y].health;
					map_obstacles[x][y].linked_pos = world_pos;
					map_obstacles[x][y].is_block_tanks = true;   // Blocks tanks
					map_obstacles[x][y].is_block_bullets = true; // Blocks bullets (destructible)
					break;

				case OBSTACLE_WATER:
					map_obstacles[x][y].color = BLUE;
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].is_block_tanks = true;   // Blocks tanks (unless bridge)
					map_obstacles[x][y].is_block_bullets = false; // Bullets pass through
					break;

				case OBSTACLE_BRIDGE:
					map_obstacles[x][y].color = BROWN;
					map_obstacles[x][y].health = 3;
					map_obstacles[x][y].max_health = 3;
					map_obstacles[x][y].is_block_tanks = false;  // Allows tanks to pass
					map_obstacles[x][y].is_block_bullets = false; // Allows bullets to pass
					break;

				case OBSTACLE_FOREST:
					map_obstacles[x][y].color = DARKGREEN;
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].is_block_tanks = false;  // Tanks can move through
					map_obstacles[x][y].is_block_bullets = false; // Bullets pass through
					break;

				case OBSTACLE_ICE_BLOCK:
					map_obstacles[x][y].color = (Color){ 173, 216, 230, 200 };
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].is_block_tanks = false;  // Tanks slide on ice
					map_obstacles[x][y].is_block_bullets = false; // Bullets pass through
					break;

				case OBSTACLE_QUICKSAND:
					map_obstacles[x][y].color = (Color){ 194, 154, 108, 255 };
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].is_block_tanks = false;  // Tanks move slowly
					map_obstacles[x][y].is_block_bullets = false; // Bullets pass through
					break;

				case OBSTACLE_CONVEYOR_BELT:
					map_obstacles[x][y].color = (Color){ 80, 80, 80, 255 };
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].direction = get_conveyor_direction_from_char(map_char);
					map_obstacles[x][y].is_block_tanks = false;  // Tanks affected by conveyor
					map_obstacles[x][y].is_block_bullets = false; // Bullets pass through
					break;

				case OBSTACLE_MINE_FIELD:
					map_obstacles[x][y].color = (Color){ 101, 67, 33, 255 };
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].effect_radius = 48;
					map_obstacles[x][y].is_block_tanks = false;  // Tanks trigger mines
					map_obstacles[x][y].is_block_bullets = false; // Bullets pass through
					break;

				case OBSTACLE_HEALING_STATION:
					map_obstacles[x][y].color = WHITE;
					map_obstacles[x][y].health = 3;
					map_obstacles[x][y].max_health = 3;
					map_obstacles[x][y].uses_remaining = 1;
					map_obstacles[x][y].effect_radius = BLOCK_SIZE;
					map_obstacles[x][y].is_block_tanks = false;  // Tanks can use station
					map_obstacles[x][y].is_block_bullets = false; // Bullets pass through
					break;

				case OBSTACLE_AMMO_DEPOT:
					map_obstacles[x][y].color = (Color){ 139, 69, 19, 255 };
					map_obstacles[x][y].health = 2;
					map_obstacles[x][y].max_health = 2;
					map_obstacles[x][y].uses_remaining = 1;
					map_obstacles[x][y].effect_radius = BLOCK_SIZE;
					map_obstacles[x][y].is_block_tanks = false;  // Tanks can use depot
					map_obstacles[x][y].is_block_bullets = false; // Bullets pass through
					break;

				case OBSTACLE_SHIELD_GENERATOR:
					map_obstacles[x][y].color = (Color){ 100, 100, 150, 255 };
					map_obstacles[x][y].health = 4;
					map_obstacles[x][y].max_health = 4;
					map_obstacles[x][y].uses_remaining = 1;
					map_obstacles[x][y].effect_radius = BLOCK_SIZE;
					map_obstacles[x][y].is_block_tanks = false;  // Tanks can use generator
					map_obstacles[x][y].is_block_bullets = false; // Bullets pass through
					break;

				case OBSTACLE_GATE:
					map_obstacles[x][y].color = (Color){ 139, 69, 19, 255 };
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].state = false;
					map_obstacles[x][y].timer = 2.0f + (rand() % 4);
					// Gates block when closed, allow when open
					map_obstacles[x][y].is_block_tanks = !map_obstacles[x][y].state;
					map_obstacles[x][y].is_block_bullets = !map_obstacles[x][y].state;
					break;

				case OBSTACLE_REFLECTOR:
					map_obstacles[x][y].color = (Color){ 200, 200, 255, 255 };
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].is_block_tanks = false;  // Tanks can move through
					map_obstacles[x][y].is_block_bullets = true; // Reflects bullets (special case)
					break;

				case OBSTACLE_TELEPORTER:
					map_obstacles[x][y].color = PURPLE;
					map_obstacles[x][y].health = 999;
					map_obstacles[x][y].max_health = 999;
					map_obstacles[x][y].effect_radius = BLOCK_SIZE;
					map_obstacles[x][y].is_block_tanks = false;  // Tanks teleport through
					map_obstacles[x][y].is_block_bullets = true; // Bullets teleport (special case)
					break;
				}
			}
		}
	}
}

bool is_spawn_position_safe(int map_x, int map_y)
{
	extern Obstacle map_obstacles[MAP_WIDTH][MAP_HEIGHT];

	if(map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
		return false;
	}

	if(map_obstacles[map_x][map_y].active) {
		switch(map_obstacles[map_x][map_y].type) {
		case OBSTACLE_EMPTY:
		case OBSTACLE_FOREST:
		case OBSTACLE_ICE_BLOCK:
		case OBSTACLE_QUICKSAND:
		case OBSTACLE_CONVEYOR_BELT:
		case OBSTACLE_HEALING_STATION:
		case OBSTACLE_AMMO_DEPOT:
		case OBSTACLE_SHIELD_GENERATOR:
			break;
		case OBSTACLE_GATE:
			if(!map_obstacles[map_x][map_y].state) {
				return false;
			}
			break;
		default:
			return false;
		}
	}

	int passable_neighbors = 0;
	int directions[8][2] = {
		{-1, -1}, {0, -1}, {1, -1},
		{-1,  0},          {1,  0},
		{-1,  1}, {0,  1}, {1,  1}
	};

	for(int i = 0; i < 8; i++) {
		int check_x = map_x + directions[i][0];
		int check_y = map_y + directions[i][1];

		if(check_x >= 0 && check_x < MAP_WIDTH && check_y >= 0 && check_y < MAP_HEIGHT) {
			bool is_passable = true;

			if(map_obstacles[check_x][check_y].active) {
				switch(map_obstacles[check_x][check_y].type) {
				case OBSTACLE_EMPTY:
				case OBSTACLE_FOREST:
				case OBSTACLE_ICE_BLOCK:
				case OBSTACLE_QUICKSAND:
				case OBSTACLE_CONVEYOR_BELT:
				case OBSTACLE_HEALING_STATION:
				case OBSTACLE_AMMO_DEPOT:
				case OBSTACLE_SHIELD_GENERATOR:
				case OBSTACLE_BRIDGE:
					break;
				case OBSTACLE_GATE:
					if(!map_obstacles[check_x][check_y].state) {
						is_passable = false;
					}
					break;
				default:
					is_passable = false;
					break;
				}
			}

			if(is_passable) {
				passable_neighbors++;
			}
		}
	}

	return passable_neighbors >= 3;
}

Vector2 find_safe_spawn_position(void)
{
	for(int attempts = 0; attempts < 200; attempts++) {
		int x = 1 + (rand() % (MAP_WIDTH - 2));
		int y = 1 + (rand() % (MAP_HEIGHT - 2));

		if(is_spawn_position_safe(x, y)) {
			Vector2 world_pos = map_grid_to_world(x, y);
			return (Vector2) {
				world_pos.x + (BLOCK_SIZE - TANK_SIZE) / 2,
					world_pos.y + (BLOCK_SIZE - TANK_SIZE) / 2
			};
		}
	}

	Vector2 center = map_grid_to_world(MAP_WIDTH / 2, MAP_HEIGHT / 2);
	return (Vector2) {
		center.x + (BLOCK_SIZE - TANK_SIZE) / 2,
			center.y + (BLOCK_SIZE - TANK_SIZE) / 2
	};
}

#endif // GAME_MAP_H