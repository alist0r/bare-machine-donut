#include "vga.h"

static const uint32_t VGA_WIDTH = 320; 
static const uint32_t VGA_HIGHT = 200;
uint8_t *term_buf  = (uint8_t*)(0xA0000); //graphics buffer loacation

void term_init() {
	//just in case i mean i dont need this line or func
	term_buf  = (uint8_t*)(0xA0000);
}

void put_pixel(uint8_t color, uint32_t row, uint32_t col) {
	term_buf[col + (row * VGA_WIDTH)] = color;
}

void fill_screen(uint8_t color) {
	for (uint32_t i = 0; i < VGA_HIGHT; i++) {
		for (uint32_t j = 0; j < VGA_WIDTH; j++) {
			put_pixel(color, i, j);
		}
	}
}
