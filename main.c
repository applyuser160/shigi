#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "board.c"

int main(void)
{
    Board board = initBoard();
    displayBoard(board);
    return 0;
}