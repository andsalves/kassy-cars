#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "display_definitions.h"

void default_transmit(uint16_t params[], int length);

void activate_display();

void clear_display();

void create_new_character(uint8_t display_position, uint8_t character_defs[8]);

bool validate_position(Position position);

bool set_cursor_position(Position position);

void put_char_at(uint8_t char_addr, Position position);

bool compare_positions(Position a, Position b);

void put_string_at(char char_addr[], Position position);

Position move_char(Position position, uint8_t char_addr, uint8_t direction);

void display_cmd_delay(uint16_t delay);
