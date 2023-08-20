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
    initNode(node);
    for (unsigned int col = 0; col < fields; col++)
    {
        switch (col)
        {
        case ID:
            // ポインタを渡すやり方だと、狂うので、値渡しにする　parentIdも同じ
            for (int i = 0; i < 43; i++)
            {
                (*node).id[i] = mysql_row[col][i];
            }
            break;
        case PARENTID:
            if (mysql_row[col] != 0)
            {
                for (int i = 0; i < 43; i++)
                {
                    (*node).parentId[i] = mysql_row[col][i];
                }
            }
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
    }
    (*node).move.piece = generateAPiece((*node).move.piece.piece.name, (*node).move.piece.index, (*node).turnNumber % 2 == 1);
}

int base(Node *node, char* query, int queryLength, SQL_TYPE type)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    const char *SERV = "192.168.0.220";
    const char *USER = "root";
    const char *PASSWORD = "root";
    const char *DB_NAME = DBNAME;
    const unsigned int PORT = 3306;

    conn = mysql_init(NULL); 
    mysql_options(conn, MYSQL_SET_CHARSET_NAME, "cp932"); 
    int ssl_mode = SSL_MODE_DISABLED;
    mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode); //ssl-mode=DISABLED
 
    if( !mysql_real_connect(conn,SERV,USER,PASSWORD,DB_NAME,PORT,NULL,0)){
        fprintf(stderr, "c%s\r\n", mysql_error(conn)); 
        exit(-1);
    }
 
    if( mysql_query( conn, query) ){
        fprintf(stderr, "e%s\nsql:%s\r\n", mysql_error(conn), query);
        exit(-1);
    }
    res = mysql_store_result(conn);
 
    int resultCount;
    if (type == SELECT)
    {
        unsigned long long rows = mysql_num_rows(res);
        for (int i = 0; i < rows; i++)
        {
            row = mysql_fetch_row(res);
            unsigned int fields = mysql_num_fields(res);
            setNode(&(node[i]), row, fields);
        }
        resultCount = rows;
    }

    if(NULL != res){
        mysql_free_result(res);
    }
 
    if(NULL != conn){
        mysql_close(conn);
    }

    memset(query, 0, queryLength * sizeof(char));

    return resultCount;
}

// SELECT
int selectq(Node *node, char query[256])
{
    return base(node, query, 256, SELECT);
}

// INSERT
void insert(char query[256])
{
    Node *node;
    base(node, query, 256, INSERT);
}

// UPDATE
void update(char query[256])
{
    Node *node;
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
    sprintf(query, "insert into %s.%s(ID, parentID, turnNumber, `row`, `column`, pieceName, pieceID, throughCount, drawCount, firstWinCount, secondWinCount)values %s;\0",\
        DBNAME, TABLENAME, values);
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
    sprintf(query, "insert into %s.%s(ID, parentID, turnNumber, `row`, `column`, pieceName, pieceID, throughCount, drawCount, firstWinCount, secondWinCount)values %s;\0", \
        DBNAME, TABLENAME, values);
    Node *n;
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
        throughCount = %d, drawCount = %d, firstWinCount = %d, secondWinCount = %d where ID = '%s';\0",\
        DBNAME, TABLENAME, node.turnNumber, node.move.address.row, node.move.address.column, node.move.piece.piece.name, node.move.piece.index, \
        node.throughCount, node.drawCount, node.fiWinCount, node.seWinCount, node.id);
    Node *n;
    base(n, query, 256, UPDATE);
    memset(query, 0, 256 * sizeof(char));
}

// SELECT where ID
int selectWhereID(Node *node, char ID[43])
{
    char query[256];
    memset(query, 0, 256 * sizeof(char));
    sprintf(query, "select * from %s.%s where ID = '%s';\0", DBNAME, TABLENAME, ID);
    int result = selectq(node, query);
    memset(query, 0, 256 * sizeof(char));
    return result;
}

// SELECT where parentID
int selectWhereParentID(char parentID[43], Node *node)
{
    char query[256];
    memset(query, 0, 256 * sizeof(char));
    sprintf(query, "select * from %s.%s where parentID = '%s';\0", DBNAME, TABLENAME, parentID);
    int result = selectq(node, query);
    memset(query, 0, 256 * sizeof(char));
    return result;
}