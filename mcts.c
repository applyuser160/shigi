#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "tree.c"

void learn(void)
{
    // モンテカルロ木
    Node *tree;
    // jsonParse("output2.json", &tree);
    // outputTree(*tree);
    initNode(&tree);
    printf("a\n");
    (*tree).id = insertFromNode(*tree);
    printf("id:%d\n", (*tree).id);
    printf("lsid:%d\n", getLastInsertedID());

    for (int i = 0; i < NUMBER_OF_SEARCH; i++)
    {
        // 盤面
        Condition condition = initCondition();

        displayCondition(condition);

        Node *currentNode = tree;

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
            int selected = ucb(*currentNode, i, condition.turn);
            // int selected = randBetween((*currentNode).childCount - 1,0);
            Move nextMove = (*(*currentNode).child[selected]).move;

            // 選択されたノードを除いてメモリ開放
            freeChildNode(currentNode, selected);

            // 手を指す
            executeMove(&condition, nextMove);

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

            if ((*currentNode).turnNumber > 0)
            {
                currentNode = (*currentNode).parent;
            }
            // データベースに反映
            updateFromNode(*currentNode);
        }
        printf("serch turn:%d\n", i);
    }

    printf("tree created\n");

    free(tree);

    printf("end\n");

}