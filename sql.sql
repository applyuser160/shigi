create table Node (
  ID INT not null auto_increment comment 'ID'
  , parentID INT not null comment 'parentID'
  , turnNumber INT not null comment 'turnNumber'
  , row INT not null comment 'row'
  , column INT not null comment 'column'
  , pieceName INT not null comment 'pieceName'
  , pieceID INT not null comment 'pieceID'
  , throughCount INT not null comment 'throughCount'
  , drawCount INT not null comment 'drawCount'
  , firstWinCount INT not null comment 'firstWinCount'
  , secondWinCount INT not null comment 'secondWinCount'
  , constraint Node_PKC primary key (ID)
) comment 'Node' ;