/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBOOKMARKSINSERTCOMMAND_H
#define FKBOOKMARKSINSERTCOMMAND_H
//-----------------------------------------------------
#include "fkbookmarksremovecommand.h"
//-----------------------------------------------------
//-----------------------------------------------------
class CFKBookmarksInsertCommand : public CFKBookmarksRemoveCommand
{
public:
    CFKBookmarksInsertCommand(CFKBookmarksMgr* pBookmarksMgr,
        CFKBookmarkNode* pParent, CFKBookmarkNode* pNode, int nRow);
public:
    void        undo();
    void        redo();
};
//-----------------------------------------------------
#endif // FKBOOKMARKSINSERTCOMMAND_H
