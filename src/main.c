#include "vga.h"

struct vector3 {
	float x;
	float y;
	float z;
};

struct vector2 {
	float x;
	float y;
};

struct donutinfo {
	float R1;
	float R2;
	uint32_t points;
	uint32_t circles;
};

const float PI = 3.14159265358979323846264338327950;
const float FULL_ROTATION = 360 * (PI / 180.0);
const uint32_t WIDTH = 320;
const uint32_t HIGHT = 200;
float zbuffer[200][320];

//borrowed math functions from
//https://github.com/lucianoforks/tetris-os/blob/master/src/math.c
float sin(float x) {
	float result;
	asm("fsin" : "=t"(result) : "0"(x));
	return result;
}

float cos(float x) {
	return sin(x + PI / 2.0);
}
//end of borrowed code

struct vector3 rotate_z(struct vector3 point3d, float theta) {
	return (struct vector3){
		point3d.x * cos(theta) - point3d.y * sin(theta),
		point3d.y * cos(theta) + point3d.x * sin(theta),
		point3d.z
	};
}

struct vector3 rotate_x(struct vector3 point3d, float theta) {
	return (struct vector3){
		point3d.x,
		point3d.y * cos(theta) - point3d.z * sin(theta),
		point3d.z * cos(theta) + point3d.y * sin(theta)
	};
}

struct vector3 rotate_y(struct vector3 point3d, float theta) {
	return (struct vector3){
		point3d.x * cos(theta) - point3d.z * sin(theta),
		point3d.y,
		point3d.z * cos(theta) + point3d.x * sin(theta)
	};
}

struct vector3 map_point_on_circle (float radias, float theta, float phi, float angle) {
	struct vector3 point3d = (struct vector3){0, radias * sin(theta), radias * cos(theta)};
	point3d = rotate_z(point3d, phi + PI / 2.0);
	point3d = rotate_z(point3d, angle / 2);	
	point3d = rotate_x(point3d, angle);	
	point3d = rotate_y(point3d, angle);	
	return point3d;
}

void fill_zbuffer(struct donutinfo donut, float angle) {
	struct vector2 center = (struct vector2){WIDTH / 2, HIGHT / 2};
	struct vector2 dif = (struct vector2){(WIDTH - center.x)/2, HIGHT - center.y};
	for (uint32_t origin = 0; origin < donut.circles; origin++) {
		float phi = (FULL_ROTATION / donut.circles) * origin;
		struct vector3 curser3d = (struct vector3){
			0.0 + donut.R1 * cos(phi),
			0.0 + donut.R1 * sin(phi),
			0.0
		};
		curser3d = rotate_z(curser3d, angle / 2);
		curser3d = rotate_x(curser3d, angle);
		curser3d = rotate_y(curser3d, angle);
		for (uint32_t point = 0; point < donut.points; point++) {
			float theta = (FULL_ROTATION / donut.points) * point;
			struct vector3 rotation3d = map_point_on_circle(donut.R2, theta, phi, angle);
			curser3d = (struct vector3){
				curser3d.x - (rotation3d.x / donut.points),
				curser3d.y - (rotation3d.y / donut.points),
				curser3d.z - (rotation3d.z / donut.points)
			};
			uint32_t x = center.x + (dif.x * curser3d.x) / (2 - curser3d.z);
			uint32_t y = center.y + (dif.y * curser3d.y) / (2 - curser3d.z);
			if (y < HIGHT && y >= 0 
				&& x < WIDTH && x >= 0
				&& curser3d.z > zbuffer[y][x]) {
				zbuffer[y][x] = curser3d.z;
			}
		}
	}
}

uint8_t select_shade(float num) {
	uint8_t shades[16] = {
		0x2E, 0x30, 0x31, 0x32, 0x47, 0x48, 0x49, 0x76,
		0x77, 0x78, 0x79, 0xBE, 0xBF, 0xC0, 0xC1, 0x00
	};
	num = num * -1;
	num = num * 5;
	num = num + 6;
	if (num > 15) {
		return shades[14];
	} else if (num < 0) {
		return shades[0];
	}
	return shades[(uint32_t)num];
}


void draw_zbuffer() {
	for (int row = 0; row < HIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			if (zbuffer[row][col] != -10) {
				put_pixel(select_shade(zbuffer[row][col]), row, col);
			} else {
				put_pixel(0x00, row, col);
			}
		}
	}
}

void main() {
	const struct donutinfo donut = (struct donutinfo){0.5, 1.75, 32, 32};
	float angle = 0.0;
	term_init();
	fill_screen(0x00); //clr screen
	for(;;) {
		for (int i = 0; i < HIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) { 
				zbuffer[i][j] = -10.0;
			}
		}
		angle = angle + (FULL_ROTATION / 1000);
		if (angle > FULL_ROTATION) {
			angle = 0;
		}
		fill_zbuffer(donut, angle);
		draw_zbuffer();
	}
}
