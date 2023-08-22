#include "util.h"

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
    unsigned long long result;
    while (1)
    {
        struct timeval t1;
        mingw_gettimeofday(&t1, NULL);
        srand(t1.tv_usec * t1.tv_sec);
        result = (((unsigned long long)rand() * (unsigned long long)rand() * (unsigned long long)rand() * (unsigned long long)rand()) % (max - min + 1)) + (unsigned long long)min;
        if (result != 1ULL) break;
    }
    return result;
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

void generateUUID(char *uuid)
{
    unsigned long long n1 = randULL(UUID_LIMITED, 1);
    unsigned long long dvb = UUID_CHAR_KIND;
    unsigned long long dv = UUID_LIMITED / UUID_CHAR_KIND;
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
            n1 -= (unsigned long long)quotient * dv;
            dv = dv / dvb;
            uuid[i] = convertChar(quotient);
        }
    }
    uuid[43] = 0;
}

void copyUuid(char **copy, char **copied)
{
    for (int i = 0; i < 43; i++)
    {
        (*copied)[i] = (*copy)[i];
    }
}