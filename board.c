#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "piece.c"

#define NUMBER_OF_EDGE 9
#define NUMBER_OF_SQUARES 81

typedef enum SquareStatus
{
    NOT_USED = false,
    USED = true,
}SquareStatus;

typedef struct Address
{
    int row;
    int column;
}Address;

typedef struct Square
{
    Address address;
    SquareStatus isUsed;
    APiece piece;
}Square;

typedef struct Board
{
    Square squares[NUMBER_OF_SQUARES];
}Board;

typedef struct Condition
{
    Board board;
    Turn turn;
    int turnNumber;
}Condition;

typedef struct Move
{
    Address address;
    APiece piece;
}Move;

void deployAPiece(Board *board, APiece piece, int row, int column)
{
    (*board).squares[9 * (row - 1) + (column - 1)].isUsed = USED;
    (*board).squares[9 * (row - 1) + (column - 1)].piece = piece;
}

void initBoard(Board *board)
{
    for (int i = 0; i < NUMBER_OF_SQUARES; i++)
    {
        (*board).squares[i].address.row = i / 9 + 1;
        (*board).squares[i].address.column = i % 9 + 1;
        (*board).squares[i].isUsed = NOT_USED;
    }
    deployAPiece(board, generateAPiece(KING, 0, FIRST), 9, 5);
    deployAPiece(board, generateAPiece(ROOK, 0, FIRST), 8, 2);
    deployAPiece(board, generateAPiece(BICHOP, 0, FIRST), 8, 8);
    deployAPiece(board, generateAPiece(GOLDGENERAL, 0, FIRST), 9, 4);
    deployAPiece(board, generateAPiece(GOLDGENERAL, 1, FIRST), 9, 6);
    deployAPiece(board, generateAPiece(SILVERGENERAL, 0, FIRST), 9, 3);
    deployAPiece(board, generateAPiece(SILVERGENERAL, 1, FIRST), 9, 7);
    deployAPiece(board, generateAPiece(KNIGHT, 0, FIRST), 9, 2);
    deployAPiece(board, generateAPiece(KNIGHT, 1, FIRST), 9, 8);
    deployAPiece(board, generateAPiece(LANCE, 0, FIRST), 9, 1);
    deployAPiece(board, generateAPiece(LANCE, 1, FIRST), 9, 9);
    deployAPiece(board, generateAPiece(PAWN, 0, FIRST), 7, 1);
    deployAPiece(board, generateAPiece(PAWN, 1, FIRST), 7, 2);
    deployAPiece(board, generateAPiece(PAWN, 2, FIRST), 7, 3);
    deployAPiece(board, generateAPiece(PAWN, 3, FIRST), 7, 4);
    deployAPiece(board, generateAPiece(PAWN, 4, FIRST), 7, 5);
    deployAPiece(board, generateAPiece(PAWN, 5, FIRST), 7, 6);
    deployAPiece(board, generateAPiece(PAWN, 6, FIRST), 7, 7);
    deployAPiece(board, generateAPiece(PAWN, 7, FIRST), 7, 8);
    deployAPiece(board, generateAPiece(PAWN, 8, FIRST), 7, 9);

    deployAPiece(board, generateAPiece(KING, 0, SECOND), 1, 5);
    deployAPiece(board, generateAPiece(ROOK, 0, SECOND), 2, 8);
    deployAPiece(board, generateAPiece(BICHOP, 0, SECOND), 2, 2);
    deployAPiece(board, generateAPiece(GOLDGENERAL, 0, SECOND), 1, 4);
    deployAPiece(board, generateAPiece(GOLDGENERAL, 1, SECOND), 1, 6);
    deployAPiece(board, generateAPiece(SILVERGENERAL, 0, SECOND), 1, 3);
    deployAPiece(board, generateAPiece(SILVERGENERAL, 1, SECOND), 1, 7);
    deployAPiece(board, generateAPiece(KNIGHT, 0, SECOND), 1, 2);
    deployAPiece(board, generateAPiece(KNIGHT, 1, SECOND), 1, 8);
    deployAPiece(board, generateAPiece(LANCE, 0, SECOND), 1, 1);
    deployAPiece(board, generateAPiece(LANCE, 1, SECOND), 1, 9);
    deployAPiece(board, generateAPiece(PAWN, 0, SECOND), 3, 1);
    deployAPiece(board, generateAPiece(PAWN, 1, SECOND), 3, 2);
    deployAPiece(board, generateAPiece(PAWN, 2, SECOND), 3, 3);
    deployAPiece(board, generateAPiece(PAWN, 3, SECOND), 3, 4);
    deployAPiece(board, generateAPiece(PAWN, 4, SECOND), 3, 5);
    deployAPiece(board, generateAPiece(PAWN, 5, SECOND), 3, 6);
    deployAPiece(board, generateAPiece(PAWN, 6, SECOND), 3, 7);
    deployAPiece(board, generateAPiece(PAWN, 7, SECOND), 3, 8);
    deployAPiece(board, generateAPiece(PAWN, 8, SECOND), 3, 9);
}

void displayBoard(Board board)
{
    for (int i = 0; i < NUMBER_OF_SQUARES; i++)
    {

    }
}