#include <hf-risc.h>
#include "vga_drv.h"

#define NUMBER_OF_BARS = 13*9
#define A_LEFT			0x2190
#define A_UP			0x2191
#define A_RIGHT			0x2192
#define A_DOWN			0x2193
#define A_UP_LEFT		0x2196
#define A_UP_RIGHT		0x2197
#define A_DOWN_LEFT		0x2199
#define A_DOWN_RIGHT	0x2198


struct ball_s {
	unsigned int ballx, bally;
	unsigned int last_ballx, last_bally;
	int dx, dy;
	int spx, spy;
	int spxcnt, spycnt;
};

struct paddle {
	int x, y;
	int cor;
	int spx;
	int div;
	int w, h;
};

struct bars {
	int x, y;
	int w, h;
	int cor;
};

void init_bars(struct bars *b[NUMBER_OF_BARS])
{
	string colors[9][10] = {RED, LRED, YELLOW, GREEN, LGREEN, CYAN, LCYAN, BLUE, LBLUE};

	for (int i = 0, l = 0; i < 280; i += 21) {
		int base = 5
		for(int j = 0; j < 9 ; j++, l++){
			display_frectangle	(5+i, base,	20, 8, colors[j]);
			b[l]->x = 5+i;
			b[l]->y = base;
			b[l]->w = 20;
			b[l]->h = 8;
			b[l]->cor = colors[j];
			base += 9;
		}
	}
}

// void init_bars(struct bars *b[NUMBER_OF_BARS])
// {
// 	for (int i = 0, l = 0; i < 280; i += 21) {
// 		display_frectangle(5+i, 5, 20, 8, RED);
// 		display_frectangle(5+i, 14, 20, 8, LRED);
// 		display_frectangle(5+i, 23, 20, 8, YELLOW);
// 		display_frectangle(5+i, 32, 20, 8, GREEN);
// 		display_frectangle(5+i, 41, 20, 8, LGREEN);
// 		display_frectangle(5+i, 50, 20, 8, CYAN);
// 		display_frectangle(5+i, 59, 20, 8, LCYAN);
// 		display_frectangle(5+i, 68, 20, 8, BLUE);
// 		display_frectangle(5+i, 77, 20, 8, LBLUE);
// 	}
// }

void init_paddle(struct paddle *p) {
	p->x = 130;
	p->y = 170;
	p->cor = WHITE;
	p->spx = 1;
	p->div = 10;
	p->w = 20;
	p->h = 4;
	display_frectangle(p->x, p->y, p->w, p->h, p->cor); //paddle
}

void init_display(struct bars *b[NUMBER_OF_BARS])
{
	display_background(BLACK);
	init_bars(b);
}

void init_ball(struct ball_s *ball, int x, int y, int dx, int dy, int spx, int spy)
{
	ball->ballx = x;
	ball->bally = y;
	ball->last_ballx = ball->ballx;
	ball->last_bally = ball->bally;
	ball->dx = dx;
	ball->dy = dy;
	ball->spx = spx;
	ball->spxcnt = spx;
	ball->spy = spy;
	ball->spycnt = spy;
}

void init_input()
{
	/* configure GPIOB pins 8 .. 12 as inputs */
	GPIOB->DDR &= ~(MASK_P8 | MASK_P9 | MASK_P10 | MASK_P11 | MASK_P12);
}

void test_limits(struct ball_s *ball, char *limits)
{
	unsigned int ballx, bally;
	
	ballx = ball->ballx;
	bally = ball->bally;
	
	limits[0] = display_getpixel(ballx-1, bally-1);		//D_left
	limits[1] = display_getpixel(ballx, bally-1);		//Down
	limits[2] = display_getpixel(ballx+1, bally-1);		//D_right
	limits[3] = display_getpixel(ballx-1, bally);		//Left
	limits[4] = 0;
	limits[5] = display_getpixel(ballx+1, bally);		//Right
	limits[6] = display_getpixel(ballx-1, bally+1);		//U_left
	limits[7] = display_getpixel(ballx, bally+1);		//Up
	limits[8] = display_getpixel(ballx+1, bally+1);		//U_right
}

char test_collision(struct ball_s *ball, char *dir)
{
	char hit = 0;
	int i;
	char limits[9];
	
	if ((ball->spx == ball->spxcnt) || (ball->spy == ball->spycnt)) {
		test_limits(ball, limits);

		if ((ball->ballx < VGA_WIDTH-1) && (ball->ballx > 0) && (ball->bally < VGA_HEIGHT-1) && (ball->bally > 0)) {
			char directions[9] = {A_DOWN_LEFT, A_DOWN, A_DOWN_RIGHT, A_LEFT, 0, A_RIGHT, A_UP_LEFT, A_UP, A_UP_RIGHT};
			for (i = 0; i < 9; i++) {
				if (limits[i]) {
					hit = 1;
					*dir = directions[i];
					break;
				}
			}
			
			if (limits[1]) ball->dy = -ball->dy;			//Down
			if (limits[3]) ball->dx = -ball->dx;			//Left
			if (limits[5]) ball->dx = -ball->dx;			//Right
			if (limits[7]) ball->dy = -ball->dy;			//Up

			if (limits[0]) {								//D_left
				if (!limits[3]) ball->dx = -ball->dx;
				if (!limits[1]) ball->dy = -ball->dy;
			}
			if (limits[2]) {								//D_right
				if (!limits[5]) ball->dx = -ball->dx;
				if (!limits[1]) ball->dy = -ball->dy;
			}
			if (limits[6]) {								//U_left
				if (!limits[3]) ball->dx = -ball->dx;
				if (!limits[7]) ball->dy = -ball->dy;
			}
			if (limits[8]) {								//U_right
				if (!limits[5]) ball->dx = -ball->dx;
				if (!limits[7]) ball->dy = -ball->dy;
			}
		} else {
			if ((ball->ballx + ball->dx > VGA_WIDTH-1) || (ball->ballx + ball->dx < 1))
				ball->dx = -ball->dx;
			if ((ball->bally + ball->dy > VGA_HEIGHT-1) || (ball->bally + ball->dy < 1))
				ball->dy = -ball->dy;
		}
	}
	
	return hit;
}

void update_ball(struct ball_s *ball, char hit)
{
	ball->last_ballx = ball->ballx;
	ball->last_bally = ball->bally;
	
	if (hit) {
		ball->spxcnt = 1;
		ball->spycnt = 1;
	}
	
	if (--ball->spxcnt == 0) {
		ball->spxcnt = ball->spx;
		ball->ballx = ball->ballx + ball->dx;
	}
	if (--ball->spycnt == 0) {
		ball->spycnt = ball->spy;
		ball->bally = ball->bally + ball->dy;
	}

	if ((ball->spx == ball->spxcnt) || (ball->spy == ball->spycnt)) {
		display_pixel(ball->last_ballx, ball->last_bally, BLACK);
		display_pixel(ball->ballx, ball->bally, WHITE);
	}
}

void get_input(struct paddle *p)
{
	if (GPIOB->IN & MASK_P10) { // left
		int temp = p->x;
		p->x = (p->x - p->spx < 0 ? 0 : p->x - p->spx);
		display_frectangle(temp, p->y, p->w, p->h, BLACK);
		display_frectangle(p->x, p->y, p->w, p->h, p->cor);
	}	
	if (GPIOB->IN & MASK_P11) { // right
		int temp = p->x;
		p->x = (p->x + p->spx > 300 ? 300 : p->x + p->spx);
		display_frectangle(temp, p->y, p->w, p->h, BLACK);
		display_frectangle(p->x, p->y, p->w, p->h, p->cor);
	}
}

int main(void)
{
	struct ball_s	ball1, ball2, ball3;
	struct paddle	pad;
	struct ball_s	*pball1 = &ball1;
	struct ball_s	*pball2 = &ball2;
	struct ball_s	*pball3 = &ball3;
	struct paddle	*pad_pointer = &pad;
	struct bars		*b[NUMBER_OF_BARS];
	char hit;

	init_display(b);
	init_paddle(pad_pointer);
	init_ball(pball1, 20, 200, 1, -1, 3, 7);
	init_ball(pball2, 30, 200, 1, -1, 5, 11);
	init_ball(pball3, 115, 150, -1, -1, 10, 3);
	init_input();

	while (1) {
		hit = test_collision(pball1);
		update_ball(pball1, hit);
		hit = test_collision(pball2);
		update_ball(pball2, hit);
		hit = test_collision(pball3);
		update_ball(pball3, hit);
		delay_ms(1);
		get_input(pad_pointer);
	}

	return 0;
}
