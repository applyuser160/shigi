#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <tchar.h>
#include <math.h>
#include "mysql.c"

#define NUMBER_OF_SEARCH 100
#define NUMBER_OF_CHARACTERS_FOR_A_NODE 235

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
    randBetween(61, 0);
    char c = (char)randBetween(61, 0);
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

void generateUUID2(char **uuid)
{
    *uuid = (char*)calloc(43, sizeof(char));
    u_int64 n1 = randULL(ULLONG_MAX - 1, 0);
    u_int64 dvb = 62;
    u_int64 dv = dvb;
    for (int i = 0; i < 10; i++)
    {
        uuid[i] = convertChar(n1 % dv);
        dv = dv * dv;
    }
}

void initNode(Node **node)
{
    Node *one = (struct Node *)calloc(1, sizeof(Node));
    generateUUID(&((*one).id));
    (*one).parentId = "";
    (*one).childCount = 0;
    (*one).turnNumber = 0;
    (*one).move.address.row = 0;
    (*one).move.address.column = 0;
    (*one).move.piece = generateAPiece(NON, 0, FIRST);
    (*one).throughCount = 0;
    (*one).drawCount = 0;
    (*one).fiWinCount = 0;
    (*one).seWinCount = 0;
    *node = one;
}

bool equalAddress(Address left, Address right)
{
    return left.row == right.row && left.column == right.column;
}

bool equalAPiece(APiece left, APiece right)
{
    return left.index == right.index && left.piece.name == right.piece.name;
}

bool equalMove(Move left, Move right)
{
    return equalAddress(left.address, right.address) && equalAPiece(left.piece, right.piece);
}

// ノードが既に作成済みの場合インデックスを返す
int isCreated(Node *child, Node *parent)
{
    Node *childlist;
    int childlistCount = selectWhereParentID(&childlist, (*parent).id);
    for (int i = 0; i < childlistCount; i++)
    {
        if (equalMove(childlist[i].move, (*child).move))
        {
            return i;
        }
    }
    return -1;
}

// ノードを現在のノードの下に作成する
// 次のノードのインデックスを返す
int deployNode(Node *child, Node *parent)
{
    int index = isCreated(child, parent);
    if (index == -1)
    {
        // childNodeのparentIdにparentNodeのidを設定
        (*child).parentId = (*parent).id;

        // 次のノード数をインクリメント
        (*parent).childCount++;

        return (*parent).childCount - 1;
    }
    else
    {
        free(child);
        return index;
    }
}

// UCBアルゴリズム
int ucb(Node *nextNodelist, int nextNodeCount, int t, Turn turn)
{
    int selected_arm = 0;
    double max_ucb = 0.0;

    for (int i = 0; i < nextNodeCount; i++)
    {
        if (nextNodelist[i].throughCount == 0)
        {
            selected_arm = i;
            break;
        }

        int winCount;
        switch (turn)
        {
        case FIRST:
            winCount = nextNodelist[i].fiWinCount;
            break;
        case SECOND:
            winCount = nextNodelist[i].seWinCount;
            break;
        }

        double avg_reward = ((double)winCount + (double)nextNodelist[i].drawCount * 0.5) / (double)nextNodelist[i].throughCount;

        double ucb_value = avg_reward + sqrt(2 * log(t) / nextNodelist[i].throughCount);

        if (ucb_value > max_ucb)
        {
            max_ucb = ucb_value;
            selected_arm = i;
        }
    }
    return selected_arm;
}

// 現在指すことができる手のノードを作成する
void createNodeFromPossiblePlace(Node **nextNodelist, int *nextNodeCount, Node node, Condition condition)
{
    // printf("createNode\n");
    Move *pointableHands;
    *nextNodeCount = serchPointableHands(condition, &pointableHands);
    // printf("nextNodeCount:%d\n", *nextNodeCount);

    *nextNodelist = (Node*)calloc(*nextNodeCount, sizeof(Node));
    // printf("calloc success\n");
    // printf("node.id:%s|\n", node.id);
    // printf("node:%p\n", &node);

    // データベースより子ノードの取得
    Node* child = (Node*)calloc(*nextNodeCount, sizeof(Node));
    int count = selectWhereParentID(&child, node.id);
    // printf("node.id:%s\n", node.id);
    // printf("count:%d\n", count);

    for (int i = 0; i < *nextNodeCount; i++)
    {
        char* id;
        bool hasCreated = false;
        for (int j = 0; j < count; j++)
        {
            if (equalMove((child[j]).move, pointableHands[i]))
            {
                // printf("hascreated\n");
                // printf("child[%d]:id%s,parentid%s\n", j, child[j].id, child[j].parentId);
                hasCreated = true;
                (*nextNodelist)[i] = child[j];
                break;
            }
        }

        if (!hasCreated)
        {
            // printf("not created\n");
            Node *newNode;
            initNode(&newNode);
            (*newNode).turnNumber = node.turnNumber + 1;
            (*newNode).move.address.row = pointableHands[i].address.row;
            (*newNode).move.address.column = pointableHands[i].address.column;
            (*newNode).move.piece = pointableHands[i].piece;
            (*newNode).parentId = node.id;
            (*nextNodelist)[i] = *newNode;
        }
        // printf("child[%d].id:%s\n", i, (*nextNodelist)[i].id);
    }
}
