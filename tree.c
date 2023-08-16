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
#include "parson.c"

#define NUMBER_OF_SEARCH 100
#define NUMBER_OF_CHARACTERS_FOR_A_NODE 235

#define JSON2STRUCT_STR(_json, _struct, _key, _size)                           \
    do                                                                         \
    {                                                                          \
        strncpy(_struct._key, json_object_dotget_string(_json, #_key), _size); \
    } while (0);

#define JSON2STRUCT_NUM(_json, _struct, _key)                   \
    do                                                          \
    {                                                           \
        _struct._key = json_object_dotget_number(_json, #_key); \
    } while (0);

#define JSON2STRUCT_BOOL(_json, _struct, _key)                   \
    do                                                           \
    {                                                            \
        _struct._key = json_object_dotget_boolean(_json, #_key); \
    } while (0);

char randChar()
{
    randBetween(61, 0);
    char c = (char)randBetween(61, 0);
    if (c > 35)
    {
        c += 61;
    }
    else if (c > 9)
    {
        c += 55;
    }
    else
    {
        c += 48;
    }
    return c;
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

void initNode(Node **node)
{
    Node *one = (struct Node *)calloc(1, sizeof(Node));
    generateUUID(&((*one).id));
    (*one).parentId = "";
    (*one).parent = NULL;
    (*one).childCount = 0;
    (*one).child = NULL;
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

void displayNode(Node node)
{
    printf("childCount:%d\n", node.childCount);
    printf("turnNumber:%d\n", node.turnNumber);
    printf("address:%d,%d\n", node.move.address.row, node.move.address.column);
    printf("piece:%s\n", pieceToString(node.move.piece.piece));
    printf("throughCount:%d\n", node.throughCount);
    printf("drawCount:%d\n", node.drawCount);
    printf("fiWinCount:%d\n", node.fiWinCount);
    printf("seWinCount:%d\n", node.seWinCount);
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
    // printf("eq\n");
    // printf("rightad:%d,%d\n", right.address.row, right.address.column);
    // printf("leftad:%d,%d\n", left.address.row, left.address.column);
    
    return equalAddress(left.address, right.address) && equalAPiece(left.piece, right.piece);
}

// ノードが既に作成済みの場合インデックスを返す
int isCreated(Node *child, Node *parent)
{
    for (int i = 0; i < (*parent).childCount; i++)
    {
        if (equalMove((*(*parent).child[i]).move, (*child).move))
        {
            return i;
        }
    }
    return -1;
}

// ノードを文字列に変換
JSON_Object* convertJsonObject(JSON_Object *jsonObject, struct Node node)
{
    json_object_set_number(jsonObject, "turnNumber", node.turnNumber);
    json_object_set_number(jsonObject, "row", node.move.address.row);
    json_object_set_number(jsonObject, "column", node.move.address.column);
    json_object_set_number(jsonObject, "pieceName", node.move.piece.piece.name);
    json_object_set_number(jsonObject, "index", node.move.piece.index);
    json_object_set_number(jsonObject, "throughCount", node.throughCount);
    json_object_set_number(jsonObject, "drawCount", node.drawCount);
    json_object_set_number(jsonObject, "fiWinCount", node.fiWinCount);
    json_object_set_number(jsonObject, "seWinCount", node.seWinCount);
    json_object_set_number(jsonObject, "childCount", node.childCount);
    JSON_Value *childJsonValue = json_value_init_array();
    JSON_Array *childJsonArray = json_value_get_array(childJsonValue);
    for (int o = 0; o < node.childCount; o++)
    {
        JSON_Value *achildJsonValue = json_value_init_object();
        JSON_Object *achildJsonObject = json_value_get_object(achildJsonValue);
        achildJsonObject = convertJsonObject(achildJsonObject, *node.child[o]);
        json_array_append_value(childJsonArray, achildJsonValue);
    }
    json_object_dotset_value(jsonObject, "child", childJsonValue);
    
    return jsonObject;
}

void outputTree(char* filename, Node tree)
{
    // 木を出力

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    root_object = convertJsonObject(root_object, tree);

    json_serialize_to_file(root_value, filename);

    json_value_free(root_value);

    printf("tree file created\n");
}

// jsonオブジェクトを、node構造体に変換
struct Node* convertNode(JSON_Object *json)
{
    struct Node *node = (struct Node *)calloc(1, sizeof(Node));
    JSON2STRUCT_NUM(json, (*node), turnNumber);
    (*node).move.address.row = json_object_get_number(json, "row");
    (*node).move.address.column = json_object_get_number(json, "column");
    (*node).move.piece.piece.name = json_object_get_number(json, "pieceName");
    (*node).move.piece.index = json_object_get_number(json, "index");
    JSON2STRUCT_NUM(json, (*node), throughCount);
    JSON2STRUCT_NUM(json, (*node), drawCount);
    JSON2STRUCT_NUM(json, (*node), fiWinCount);
    JSON2STRUCT_NUM(json, (*node), seWinCount);
    JSON2STRUCT_NUM(json, (*node), childCount);
    JSON_Array *childJson = json_object_get_array(json, "child");
    (*node).child = (struct Node **)calloc((*node).childCount, sizeof(Node*));
    for (int o = 0; o < (*node).childCount; o++)
    {
        (*node).child[o] = (struct Node *)calloc(1, sizeof(Node));
    }
    for (int i = 0; i < (*node).childCount; i++)
    {
        (*node).child[i] = convertNode(json_array_get_object(childJson, i));
        (*(*node).child[i]).parent = node;
    }
    return node;
}

// jsonファイルの解析
void jsonParse(char* filename, Node **node)
{
    JSON_Value *root_value = json_parse_file(filename);
    JSON_Object *root = json_object(root_value);

    *node = convertNode(root);

    json_value_free(root_value);

    printf("jsonParse finished\n");
}

// ノードを現在のノードの下に作成する、作成済みの場合は、通過数を加える
// 次のノードのインデックスを返す
int deployNode(Node *child, Node *parent)
{
    int index = isCreated(child, parent);
    if (index == -1)
    {
        // childNodeにparentにparentNodeを設定
        (*child).parentId = (*parent).id;
        (*child).parent = parent;

        // 次のノード数をインクリメント
        (*parent).childCount++;

        // 現在の次のノードを一時的に移す
        Node **old = (*parent).child;

        // 新しい領域を確保（２階層のうちの１階層目）
        (*parent).child = (Node **)calloc((*parent).childCount, sizeof(Node*));

        // 確保した領域に、既存のアドレスをセット
        for (int o = 0; o < (*parent).childCount - 1; o++)
        {
            (*parent).child[o] = old[o];
        }

        // 確保した領域の最後に、新規のノードをセット
        (*parent).child[(*parent).childCount - 1] = child;

        return (*parent).childCount - 1;
    }
    else
    {
        free(child);
        return index;
    }
}

// UCBアルゴリズム
int ucb(struct Node node, int t, Turn turn)
{
    int selected_arm = 0;
    double max_ucb = 0.0;

    for (int i = 0; i < node.childCount; i++)
    {
        if ((*node.child[i]).throughCount == 0)
        {
            selected_arm = i;
            break;
        }

        int winCount;
        switch (turn)
        {
        case FIRST:
            winCount = (*node.child[i]).fiWinCount;
            break;
        case SECOND:
            winCount = (*node.child[i]).seWinCount;
            break;
        }

        double avg_reward = ((double)winCount + (double)(*node.child[i]).drawCount * 0.5) / (double)(*node.child[i]).throughCount;

        double ucb_value = avg_reward + sqrt(2 * log(t) / (*node.child[i]).throughCount);

        if (ucb_value > max_ucb)
        {
            max_ucb = ucb_value;
            selected_arm = i;
        }
    }
    return selected_arm;
}

// 現在指すことができる手のノードを作成する
void createNodeFromPossiblePlace(struct Node *node, Condition condition)
{
    // printf("node.id:%s\n", (*node).id);

    Move *pointableHands;
    int pointableHandsCount = serchPointableHands(condition, &pointableHands);

    // データベースより子ノードの取得
    Node* child;
    int count = selectWhereParentID(&child, (*node).id);
    // printf("count:%d\n", pointableHandsCount);

    // データベースに追加するノードリスト
    Node* insertNodes;
    int insertCount = 0;

    for (int i = 0; i < pointableHandsCount; i++)
    {
        char* id;
        bool hasCreated = false;
        for (int j = 0; j < count; j++)
        {
            if (equalMove((child[j]).move, pointableHands[i]))
            {
                // printf("hasCreated\n");
                hasCreated = true;
                deployNode(&child[j], node);
            }
        }

        if (!hasCreated)
        {
            Node *newNode;
            initNode(&newNode);
            (*newNode).turnNumber = (*node).turnNumber + 1;
            (*newNode).move.address.row = pointableHands[i].address.row;
            (*newNode).move.address.column = pointableHands[i].address.column;
            (*newNode).move.piece = pointableHands[i].piece;
            deployNode(newNode, node);
            Node *old = insertNodes;
            insertNodes = (Node*)calloc(insertCount + 1, sizeof(Node));
            for (int o = 0; o < insertCount; o++)
            {
                insertNodes[o] = old[o];
            }
            insertNodes[insertCount] = *newNode;
            insertCount++;
        }
        // printf("i:%d\n", i);
    }
    // printf("bulk:%d\n", insertCount);

    if (insertCount > 0)
    {
        bulkinsert(insertNodes, insertCount);
    }
}

// 子のノードの内、１つを除いて、メモリを開放する
void freeChildNode(Node *node, int index)
{
    for (int i = 0; i < (*node).childCount; i++)
    {
        if (i != index)
        {
            printf("d1\n");
            free((*node).child[i]);
            printf("d2\n");
        }
    }
}

