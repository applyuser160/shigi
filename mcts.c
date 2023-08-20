#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "tree.c"

// 手法
// 指した手のみを保持するノードの配列を用いる
// 次にさせる手は、計算し、構造体リストに格納する　ターン終了時に開放
void learn(void)
{
    // モンテカルロ木
    Node *tree = (Node*)calloc(1, sizeof(Node));

    // topnode sql:insert into [dbname].[tablename] values('sZG6neL2-sZQ7-oeLC-tjQ7-yfVZG7xeVL2t-524566', null, 0, 0, 0, -1, 0, 0, 0, 0, 0);
    char* topNodeId = "sZG6neL2-sZQ7-oeLC-tjQ7-yfVZG7xeVL2t-524566";
    selectWhereID(tree, topNodeId);

    for (int i = 0; i < NUMBER_OF_SEARCH; i++)
    {
        // 盤面
        Condition condition = initCondition();

        Node *currentNode = (Node*)calloc(condition.turnNumber, sizeof(Node));
        selectWhereID(currentNode, topNodeId);

        // 勝った方
        Turn winner;

        while (1)
        {
            Node *nextNodelist;
            int nextNodeCount;

            // UCBにより、手を評価し、選択する
            createNodeFromPossiblePlace(&nextNodelist, &nextNodeCount, currentNode[condition.turnNumber - 1], condition);
            int selected = ucb(nextNodelist, nextNodeCount, condition.turnNumber, condition.turn);
            // int selected = randBetween(nextNodeCount - 1,0);

            Node *oldNode = currentNode;
            currentNode = (Node*)calloc(condition.turnNumber + 1, sizeof(Node));
            for (int j = 0; j < condition.turnNumber; j++)
            {
                currentNode[j] = oldNode[j];
            }
            free(oldNode);
            currentNode[condition.turnNumber] = nextNodelist[selected];
            for (int j = 0; j < nextNodeCount; j++)
            {
                if (j != selected)
                {
                    free(nextNodelist[j].id);
                }
            }
            free(nextNodelist);

            // 手を指す
            executeMove(&condition, currentNode[condition.turnNumber].move);

            // 終了判定
            if (!isEnd(condition, &winner) && condition.turnNumber < 500)
            {
                // ターン数の加算
                condition.turnNumber++;
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
            currentNode[j].throughCount++;
            if (condition.turnNumber == 500)
            {
                currentNode[j].drawCount++;
            }
            else
            {
                switch (winner)
                {
                case FIRST:
                    currentNode[j].fiWinCount++;
                    break;
                case SECOND:
                    currentNode[j].seWinCount++;
                    break;
                }
            }
        }
        // データベースに反映
        Node *nodeForInsert = (Node *)calloc(condition.turnNumber, sizeof(Node));
        int nodeForInsertCount = 0;
        for (int j = 0; j <= condition.turnNumber; j++)
        {
            if (currentNode[j].throughCount > 1 || currentNode[j].turnNumber == 0)
            {
                updateFromNode(currentNode[j]);
            }
            else
            {
                nodeForInsert[nodeForInsertCount] = currentNode[j];
                nodeForInsertCount++;
            }
        }
        // for (int j = 1; j <= condition.turnNumber; i++)
        // {
        //     free(currentNode[j].id);
        //     // if(currentNode[j].parentId == 0)free(currentNode[j].parentId);
        // }
        free(currentNode);
        bulkinsert(nodeForInsert, nodeForInsertCount);
        free(nodeForInsert);

        printf("serch turn:%d\n", i);
        usleep(10);
    }

    printf("end\n");
}