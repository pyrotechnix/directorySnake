#include <iostream>
#include <ncurses.h>
#include <vector>
#include <chrono>
#include <ctime>
#include <thread>
#include <cstdlib>
#include <tuple>


using namespace std;



vector<vector<int>> initBoard(int height, int width)
{
	vector<vector<int>> board(height, vector<int>(width, 0));
	return board;
}



WINDOW  *createWindowCentered(int height, int width)
{
	width *= 2;
	height += 2;
	width += 3;
	//Adding 2 to width and height so that there are 
	//that many characters inside the window
	int top = LINES / 2 - height / 2;
	int left = COLS / 2- width / 2;

	WINDOW *local_win;
	local_win = newwin(height, width, top, left);	
	box(local_win, 0, 0);
	return local_win;
}

void drawWin(WINDOW *local_win, std::vector<std::vector<int>> board, int appleX, int appleY)
{
	const char* snakeBody = "O";
	const char* apple = "a";
	move(0, 0);
	for (int i = 0; i < board.size(); i++)
	{
		for(int j = 0; j < board[i].size(); j++)
		{
			if (board[i][j] > 0)
			{
				wmove(local_win, i + 1, j * 2 + 2);
				waddstr(local_win, snakeBody);
			}
			else if (i == appleY && j == appleX)
			{
				wmove(local_win, i + 1, j * 2 + 2);
				waddstr(local_win, apple);
			}
		       	else if (board[i][j] == 0)
			{
				wmove(local_win, i + 1, j * 2 + 2);
				waddstr(local_win, " "); 
			}

		}
	}
	wrefresh(local_win);
}

void makePlayer(vector<vector<int>> &board, int strength)
{
	board[0][0] = strength;
}


//bool to return false if player dies :(
bool updatePlayerPos(vector<vector<int>> &board, int dir, int &currX, int &currY, int strength)
{
	//Up
	if (dir == 0)
	{
		currY--;
	}
	//Right
	else if (dir == 1)
	{
		currX++;
	}
	//Down
	else if (dir == 2)
	{
		currY++;
	}
	//Left
	else if (dir == 3)
	{
		currX--;
	}
	if (currX < 0 || currX >= board[0].size() || currY < 0 || currY >= board.size() || board[currY][currX] != 0)
	{
		return false;
	}
	board[currY][currX] = strength;
	return true;
}


void updateBoard(vector<vector<int>> &board)
{
	for (int i = 0; i < board.size(); i++)
	{
		for(int j = 0; j < board[i].size(); j++)
		{
			if(board[i][j] > 0)
			{
				board[i][j] -= 1;
			}
		}
	}
}


bool checkApple(int x, int y, int appleX, int appleY, vector<vector<int>> &board)
{
	if (appleX == x && appleY == y)
	{
		for(int i = 0; i < board.size(); i++)
		{
			for(int j = 0; j < board[i].size(); j++)
			{
				if (board[i][j] != 0)
				{
					board[i][j] += 1;
				}
			}
		}
		return true;
	}
	return false;
}

std::tuple<int, int> generateNewApplePos(vector<vector<int>> &board)
{
	int appleX = rand() % board[0].size();
	int appleY = rand() % board.size();
	while (board[appleY][appleX] != 0)
	{
		appleX = rand() % board[0].size();
		appleY = rand() % board.size();
	}
	return std::make_tuple(appleX, appleY);
}

int main()
{
	srand(time(0));
	double frameTimer = 0;
	double lastFrame = 0;
	double frameLen = 0.2;
	const int height = 20;
	const int width = 20;
	int x = 0;
	int y = 0;
	int strength = 1;
	int dir = 2;
	std::vector<std::vector<int>> board = initBoard(height, width);
	auto [newX, newY] = generateNewApplePos(board);
	int appleX = newX;
	int appleY = newY;
	initscr();
	refresh();
	nodelay(stdscr, TRUE);
	WINDOW *gameWin;	
	gameWin = createWindowCentered(height, width);
	makePlayer(board, strength);
	int ch;
	while (true)
	{
		ch = getch();
		if (ch != ERR)
		{
			if (ch == 'q' or ch == 'Q')
			{
				break;
			}
			else if (ch == 'w')
			{
				//UP
				dir = 0;
			}
			else if (ch == 'd')
			{
				//RIGHT
				dir = 1;
			}
			else if (ch == 's')
			{
				//DOWN
				dir = 2;
			}
			else if (ch == 'a')
			{
				//LEFT
				dir = 3;
			}
		}
		updateBoard(board);
		if (!updatePlayerPos(board, dir, x, y, strength))
		{
			
			break;
		}			
		if (checkApple(x, y, appleX, appleY, board))
		{
			strength += 1;
			auto [newX, newY] = generateNewApplePos(board);
			appleX = newX;
			appleY = newY;
		}
		drawWin(gameWin, board, appleX, appleY);
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	delwin(gameWin);
	WINDOW *gameOverWin;
	gameOverWin = createWindowCentered(5, 19);
	wmove(gameOverWin, 3, 16);
	waddstr(gameOverWin, "YOU LOSE");
	wmove(gameOverWin, 4, 12);
	waddstr(gameOverWin, "press q to quit");
	wrefresh(gameOverWin);
	nodelay(stdscr, FALSE);
	getch();
	endwin();
}
