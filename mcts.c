#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "tree.c"

void learn(void)
{
    printf("start\n");
    // モンテカルロ木
    Node *tree;
    printf("start\n");

    // 既にトップノードが作成済みの場合
    char* topNodeId = "sZG6neL2-sZQ7-oeLC-tjQ7-yfVZG7xeVL2t-524566";
    printf("%s\n", topNodeId);
    selectWhereID(&tree, topNodeId);

    // // まだトップノードが作成されていない場合
    // initNode(&tree);
    // insertFromNode(*tree);
    // char* topNodeId = (*tree).id;
    printf("idp:%p\n", tree);
    // printf("id:%s\n", (*tree).id);

    

    for (int i = 0; i < NUMBER_OF_SEARCH; i++)
    {
        // 盤面
        Condition condition = initCondition();

        // displayCondition(condition);

        Node *currentNode;
        selectWhereID(&currentNode, topNodeId);

        // ターンナンバー
        int turnNumber = 0;

        // 手番
        Turn turn = FIRST;

        // 勝った方
        Turn winner;

        while (1)
        {

            // UCBにより、手を評価し、選択する
            createNodeFromPossiblePlace(currentNode, condition);
            // int selected = ucb(*currentNode, i, condition.turn);
            int selected = randBetween((*currentNode).childCount - 1,0);
            Move nextMove = (*(*currentNode).child[selected]).move;

            // 選択されたノードを除いてメモリ開放
            // freeChildNode(currentNode, selected);
            // free((*currentNode).child);

            // 手を指す
            executeMove(&condition, nextMove);

            // displayCondition(condition);

            // ターン数の加算
            condition.turnNumber++;

            // 新たなノードを作成
            Node *node = (*currentNode).child[selected];
            (*node).turnNumber = condition.turnNumber;
            (*node).move = nextMove;
            
            // データベースに反映
            updateFromNode(*node);

            // 現在のノードを移動
            currentNode = (*currentNode).child[selected];

            // 終了判定
            if (!isEnd(condition, &winner) || condition.turnNumber > 500)
            {
                // 次のターンへ
                condition.turn = !condition.turn;
            }
            else
            {
                break;
            }
        }

        // 結果を逆伝播する
        for (int j = condition.turnNumber; j > -1; j--)
        {
            // 通過数と結果を加算
            (*currentNode).throughCount++;
            if (condition.turnNumber > 500)
            {
                (*currentNode).drawCount++;
            }
            else
            {
                switch (winner)
                {
                case FIRST:
                    (*currentNode).fiWinCount++;
                    break;
                case SECOND:
                    (*currentNode).seWinCount++;
                    break;
                }
            }

            // データベースに反映
            updateFromNode(*currentNode);

            if ((*currentNode).turnNumber > 0)
            {
                printf("currentnodep:%s\n", (*currentNode).parentId);
                selectWhereID(&currentNode, (*currentNode).parentId);
            }
        }
        free(currentNode);
        printf("serch turn:%d\n", i);
    }

    printf("tree created\n");

    free(tree);

    printf("end\n");

}