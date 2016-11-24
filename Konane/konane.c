/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.c
 * Author: Delaney Waldo
 *
 * Created on October 31, 2016, 5:55 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#include "tree.h"

#define BOARD_SIZE 8

// Board state struct (a struct rather than 2d array to make it easier to
// pass around to different functions.
struct state {
	char board[BOARD_SIZE][BOARD_SIZE];
	char *action; // action to get to this move (use in get_moves())
	int eval_value;
};

//static struct tree_node *tree_root;
static char player;
static char turn = 'B';

static bool tree_change = false; // Tree has been changed and thus tree creation plugin should change.

static bool is_running = true;

/**
 * print_board() - prints a board
 * @state - state to print
 */
static void print_board(struct state *state) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			// don't check if null character because board should be right */
			fprintf(stdout, "%c", state->board[i][j]);
		}
		// get rid of new character line at end of line. */
		fprintf(stdout, "\n");
	}
	fprintf(stdout, "\n");
}


/**
 * print_usage() - prints usage of application
 */
void print_usage(char *exec_name)
{
	fprintf(stdout, "Usage: %s file player\n", exec_name);
	fprintf(stdout, "file - filename of board\n");
}

/**
 * update_state() - apply a move
 * @move - move to make
static void update_state(char *move)
{
	int child_count = 0;
	struct tree_node **children_of_root = get_tree_children(tree_root, &child_count);
	for (int i = 0; i < child_count; i++) {
		struct state *state = children_of_root[i]->value;
		if (strcmp(state->action, move) == 0) {
			tree_root = children_of_root[i];
			tree_change = true;
			return;
		}
	}
	fprintf(stderr, "ERROR: Failed to find new state in tree after move.\n");
	exit(1);
}
*/
/**
 * create_state() - creates a new board state object
 * @parent - parent to inherit board state from
 * @action - action to get a change.
 *
 * Returns a new state
 */
static struct state *create_state(struct state *parent, char *action)
{
	struct state *state = malloc(sizeof(struct state));

	if (state == NULL) {
		fprintf(stderr, "ERROR: Memory allocation failure.\n");
		exit(1);
	}

	size_t size_of_board = BOARD_SIZE * BOARD_SIZE;

	if (parent != NULL) {
		memcpy(state->board, parent->board, size_of_board);
	}

	state->action = action;

	return state;
}

/*
 * count_o() - return the number of "O" spaces on current board
 * used to determine minimum number of memory to allocate get_moves()
*/
int count_o (struct state *current)
{
	int count = 0;

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			// don't check if null character because board should be right */
			if ('O' == current->board[i][j])
				count++;
		}
	}

	return count;
}

int translate_letter(char letter)
{
	//fprintf("%c \n", letter);
	switch (letter){
		case 'A':
			return 0;
		case 'B':
			return 1;
		case 'C':
			return 2;
		case 'D':
			return 3;
		case 'E':
			return 4;
		case 'F':
			return 5;
		case 'G':
			return 6;
		case 'H':
			return 7;
		}
	return 0; // if letter not possible
}

void update_board(struct state *change)
{

	char *move = change->action;
	char *prev, *new, dash;
	dash = '-';
	
	char board[BOARD_SIZE][BOARD_SIZE];
	memcpy(board, change->board, sizeof(change->board));

	prev = malloc(sizeof(char)*8);

	// location of initial square
	int j = translate_letter(*(move + 0)); // letter is column
	int i = (int)*(move + 1) - 8; // number is row.

	// adjust board
	*prev = board[i][j];
	board[i][j] = 'O';

	if (*(move +2) != '-') {

	} else {
		// location of square to move to
		int p = translate_letter(*(move + 3)); // letter is column
		int q = (int)*(move + 4) - 8; // number is row.

		// TODO :check if move is valid

		// change middle square being hopped over
		if ( i == q) {
			if (j > p) {
				change->board[i][j-1] = 'O';
			} else if (j < p) {
				change->board[i][j+1] = 'O';
			}
		}
		if ( j == p) {
			if (i > q) {
				change->board[i-1][j] = 'O';
			} else if (i < q) {
				change->board[i+1][j] = 'O';
			}
		}
		*new = change->board[q][p];
		change->board[q][p] = *prev;
	}

	free(prev);

}

/**
 * get_moves() - returns a list of avaiable moves from given state
 * @current - current state
 * @count - amount of moves
*/
struct state *get_moves (struct state *current, int *count)
{
	//TODO

	struct state **states;
	int tmpcount = 0;
	int empty = count_o (current);

	if (current == NULL) {
		fprintf(stderr, "ERROR: No board to get moves from.\n");
		exit(1);
	}

	// initial states are  exceptions
	if ((current->action == NULL) && (player == 'B')) {
		states = malloc(sizeof(struct state)*2);
		if (states == NULL) {
			fprintf(stderr, "ERROR: Memory allocation failure.\n");
			exit(1);
		}

		struct state *state1 = create_state(current, 'D4');
		update_board(state1);
		tmpcount++;
		struct state *state2 = create_state(current, 'E5');
		update_board(state2);
	//	*(states + 0) = state1;
	//	*(states + 1) = state2;

	} else if (empty == 1) {
		// we must be white
		states = malloc(sizeof(struct state)*2);
		if (states == NULL) {
			fprintf(stderr, "ERROR: Memory allocation failure.\n");
			exit(1);
		}

		struct state *state1 = create_state(current, 'E4');
		update_board(state1);
		tmpcount++;
		
		struct state *state2 = create_state(current, 'D5');
		update_board(state2);
	//	*(states + 0) = state1;
	//	*(states + 1) = state2;
	}

	return *(states + 0);
}



static void free_board_state(struct state *state)
{
	free(state);
}

static char *decide_move()
{
	// TODO
	char *move;
	move = "cat";

	return move;
}
//---------------------------------------------------------
// State tree creation thread
//---------------------------------------------------------

static void _free_board_state(void *state)
{
	free_board_state(state);
}

static void _tree_node_eval(struct tree_node *node)
{
	if (tree_change)
		return;

	if (node->child_count == 0) {
		int count = 0;
		struct state *states = get_moves(node->value, &count);
		for (int i = 0; i < count; i++) {
		//	add_tree_child(node, states[i]);
		}
	}
}
/*
static void *make_tree()
{
	// TODO: Exit this thread cleanly.
	while (is_running) {
		struct tree_node *current = tree_root;
		breadth_traverse(current, -1, _tree_node_eval);
		if (current != tree_root) {
			recursive_free_tree(current, _free_board_state, tree_root);
		}
		tree_change = true;
	}
	pthread_exit(NULL);
}*/

//---------------------------------------------------------

//---------------------------------------------------------
// Game handle thread
//---------------------------------------------------------

static void *game_handler()
{
	while (is_running) {
		if (player == turn) {
			// IMPORTANT New line at end of move?
			char *move = decide_move();
			fprintf(stdout, "%s", move);
			//update_state(move);
		} else {
			char move[6];
			fscanf(stdin, "%5s", move);
			//update_state(move);
		}
	}
	pthread_exit(NULL);
}


//---------------------------------------------------------


void setup_game(int argc, char *argv[]) {
	// "board.txt", "B"
	// first arg is name of .txt file
	// second arg is B or W
	if (argc != 3) {
		fprintf(stderr, "ERROR: Invalid number of arguments provided\n");
		print_usage(argv[0]);
		exit(1);
	}

	char *filename = argv[1];

	FILE *fp;

	fp = fopen(filename, "r");

	// error check.
	if (fp == NULL) {
		fprintf(stderr, "ERROR: Failed to open file %s", argv[0]);
		exit(1);
	}

	player = *argv[2];

	struct state *initial_state = create_state(NULL, NULL);

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			// don't check if null character because board should be right
			initial_state->board[i][j] = (char)fgetc(fp);
		}
		// get rid of new character line at end of line.
		fgetc(fp);
		fgetc(fp);
	}
	// TODO REMOVE

	int *count = 0;
	print_board(initial_state);
	struct state *state;
	state = malloc(sizeof(struct state));
	state = create_state(initial_state, NULL);

	print_board(state);

	while (count == 0) {
		if (player == turn) {
			// IMPORTANT New line at end of move?
			//THIS IS THE PROBLEM HERE
			state = get_moves(initial_state, count);

			//move = state->action;
			print_board(state);
			fprintf(stdout, "%s", state->action);
			//update_state(move);
			*count = 1;
		} else {
			char move[6];
			fscanf(stdin, "%5s", move);
		//	update_state(move);
		}
	}

//	tree_root = init_tree(initial_state);
}

int main(int argc, char* argv[]) {
	setup_game(argc, argv);

//	pthread_t handler;
	//pthread_t tree_creation;
/*
	if (pthread_create(&handler, NULL, game_handler, NULL) != 0)
		fprintf(stderr, "ERROR: Failed to create game handler thread.\n");
	if (pthread_create(&tree_creation, NULL, make_tree, NULL) != 0)
		fprintf(stderr, "ERROR: Failed to create game handler thread.\n");
*/
//	pthread_exit(NULL);

	return 0;
}