
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "gpio.h"
#include "display_functions.h"

void default_transmit(uint16_t params[], int length) {
    HAL_I2C_Master_Transmit(&hi2c1, DISPLAY_DEV_ADDRESS, params, length, 7);
//    display_cmd_delay(8);
}

void activate_display() {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
    display_cmd_delay(5);
}

void clear_display() {
    uint8_t params[] = {DISPLAY_BASE_ADDR, DISPLAY_CLEAR_SCREEN};

    default_transmit(params, sizeof(params));
}

void create_new_character(uint8_t display_position, uint8_t char_defs[8]) {
    uint8_t params[] = {
            DISPLAY_BASE_ADDR, DISPLAY_LOAD_CUSTOM_CHARACTER, display_position,
            char_defs[0], char_defs[1], char_defs[2], char_defs[3], char_defs[4],
            char_defs[5], char_defs[6], char_defs[7]
    };

    default_transmit(params, sizeof(params));
}

bool validate_position(Position position) {
    char *error_message = "";

    if (position.row > DISPLAY_ROWS || position.row < 1) {
        error_message = "Row value is invalid. \n";
    } else {
        if (position.col > DISPLAY_COLUMNS || position.col < 1) {
            error_message = "Column value is invalid. \n";
        }
    }

    if (strlen(error_message)) {
        printf(error_message);
        return false;
    }

    return true;
}

bool set_cursor_position(Position position) {
    uint8_t rows_map[DISPLAY_ROWS] = {
            DISPLAY_POSITION_R1_C1, DISPLAY_POSITION_R2_C1, DISPLAY_POSITION_R3_C1, DISPLAY_POSITION_R4_C1
    };

    if (validate_position(position)) {
        uint8_t cursor = (uint8_t) (rows_map[position.row - 1] + (position.col - 1));
        uint8_t params[] = {DISPLAY_BASE_ADDR, DISPLAY_SET_CURSOR, cursor};

        default_transmit(params, sizeof(params));
        return true;
    }

    return false;
}

void put_char_at(uint8_t char_addr, Position position) {
    if (set_cursor_position(position)) {
        uint8_t params[] = {char_addr};

        default_transmit(params, sizeof(params));
    }
}

bool compare_positions(Position a, Position b) {
	if (a.row == b.row && a.col == b.col) {
		return true;
	}

	return false;
}

void put_string_at(char char_addr[], Position position) {
    if (set_cursor_position(position)) {
        default_transmit(char_addr, strlen(char_addr));
    }
}

Position move_char(Position position, uint8_t char_addr, uint8_t direction) {
    signed int row_var = 0x30;
    signed int col_var = 0x03;
    uint8_t params_blank[] = {DISPLAY_BLANK_CHAR_POSITION};
    uint8_t params_char[] = {char_addr};
    char error_message[120];

    switch (direction) {
        case DIRECTION_LEFT_RIGHT:// 0x03
        case DIRECTION_TOP_BOTTOM:// 0x30
        case DIRECTION_RIGHT_LEFT:// 0x01
        case DIRECTION_BOTTOM_TOP:// 0x10

            row_var &= direction;
            col_var &= direction;

            row_var >>= 4;
            row_var -= (row_var ? 0x02 : 0x00);
            col_var -= (col_var ? 0x02 : 0x00);

            if (row_var > 1 || row_var < -1 || col_var > 1 || col_var < -1 || (!col_var && !row_var)) {
            	sprintf(error_message, "Erro na definição de col_var e row_var: %d e %d", row_var, col_var);
				show_error_message(error_message, strlen(error_message));
                return position;
            }

			if (set_cursor_position(position)) {
				default_transmit(params_blank, 1);
			}

			position.row += row_var;
			position.col += col_var;

			if (set_cursor_position(position)) {
				default_transmit(params_char, 1);
			}

            break;
        default:
            *error_message = "Erro: Direção de movimento inválida";
			show_error_message(error_message, strlen(error_message));
    }

    return position;
}

void clear_position(Position position) {
	uint8_t params_blank[] = {DISPLAY_BLANK_CHAR_POSITION};

	if (set_cursor_position(position)) {
		default_transmit(params_blank, 1);
	}
}

void clear_area(Position position) {
	uint8_t params_blank[] = {DISPLAY_BLANK_CHAR_POSITION};

	if (set_cursor_position(position)) {
		default_transmit(params_blank, 1);
	}
}

void display_cmd_delay(uint16_t delay) {
//    Sleep(delay);
	HAL_Delay(delay);
}
