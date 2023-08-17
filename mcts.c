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
    unsigned long long num = randULL(ULLONG_MAX - 1, 0);
    printf("num:%llu\n", num);
    printf("num:%llx\n", num);
    printf("num:%llo\n", num);
    return;


    // モンテカルロ木
    Node *tree;

    // topnode sql:insert into [dbname].[tablename] values('sZG6neL2-sZQ7-oeLC-tjQ7-yfVZG7xeVL2t-524566', null, 0, 0, 0, -1, 0, 0, 0, 0, 0);
    char* topNodeId = "sZG6neL2-sZQ7-oeLC-tjQ7-yfVZG7xeVL2t-524566";
    selectWhereID(&tree, topNodeId);
    printf("start\n");

    for (int i = 0; i < NUMBER_OF_SEARCH; i++)
    {
        // 盤面
        Condition condition = initCondition();

        Node *currentNode = (Node*)calloc(condition.turnNumber, sizeof(Node));
        selectWhereID(&currentNode, topNodeId);
        printf("cu[0]:%s\n", currentNode[0].id);

        // 勝った方
        Turn winner;

        while (1)
        {
            if (i == 1 && condition.turnNumber > 3)return;
            Node *nextNodelist;
            int nextNodeCount;

            // UCBにより、手を評価し、選択する
            // printf("a%d\n", condition.turnNumber);
            printf("currentNode[%d].id:%s\n", condition.turnNumber - 1, currentNode[condition.turnNumber - 1].id);
            printf("currentNode[%d].parentId:%s\n", condition.turnNumber - 1, currentNode[condition.turnNumber - 1].parentId);
            createNodeFromPossiblePlace(&nextNodelist, &nextNodeCount, currentNode[condition.turnNumber - 1], condition);
            // printf("b\n");
            // int selected = ucb(nextNodelist, nextNodeCount, i, condition.turn);
            int selected = randBetween(nextNodeCount - 1,0);
            // printf("nextNodelist[%d].parentid:%s\n", selected, nextNodelist[selected].parentId);

            Node *oldNode = currentNode;
            currentNode = (Node*)calloc(condition.turnNumber + 1, sizeof(Node));
            for (int i = 0; i < condition.turnNumber; i++)
            {
                currentNode[i] = oldNode[i];
            }
            free(oldNode);
            currentNode[condition.turnNumber] = nextNodelist[selected];
            // printf("currentNode[%d].id:%s\n", condition.turnNumber, currentNode[condition.turnNumber].id);
            // printf("currentNode[%d].parentId:%s\n", condition.turnNumber, currentNode[condition.turnNumber].parentId);
            // printf("c\n");

            // nextNodelist開放
            // printf("%p\n", nextNodelist);
            // free(nextNodelist);
            // printf("d\n");

            // 手を指す
            executeMove(&condition, currentNode[condition.turnNumber].move);

            // displayCondition(condition);

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
            if (condition.turnNumber > 500)
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
        for (int i = 0; i < condition.turnNumber; i++)
        {
            if (currentNode[i].throughCount > 1 || currentNode[i].turnNumber == 0)
            {
                updateFromNode(currentNode[i + 1]);
            }
            else
            {
                nodeForInsert[nodeForInsertCount] = currentNode[i];
                nodeForInsertCount++;
            }
        }
        free(currentNode);
        bulkinsert(nodeForInsert, nodeForInsertCount);
        free(nodeForInsert);

        printf("serch turn:%d\n", i);
    }

    printf("end\n");
}