#include "game.c"

// モンテカルロ木のノード
typedef struct Node
{
    // ID
    int id;

    // parentID
    int parentID;

    // 前のノード
    struct Node *parent;

    // 次のノード数
    int childCount;

    // 次のノード
    struct Node **child;

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