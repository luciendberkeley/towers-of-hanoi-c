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
	int held;
	int moves;
} Game;

void generateBoard(Game* game) {
	game->held = -1;
	game->moves = 0;

	for(int i=0; i < 3; i++) {
		Tower *tower = &game->board[i];
		for(int j=0; j < 8; j++) {
			tower->disks[j] = 0;
		}
		tower->topIndex = -1;

		if(i == 0) {
			for(int j=0; j < 8; j++) {
				tower->disks[j] = j + 1;
			}
			tower->topIndex = 0;
		}
	}
}

void printBoard(Game* game) {
	char *lookup[] = {
		"        |        ",
		"       ###       ",
		"      #####      ",
		"     #######     ",
		"    #########    ",
		"   ###########   ",
		"  #############  ",
		" ############### ",
		"#################",
	};
	
	char *lookupHeld[] = {
		"        |        ",
		"       OOO       ",
		"      OOOOO      ",
		"     OOOOOOO     ",
		"    OOOOOOOOO    ",
		"   OOOOOOOOOOO   ",
		"  OOOOOOOOOOOOO  ",
		" OOOOOOOOOOOOOOO ",
		"OOOOOOOOOOOOOOOOO",
	};

	for(int row=0; row < 8; row++) {
		for(int i=0; i < 3; i++) {
			printf("  ");
			int val = game->board[i].disks[row];
			if(game->held == i && game->board[i].topIndex == row) printf("%s", lookupHeld[val]);
			else printf("%s", lookup[val]);
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

	if(targetTop == -1) targetTop = 7;

	if(targetTower->disks[targetTop] == 0 || targetTower->disks[targetTop] >= heldTower->disks[heldTop]) {
		int newVal = heldTower->disks[heldTop];
		heldTower->disks[heldTop] = 0;
		heldTower->topIndex++;
		if(targetTower->topIndex == -1) {
			targetTower->disks[7] = newVal;
			targetTower->topIndex = 7;
		} else {
			targetTower->disks[targetTop - 1] = newVal;
			targetTower->topIndex = targetTop - 1;
		}

		if(heldTower->topIndex == 8) heldTower->topIndex = -1;

		game->moves++;
	}
	
	game->held = -1;
}

int gameLoop(int c, Game* game) {
	system("clear");

	switch(c) {
		case 's':
			printf("Press arrow keys (or colemak eio) to start. q to quit\n");
			return 1;
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

	Game game;
	generateBoard(&game);
	gameLoop((int) 's', &game);

	while(1) {
		ch = getchar();
		if(gameLoop(ch, &game) != 1) break;
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return 0;
}
