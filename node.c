#include "game.c"

// モンテカルロ木のノード
typedef struct Node
{
    // ID
    char *id;

    // parentID
    char *parentId;

    // 次のノード数
    int childCount;

    // ターン数
    int turnNumber;

    // 指し手
    Move move;

    // 通過数
    int throughCount;

    // 引分数
    int drawCount;

    // 勝利数 FIRST
    int fiWinCount;

    // 勝利数 SECOND
    int seWinCount;
} Node;

void initNode(Node *node)
{
    (*node).id = (char*)calloc(43, sizeof(char));
    generateUUID2((*node).id);
    (*node).parentId = (char*)calloc(43, sizeof(char));
    (*node).childCount = 0;
    (*node).turnNumber = 0;
    (*node).move.address.row = 0;
    (*node).move.address.column = 0;
    (*node).move.piece = generateAPiece(NON, 0, FIRST);
    (*node).throughCount = 0;
    (*node).drawCount = 0;
    (*node).fiWinCount = 0;
    (*node).seWinCount = 0;
}