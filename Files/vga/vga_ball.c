#include <hf-risc.h>
#include "vga_drv.h"

#define VERT_BAR_BASE	9
#define NUMBER_OF_BARS  13 * VERT_BAR_BASE
#define HIT_PADDLE		'P'
#define HIT_BLOCK		'B'
#define HIT_FLOOR		'F'
#define LAYER_BASE 		4
#define LAYER_0			LAYER_BASE + 1 + 8 	// 13
#define LAYER_1			LAYER_0 + 1 + 8		// 22
#define LAYER_2			LAYER_1 + 1 + 8		// 31
#define LAYER_3			LAYER_2 + 1 + 8		// 40
#define LAYER_4			LAYER_3 + 1 + 8		// 49
#define LAYER_5			LAYER_4 + 1 + 8		// 58
#define LAYER_6			LAYER_5 + 1 + 8		// 67
#define LAYER_7			LAYER_6 + 1 + 8		// 76
#define LAYER_8			LAYER_7 + 1 + 8		// 85


struct ball_s {
	int ballx, bally;
	int last_ballx, last_bally;
	int dx, dy;
	int spx, spy;
	int spxcnt, spycnt;
	char ded;
};

struct paddle {
	unsigned int x, y;
	unsigned int cor;
	int spx;
	int lastdir;
	unsigned int div;
	unsigned int w, h;
	unsigned int moment;
};

struct bars {
	unsigned int x, y;
	int w, h;
	int cor;
	char deleted;
};

struct points {
	char d0, d1, d2, d3, d4;
	unsigned int pts;
};

void init_bars(struct bars *b[NUMBER_OF_BARS])
{
 	for (int i = 0, l = 0; i < 280; i += 21) {
 		display_frectangle(5+i, 5, 20, 8, RED);
		b[l]->x = 5+i;
		b[l]->y = 5;
		b[l]->w = 20;
		b[l]->h = 8;
		b[l]->deleted = 0;
		b[l++]->cor = RED;
 		display_frectangle(5+i, 14, 20, 8, LRED);
		b[l]->x = 5+i;
		b[l]->y = 14;
		b[l]->w = 20;
		b[l]->h = 8;
		b[l]->deleted = 0;
		b[l++]->cor = LRED;
 		display_frectangle(5+i, 23, 20, 8, YELLOW);
		b[l]->x = 5+i;
		b[l]->y = 23;
		b[l]->w = 20;
		b[l]->h = 8;
		b[l]->deleted = 0;
		b[l++]->cor = YELLOW;
 		display_frectangle(5+i, 32, 20, 8, GREEN);
		b[l]->x = 5+i;
		b[l]->y = 32;
		b[l]->w = 20;
		b[l]->h = 8;
		b[l]->deleted = 0;
		b[l++]->cor = GREEN;
 		display_frectangle(5+i, 41, 20, 8, LGREEN);
		b[l]->x = 5+i;
		b[l]->y = 41;
		b[l]->w = 20;
		b[l]->h = 8;
		b[l]->deleted = 0;
		b[l++]->cor = LGREEN;
 		display_frectangle(5+i, 50, 20, 8, CYAN);
		b[l]->x = 5+i;
		b[l]->y = 50;
		b[l]->w = 20;
		b[l]->h = 8;
		b[l]->deleted = 0;
		b[l++]->cor = CYAN;
 		display_frectangle(5+i, 59, 20, 8, LCYAN);
		b[l]->x = 5+i;
		b[l]->y = 59;
		b[l]->w = 20;
		b[l]->h = 8;
		b[l]->deleted = 0;
		b[l++]->cor = LCYAN;
 		display_frectangle(5+i, 68, 20, 8, BLUE);
		b[l]->x = 5+i;
		b[l]->y = 68;
		b[l]->w = 20;
		b[l]->h = 8;
		b[l]->deleted = 0;
		b[l++]->cor = BLUE;
 		display_frectangle(5+i, 77, 20, 8, LBLUE);
		b[l]->x = 5+i;
		b[l]->y = 77;
		b[l]->w = 20;
		b[l]->h = 8;
		b[l]->deleted = 0;
		b[l++]->cor = LBLUE;
 	}
 }

void init_paddle(struct paddle *p) {
	p->x = 130;
	p->y = 170;
	p->cor = WHITE;
	p->spx = 1;
	p->div = 10;
	p->moment = 0;
	p->lastdir = 0;
	p->w = 20;
	p->h = 4;
	display_frectangle(p->x, p->y, p->w, p->h, p->cor); //paddle
}

void init_display(struct bars *b[NUMBER_OF_BARS])
{
	display_background(BLACK);
	init_bars(b);
	for (int i = 0; i < NUMBER_OF_BARS; i++) {
		printf("Barra %d, (%d, %d), w: %d, h: %d, cor: %d\n", i, b[i]->x, b[i]->y, b[i]->w, b[i]->h, b[i]->cor);
	}
}

void init_points(struct points *points) {
	points->d0 = '0';
	points->d1 = '0';
	points->d2 = '0';
	points->d3 = '0';
	points->d4 = '0';
	points->pts = 0;

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
	ball->ded = 0;
}

void init_input()
{
	/* configure GPIOB pins 8 .. 12 as inputs */
	GPIOB->DDR &= ~(MASK_P8 | MASK_P9 | MASK_P10 | MASK_P11 | MASK_P12);
}

void inc_points(char cor, struct points *points) {
	unsigned int p = 0;
	switch (cor) {
		case RED: p = 9; break;
		case LRED: p = 8; break;
		case YELLOW: p = 7; break;
		case GREEN: p = 6; break;
		case LGREEN: p = 5; break;
		case CYAN: p = 4; break;
		case LCYAN: p = 3; break;
		case BLUE: p = 2; break;
		case LBLUE: p = 1; break;
		default: return;
	}
	points->pts += p;
}

void draw_points(struct points *points) {
	int i, j;
	for (i = 0, j = 0; i < 5; i++, j += 20) {
		switch (i) {
			case 0: display_char(points->d0, 0 + j, 190, 2, WHITE); break;
			case 1: display_char(points->d1, 0 + j, 190, 2, WHITE); break;
			case 2: display_char(points->d2, 0 + j, 190, 2, WHITE); break;
			case 3: display_char(points->d3, 0 + j, 190, 2, WHITE); break;
			case 4: display_char(points->d4, 0 + j, 190, 2, WHITE); break;
			default: return;
		}
	}
}

void erase_points(struct points *points) {
	display_frectangle(0, 190, 100, 20, BLACK);
}

void update_points(struct points *points) {
	unsigned int aux = points->pts;
	unsigned int temp = 0;
	temp = (aux % 10) + 48;
	aux /= 10;
	points->d4 = temp;
	temp = (aux % 10) + 48;
	aux /= 10;
	points->d3 = temp;
	temp = (aux % 10) + 48;
	aux /= 10;
	points->d2 = temp;
	temp = (aux % 10) + 48;
	aux /= 10;
	points->d1 = temp;
	temp = (aux % 10) + 48;
	aux /= 10;
	points->d0 = temp;
	erase_points(points);
	draw_points(points);
}

char bars_col_test(int bx, int by, struct bars *bar[NUMBER_OF_BARS], struct points *pnts)
{
	// if(b->bally <= LAYER_8+1 && b->bally >= LAYER_BASE-1)
	{
		for (int i = 0; i < NUMBER_OF_BARS; i++){
			if (bar[i]->deleted) continue;
			if (bx >= bar[i]->x && bx <= bar[i]->x + bar[i]->w) {
				if (by >= bar[i]->y && by <= bar[i]->y + bar[i]->h)
				{ /*Hit a bar*/
					display_frectangle(bar[i]->x,bar[i]->y,bar[i]->w,bar[i]->h, BLACK);
					inc_points(bar[i]->cor, pnts); //update points here because bar[i] is found here, ideally bars_col_test would return a tuple with the hit char and the color char
					bar[i]->deleted = 1;
					if (bx >= bar[i]->x && bx <= bar[i]->x + 2) return 'K';
					if (bx <= bar[i]->x + bar[i]->w && bx >= bar[i]->x + bar[i]->w - 2) return 'K';
					return '1';
				}
			}
		}
	}
	return '0';
}

char padd_col_test(struct ball_s *b, struct paddle *p)
{
	// if (b->bally+1 >= (p->y - p->h) /*paddle base+height*/ && b->bally + 1<= VGA_HEIGHT)
	{
		/*next hit either is on paddle or ball is dead*/
		if ((b->ballx >= p->x && b->ballx <= (p->x + p->w)))
		{
			b->dy= -b->dy;
			return HIT_PADDLE;
		}
		else if ((b->ballx+1 >= p->x && b->ballx-1 <= (p->x + p->w)))
		{
			b->dy= -b->dy;
			b->dx= -b->dx;
			return HIT_PADDLE;
		}
		return HIT_FLOOR;
	}
	return 0;
}

char colision_test(struct ball_s *b, struct bars *bar[NUMBER_OF_BARS], struct paddle *p, struct points *pnts)
{
	if (b->bally <= 0 ) 								 b->dy = -b->dy;
	if (b->bally >= p->y + p->h + 10)					{b->ded = 1;	b->dy = -b->dy;}
	if (b->ballx >= VGA_WIDTH		|| b->ballx <= 0 ) 	 b->dx = -b->dx;
	char ret = '0';
	if (b->bally+1 >= p->y /*paddle base+height*/ && b->bally + 1<= VGA_HEIGHT) ret = padd_col_test(b,p);
	ret = bars_col_test(b->ballx, b->bally, bar, pnts);
	return ret;

}

void update_ball(struct ball_s *ball, char hit, struct paddle *p)
{
	if (hit != '0') {
		if (hit == 'K') ball->dx = -ball->dx;
		if (hit == '1') ball->dy = -ball->dy;
	}
	ball->last_ballx = ball->ballx;
	ball->last_bally = ball->bally;
		if (!ball->ded)
		{
		ball->ballx = ball->ballx + ball->dx;
		ball->bally = ball->bally + ball->dy;
		display_pixel(ball->last_ballx, ball->last_bally, BLACK);
		display_pixel(ball->ballx, ball->bally, WHITE);
		}
		else 
		{
			ball->bally = p->y - 3;
			ball->ballx = p->x +(p->w/2);
			display_pixel(ball->last_ballx, ball->last_bally, BLACK);
			display_pixel(ball->ballx, ball->bally, WHITE);
		}

}

void get_input(struct paddle *p, struct ball_s *b)
{
	if (GPIOB->IN & MASK_P10 && p->x > 5) { // left
		int temp = p->x;
		p->lastdir = 1;
		if (p->div > 0) p->div--;
		else {
			p->x = (p->x - p->moment < 0 ? 0 : p->x - p->moment);
			p->moment = (p->moment < 3 ? p->moment + 1 : 3);
			display_frectangle(temp, p->y, (p->w), p->h, BLACK);
			display_frectangle(p->x, p->y, (p->w), p->h, p->cor);
			p->div = 1;
		}
	} else if (p->moment > 3 && p->lastdir == -1 && p->x > 5) {
		int temp = p->x;
		if (p->div > 0) p->div--;
		else {
			p->x = (p->x - p->moment > 0 ? 0 : p->x - p->moment);
			p->moment--;
			display_frectangle(temp, p->y, (p->w), p->h, BLACK);
			display_frectangle(p->x, p->y, (p->w), p->h, p->cor);
			p->div = 1;
		}
	}
	if (GPIOB->IN & MASK_P11 && p->x < 280) { // right
		p->lastdir = -1;
		int temp = p->x;
		if (p->div > 0) p->div--;
		else {
			p->x = (p->x + p->moment > 280 ? 280 : p->x + p->moment);
			p->moment = (p->moment < 3 ? p->moment + 1 : 3);
			display_frectangle(temp, p->y, (p->w), p->h, BLACK);
			display_frectangle(p->x, p->y, (p->w), p->h, p->cor);
			p->div = 1;
		}
	} else if (p->moment > 5 && p->lastdir == -1) {
		int temp = p->x;
		if (p->div > 0) p->div--;
		else {
			p->x = (p->x + p->moment > 280 ? 280 : p->x + p->moment);
			p->moment--;
			display_frectangle(temp, p->y, (p->w), p->h, BLACK);
			display_frectangle(p->x, p->y, (p->w), p->h, p->cor);
			p->div = 1;
		}
	}
	if (p->moment == 0) p->lastdir = 0;

	if (GPIOB->IN & MASK_P8 && b->ded)	/*CENTER*/
	{
		b->dy = -1;
		b->dx = 0;
		b->ded = 0;
	}
	if (GPIOB->IN & MASK_P9 && b->ded)	/*UP*/
	{
		b->dy = -1;
		b->dx = -1;
		b->ded = 0;
	}
	if (GPIOB->IN & MASK_P12 && b->ded)	/*DOWN*/
	{
		b->dy = -1;
		b->dx = 1;
		b->ded = 0;
	}
}

int main(void)
{
	struct ball_s	ball1; 
	struct paddle	pad;
	struct ball_s	*pball1 = &ball1;
	struct paddle	*pad_pointer = &pad;
	struct bars		*b[NUMBER_OF_BARS];
	struct points points;
	struct points *points_ptr = &points;
	char hit;
	char dir;
	
	heap_init((uint32_t *)&_heap_start, 8192);

	for (int i = 0; i < NUMBER_OF_BARS; i++) b[i] = malloc(sizeof(struct bars));

	init_display(b);
	init_points(points_ptr);
	draw_points(points_ptr);
	init_paddle(pad_pointer);
	init_ball(pball1, 150, 160, -1, 1, 90, 90);
	init_input();

	while (1) {
		hit = colision_test(pball1,b,pad_pointer, points_ptr);
		if (hit != '0') update_points(points_ptr);
		update_ball(pball1, hit, pad_pointer);
		get_input(pad_pointer, pball1);
		delay_ms(10);
	}

	return 0;
}
