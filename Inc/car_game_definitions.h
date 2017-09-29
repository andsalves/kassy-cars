
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "display_functions.h"

#define DEFAULT_CARS_START_COL 4

typedef struct Car {
	Position position;
	uint16_t car_speed;
	uint8_t char_addr;
} Car;

typedef struct Level {
    uint8_t level;
    uint16_t cars_qtd;
    uint16_t cars_speed;
    uint16_t cols_until_new_car;
    uint64_t score;
} Level;

void create_cars_characters();

void show_you_died_message();

void show_error_message(char *error_message, uint8_t length);

void print_hero_car(Car hero_car);

void print_score(uint8_t score);

Position move_car(Car car, uint8_t direction);

void listen_hero_controls(Car *hero_car, bool *busy);

bool level_send_cars(Car cars[], struct Level *level, Car hero_car);

bool execute_level(struct Level *level);
