
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../Inc/display_definitions.h"

int main() {

	printf("Ok");

	Position position = {row:3,col:20};
	uint8_t direction = DIRECTION_BOTTOM_TOP;
	char error_message[200];

	signed int row_var = 0x30;
	signed int col_var = 0x03;

	sprintf(error_message, "Row inicial: %d \n", row_var);
	printf(error_message);

	row_var &= direction;
	col_var &= direction;

	sprintf(error_message, "Row depois do bit a bit: %d \n", row_var);
	printf(error_message);

	row_var >>= 4;

	sprintf(error_message, "Row depois do shift: %d \n", row_var);
	printf(error_message);

	row_var -= (row_var ? 0x02 : 0x00);
	col_var -= (col_var ? 0x02 : 0x00);

	sprintf(error_message, "Row depois da subtração: %d \n", row_var);
	printf(error_message);

	if (row_var > 1 || row_var < -1 || col_var > 1 || col_var < -1 || (!col_var && !row_var)) {
		sprintf(error_message, "Erro na definição de col_var e row_var: %d e %d", row_var, col_var);
		printf(error_message);
	}

	position.row += row_var;
	position.col += col_var;

	return 0;
}
