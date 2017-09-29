
#include "stm32f4xx_hal.h"
#include "car_game_definitions.h"

uint8_t CAR_CHAR_DEFINITIONS[] = {0x00, 0x05, 0x07, 0x0d, 0x0d, 0x07, 0x05, 0x00};
uint8_t HERO_CAR_CHAR_DEFINITIONS[] = {0x00, 0x05, 0x07, 0x0f, 0x0f, 0x07, 0x05, 0x00};

Position HERO_DEFAULT_POSITION = {row:3, col:20};
Position SCORE_DEFAULT_POSITION = {row:1, col:1};
Position LEVEL_DEFAULT_POSITION = {row:4, col:1};

void create_cars_characters() {
	create_new_character(DISPLAY_CUSTOM_CHAR_POSITION, CAR_CHAR_DEFINITIONS);
	create_new_character(DISPLAY_CUSTOM_CHAR_POSITION_2, HERO_CAR_CHAR_DEFINITIONS);
}

void show_you_died_message() {
	clear_display();
	display_cmd_delay(10);
	char *message = "You died stupid!";
	Position pos = {row:2,col:3};
	char *messager = ":(";
    Position posi = {row:3,col:10};

	set_cursor_position(pos);
	default_transmit(message, strlen(message));
	set_cursor_position(posi);
	default_transmit(messager, strlen(messager));
}

void show_error_message(char *error_message, uint8_t length) {
	clear_display();
	Position pos = {row:2,col:3};

	set_cursor_position(pos);
	default_transmit(error_message, length);
}

void print_hero_car(Car hero_car) {
	if (set_cursor_position(hero_car.position)) {
		uint8_t params_car[] = {hero_car.char_addr};
		default_transmit(params_car, 1);
	}
}

void print_score(uint8_t score) {
	char score_char[4];
	sprintf(score_char, "%04d", score);

	if (set_cursor_position(SCORE_DEFAULT_POSITION)) {
		default_transmit(score_char, 4);
	}
}

void print_level(uint8_t level) {
	char level_char[2];
	sprintf(level_char, "%02d", level);

	if (set_cursor_position(LEVEL_DEFAULT_POSITION)) {
		default_transmit(level_char, 2);
	}
}

Position move_car(Car car, uint8_t direction) {
	return move_char(car.position, car.char_addr, direction);
}

void listen_hero_controls(struct Car *hero_car, bool *busy) {
	if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4) == GPIO_PIN_RESET && hero_car->position.row > 1 && !*busy) {
		*busy = true;

		hero_car->position = move_car(*hero_car, DIRECTION_BOTTOM_TOP);
		display_cmd_delay(90);
		*busy = false;
	}

	if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_5) == GPIO_PIN_RESET && hero_car->position.row < 4 && !*busy) {
		*busy = true;

		hero_car->position = move_car(*hero_car, DIRECTION_TOP_BOTTOM);
		display_cmd_delay(90);
		*busy = false;
	}
}

bool level_send_cars(Car cars[], struct Level *level, Car hero_car) {
    bool died = false;
	uint32_t manual_delay_count = 0; // Delay "manual"
	bool move_cars = true;
	bool completed = false;
	bool controls_busy = false;

	while (1) {
		manual_delay_count++;
		listen_hero_controls(&hero_car, &controls_busy);
		move_cars = (manual_delay_count % level->cars_speed) == 0;

		for (uint8_t i = 0; i < level->cars_qtd; i++) {
			uint8_t min_col = level->cols_until_new_car + DEFAULT_CARS_START_COL;

			if ((i == 0 || cars[i-1].position.col > min_col) && move_cars) {
				cars[i].position = move_car(cars[i], DIRECTION_LEFT_RIGHT);

				if (compare_positions(hero_car.position, cars[i].position)) {
					died = true;
					break;
				}

				if (cars[i].position.col == (DISPLAY_COLUMNS + 1)) {
					level->score++;
				}

				if (cars[level->cars_qtd-1].position.col >= (DISPLAY_COLUMNS + 1)) {
					completed = true;
				}
			}
		}

		if (died) {
			return false;
		}

		if (completed) {
			clear_position(hero_car.position);

			break;
		}

		if (move_cars) {
			// Renova hero_car em sua posição para garantir que fluxo dos outros não o faça sumir
			print_hero_car(hero_car);
			// Printa score a cada movimento dos carros
			print_score(level->score);
			// Printa level
			print_level(level->level);
		}
	}

	return true;
}

bool execute_level(struct Level *level) {
    Car cars[level->cars_qtd];
    uint8_t speed = level->cars_speed;
    uint8_t which_row = 1;
    srand((unsigned int) time(NULL));

    for (uint8_t i = 0; i < level->cars_qtd; i++) {
    	uint8_t new_row = (rand() % DISPLAY_ROWS) + 1;
    	which_row = new_row != which_row ? new_row : new_row + 1; // Evitar que proximo carrinho tenha a mesma linha

        Position car_pos = {row:which_row,col:DEFAULT_CARS_START_COL};
    	Car car = {position:car_pos,car_speed:speed,char_addr:DISPLAY_CUSTOM_CHAR_POSITION};
    	cars[i] = car;
    }

    Car hero_car = {position:HERO_DEFAULT_POSITION,car_speed:speed,char_addr:DISPLAY_CUSTOM_CHAR_POSITION_2};
	print_hero_car(hero_car);

	bool completed = level_send_cars(cars, level, hero_car);

    if(!completed){
		show_you_died_message();
    }

    return completed;
}
