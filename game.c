#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "board.c"

int randBetween(int max, int min)
{
    struct timeval t1;
    mingw_gettimeofday(&t1, NULL);
    srand(t1.tv_usec * t1.tv_sec);
    return (rand()%(max - min + 1)) + min;
}

unsigned long long randULL(unsigned long long max, unsigned long long min)
{
    struct timeval t1;
    mingw_gettimeofday(&t1, NULL);
    srand(t1.tv_usec * t1.tv_sec);
    return (((unsigned long long)rand() * (unsigned long long)rand()) % (max - min + 1)) + (unsigned long long)min;
}

void game()
{
    Condition condition = initCondition();
    Move *pointableHands;
    int fc;
    Turn winner;

    for (int i = 1; i <= 400; i++)
    {
        if (i != 1)
        {
            condition.turn = !condition.turn;
            condition.turnNumber = i;
        }
        fc = serchPointableHands(condition, &pointableHands);
        // printf("fc%d\n", fc);
        // for (int i = 0; i < fc; i++)
        // {
        //     Address address = pointableHands[i].address;
        //     APiece piece = pointableHands[i].piece;
        //     printf("[%d]row:%d,column:%d,name:%s\n", i, address.row, address.column, pieceToString(piece.piece));
        // }
        int x = randBetween(fc - 1, 0);
        executeMove(&condition, pointableHands[x]);
        // printf("turn%d\n", condition.turnNumber);
        // displayCondition(condition);
        if (isEnd(condition, &winner))
        {
            if (winner)
            {
                printf("first won\n");
            }
            else
            {
                printf("second won\n");
            }
            return;
        }
    }
    printf("dwaw\n");
}