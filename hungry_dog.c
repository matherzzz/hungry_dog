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
	for (size_t i = 0; i < map->size; i++) {
		for (size_t j = 0; j < map->size * 2; j++)
			printw("%c", (map->field)[i][j]);
		printw("\n");
	}
}

void create_field(struct playing_field* map) {
	map->field = malloc(sizeof(char*) * map->size);
        for (size_t i = 0; i < map->size; i++)
                (map->field)[i] = malloc(sizeof(char) * map->size * 2);
	fill_str((map->field)[0], '#', map->size * 2);
        for (size_t i = 1; i < map->size - 1; i++)
                fill_str((map->field)[i], ' ', map->size * 2);
        fill_str((map->field)[map->size - 1], '#', map->size * 2);
}

void free_field(struct playing_field* map) {
	for (size_t i = 0; i < map->size; i++)
		free((map->field)[i]);
	free(map->field);
}

void generate_new_coords(struct enemy* enemy, const struct dog* const dog, size_t size) {
	do {
		enemy->x = 1 + rand()%(2 * size - 2);
		enemy->y = 1 + rand()%(size - 2);
	} while (dog->x == enemy->x || dog->y == enemy->y);
}

bool is_eat(struct enemy* enemy, struct dog* dog) {
	return enemy->x == dog->x && enemy->y == dog->y;
}

bool is_crash(struct dog* dog, size_t size) {
	return dog->x == 0 || dog->x == 2*size - 1 || dog->y == 0 || dog->y == size - 1;
}

int main() {
	char key;
	int64_t score = 0;
	int64_t pause = 50;
	struct playing_field map = {0};
	printf("Enter size of playing field: ");
	scanf("%zu", &(map.size));
	create_field(&map);
	struct dog dog = {'@', 5, 5, 1, 0};
	struct enemy enemy = {'o', 0, 0, 15};
	generate_new_coords(&enemy, &dog, map.size);
	initscr();
	do {
		clear();
		if (is_crash(&dog, map.size)) break;
		if (is_eat(&enemy, &dog)) {
			score += enemy.cost;
			generate_new_coords(&enemy, &dog, map.size);
		}
		map.field[dog.y][dog.x] = dog.skin;
		map.field[enemy.y][enemy.x] = enemy.skin;
		printw("score: %"PRId64"\n", score);
		print_field(&map);
		timeout(0);
		key = getch();
		noecho();
		map.field[dog.y][dog.x] = ' ';
		if (key == 'w') { dog.speedY = -1; dog.speedX = 0; pause = 100; }
		if (key == 's') { dog.speedY = 1; dog.speedX = 0; pause = 100; }
		if (key == 'a') { dog.speedX = -1; dog.speedY = 0; pause = 50; }
		if (key == 'd') { dog.speedX = 1; dog.speedY = 0; pause = 50; }
		dog.x += dog.speedX;
		dog.y += dog.speedY;
		napms(pause);
	} while (key != 'e');
	endwin();
	free_field(&map);
	return 0;
}


