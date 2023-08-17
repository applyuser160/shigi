#include <stdio.h>
#include <stdlib.h>
#include "node.c"
#include "mysql-8.1.0-winx64/include/mysql.h" /* MySQLを利用するための構造体の定義などが書かれたヘッダファイル */

#define DBNAME "shogi"
#define TABLENAME "node"

typedef enum SQL_TYPE
{
    SELECT = 1,
    INSERT = 2,
    UPDATE = 3
}SQL_TYPE;

typedef enum COLUMN_NO
{
    ID = 0,
    PARENTID = 1,
    TURNNUMBER = 2,
    ROW = 3,
    COLUMN = 4,
    PIECENAME = 5,
    PIECEID = 6,
    THROUGHCOUNT = 7,
    DRAWCOUNT = 8,
    FIRSTWINCOUNT = 9,
    SECONDWINCOUNT = 10
}COLUMN_NO;

void setNode(Node *node, MYSQL_ROW mysql_row, unsigned int fields)
{
    for (unsigned int col = 0; col < fields; col++)
    {
        switch (col)
        {
        case ID:
            (*node).id = mysql_row[col];
            break;
        case PARENTID:
            (*node).parentId = mysql_row[col];
            break;
        case TURNNUMBER:
            (*node).turnNumber = atoi(mysql_row[col]);
            break;
        case ROW:
            (*node).move.address.row = atoi(mysql_row[col]);
            break;
        case COLUMN:
            (*node).move.address.column = atoi(mysql_row[col]);
            break;
        case PIECENAME:
            (*node).move.piece.piece.name = atoi(mysql_row[col]);
            break;
        case PIECEID:
            (*node).move.piece.index = atoi(mysql_row[col]);
            break;
        case THROUGHCOUNT:
            (*node).throughCount = atoi(mysql_row[col]);
            break;
        case DRAWCOUNT:
            (*node).drawCount = atoi(mysql_row[col]);
            break;
        case FIRSTWINCOUNT:
            (*node).fiWinCount = atoi(mysql_row[col]);
            break;
        case SECONDWINCOUNT:
            (*node).seWinCount = atoi(mysql_row[col]);
            break;
        }
        // printf("%s ", mysql_row[col]);
    }
    (*node).move.piece = generateAPiece((*node).move.piece.piece.name, (*node).move.piece.index, (*node).turnNumber % 2 == 1);
    // printf("\r\n");
}

int base(Node **node, char* query, int queryLength, SQL_TYPE type)
{
    MYSQL *conn; /* MySQLとの接続を表す構造体 */
    MYSQL_RES *res; /* SELECT文（など）の結果を表す構造体 */
    MYSQL_ROW row; /* MYSQL_RESの中の1レコードを示す構造体 */

    const char *SERV = "192.168.0.220"; /* MySQLが設置してあるサーバのIPアドレスorホスト名 */
    const char *USER = "root"; /* MySQLにアクセスする際のユーザ名 */
    const char *PASSWORD = "root"; /* MySQLにアクセスする際にパスワード */
    const char *DB_NAME = DBNAME; /* 接続先のMySQLで使用するデータベース名 */
    const unsigned int PORT = 3306; /* 接続先のサーバでMySQLが稼働しているポート番号 */

    conn = mysql_init(NULL); /* 接続の初期化 */
    mysql_options(conn, MYSQL_SET_CHARSET_NAME, "cp932"); /* オプション設定。今回は文字コードの設定をcp932に設定している。 */
    int ssl_mode = SSL_MODE_DISABLED;
    mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode); //ssl-mode=DISABLED
 
    /* 接続 */
    /* mysql_real_connect()関数：MySQLと接続を開始 */
    if( !mysql_real_connect(conn,SERV,USER,PASSWORD,DB_NAME,PORT,NULL,0) ){
        fprintf(stderr, "c%s\r\n", mysql_error(conn)); /* エラーが発生した場合、標準エラー出力に内容を表示 */
        exit(-1);
    }
 
    /* query実行 */
    /* mysql_query()関数：queryを実行 */
    if( mysql_query( conn, query) ){
        fprintf(stderr, "e%s\nsql:%s\r\n", mysql_error(conn), query);
        exit(-1);
    }
    /* 結果を取得 */
    /* mysql_use_result()関数：queryの実行結果全体を取得 */
    res = mysql_store_result(conn);
 
    /* 結果の処理 */
    int resultCount;
    if (type == SELECT)
    {
        unsigned long long rows = mysql_num_rows(res);
        Node *result = (Node *)calloc(rows, sizeof(Node));
        for (int i = 0; i < rows; i++)
        {
            row = mysql_fetch_row(res);
            unsigned int fields = mysql_num_fields(res);
            printf("setnode:%p\n", result[i]);
            setNode(&(result[i]), row, fields);
        }
        *node = result;
        resultCount = rows;
    }

    /* 構造体の解放処理 */
    /* mysql_free_result()関数：検索結果を保持している構造体のメモリを解放 */
    if(NULL != res){
        mysql_free_result(res);
    }
 
    /* 接続の解放処理 */
    /* mysql_close()関数：mysqlとの接続を切断 */
    if(NULL != conn){
        mysql_close(conn);
    }

    memset(query, 0, queryLength * sizeof(char));

    return resultCount;
}

// SELECT
int selectq(Node **node, char query[256])
{
    return base(node, query, 256, SELECT);
}

// INSERT
void insert(char query[256])
{
    Node **node;
    base(node, query, 256, INSERT);
}

// UPDATE
void update(char query[256])
{
    Node **node;
    base(node, query, 256, UPDATE);
}

// INSERT from Node
char* insertFromNode(Node node)
{
    char values[256];
    sprintf(values, "('%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)", \
        node.id, node.parentId, node.turnNumber, node.move.address.row, node.move.address.column, node.move.piece.piece.name, node.move.piece.index, \
        node.throughCount, node.drawCount, node.fiWinCount, node.seWinCount);
    char query[256];
    sprintf(query, "insert into %s.%s(ID, parentID, turnNumber, `row`, `column`, pieceName, pieceID, throughCount, drawCount, firstWinCount, secondWinCount)values %s;", DBNAME, TABLENAME, values);
    // printf("%s\n", query);
    insert(query);
    memset(query, 0, 256 * sizeof(char));
    memset(values, 0, 256 * sizeof(char));
    return node.id;
}

// MULTI INSERT 追加したIDの取得部分を実装していない
void bulkinsert(Node *node, int count)
{
    char* values = (char*)calloc(256 * count, sizeof(char));
    for (int i = 0; i < count; i++)
    {
        char* value = (char*)calloc(256, sizeof(char));
        sprintf(value, "('%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)", \
            node[i].id, node[i].parentId, node[i].turnNumber, node[i].move.address.row, node[i].move.address.column, node[i].move.piece.piece.name, node[i].move.piece.index, \
            node[i].throughCount, node[i].drawCount, node[i].fiWinCount, node[i].seWinCount);
        if (i > 0)strcat(values, ",");
        strcat(values, value);
        memset(value, 0, 256 * sizeof(char));
        free(value);
    }
    char* query = (char*)calloc(256 * (count + 1), sizeof(char));
    sprintf(query, "insert into %s.%s(ID, parentID, turnNumber, `row`, `column`, pieceName, pieceID, throughCount, drawCount, firstWinCount, secondWinCount)values %s;", DBNAME, TABLENAME, values);
    // printf("%s\n", query);
    Node **n;
    base(n, query, 256 * (count + 1), INSERT);
    memset(query, 0, 256 * (count + 1) * sizeof(char));
    memset(values, 0, 256 * count * sizeof(char));
    free(values);
    free(query);
}

// UPDATE from Node
void updateFromNode(Node node)
{
    char query[256];
    memset(query, 0, 256 * sizeof(char));
    sprintf(query, "update %s.%s set \
        turnNumber = %d, `row` = %d, `column` = %d, pieceName = %d, pieceID = %d, \
        throughCount = %d, drawCount = %d, firstWinCount = %d, secondWinCount = %d where ID = '%s';", DBNAME, TABLENAME,\
        node.turnNumber, node.move.address.row, node.move.address.column, node.move.piece.piece.name, node.move.piece.index, \
        node.throughCount, node.drawCount, node.fiWinCount, node.seWinCount, node.id);
    // printf("%s\n", query);
    Node **n;
    base(n, query, 256, UPDATE);
    memset(query, 0, 256 * sizeof(char));
}

// SELECT where ID
int selectWhereID(Node **node, char ID[43])
{
    char query[256];
    memset(query, 0, 256 * sizeof(char));
    sprintf(query, "select * from %s.%s where ID = '%s';", DBNAME, TABLENAME, ID);
    int result = selectq(node, query);
    memset(query, 0, 256 * sizeof(char));
    return result;
}

// SELECT where parentID
int selectWhereParentID(Node **node, char parentID[43])
{
    char query[256];
    memset(query, 0, 256 * sizeof(char));
    sprintf(query, "select * from %s.%s where parentID = '%s';", DBNAME, TABLENAME, parentID);
    printf("Query: %s\n", query); // デバッグ出力
    int result = selectq(node, query);
    printf("selectWhereParentID result: %d\n", result); // デバッグ出力
    memset(query, 0, 256 * sizeof(char));
    return result;
}

