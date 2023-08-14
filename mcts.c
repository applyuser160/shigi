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
    jsonParse("output2.json", &tree);
    // outputTree(*tree);
    // initNode(&tree);

    // printf("top:%p\n", tree);
    // printf("child:%p\n", (*tree).child);

    for (int i = 0; i < NUMBER_OF_SEARCH; i++)
    {
        // 盤面
        Condition condition = initCondition();

        // displayCondition(condition);

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
            // printf("a\n");
            createNodeFromPossiblePlace(currentNode, condition);
            // printf("b\n");
            Move nextMove = (*(*currentNode).child[ucb(*currentNode, i, condition.turn)]).move;
            // Move nextMove = (*(*currentNode).child[randBetween((*currentNode).childCount - 1,0)]).move;
            // printf("c\n");

            // 手を指す
            executeMove(&condition, nextMove);
            // displayCondition(condition);

            // ターン数の加算
            condition.turnNumber++;

            // 新たなノードを作成
            Node *node;
            initNode(&node);
            (*node).turnNumber = condition.turnNumber;
            (*node).move = nextMove;
            
            // 新たなノードを現在のノードの下に設置（まだ作成されていないノードの場合）
            int nextIndex = deployNode(node, currentNode);

            // 現在のノードを移動
            currentNode = (*currentNode).child[nextIndex];

            // 終了判定
            if (!isEnd(condition, &winner) || condition.turnNumber > 500)
            {
                // 次のターンへ
                condition.turn = !condition.turn;
            }
            else
            {
                // printf("turn%d\n", condition.turnNumber);
                // printf("winner %d\n", winner);
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
        }
        printf("serch turn:%d\n", i);
    }

    printf("tree created\n");

    outputTree("output2.json", *tree);

    free(tree);

    printf("json file created\n");

    printf("end\n");

}