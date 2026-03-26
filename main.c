#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

#define COUNT 8

typedef struct {
	int disks[COUNT];
	int topIndex;
} Tower;

typedef struct {
	Tower board[3];
	int held;
	int moves;
} Game;

void generateBoard(Game* game) {
	game->held = -1;
	game->moves = 0;

	for(int i=0; i < 3; i++) {
		Tower *tower = &game->board[i];
		for(int j=0; j < COUNT; j++) {
			tower->disks[j] = 0;
		}
		tower->topIndex = -1;

		if(i == 0) {
			for(int j=0; j < COUNT; j++) {
				tower->disks[j] = j + 1;
			}
			tower->topIndex = 0;
		}
	}
}

uint64_t getElapsed() {
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
		return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
	} else {
		return 0;
	}
}

void printboard(game* game) {
	for(int row=0; row < count; row++) {
		for(int i=0; i < 3; i++) {
			printf("  ");
			int val = game->board[i].disks[row];
			int held = game->held == i && game->board[i].topindex == row;
			for(int j=0; j < count * 2 + 1; j++) {
				int occupied = (val == 0 ? -1 : val) >= abs(count - j);
				if(held && occupied) {
					printf("o");
				} else if(!held && occupied) {
					printf("#");
				} else if(j == count) {
					printf("|");
				} else {
					printf(" ");
				}
			}
		}

		printf("  \n");
	}
}

void gameMove(Game* game, int ind) {
	int held = game->held;

	if(held == -1) {
		Tower *tower = &game->board[ind];
		if(tower->topIndex == -1) {
			return;
		}
		game->held = ind;
		return;
	}

	if(held == ind) {
		game->held = -1;
		return;
	}

	Tower *heldTower = &game->board[held];
	int heldTop = heldTower->topIndex;

	Tower *targetTower = &game->board[ind];
	int targetTop = targetTower->topIndex;

	if(targetTop == -1) targetTop = COUNT - 1;

	if(targetTower->disks[targetTop] == 0 || targetTower->disks[targetTop] >= heldTower->disks[heldTop]) {
		int newVal = heldTower->disks[heldTop];
		heldTower->disks[heldTop] = 0;
		heldTower->topIndex++;
		if(targetTower->topIndex == -1) {
			targetTower->disks[COUNT - 1] = newVal;
			targetTower->topIndex = COUNT - 1;
		} else {
			targetTower->disks[targetTop - 1] = newVal;
			targetTower->topIndex = targetTop - 1;
		}

		if(heldTower->topIndex == COUNT) heldTower->topIndex = -1;

		game->moves++;
	}
	
	game->held = -1;
}

int gameLoop(int c, Game* game, uint64_t startMS) {
	system("clear");

	switch(c) {
		case 's':
			printf("Press arrow keys (or colemak eio) to start. q to quit\n");
			break;
		case 'q':
			return 0;
		case 'e':
			gameMove(game, 0);
			break;
		case 'i':
			gameMove(game, 1);
			break;
		case 'o':
			gameMove(game, 2);
			break;
		default:
			printf("Invalid key\n");
			break;
	}

	Tower *rightTower = &game->board[2];
	int ordered = 1;
	for(int i=0; i < COUNT; i++) {
		if(rightTower->disks[i] != i + 1) ordered = 0;
	}

	uint64_t endMS = getElapsed();
	uint64_t elapsedMS = endMS - startMS;
	if(ordered) {
		int extraMoves = game->moves - (pow(2, COUNT) - 1);
		int threeMinutes = 1000 * 60 * 3;
		int score = round(10000 * (threeMinutes / (elapsedMS * pow(1.05, extraMoves))));
		
		printf("Game Over! Your moves: %d, Your time (s): %f\n", game->moves, (double) elapsedMS / 1000);

		printf("Final score: %d\n", score);
		return 0;
	}

	printf("Moves: %d\n", game->moves);
	printBoard(game);


	return 1;
}


int main() {
	struct termios oldt, newt;
	int ch;

	// Clone current settings
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;

	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	uint64_t startMS = getElapsed();
	
	Game game;
	generateBoard(&game);
	gameLoop((int) 's', &game, startMS);

	while(1) {
		ch = getchar();
		if(gameLoop(ch, &game, startMS) != 1) break;
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return 0;
}
