#include "def.h"
#include "primlib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ARENA_HEIGHT 20
#define ARENA_WIDTH 10
#define SIZE 30
#define N_X_1 1000
#define N_Y_1 200
#define RIGHT 1
#define LEFT -1
#define FRAME_DELAY 50
#define MOVABLE 1
#define MOV_PIVOT 2
#define HOLD_MOV 3
#define HOLD_PIV 4
#define EMPTY 0
#define BLOCK 9

char game_matirx[ARENA_HEIGHT][ARENA_WIDTH];

void background()
{
	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
	gfx_line(gfx_screenWidth() / 2 - ARENA_WIDTH * SIZE / 2, gfx_screenHeight(),
			 gfx_screenWidth() / 2 - ARENA_WIDTH * SIZE / 2,
			 gfx_screenHeight() - ARENA_HEIGHT * SIZE, GREEN);
	gfx_line(gfx_screenWidth() / 2 + ARENA_WIDTH * SIZE / 2, gfx_screenHeight(),
			 gfx_screenWidth() / 2 + ARENA_WIDTH * SIZE / 2,
			 gfx_screenHeight() - ARENA_HEIGHT * SIZE, GREEN);

	gfx_line(gfx_screenWidth() / 2 - ARENA_WIDTH * SIZE / 2,
			 gfx_screenHeight() - ARENA_HEIGHT * SIZE,
			 gfx_screenWidth() / 2 + ARENA_WIDTH * SIZE / 2,
			 gfx_screenHeight() - ARENA_HEIGHT * SIZE, GREEN);

	gfx_line(N_X_1, N_Y_1, N_X_1, N_Y_1 + 6 * SIZE, GREEN);
	gfx_line(N_X_1, N_Y_1 + 6 * SIZE, N_X_1 + 6 * SIZE, N_Y_1 + 6 * SIZE,
			 GREEN);
	gfx_line(N_X_1 + 6 * SIZE, N_Y_1 + 6 * SIZE, N_X_1 + 6 * SIZE, N_Y_1,
			 GREEN);
	gfx_line(N_X_1 + 6 * SIZE, N_Y_1, N_X_1, N_Y_1, GREEN);
}

void draw()
{
	int left = gfx_screenWidth() / 2 - ARENA_WIDTH * SIZE / 2;
	int up = gfx_screenHeight() - ARENA_HEIGHT * SIZE;
	for (int i = 0; i < ARENA_HEIGHT; i++) {
		for (int j = 0; j < ARENA_WIDTH; j++) {
			switch (game_matirx[i][j]) {
			case MOVABLE:
				gfx_filledRect(left + j * SIZE + 1, up + i * SIZE + 1,
							   left + (j + 1) * SIZE - 1,
							   up + (i + 1) * SIZE - 1, GREEN);
				break;
			case MOV_PIVOT:
				gfx_filledRect(left + j * SIZE + 1, up + i * SIZE + 1,
							   left + (j + 1) * SIZE - 1,
							   up + (i + 1) * SIZE - 1, YELLOW);
				break;
			case BLOCK:
				gfx_filledRect(left + j * SIZE + 1, up + i * SIZE + 1,
							   left + (j + 1) * SIZE - 1,
							   up + (i + 1) * SIZE - 1, RED);
				break;
			}
		}
	}
}

void next_piece(int piece)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			switch (pieces[piece][0][i][j]) {
			case MOVABLE:
				gfx_filledRect(N_X_1 + (j + 1) * SIZE + 1,
							   N_Y_1 + (i + 1) * SIZE + 1,
							   N_X_1 + (j + 2) * SIZE - 1,
							   N_Y_1 + (i + 2) * SIZE - 1, GREEN);
				break;
			case MOV_PIVOT:
				gfx_filledRect(N_X_1 + (j + 1) * SIZE + 1,
							   N_Y_1 + (i + 1) * SIZE + 1,
							   N_X_1 + (j + 2) * SIZE - 1,
							   N_Y_1 + (i + 2) * SIZE - 1, YELLOW);
				break;
			}
		}
	}
}

int new_element(int piece)
{
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = ARENA_WIDTH / 2 - 2, k = 0; j < ARENA_WIDTH / 2 + 2;
			 j++, k++) {
			if (game_matirx[i][j] != EMPTY && pieces[piece][0][i][k] != EMPTY)
				return 1;
			if (game_matirx[i][j] == EMPTY && pieces[piece][0][i][k] != EMPTY) {
				game_matirx[i][j] = pieces[piece][0][i][k];
			}
		}
	}
	return 0;
}

void freeze()
{
	for (int i = ARENA_HEIGHT - 1; i >= 0; i--) {
		for (int j = 0; j < ARENA_WIDTH; j++) {
			if (game_matirx[i][j] == MOVABLE ||
				game_matirx[i][j] == MOV_PIVOT ||
				game_matirx[i][j] == HOLD_MOV || game_matirx[i][j] == HOLD_PIV)
				game_matirx[i][j] = BLOCK;
		}
	}
}

void go_down()
{
	for (int i = ARENA_HEIGHT - 1; i >= 0; i--) {
		for (int j = 0; j < ARENA_WIDTH; j++) {
			if (game_matirx[i][j] == HOLD_MOV ||
				game_matirx[i][j] == HOLD_PIV) {
				game_matirx[i + 1][j] = game_matirx[i][j] - 2;
				game_matirx[i][j] = EMPTY;
			}
		}
	}
}

int next_frame(int if_freeze)
{
	for (int i = ARENA_HEIGHT - 1; i >= 0; i--) {
		for (int j = 0; j < ARENA_WIDTH; j++) {
			if (game_matirx[i][j] == MOVABLE ||
				game_matirx[i][j] == MOV_PIVOT) {
				if (i == ARENA_HEIGHT - 1 ||
					(game_matirx[i + 1][j] != EMPTY &&
					 game_matirx[i + 1][j] != HOLD_MOV &&
					 game_matirx[i + 1][j] != HOLD_PIV)) {
					if (if_freeze)
						freeze();
					return 1;
				}
				if (game_matirx[i + 1][j] == EMPTY ||
					game_matirx[i + 1][j] == HOLD_MOV ||
					game_matirx[i + 1][j] == HOLD_PIV)
					game_matirx[i][j] += 2;
			}
		}
	}
	go_down();
	return 0;
}

int rotate(int type, int rotation)
{
	int pos[2], pivot_prev[2], pivot_new[2], i, j, allowed = 1, m, n;
	for (i = 3; i >= 0; i--) {
		for (j = 0; j < 4; j++) {
			if (pieces[type][rotation][i][j] == MOV_PIVOT) {
				pivot_prev[0] = i;
				pivot_prev[1] = j;
			}
			if (pieces[type][(rotation + 1) % 4][i][j] == MOV_PIVOT) {
				pivot_new[0] = i;
				pivot_new[1] = j;
			}
		}
	}

	for (i = ARENA_HEIGHT - 1; i >= 0; i--) {
		for (j = 0; j < ARENA_WIDTH; j++) {
			if (game_matirx[i][j] == MOV_PIVOT) {
				pos[0] = i;
				pos[1] = j;
			}
		}
	}
	for (i = pos[0] - pivot_new[0], m = 0; m < 4; i++, m++) {
		for (j = pos[1] - pivot_new[1], n = 0; n < 4; j++, n++) {
			if (pieces[type][(rotation + 1) % 4][m][n]) {
				if (i > ARENA_HEIGHT - 1 || i < 0 || j > ARENA_WIDTH - 1 ||
					j < 0)
					allowed = 0;
				else if (game_matirx[i][j] == BLOCK)
					allowed = 0;
			}
		}
	}
	if (allowed) {
		for (i = pos[0] - pivot_prev[0], m = 0; m < 4; i++, m++) {
			for (j = pos[1] - pivot_prev[1], n = 0; n < 4; j++, n++) {
				if (i >= 0 && i < ARENA_HEIGHT && j >= 0 && j < ARENA_WIDTH)
					game_matirx[i][j] =
						game_matirx[i][j] - pieces[type][rotation][m][n];
			}
		}
		for (i = pos[0] - pivot_new[0], m = 0; m < 4; i++, m++) {
			for (j = pos[1] - pivot_new[1], n = 0; n < 4; j++, n++) {
				if (i >= 0 && i < ARENA_HEIGHT && j >= 0 && j < ARENA_WIDTH)
					game_matirx[i][j] = game_matirx[i][j] +
										pieces[type][(rotation + 1) % 4][m][n];
			}
		}
		return (rotation + 1) % 4;
	}
	return rotation;
}

void clean()
{
	for (int i = ARENA_HEIGHT - 1; i >= 0; i--) {
		for (int j = 0; j < ARENA_WIDTH; j++) {
			if (game_matirx[i][j] == HOLD_MOV ||
				game_matirx[i][j] == HOLD_PIV) {
				game_matirx[i][j] -= 2;
			}
		}
	}
}

void go_side(int side)
{
	for (int i = ARENA_HEIGHT - 1; i >= 0; i--) {
		if (side == LEFT) {
			for (int j = 0; j < ARENA_WIDTH; j++) {
				if (game_matirx[i][j] == HOLD_MOV ||
					game_matirx[i][j] == HOLD_PIV) {
					game_matirx[i][j + side] = game_matirx[i][j] - 2;
					game_matirx[i][j] = EMPTY;
				}
			}
		}
		else {
			for (int j = ARENA_WIDTH - 1; j >= 0; j--) {
				if (game_matirx[i][j] == HOLD_MOV ||
					game_matirx[i][j] == HOLD_PIV) {
					game_matirx[i][j + side] = game_matirx[i][j] - 2;
					game_matirx[i][j] = EMPTY;
				}
			}
		}
	}
}

int movement(int side /*right = 1, left = -1*/)
{
	for (int i = ARENA_HEIGHT - 1; i >= 0; i--) {
		if (side == LEFT) {
			for (int j = 0; j < ARENA_WIDTH; j++) {
				if (game_matirx[i][j] == MOVABLE ||
					game_matirx[i][j] == MOV_PIVOT) {
					if ((j == 0 || (game_matirx[i][j - 1] != EMPTY &&
									game_matirx[i][j - 1] != HOLD_MOV &&
									game_matirx[i][j - 1] != HOLD_PIV))) {
						clean();
						return 0;
					}
					if (game_matirx[i][j - 1] == EMPTY ||
						game_matirx[i][j - 1] == HOLD_MOV ||
						game_matirx[i][j - 1] == HOLD_PIV)
						game_matirx[i][j] += 2;
				}
			}
		}
		if (side == RIGHT) {
			for (int j = ARENA_WIDTH - 1; j >= 0; j--) {
				if (game_matirx[i][j] == MOVABLE ||
					game_matirx[i][j] == MOV_PIVOT) {
					if ((j == ARENA_WIDTH - 1 ||
						 (game_matirx[i][j + 1] != EMPTY &&
						  game_matirx[i][j + 1] != HOLD_MOV &&
						  game_matirx[i][j + 1] != HOLD_PIV))) {
						clean();
						return 0;
					}
					if (game_matirx[i][j + 1] == EMPTY ||
						game_matirx[i][j + 1] == HOLD_MOV ||
						game_matirx[i][j + 1] == HOLD_PIV)
						game_matirx[i][j] += 2;
				}
			}
		}
	}
	go_side(side);
	return 1;
}

void line()
{
	int counter;
	for (int i = ARENA_HEIGHT - 1; i >= 0; i--) {
		counter = 0;
		for (int j = 0; j < ARENA_WIDTH; j++) {
			if (game_matirx[i][j] == BLOCK)
				counter++;
		}
		if (counter == ARENA_WIDTH) {
			for (int m = i; m >= 0; m--) {
				for (int n = 0; n < ARENA_WIDTH; n++) {
					if (m > 0)
						game_matirx[m][n] = game_matirx[m - 1][n];
					else
						game_matirx[m][n] = EMPTY;
				}
			}
			i++;
		}
	}
}

void to_bottom()
{
	while (next_frame(0) == 0)
		;
	clean();
}

void game_over()
{
	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
	gfx_textout(gfx_screenWidth() / 2 - 30, gfx_screenHeight() / 2 - 5,
				"GAMEOVER", RED);
	gfx_updateScreen();
}

int main(int argc, char* argv[])
{
	if (gfx_init()) {
		exit(3);
	}
	srand(time(NULL));
	int counter = 0, el, end = 0, piece = rand() % 7, next = rand() % 7,
		rotation = 0, wait_rot, wait_mov;
	new_element(piece);

	do {
		background();
		draw();
		next_piece(next);
		gfx_updateScreen();
		counter++;
		if (counter == FRAME_DELAY) {
			el = next_frame(1);
			line();
			counter = 0;
			if (el) {
				piece = next;
				next = rand() % 7;
				rotation = 0;
				if (new_element(piece))
					end = 1;
			}
		}
		if (!gfx_isKeyDown(SDLK_SPACE))
			wait_rot = 0;
		if (gfx_isKeyDown(SDLK_SPACE) && wait_rot == 0) {
			rotation = rotate(piece, rotation);
			wait_rot = 1;
		}

		if (!gfx_isKeyDown(SDLK_RIGHT) && !gfx_isKeyDown(SDLK_LEFT))
			wait_mov = 0;
		if (gfx_isKeyDown(SDLK_RIGHT) && wait_mov == 0) {
			movement(RIGHT);
			wait_mov = 1;
		}
		if (gfx_isKeyDown(SDLK_LEFT) && wait_mov == 0) {
			movement(LEFT);
			wait_mov = 1;
		}
		if (gfx_isKeyDown(SDLK_DOWN) && wait_mov == 0) {
			to_bottom();
			wait_mov = 1;
			counter--;
		}
	} while (!gfx_isKeyDown(SDLK_ESCAPE) && !end);
	game_over();
	gfx_getkey();
	return 0;
}
