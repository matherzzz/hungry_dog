#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <inttypes.h>

struct playing_field {
	char** field;
	size_t size;
};

struct dog {
	char skin;
	int32_t x;
	int32_t y;
	int32_t speedX;
	int32_t speedY;
};

struct list {
	char skin;
	int32_t x;
	int32_t y;
	struct list* next;
};

struct list* node_create( char skin, int32_t x, int32_t y ) {
	struct list* node_ptr = malloc(sizeof(struct list));
	if (node_ptr) {
		node_ptr->skin = skin;
	    	node_ptr->x = x;
	    	node_ptr->y = y;
        	node_ptr->next = NULL;
	}
	return node_ptr;
}

void list_destroy( struct list* list ) {
	while (list) {
		struct list* del = list;
		list = list -> next;
		free(del);
	}
}

struct list* list_last( struct list * list ) {
	while (list && list -> next) list = list -> next;
	return list;
}

void list_add_back( struct list** old, char skin, int32_t x, int32_t y ) {
    if (!old) return;
    if (!(*old)) *old = node_create(skin, x, y);
    else list_last(*old)->next = node_create(skin, x, y);
}

void update_snake(struct list* list, int32_t x, int32_t y) {
	if (list->next) update_snake(list->next, list->x, list->y);
	list->x = x;
	list->y = y;
}

struct enemy {
	char skin;
	int32_t x;
	int32_t y;
	int64_t cost;
};

void fill_str(char* str, char s, size_t sz) {
	*str = '#';
	*(str + sz - 1) = '#';
	for (size_t i = 1; i < sz - 1; i++)
		*(str + i) = s;
}

void print_field(struct playing_field* map) {
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(2));
	int8_t color;
	for (size_t i = 0; i < map->size; i++) {
		for (size_t j = 0; j < map->size * 2; j++) {
			if ((map->field)[i][j] == '#') color = 2;
			else if((map->field)[i][j] == '*') color = 1;
			else color = 3;
			attron(COLOR_PAIR(color));
			printw("%c", (map->field)[i][j]);
			attroff(COLOR_PAIR(color));
		}
		printw("\n");
	}
}

void clear_map(struct playing_field* map) {
        for (size_t i = 1; i < map->size - 1; i++)
                fill_str((map->field)[i], ' ', map->size * 2);
}

void create_field(struct playing_field* map) {
	map->field = malloc(sizeof(char*) * map->size);
        for (size_t i = 0; i < map->size; i++)
                (map->field)[i] = malloc(sizeof(char) * map->size * 2);
	fill_str((map->field)[0], '#', map->size * 2);
	clear_map(map);
	fill_str((map->field)[map->size - 1], '#', map->size * 2);
}

void free_field(struct playing_field* map) {
	for (size_t i = 0; i < map->size; i++)
		free((map->field)[i]);
	free(map->field);
}

void generate_new_coords(struct enemy* enemy, const struct playing_field* const map) {
	do {
		enemy->x = 1 + rand()%(2 * map->size - 2);
		enemy->y = 1 + rand()%(map->size - 2);
	} while ((map->field)[enemy->y][enemy->x] != ' ');
}

bool is_eat(struct enemy* enemy, struct list* list) {
	return enemy->x == list->x && enemy->y == list->y;
}

bool is_crash(struct list* list, struct playing_field* map) {
	return (map->field)[list->y][list->x] != ' ';
}

void paint_snake(struct playing_field* map, struct list* list) {
	while(list) {
		(map->field)[list->y][list->x] = list->skin;
		list = list->next;
	}
}

int main() {
	char key;
	int64_t score = 0;
	int64_t pause = 50;
	int32_t speedX = 1;
	int32_t speedY = 0;
	struct playing_field map = {0};
	printf("Enter size of playing field: ");
	scanf("%zu", &(map.size));
	create_field(&map);
	struct list* snake = node_create('>', 5, 5);
	struct enemy enemy = {'*', 0, 0, 15};
	generate_new_coords(&enemy, &map);
	initscr();
	do {
		clear();
		if (is_eat(&enemy, snake)) {
			score += enemy.cost;
			int32_t oldX = enemy.x;
			int32_t oldY = enemy.y;
			generate_new_coords(&enemy, &map);
			map.field[oldY][oldX] = ' ';
			list_add_back(&snake, 'o', snake->x, snake->y );
		}
		if (is_crash(snake, &map)) break;
		clear_map(&map);
		paint_snake(&map, snake);
		map.field[enemy.y][enemy.x] = enemy.skin;
		printw("score: %"PRId64"\n", score);
		print_field(&map);
		printw("Press 'e' to stop the game ");
		timeout(0);
		key = getch();
		noecho();
		if (key == 'w' && speedY == 0) { snake->skin = '^'; speedY = -1; speedX = 0; pause = 100; }
		if (key == 's' && speedY == 0) { snake->skin = 'v'; speedY = 1; speedX = 0; pause = 100; }
		if (key == 'a' && speedX == 0) { snake->skin = '<'; speedX = -1; speedY = 0; pause = 50; }
		if (key == 'd' && speedX == 0) { snake->skin = '>'; speedX = 1; speedY = 0; pause = 50; }
		update_snake(snake, snake->x + speedX, snake->y + speedY);
		napms(pause);
	} while (key != 'e');
	timeout(-1);
	clear();
	printw("score: %"PRId64"\n", score);
	print_field(&map);
	if (key == 'e') printw("YOU HAVE FINISHED THE GAME\n");
	else printw("GAME OVER\n");
	printw("Press 'e' to exit the terminal");
	do {
		key = getch();
	} while (key != 'e');
	endwin();
	list_destroy(snake);
	free_field(&map);
	return 0;
}


