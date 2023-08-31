#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  game_state_t* defstate = malloc(sizeof(game_state_t));
  defstate->num_rows = 18;
  defstate->num_snakes = 1;
  defstate->snakes = (snake_t*)malloc((defstate->num_snakes)*sizeof(snake_t));
  snake_t* defsnake = malloc(sizeof(snake_t));


  defsnake->tail_row = 2;
  defsnake->tail_col = 2;
  defsnake->head_row = 2;
  defsnake->head_col = 4;
  defsnake->live = 1;
  defstate->snakes[0] = *defsnake;
  free(defsnake);
  defstate->board = (char**)malloc((defstate->num_rows)*sizeof(char*));
  
  char basicrow[21] = "#                  #\0";
  for (int i = 0; i < (defstate->num_rows); i++) { 
      defstate->board[i] = (char*)malloc(21*sizeof(char));
      if (i != 2) {
          strcpy(defstate->board[i], basicrow);
      }
  }
  
  strcpy(defstate->board[2] , "# d>D    *         #\0");
  strcpy(defstate->board[0],"####################\0");
  strcpy(defstate->board[17],"####################\0");
  return defstate;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
//  for (int i = 0; i < state->num_snakes; i++) {
  //    free(state->snakes[i]);
 // }
  free(state->snakes);

  for (int i = 0; i < state-> num_rows; i++) {
      free(state->board[i]);
  }
  free(state->board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++) {
      fprintf(fp, "%s\n" ,state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
      return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x') {
      return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if (is_head(c) || is_tail(c) || c == '<' || c == '^' || c == '>' || c == 'v') {
      return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^') {  
      return 'w';
  } 
  if (c == '>') {
      return 'd';
  }
  if (c == '<') {
      return 'a';
  }
  if (c == 'v') {
      return 's';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W') {
      return '^';
  }  
  if (c == 'A') {
      return '<';
  }
  if (c == 'S') {
      return 'v';
  }
  if (c == 'D') {
      return '>';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 's' || c == 'v' || c == 'S') {
      return (cur_row + 1);
  }
  if (c == '^' || c == 'w' || c == 'W') {
      return (cur_row - 1);
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == 'a' || c == '<' || c == 'A') {
      return (cur_col - 1);
  }
  if (c == 'd' || c == '>' || c == 'D') {
      return (cur_col + 1);
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t sn = state->snakes[snum];
  snake_t* s = &sn;
  int col = get_next_col(s->head_col, get_board_at(state, s->head_row, s->head_col));
  int row = get_next_row(s->head_row, get_board_at(state, s->head_row, s->head_col));


  return get_board_at(state, row, col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.

  snake_t* s = &(state->snakes[snum]);
  int row = get_next_row(s->head_row, get_board_at(state, s->head_row, s->head_col));
  int col = get_next_col(s->head_col, get_board_at(state, s->head_row, s->head_col));
  char head = get_board_at(state, s->head_row, s->head_col);
  set_board_at(state, row, col, head);
  set_board_at(state, s->head_row, s->head_col, head_to_body(head));
  s->head_row = row;
  s->head_col = col;
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* s = &(state->snakes[snum]);
  int row = get_next_row(s->tail_row, get_board_at(state, s->tail_row, s->tail_col));
  int col = get_next_col(s->tail_col, get_board_at(state, s->tail_row, s->tail_col));
  set_board_at(state, row, col, body_to_tail(get_board_at(state, row,col))); 
  set_board_at(state, s->tail_row, s->tail_col, ' ');
  s->tail_row = row;
  s->tail_col = col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_snakes; i++) {
      snake_t* s = &(state->snakes[i]);
      char next = next_square(state, i);
      if (next == '#' || is_snake(next)) {
          s->live = 0;
          set_board_at(state, s->head_row, s->head_col, 'x');
      } else {
          if (next == '*') { 
              update_head(state, i);
              add_food(state);
          } else {
              update_head(state, i);
              update_tail(state, i);
          }
      }

  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
      return NULL;
  }
  game_state_t* state = malloc(sizeof(game_state_t));
  size_t char_cap = 50;
  size_t line_cap = 50;
  char** lines = malloc(sizeof(char*)*line_cap);
  size_t num_lines = 0;
  size_t num_chars = 0;
  char c;
  while(1){
      c = fgetc(file);
      if (ferror(file)) {
          return 1;
      }
      if (feof(file)) {
          if (num_chars!=0) {
              lines[num_lines] = realloc(lines[num_lines], num_chars);
              lines[num_lines][num_chars-1] = '\0';
              num_lines++;
          }
          break;

      }
      if (num_chars==0) {
          lines[num_lines] = malloc(char_cap);
      }
      lines[num_lines][num_chars] = c;
      num_chars++;
      if (num_chars % char_cap == 0) {
          char_cap = 2*char_cap;
          lines[num_lines] = realloc(lines[num_lines], sizeof(char)*char_cap);
      }
      if (c == '\n') {
          lines[num_lines] = realloc(lines[num_lines], num_chars);
          lines[num_lines][num_chars-1] = '\0';
          num_lines++;
          num_chars = 0;

          if (num_lines % line_cap == 0) {
              line_cap = 2*line_cap;
              lines = realloc(lines, sizeof(char*)*line_cap);

          }
      }
          
      
  }
  fclose(file);
  lines = realloc(lines, sizeof(char*)*num_lines);
  state->board = lines;
  state->num_rows = num_lines;
  state->snakes = NULL;
  state->num_snakes = 0;

  return state;
    
}


/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* s = &(state->snakes[snum]);
  char c = get_board_at(state, s->tail_row, s->tail_col);
  unsigned int nrow = s->tail_row;
  unsigned int ncol = s->tail_col;
  while (c!='W' && c!='A' && c!='S' && c!='D') {
      nrow = get_next_row(nrow, c);
      ncol = get_next_col(ncol, c);
      c = get_board_at(state, nrow, ncol);
  }
  s->head_row = nrow;
  s->head_col = ncol;
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  state->snakes = (snake_t*)malloc(sizeof(snake_t));
  for (int i = 0; i < state->num_rows; i++) {
      for (int j = 0; j < strlen(state->board[i]); j++) {
          if (is_tail(state->board[i][j])) {
              state->num_snakes++;
            
              state->snakes = realloc(state->snakes, (sizeof(snake_t)*(state->num_snakes)));
              
              snake_t* snake = malloc(sizeof(snake_t));
              snake->tail_row = i;
              snake->tail_col = j;
              state->snakes[(state->num_snakes)-1] = *snake;
              find_head(state, (state->num_snakes)-1);
              if (get_board_at(state,state->snakes[(state->num_snakes)-1].head_row,state->snakes[(state->num_snakes)-1].head_col) == 'x') {
                  state->snakes[(state->num_snakes)-1].live = false;
              } else {
                  state->snakes[(state->num_snakes)-1].live = true;
              }

          }

      }
  }
  return state;
}
