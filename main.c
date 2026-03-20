#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

typedef struct {
	int disks[8];
	int topIndex;
} Tower;

typedef struct {
	Tower board[3];
	int gameOver;
} Game;

void generateBoard(Game* game) {
	for(int i=0; i < 3; i++) {
		Tower *tower;
		for(int j=0; j < 8; j++) {
			tower->disks[j] = 0;
		}
		tower->topIndex = -1;

		if(i == 0) {
			for(int j=0; j < 8; j++) {
				tower->disks[j] = 8-j;
			}
			tower->topIndex = 7;
		}
		game->board[i] = *tower;
	}
}

void printBoard(Game* game) {
	for(int i=0; i < 3; i++) {
		printf("[");
		for(int j=0; j < 8; j++) {
			for(int k=0; k < game->board[i][j]; k++) {
				printf("#"); 
			}
			printf("\n");
		}
		printf("]\n");
	}
}

int gameLoop(int c, Game* game) {
	system("clear");

	if(c == 's') {
		printf("Press arrow keys (or colemak eio) to start. q to quit\n");
		return 1;
	}

	if(c == 'q') return 0;

	printf("Pressed: %c\n", c);
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

	Game *game;
	generateBoard(game);
	gameLoop((int) 's', game);

	while(1) {
		ch = getchar();
		if(gameLoop(ch, game) != 1) break;
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return 0;
}
