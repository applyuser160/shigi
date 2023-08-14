#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "piece.c"

typedef enum SquareStatus
{
    NOT_USED = false,
    USED = true,
}SquareStatus;

typedef struct Address
{
    int row;
    int column;
}Address;

typedef struct Square
{
    Address address;
    SquareStatus isUsed;
    APiece piece;
}Square;

typedef struct Board
{
    Square squares[NUMBER_OF_SQUARES];
}Board;

typedef struct Condition
{
    Board board;
    APiece notOnBoard[NUMBER_OF_PIECES];
    Turn turn;
    int turnNumber;
}Condition;

typedef struct Move
{
    Address address;
    APiece piece;
}Move;

// 駒を配置する
void deployAPiece(Board *board, APiece piece, int row, int column)
{
    (*board).squares[9 * (row - 1) + (column - 1)].isUsed = USED;
    (*board).squares[9 * (row - 1) + (column - 1)].piece = piece;
}

// 初期配置を返す
Board initBoard()
{
    Board board;
    for (int i = 0; i < NUMBER_OF_SQUARES; i++)
    {
        int row = i / 9 + 1;
        int column = i % 9 + 1;
        Address address = {row, column};
        APiece piece = generateAPiece(NON, 0, FIRST);
        Square square = {address, NOT_USED, piece};
        board.squares[i] = square;
    }

    deployAPiece(&board, generateAPiece(KING, 1, FIRST), 9, 5);
    deployAPiece(&board, generateAPiece(ROOK, 2, FIRST), 8, 2);
    deployAPiece(&board, generateAPiece(BICHOP, 3, FIRST), 8, 8);
    deployAPiece(&board, generateAPiece(GOLDGENERAL, 4, FIRST), 9, 4);
    deployAPiece(&board, generateAPiece(GOLDGENERAL, 5, FIRST), 9, 6);
    deployAPiece(&board, generateAPiece(SILVERGENERAL, 6, FIRST), 9, 3);
    deployAPiece(&board, generateAPiece(SILVERGENERAL, 7, FIRST), 9, 7);
    deployAPiece(&board, generateAPiece(KNIGHT, 8, FIRST), 9, 2);
    deployAPiece(&board, generateAPiece(KNIGHT, 9, FIRST), 9, 8);
    deployAPiece(&board, generateAPiece(LANCE, 10, FIRST), 9, 1);
    deployAPiece(&board, generateAPiece(LANCE, 11, FIRST), 9, 9);
    deployAPiece(&board, generateAPiece(PAWN, 12, FIRST), 7, 1);
    deployAPiece(&board, generateAPiece(PAWN, 13, FIRST), 7, 2);
    deployAPiece(&board, generateAPiece(PAWN, 14, FIRST), 7, 3);
    deployAPiece(&board, generateAPiece(PAWN, 15, FIRST), 7, 4);
    deployAPiece(&board, generateAPiece(PAWN, 16, FIRST), 7, 5);
    deployAPiece(&board, generateAPiece(PAWN, 17, FIRST), 7, 6);
    deployAPiece(&board, generateAPiece(PAWN, 18, FIRST), 7, 7);
    deployAPiece(&board, generateAPiece(PAWN, 19, FIRST), 7, 8);
    deployAPiece(&board, generateAPiece(PAWN, 20, FIRST), 7, 9);

    deployAPiece(&board, generateAPiece(KING, 21, SECOND), 1, 5);
    deployAPiece(&board, generateAPiece(ROOK, 22, SECOND), 2, 8);
    deployAPiece(&board, generateAPiece(BICHOP, 23, SECOND), 2, 2);
    deployAPiece(&board, generateAPiece(GOLDGENERAL, 24, SECOND), 1, 4);
    deployAPiece(&board, generateAPiece(GOLDGENERAL, 25, SECOND), 1, 6);
    deployAPiece(&board, generateAPiece(SILVERGENERAL, 26, SECOND), 1, 3);
    deployAPiece(&board, generateAPiece(SILVERGENERAL, 27, SECOND), 1, 7);
    deployAPiece(&board, generateAPiece(KNIGHT, 28, SECOND), 1, 2);
    deployAPiece(&board, generateAPiece(KNIGHT, 29, SECOND), 1, 8);
    deployAPiece(&board, generateAPiece(LANCE, 30, SECOND), 1, 1);
    deployAPiece(&board, generateAPiece(LANCE, 31, SECOND), 1, 9);
    deployAPiece(&board, generateAPiece(PAWN, 32, SECOND), 3, 1);
    deployAPiece(&board, generateAPiece(PAWN, 33, SECOND), 3, 2);
    deployAPiece(&board, generateAPiece(PAWN, 34, SECOND), 3, 3);
    deployAPiece(&board, generateAPiece(PAWN, 35, SECOND), 3, 4);
    deployAPiece(&board, generateAPiece(PAWN, 36, SECOND), 3, 5);
    deployAPiece(&board, generateAPiece(PAWN, 37, SECOND), 3, 6);
    deployAPiece(&board, generateAPiece(PAWN, 38, SECOND), 3, 7);
    deployAPiece(&board, generateAPiece(PAWN, 39, SECOND), 3, 8);
    deployAPiece(&board, generateAPiece(PAWN, 40, SECOND), 3, 9);
    return board;
}

// 盤面・持ち駒の表示
void displayCondition(Condition condition)
{
    printf("|9|8|7|6|5|4|3|2|1|\n");
    for (int i1 = 0; i1 < NUMBER_OF_EDGE; i1++)
    {
        printf("|");
        for (int i2 = NUMBER_OF_EDGE - 1; i2 > -1; i2--)
        {
            APiece piece = condition.board.squares[i1 * NUMBER_OF_EDGE + i2].piece;
            if (piece.piece.name == NON)
            {
                printf("%s|", pieceToString(piece.piece));
            }
            else if (piece.turn == FIRST)
            {
                printf("\x1b[36m%s\x1b[0m|", pieceToString(piece.piece));
            }
            else
            {
                printf("\x1b[91m%s\x1b[0m|", pieceToString(piece.piece));
            }
        }
        printf("%d\n", i1 + 1);
    }

    printf("NotOnBoard:\n");
    for (int i = 0; i < NUMBER_OF_PIECES; i++)
    {
        APiece piece = condition.notOnBoard[i];
        if (piece.piece.name == NON)
        {
            continue;
        }
        else if (piece.turn == FIRST)
        {
            printf("\x1b[36m%s,\x1b[0m", pieceToString(piece.piece));
        }
        else
        {
            printf("\x1b[91m%s,\x1b[0m", pieceToString(piece.piece));
        }
    }
    printf("\n");
}

// 持ち駒の初期化
void initNotOnBoard(APiece notOnBoard[NUMBER_OF_PIECES])
{
    for (int i = 0; i < NUMBER_OF_PIECES; i++)
    {
        notOnBoard[i] = generateAPiece(NON, 0, FIRST);
    }
}

// 状況の初期化
Condition initCondition()
{
    Board board = initBoard();
    int turnNumber = 1;
    Condition condition = {board, 0, 0, turnNumber};
    condition.turn = FIRST;
    initNotOnBoard(condition.notOnBoard);
    return condition;
}

// 任意の駒が盤面のどこに位置するか調べる　存在しない場合は、0,0を返す
Address serchPieceFromBoard(Board board, APiece piece)
{
    for (int i = 0; i < NUMBER_OF_SQUARES; i++)
    {
        APiece apiece = board.squares[i].piece;
        if (apiece.index == piece.index && apiece.piece.name != NON)
        {
            return board.squares[i].address;
        }
    }
    Address non = {0, 0};
    return non;
}

// 任意の駒が持ち駒の中のどこに位置するか調べる 存在しない場合は、-1を返す
int serchPieceFromNotOnBoard(APiece notOnBoard[NUMBER_OF_PIECES], APiece piece)
{
    for (int i = 0; i < NUMBER_OF_PIECES; i++)
    {
        APiece apiece = notOnBoard[i];
        if (apiece.index == piece.index)
        {
            return i;
        }
    }
    return -1;
}

// 盤面の任意の場所に駒を設置する
void deployPieceOnBoard(Board *board, Address address, APiece piece)
{
    for (int i = 0; i < NUMBER_OF_SQUARES; i++)
    {
        if ((*board).squares[i].address.row == address.row && (*board).squares[i].address.column == address.column)
        {
            (*board).squares[i].piece = piece;
        }
    }
}

// 持ち駒の任意の場所に駒を設置する
void deployPieceNotOnBoard(APiece notOnBoard[NUMBER_OF_PIECES], int index, APiece piece)
{
    notOnBoard[index] = piece;
}

// 任意の位置にある駒が動くことが出来るか
bool ableMove(Turn turn, PieceName name, Address address)
{
    if (turn == FIRST)
    {
        if (name == KNIGHT && address.row < 3)
        {
            return false;
        }
        else if ((name == LANCE || name == PAWN) && address.row < 2)
        {
            return false;
        }
    }
    else if (turn == SECOND)
    {
        if (name == KNIGHT && address.row > 7)
        {
            return false;
        }
        else if ((name == LANCE || name == PAWN) && address.row > 8)
        {
            return false;
        }
    }
    return true;
}

// 指定の駒が成ることが出来るか
bool ableBe(Address oldAddress, Address newAddress, APiece piece)
{
    if (piece.piece.promote)
    {
        if (piece.turn == FIRST)
        {
            return oldAddress.row < 4 || newAddress.row < 4;
        }
        else
        {
            return oldAddress.row > 6 || newAddress.row > 6;
        }
    }
    return false;
}

// 指定の方向に移動する
Square moveBoard(Board board, APiece piece, int row, int column, int length, Direction direction)
{
    Turn turn = piece.turn;

    Direction d0 = piece.piece.wayOfMove[direction].adirectionMove[0].direction;
    int ml0 = piece.piece.wayOfMove[direction].adirectionMove[0].maxLength;
    Direction d1 = piece.piece.wayOfMove[direction].adirectionMove[1].direction;
    int ml1 = piece.piece.wayOfMove[direction].adirectionMove[1].maxLength;

    // 後手の場合、向きを逆にする
    if (turn == SECOND)
    {
        d0 = switchDirection(d0);
        d1 = switchDirection(d1);
    }

    // lengthがmaxlengthを超えていた場合は、maxlengthに合わせる
    if (length > ml0)
    {
        length = ml0;
    }

    int vectorRow0;
    int vectorColumn0;
    int vectorRow1;
    int vectorColumn1;

    getAddressOfDirection(d0, &vectorRow0, &vectorColumn0);
    getAddressOfDirection(d1, &vectorRow1, &vectorColumn1);

    row += vectorRow0 * length + vectorRow1 * ml1;
    column += vectorColumn0 * length + vectorColumn1 * ml1;

    for (int i = 0; i < NUMBER_OF_SQUARES; i++)
    {
        Square square = board.squares[i];
        if (square.address.row == row && square.address.column == column)
        {
            return square;
        }
    }

    // 枠外の場合
    Address address = {0, 0};
    APiece p = generateAPiece(NON, 0, FIRST);
    Square square = {address, NOT_USED, p};
    return square;
}

// 特定の方向にnマス移動した場所を返す
Square getSquareWhenMoved(Board board, APiece piece, int row, int column, int length, Direction direction)
{
    return moveBoard(board, piece, row, column, length, direction);
}

// 桂馬の動く先
Square getSquareWhenMovedKnight(Board board, APiece piece, int row, int column, Direction direction)
{
    Turn turn = piece.turn;
    if (turn == SECOND)
    {
        direction = switchDirection(direction);
    }
}

// 二歩を検知
bool isTwoSteps(Board board, APiece piece, Address address)
{
    if (piece.piece.name == PAWN)
    {
        for (int i = 0; i < NUMBER_OF_SQUARES; i++)
        {
            if (board.squares[i].address.column == address.column)
            {
                // 同じ行
                if (board.squares[i].piece.piece.name == PAWN && board.squares[i].piece.turn == piece.turn)
                {
                    // 自分の歩
                    return true;
                }
            }
        }
        return false;
    }
    else
    {
        return false;
    }
}

// 手を追加
void addMoves(Move **moves, int *count, Address address, APiece piece)
{
    Move *old = *moves;
    *moves = (Move *)calloc(*count + 1, sizeof(Move));
    for (int i = 0; i < *count; i++)
    {
        (*moves)[i] = old[i];
    }
    Move move = {address, piece};
    (*moves)[*count] = move;
    (*count)++;
}

// その場所に指すことができる手を追加する
void serchAndAddMoves(Move **pointableHands, int *count, Square fromSquare, Square targetSquare, Turn turn)
{
    if (ableBe(fromSquare.address, targetSquare.address, fromSquare.piece))
    {
        // 成ることが出来る場合
        if (ableMove(turn, fromSquare.piece.piece.name, targetSquare.address))
        {
            // 成らなくてもまだ、動くことが可能な場合
            addMoves(pointableHands, count, targetSquare.address, fromSquare.piece);
            APiece promotedPiece = fromSquare.piece;
            promotedPiece.piece.name = getPieceNameAfterBecome(fromSquare.piece.piece);
            promotedPiece.piece.promote = NOTPROMOTABLE;
            addMoves(pointableHands, count, targetSquare.address, promotedPiece);
        }
        else
        {
            // 成らないと動くことが出来なくなる場合
            APiece promotedPiece = fromSquare.piece;
            promotedPiece.piece.name = getPieceNameAfterBecome(fromSquare.piece.piece);
            promotedPiece.piece.promote = NOTPROMOTABLE;
            addMoves(pointableHands, count, targetSquare.address, promotedPiece);
        }
    }
    addMoves(pointableHands, count, targetSquare.address, fromSquare.piece);
    return;
}

// 特定のアドレスから任意の場所に移動し、指すことが出来る手を追加する
void executeAddMoves(Move **pointableHands, int *count, Square fromSquare, Board board, Direction direction, Turn turn)
{
    WayOfMove wayOfMove = fromSquare.piece.piece.wayOfMove[direction];
    if (wayOfMove.adirectionMove[0].maxLength == NUMBER_OF_EDGE - 1)
    {
        for (int j = 1; j < NUMBER_OF_EDGE; j++)
        {
            Square targetSquare = getSquareWhenMoved(board, fromSquare.piece, fromSquare.address.row, fromSquare.address.column, j, direction);

            // 駒が置かれていたら停止
            if (targetSquare.piece.piece.name != NON)
            {
                // 置かれている駒が自分の駒の場合、その１つ前まで、置くことが出来る
                // 置かれている駒が相手の駒の場合、そこまで置くことが出来る
                if (targetSquare.piece.turn != turn)
                {
                    serchAndAddMoves(pointableHands, count, fromSquare, targetSquare, turn);
                }
                return;
            }
            // 盤面から外れたら停止
            else if (targetSquare.address.row == 0 && targetSquare.address.column == 0)
            {
                return;
            }
            else
            {
                serchAndAddMoves(pointableHands, count, fromSquare, targetSquare, turn);
            }
        }
    }
    else if (wayOfMove.adirectionMove[1].maxLength == 1)
    {
        Square targetSquare = getSquareWhenMoved(board, fromSquare.piece, fromSquare.address.row, fromSquare.address.column, 1, direction);

        // 駒が置かれていたら停止
        if (targetSquare.piece.piece.name != NON)
        {
            // 置かれている駒が自分の駒の場合、その１つ前まで、置くことが出来る
            // 置かれている駒が相手の駒の場合、そこまで置くことが出来る
            if (targetSquare.piece.turn != turn)
            {
                serchAndAddMoves(pointableHands, count, fromSquare, targetSquare, turn);
            }
            return;
        }
        // 盤面から外れたら停止
        else if (targetSquare.address.row == 0 && targetSquare.address.column == 0)
        {
            return;
        }
        else
        {
            serchAndAddMoves(pointableHands, count, fromSquare, targetSquare, turn);
        }
    }
    else if (wayOfMove.adirectionMove[0].maxLength == 1)
    {
        Square targetSquare = getSquareWhenMoved(board, fromSquare.piece, fromSquare.address.row, fromSquare.address.column, 1, direction);

        // 駒が置かれていたら停止
        if (targetSquare.piece.piece.name != NON)
        {
            // 置かれている駒が自分の駒の場合、その１つ前まで、置くことが出来る
            // 置かれている駒が相手の駒の場合、そこまで置くことが出来る
            if (targetSquare.piece.turn != turn)
            {
                serchAndAddMoves(pointableHands, count, fromSquare, targetSquare, turn);
            }
            return;
        }
        // 盤面から外れたら停止
        else if (targetSquare.address.row == 0 && targetSquare.address.column == 0)
        {
            return;
        }
        else
        {
            serchAndAddMoves(pointableHands, count, fromSquare, targetSquare, turn);
        }
    }
}

// 動かせる手を探索
int serchPointableHands(Condition condition, Move **pointableHands)
{
    // 動かせる手
    int count = 0;

    // 盤面
    for (int i = 0; i < NUMBER_OF_SQUARES; i++)
    {
        APiece piece = condition.board.squares[i].piece;
        if (piece.piece.name != NON && piece.turn == condition.turn)
        {
            Address address = condition.board.squares[i].address;
            // ８方向で行ける場所を探索
            for (int d = 0; d < NUMBER_OF_DIRECTIONS; d++)
            {
                executeAddMoves(pointableHands, &count, condition.board.squares[i], condition.board, d, condition.turn);
            }
        }
    }

    // 持ち駒
    for (int i = 0; i < NUMBER_OF_PIECES; i++)
    {
        APiece piece = condition.notOnBoard[i];
        // 有効な持ち駒
        if (piece.piece.name != NON && piece.turn == condition.turn)
        {
            for (int j = 0; j < NUMBER_OF_SQUARES; j++)
            {
                Address address = {j / 9 + 1, j % 9 + 1};
                // 駒が置かれていない場所
                if (condition.board.squares[j].piece.piece.name == NON)
                {
                    // 動けない場合
                    PieceName name = piece.piece.name;
                    if (!ableMove(condition.turn, name, address) || isTwoSteps(condition.board, piece, address))
                    {
                        continue;
                    }
                    addMoves(pointableHands, &count, address, piece);
                }
            }
        }
    }

    return count;
}

// 手を指し、状況を更新
void executeMove(Condition *condition, Move move)
{
    // 動かす対象の駒を探索し、現在の位置に存在する駒を除去する
    // 盤面
    Address address = serchPieceFromBoard((*condition).board, move.piece);
    if (address.row != 0 && address.column != 0)
    {
        deployPieceOnBoard(&(*condition).board, address, generateAPiece(NON, 0, FIRST));
    }

    // 持ち駒
    int notOnBoardIndex = serchPieceFromNotOnBoard((*condition).notOnBoard, move.piece);
    if (notOnBoardIndex != -1)
    {
        deployPieceNotOnBoard((*condition).notOnBoard, notOnBoardIndex, generateAPiece(NON, 0, FIRST));
    }

    // 駒を設置し、すでに設置されている駒があった場合は、持ち駒の空いている場所に移す
    for (int i = 0; i < NUMBER_OF_SQUARES; i++)
    {
        Square *square = &(*condition).board.squares[i];
        if ((*square).address.row == move.address.row && (*square).address.column == move.address.column)
        {
            if ((*square).piece.piece.name != NON)
            {
                // すでに設置されている駒があった場合は、持ち駒の空いている場所に移す
                int ind = serchPieceFromNotOnBoard((*condition).notOnBoard, generateAPiece(NON, 0, FIRST));
                (*square).piece.turn = !(*square).piece.turn;
                (*square).piece.piece.name = getPieceNameBeforeBecome((*square).piece.piece);
                (*square).piece.piece.promote = getPromoteByPieceName((*square).piece.piece.name);
                deployPieceNotOnBoard((*condition).notOnBoard, ind, (*square).piece);
            }
            
            // 駒を設置する
            deployPieceOnBoard(&(*condition).board, move.address, move.piece);
        }
    }
}

// 終了判定
bool isEnd(Condition condition, Turn *winner)
{
    for (int i = 0; i < NUMBER_OF_PIECES; i++)
    {
        if (condition.notOnBoard[i].piece.name == KING)
        {
            // 王が持ち駒に見つかったら終了
            *winner = condition.notOnBoard[i].turn;
            return true;
        }
    }
    return false;
}