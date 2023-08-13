#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "board.c"

int main(void)
{
    Condition condition = initCondition();
    displayCondition(condition);
    Move *pointableHands;
    int fc = serchPointableHands(condition, &pointableHands);
    printf("fc:%d\n", fc);
    for (int i = 0; i < fc; i++)
    {
        Address address = pointableHands[i].address;
        APiece piece = pointableHands[i].piece;
        printf("[%d]row:%d,column:%d,name:%s\n", i, address.row, address.column, pieceToString(piece.piece));
    }
    
    return 0;
}