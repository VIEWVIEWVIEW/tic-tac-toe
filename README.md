# Tic Tac Toe

A very simple tic tac toe implementation, which allows you to play against a human or computer.
Written with the [ncurses](https://invisible-island.net/ncurses/) library.

## How to build
#### Install dependencies
```
sudo apt-get install libncurses-dev build-essential
```

#### Compile
```
gcc main.cpp -lmenu -lncurses -lstdc++ -o main
```

#### Execute
```
./main
```

## How to play
It's simple! Just use the arrow keys on your keyboard to navigate the menu. Press enter to select an enemy (computer or human) and use the same arrow keys to scroll through the game board. Then hit enter to place your symbol at the cursor's location.


## See it in Action
[![asciicast](https://asciinema.org/a/Ud8tnuEhP8y95tF5kpTEfLZ55.png)](https://asciinema.org/a/Ud8tnuEhP8y95tF5kpTEfLZ55)


## Modifying
You can easily add your own AI to play against (called "player2"-function).
The signature of your player2 function should look like this:

```c++
std::pair<int, int> player(char board[3][3], char current_player)
```


The function takes in a two dimensional array (3x3 board of tic tac toe), where the following states exist for each field:
```
' ' // an unoccupied field
'x' // field which is occupied by player1
'o' // field which is occupied by player2
```

The board itself looks like this:
```
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
```

In addition to the game board, the player2-function must also take a `char current_player` which indicates your current player symbol, whether you're `x` or `o`.

Your player2 function should return a pair with the row and column where you want to place your symbol.

### Example
Here is a simple player2 function, which always returns a random field.
```c++
std::pair<int, int> random(char board[3][3], char current_player)
{
  std::pair<int, int> move = {rand() % 3, rand() % 3};
  return move;
}
```

For your convenience you don't have to check whether the field is unoccupied or not. The game will do that for you. Just return where you think a placement would make sense (even if it doesn't!).

After you wrote your own player2 function, you need to create a new menu item.
To do this, you just need define it at the very beginning of the ``main`` function, and push it to the `menu_items`-vector:
```c++
  // main function
  struct menu_item random_player_item = {"Random Player", "Play against a player who chooses their moves completely random!", random};
  menu_items.push_back(random_player_item);
```

The item takes a title, a description and a reference to your player2-function (in this case called `random`).


Since you get a reference to the original game board, you can even create a cheating player2.
```c++
std::pair<int, int> cheater(char board[3][3], char current_player)
{
  // insta win
  board[0][0] = current_player;
  board[0][1] = current_player;
  board[0][2] = current_player;
  return {rand() % 3, rand() % 3};
}
```

