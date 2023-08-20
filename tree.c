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

#define NUMBER_OF_SEARCH 1000
#define NUMBER_OF_CHARACTERS_FOR_A_NODE 235

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
    int childlistCount = selectWhereParentID((*parent).id, childlist);
    for (int i = 0; i < childlistCount; i++)
    {
        if (equalMove(childlist[i].move, (*child).move))
        {
            return i;
        }
    }
    return -1;
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
    Move *pointableHands;
    *nextNodeCount = serchPointableHands(condition, &pointableHands);
    // printf("nextNodeCount:%d\n", *nextNodeCount);

    *nextNodelist = (Node*)calloc(*nextNodeCount, sizeof(Node));

    // データベースより子ノードの取得
    Node* child = (Node*)calloc(*nextNodeCount, sizeof(Node));
    int count = selectWhereParentID(node.id, child);

    for (int i = 0; i < *nextNodeCount; i++)
    {
        char* id;
        bool hasCreated = false;
        for (int j = 0; j < count; j++)
        {
            if (equalMove((child[j]).move, pointableHands[i]))
            {
                hasCreated = true;
                (*nextNodelist)[i] = child[j];
                break;
            }
        }

        if (!hasCreated)
        {
            Node newNode;
            initNode(&newNode);
            newNode.turnNumber = node.turnNumber + 1;
            newNode.move.address.row = pointableHands[i].address.row;
            newNode.move.address.column = pointableHands[i].address.column;
            newNode.move.piece = pointableHands[i].piece;
            newNode.parentId = node.id;
            (*nextNodelist)[i] = newNode;
        }
    }
    free(child);
    // free(pointableHands);
}
