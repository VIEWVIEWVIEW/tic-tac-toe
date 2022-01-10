#include <cstdint>
#include <functional>
#include <iostream>
#include <menu.h>
#include <ncurses.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

const int X_OFFSET = 2; // center the board on the horizontal axis below the top text (Player %c's turn)
const int Y_OFFSET = 1; // we skip the first row, since it will indicate whose turn it is

struct menu_item
{
  // title of the menu item
  string title;
  // description of the menu item
  string description;

  // a player function takes a board[3][3] which contains chars 'x', 'o', ' '
  // and a second parameter, which is the symbol of the player ('x' or 'o')
  std::pair<int, int> (*player2)(char[3][3], char);
};

// increase a value for movement (since tictactoe is played on a 3x3 board, range 0-2, the max value is 2)
void increase(int *val)
{
  (*val)++;
  if (*val >= 2)
  {
    *val = 2;
  }
}

// decrease a value for movement (since tictactoe is played on a 3x3 board, range 0-2, the min value is 0)
void decrease(int *val)
{
  (*val)--;
  if (*val <= 0)
  {
    *val = 0;
  }
}

// counts how many fields of the board are in use
int used_fields(char board[3][3])
{
  int used_fields = 0;

  // iterate through rows and columns
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      // if field is  not ' ' => used
      if (board[i][j] != ' ')
        used_fields++;
    }
  }
  return used_fields;
}

// takes a board and returns the char 'x', 'o', 'd', or ' '.
// 'x' and 'o' return whether a winner has won.
// 'd' is returned if the game ends in a draw
// ' ' is returned if the game is still undecided
char check_winner(char board[3][3])
{
  // check top row
  if (board[0][0] == board[0][1] && board[0][1] == board[0][2])
  {
    return board[0][0];
  }

  // check second row
  if (board[1][0] == board[1][1] && board[1][1] == board[1][2])
  {
    return board[1][0];
  }

  // check third row
  if (board[2][0] == board[2][1] && board[2][1] == board[2][2])
  {
    return board[2][0];
  }

  // check first column
  if (board[0][0] == board[1][0] && board[1][0] == board[2][0])
  {
    return board[0][0];
  }

  // check second column
  if (board[0][1] == board[1][1] && board[1][1] == board[2][1])
  {
    return board[0][1];
  }

  // check third column
  if (board[0][2] == board[1][2] && board[1][2] == board[2][2])
  {
    return board[0][2];
  }

  // check top left to bottom right
  if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
  {
    return board[0][0];
  }

  // check top right to bottom left
  if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
  {
    return board[0][2];
  }

  // if we used all fields, it's a draw!
  if (used_fields(board) > 8)
    return 'd';

  return ' ';
}

// a player takes the board and returns a tuple of the move
std::pair<int, int> human(char board[3][3], char current_player)
{
  // make cursor highly visible
  int x = 0;
  int y = 0;
  int c;

  // set cursor to top left field
  move(Y_OFFSET, X_OFFSET + 1);

  // loop till we hit enter
  while ((c = getch()) != 10)
  {
    // change cursor coordinate
    switch (c)
    {
    case KEY_UP:
      decrease(&y);
      break;
    case KEY_DOWN:
      increase(&y);
      break;
    case KEY_LEFT:
      decrease(&x);
      break;
    case KEY_RIGHT:
      increase(&x);
      break;
    }

    // debug
    // mvprintw(10, 0, "x: %d y: %d ", x, y);

    // move cursor to new position
    move(y * 2 + Y_OFFSET, x * 4 + X_OFFSET + 1);
  }

  return {x, y};
}

std::pair<int, int> computer(char board[3][3], char current_player)
{
  // find our who is our opponent
  char opponent = 'x';
  if (current_player == 'x')
  {
    opponent = 'o';
  }

  // generate a random move, if we are neither attacking nor defending
  std::pair<int, int> move = {rand() % 3, rand() % 3};
 
  // check all fields if the enemy would win
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      // check if field is empty
      if (board[i][j] == ' ')
      {
        // --- defense (checks if the opponent can win) ---
        // make the move with the *opponent*
        board[i][j] = opponent;

        // evaluate board
        char winner = check_winner(board);

        // if opponent would win that's bad! make our move there to prevent them from winning
        if (winner == opponent) {
          move.first = j;
          move.second = i;
        }

        // undo the move
        board[i][j] = ' ';

        // --- attack (checks if the current_player can win) ---
        // make the move
        board[i][j] = current_player;

        // evaluate board
        winner = check_winner(board);

        // if current_player would win, that's good! make our move and gg!
        if (winner == current_player) {
          move.first = j;
          move.second = i;

          // cleanup since our game logic requires to enter an empty field. we wouldn't reach the cleanup below, hence we do it here.
          board[i][j] = ' ';

          // we don't care about the rest and take the win home. this overrides any possible defense moves.
          return move;
        }
        // undo move
        board[i][j] = ' ';
      }
    }
  }

  // debug
  // mvprintw(8, 0, "Move: %d, %d", move.first, move.second);

  return move;
}

void print_board(char board[3][3])
{
  /*
  * row
  * 0   1   1
  *             col
  * 1 | 2 | 3   0
  *---+---+---
  * 4 | 5 | 6   1
  *---+---+---
  * 7 | 8 | 9   2
  * 
  */
  int y = Y_OFFSET;
  int x = X_OFFSET;

  // 1st row
  mvprintw(y, x, " %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);

  // 2nd row
  mvprintw(y + 1, x, "---+---+---\n");

  // 3rd row
  mvprintw(y + 2, x, " %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);

  // 4th row
  mvprintw(y + 3, x, "---+---+---\n");

  // 5th row
  mvprintw(y + 4, x, " %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);

  refresh();
}

// tictactoe takes two players. a player is a function that takes a board and returns a tuple of (x, y)
void tictactoe(std::pair<int, int> (*player1)(char[3][3], char), std::pair<int, int> (*player2)(char[3][3], char))
{
  // create game board
  char board[3][3] = {
      {' ', ' ', ' '},
      {' ', ' ', ' '},
      {' ', ' ', ' '}};

  char current_player = 'x'; // player 1 = x, player 2 = o

  char winner = ' ';

  mvprintw(0, 0, "Player %c's turn\n", current_player);
  print_board(board);

  // we play the game while we have no winner
  while (winner == ' ')
  {
    // player 1
    if (current_player == 'x')
    {
      bool valid_move = false;

      // loop till we get a valid input
      while (!valid_move)
      {
        // get the current move from player
        std::pair<int, int> move = player1(board, current_player);

        // if the field is unoccupied we update the board and end the loop
        if (board[move.second][move.first] == ' ')
        {
          board[move.second][move.first] = 'x';
          valid_move = true;
        }
      }

      // change to player 2
      current_player = 'o';
    }
    // player 2
    else
    {
      bool valid_move = false;

      // loop till we get a valid input
      while (!valid_move)
      {
        // get the current move from player
        std::pair<int, int> move = player2(board, current_player);

        // if the field is unoccupied we update the board and end the loop
        if (board[move.second][move.first] == ' ')
        {
          board[move.second][move.first] = 'o';
          valid_move = true;
        }
      }

      current_player = 'x';
    }

    // Update whose turn it is
    mvprintw(0, 0, "Player %c's turn\n", current_player);
    // Print updated board
    print_board(board);

    // check winner
    winner = check_winner(board);
  }

  // announce winner / draw
  if (winner != 'd')
  {
    mvprintw(10, 0, "Winner is: %c.\nPress any key to go back to the main menu.", winner);
  }
  else
  {
    mvprintw(10, 0, "Draw!\nPress any key to go back to the main menu.");
  }

  // wait for key input, clear screen and exit game function
  getch();
  erase();
}

// new main function
int main()
{
  // create vector for menu items
  vector<menu_item> menu_items;

  // -- custom player algorithms --
  // declare menu item: "Title", "Description", function_of_player
  struct menu_item human_item = {"Multiplayer", "Play against another human!", human};
  menu_items.push_back(human_item);

  struct menu_item algo_item = {"Singleplayer", "Play against a computer!", computer};
  menu_items.push_back(algo_item);

  // -- Do not touch below ---

  // initialize ncurses
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  // create vector which holds the items for the menu
  vector<ITEM *> items;

  // create items from our definitions on the top
  for (int i = 0; i <= menu_items.size(); i++)
  {
    // create item
    ITEM *item = new_item(menu_items[i].title.c_str(), menu_items[i].description.c_str());

    // set player as userpointer. We need this ugly cast since ncurses is like C99 garbage
    set_item_userptr(item, reinterpret_cast<void *>(menu_items[i].player2));

    // add item to vector
    items.push_back(item);
  }

  // create menu with the item data
  MENU *menu = new_menu(items.data());
  post_menu(menu);

  // add exit key at the bottom of the screen
  mvprintw(LINES - 1, 0, "q to Exit");

  // refresh screen
  refresh();

  // event loop for the menu
  int c;
  // get key input. If q is pressed, exit.
  while ((c = getch()) != 'q')
  {
    switch (c)
    {
    case KEY_DOWN:
      menu_driver(menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(menu, REQ_UP_ITEM);
      break;
    case 10:
      // and execute the game with the selected enemy type
      ITEM *cur_item = current_item(menu);

      // get player2 from userptr attribute of the current item
      void *player2 = item_userptr(cur_item);

      // unpost menu
      unpost_menu(menu);

      // cast void pointer to function pointer with its original type & execute game.
      // player 1 is always supposed to be a human. (since we aren't training any kind of AI;
      // in that case it would make sense to let a computer play against a computer)
      tictactoe(human, reinterpret_cast<std::pair<int, int> (*)(char[3][3], char)>(player2));

      // draw menu again after game finished
      post_menu(menu);

      // add exit key at the bottom of the screen
      mvprintw(LINES - 1, 0, "q to Exit");

      break;
    }
  }
}
