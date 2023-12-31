#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NUMBER_OF_EDGE 9
#define NUMBER_OF_SQUARES 81

// １手において、使用される８方向の最大数
#define MAXIMUM_NUMBER_OF_DIRECTIONS_DUE_TO_POSITIONAL_MOVEMENT 2

// 8方向に動く
#define NUMBER_OF_DIRECTIONS 8

// 駒の数
#define NUMBER_OF_PIECES 40

typedef enum Direction
{
    UP = 0,
    UP_LEFT = 1,
    LEFT = 2,
    DOWN_LEFT = 3,
    DOWN = 4,
    DOWN_RIGHT = 5,
    RIGHT = 6,
    UP_RIGHT = 7,
}Direction;

typedef enum PieceName
{
    // なし
    NON = -1,

    // 王将、玉将
    KING = 0,

    // 飛車
    ROOK = 1,

    // 角行
    BICHOP = 2,

    // 金将
    GOLDGENERAL = 3,

    // 銀将
    SILVERGENERAL = 4,

    // 桂馬
    KNIGHT = 5,

    // 香車
    LANCE = 6,

    // 歩兵
    PAWN = 7,

    PROMOTED_ROOK = 8,
    PROMOTED_BICHOP = 9,
    PROMOTED_SILVERGENERAL = 10,
    PROMOTED_KNIGHT = 11,
    PROMOTED_LANCE = 12,
    PROMOTED_PAWN = 13,
}PieceName;

typedef enum Promote
{
    PROMOTABLE = true,
    NOTPROMOTABLE = false,
}Promote;

typedef enum Turn
{
    FIRST = true,
    SECOND = false,
}Turn;

typedef enum TemplateMove
{
    INFRONT = 0,
    UNLIMITED = 1,
    HOP = 2,
    NOT = 3,
}TemplateMove;

typedef struct ADirectionMove
{
    Direction direction;
    int maxLength;
}ADirectionMove;

typedef struct WayOfMove
{
    bool through;
    ADirectionMove adirectionMove[MAXIMUM_NUMBER_OF_DIRECTIONS_DUE_TO_POSITIONAL_MOVEMENT];
}WayOfMove;

typedef struct Piece
{
    PieceName name;
    Promote promote;
    WayOfMove wayOfMove[NUMBER_OF_DIRECTIONS];
}Piece;

typedef struct APiece
{
    Piece piece;
    int index;
    Turn turn;
}APiece;

Promote getPromoteByPieceName(PieceName name)
{
    if (name == ROOK || name == BICHOP || name == SILVERGENERAL || name == KNIGHT || name == LANCE || name == PAWN)
    {
        return PROMOTABLE;
    }
    else
    {
        return NOTPROMOTABLE;
    }
}

WayOfMove getTemplateMove(TemplateMove move, Direction direction)
{
    WayOfMove way;
    switch (move)
    {
    case INFRONT:
        way.through = false;
        way.adirectionMove[0].maxLength = 1;
        way.adirectionMove[0].direction = direction;
        way.adirectionMove[1].maxLength = 0;
        break;
    case UNLIMITED:
        way.through = false;
        way.adirectionMove[0].maxLength = NUMBER_OF_EDGE - 1;
        way.adirectionMove[0].direction = direction;
        way.adirectionMove[1].maxLength = 0;
        break;
    case HOP:
        way.through = true;
        way.adirectionMove[0].maxLength = 1;
        way.adirectionMove[0].direction = direction;
        way.adirectionMove[1].maxLength = 1;
        way.adirectionMove[1].direction = UP;
        break;
    case NOT:
        way.through = false;
        way.adirectionMove[0].maxLength = 0;
        way.adirectionMove[0].direction = direction;
        way.adirectionMove[1].maxLength = 0;
        break;
    }
    return way;
}

Piece generatePiece(PieceName name)
{
    Piece piece;
    piece.name = name;
    piece.promote = getPromoteByPieceName(name);
    // printf("ininpiece.name:%d\n", piece.name);

    switch (name)
    {
    case NON:
        piece.wayOfMove[UP] = getTemplateMove(NOT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(NOT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(NOT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(NOT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(NOT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(NOT, UP_RIGHT);
        break;
    case KING:
        piece.wayOfMove[UP] = getTemplateMove(INFRONT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(INFRONT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(INFRONT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(INFRONT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(INFRONT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(INFRONT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(INFRONT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(INFRONT, UP_RIGHT);
        break;
    case ROOK:
        piece.wayOfMove[UP] = getTemplateMove(UNLIMITED, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(NOT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(UNLIMITED, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(UNLIMITED, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(UNLIMITED, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(NOT, UP_RIGHT);
        break;
    case BICHOP:
        piece.wayOfMove[UP] = getTemplateMove(NOT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(UNLIMITED, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(NOT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(UNLIMITED, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(NOT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(UNLIMITED, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(NOT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(UNLIMITED, UP_RIGHT);
        break;
    case GOLDGENERAL:
        piece.wayOfMove[UP] = getTemplateMove(INFRONT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(INFRONT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(INFRONT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(INFRONT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(INFRONT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(INFRONT, UP_RIGHT);
        break;
    case SILVERGENERAL:
        piece.wayOfMove[UP] = getTemplateMove(INFRONT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(INFRONT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(NOT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(INFRONT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(NOT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(INFRONT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(NOT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(INFRONT, UP_RIGHT);
        break;
    case KNIGHT:
        piece.wayOfMove[UP] = getTemplateMove(NOT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(HOP, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(NOT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(NOT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(NOT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(HOP, UP_RIGHT);
        break;
    case LANCE:
        piece.wayOfMove[UP] = getTemplateMove(UNLIMITED, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(NOT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(NOT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(NOT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(NOT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(NOT, UP_RIGHT);
        break;
    case PAWN:
        piece.wayOfMove[UP] = getTemplateMove(INFRONT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(NOT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(NOT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(NOT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(NOT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(NOT, UP_RIGHT);
        break;
    case PROMOTED_ROOK:
        piece.wayOfMove[UP] = getTemplateMove(UNLIMITED, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(INFRONT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(UNLIMITED, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(INFRONT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(UNLIMITED, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(INFRONT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(UNLIMITED, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(INFRONT, UP_RIGHT);
        break;
    case PROMOTED_BICHOP:
        piece.wayOfMove[UP] = getTemplateMove(INFRONT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(UNLIMITED, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(INFRONT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(UNLIMITED, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(INFRONT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(UNLIMITED, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(INFRONT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(UNLIMITED, UP_RIGHT);
        break;
    case PROMOTED_SILVERGENERAL:
        piece.wayOfMove[UP] = getTemplateMove(INFRONT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(INFRONT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(INFRONT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(INFRONT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(INFRONT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(INFRONT, UP_RIGHT);
        break;
    case PROMOTED_KNIGHT:
        piece.wayOfMove[UP] = getTemplateMove(INFRONT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(INFRONT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(INFRONT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(INFRONT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(INFRONT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(INFRONT, UP_RIGHT);
        break;
    case PROMOTED_LANCE:
        piece.wayOfMove[UP] = getTemplateMove(INFRONT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(INFRONT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(INFRONT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(INFRONT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(INFRONT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(INFRONT, UP_RIGHT);
        break;
    case PROMOTED_PAWN:
        piece.wayOfMove[UP] = getTemplateMove(INFRONT, UP);
        piece.wayOfMove[UP_LEFT] = getTemplateMove(INFRONT, UP_LEFT);
        piece.wayOfMove[LEFT] = getTemplateMove(INFRONT, LEFT);
        piece.wayOfMove[DOWN_LEFT] = getTemplateMove(NOT, DOWN_LEFT);
        piece.wayOfMove[DOWN] = getTemplateMove(INFRONT, DOWN);
        piece.wayOfMove[DOWN_RIGHT] = getTemplateMove(NOT, DOWN_RIGHT);
        piece.wayOfMove[RIGHT] = getTemplateMove(INFRONT, RIGHT);
        piece.wayOfMove[UP_RIGHT] = getTemplateMove(INFRONT, UP_RIGHT);
        break;
    }
    return piece;
}

APiece generateAPiece(PieceName name, int index, Turn turn)
{
    Piece p = generatePiece(name);
    APiece ap = {p, index, turn};
    return ap;
}

char* pieceToString(Piece piece)
{
    switch (piece.name)
    {
    case NON:
        return " ";
    case KING:
        return "K";
    case ROOK:
        return "R";
    case BICHOP:
        return "B";
    case GOLDGENERAL:
        return "G";
    case SILVERGENERAL:
        return "s";
    case KNIGHT:
        return "n";
    case LANCE:
        return "l";
    case PAWN:
        return "p";
    case PROMOTED_ROOK:
        return "D";
    case PROMOTED_BICHOP:
        return "H";
    case PROMOTED_SILVERGENERAL:
        return "S";
    case PROMOTED_KNIGHT:
        return "N";
    case PROMOTED_LANCE:
        return "L";
    case PROMOTED_PAWN:
        return "P";
    }
}

Direction switchDirection(Direction direction)
{
    return (direction + 4) % 8;
}

void getAddressOfDirection(Direction direction, int *vectorRow, int *vectorColumn)
{
    switch (direction)
    {
    case UP:
        *vectorRow = -1;
        *vectorColumn = 0;
        break;
    case UP_LEFT:
        *vectorRow = -1;
        *vectorColumn = 1;
        break;
    case LEFT:
        *vectorRow = 0;
        *vectorColumn = 1;
        break;
    case DOWN_LEFT:
        *vectorRow = 1;
        *vectorColumn = 1;
        break;
    case DOWN:
        *vectorRow = 1;
        *vectorColumn = 0;
        break;
    case DOWN_RIGHT:
        *vectorRow = 1;
        *vectorColumn = -1;
        break;
    case RIGHT:
        *vectorRow = 0;
        *vectorColumn = -1;
        break;
    case UP_RIGHT:
        *vectorRow = -1;
        *vectorColumn = -1;
        break;
    }
}

PieceName getPieceNameAfterBecome(Piece piece)
{
    switch (piece.name)
    {
    case NON:
        return NON;
    case KING:
        return NON;
    case ROOK:
        return PROMOTED_ROOK;
    case BICHOP:
        return PROMOTED_BICHOP;
    case GOLDGENERAL:
        return NON;
    case SILVERGENERAL:
        return PROMOTED_SILVERGENERAL;
    case KNIGHT:
        return PROMOTED_KNIGHT;
    case LANCE:
        return PROMOTED_LANCE;
    case PAWN:
        return PROMOTED_PAWN;
    case PROMOTED_ROOK:
        return NON;
    case PROMOTED_BICHOP:
        return NON;
    case PROMOTED_SILVERGENERAL:
        return NON;
    case PROMOTED_KNIGHT:
        return NON;
    case PROMOTED_LANCE:
        return NON;
    case PROMOTED_PAWN:
        return NON;
    }
}

PieceName getPieceNameBeforeBecome(Piece piece)
{
    switch (piece.name)
    {
    case NON:
        return NON;
    case KING:
        return KING;
    case ROOK:
        return ROOK;
    case BICHOP:
        return BICHOP;
    case GOLDGENERAL:
        return GOLDGENERAL;
    case SILVERGENERAL:
        return SILVERGENERAL;
    case KNIGHT:
        return KNIGHT;
    case LANCE:
        return LANCE;
    case PAWN:
        return PAWN;
    case PROMOTED_ROOK:
        return ROOK;
    case PROMOTED_BICHOP:
        return BICHOP;
    case PROMOTED_SILVERGENERAL:
        return SILVERGENERAL;
    case PROMOTED_KNIGHT:
        return KNIGHT;
    case PROMOTED_LANCE:
        return LANCE;
    case PROMOTED_PAWN:
        return PAWN;
    }
}

