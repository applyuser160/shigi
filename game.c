#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <tchar.h>
#include <math.h>
#include "mysql-8.1.0-winx64/include/mysql.h"
#include "board.c"

#define UUID_CHAR_KIND 62
#define UUID_LIMITED 839299365868340224ULL

int randBetween(int max, int min)
{
    struct timeval t1;
    usleep(1);
    mingw_gettimeofday(&t1, NULL);
    srand(t1.tv_usec * t1.tv_sec);
    return (rand()%(max - min + 1)) + min;
}

unsigned long long randULL(unsigned long long max, unsigned long long min)
{
    struct timeval t1;
    mingw_gettimeofday(&t1, NULL);
    srand(t1.tv_usec * t1.tv_sec);
    return (((unsigned long long)rand() * (unsigned long long)rand() * (unsigned long long)rand() * (unsigned long long)rand()) % (max - min + 1)) + (unsigned long long)min;
}

char convertChar(char number)
{
    if (number > 35)
    {
        number += 61;
    }
    else if (number > 9)
    {
        number += 55;
    }
    else
    {
        number += 48;
    }
    return number;
}

char randChar()
{
    randBetween(UUID_CHAR_KIND - 1, 0);
    char c = (char)randBetween(UUID_CHAR_KIND - 1, 0);
    return convertChar(c);
}

void generateUUID(char **uuid)
{
    *uuid = (char*)calloc(43, sizeof(char));
    for (int i = 0; i < 37; i++)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23 || i == 36)
        {
            (*uuid)[i] = 45;
        }
        else
        {
            (*uuid)[i] = randChar();
        }
    }
    struct timeval t1;
    mingw_gettimeofday(&t1, NULL);
    char* timestr = (char*)calloc(6, sizeof(char));
    sprintf(timestr, "%ld", t1.tv_usec);
    strcat(*uuid, timestr);
}

void generateUUID2(char *uuid)
{
    u_int64 n1 = randULL(UUID_LIMITED, 1);
    u_int64 dvb = UUID_CHAR_KIND;
    u_int64 dv = UUID_LIMITED / UUID_CHAR_KIND;
    for (int i = 0; i < 43; i++)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23 || i== 36)
        {
            uuid[i] = 45;
        }
        else if (i == 14)
        {
            uuid[i] = 52;
        }
        else
        {
            if (i == 10 || i == 24 || i == 33)
            {
                n1 = randULL(UUID_LIMITED, 1);
                dv = UUID_LIMITED / UUID_CHAR_KIND;
            }
            char quotient = n1 / dv;
            n1 -= (u_int64)quotient * dv;
            dv = dv / dvb;
            uuid[i] = convertChar(quotient);
        }
    }
    uuid[43] = 0;
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
        int x = randBetween(fc - 1, 0);
        executeMove(&condition, pointableHands[x]);
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