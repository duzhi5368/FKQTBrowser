/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkbookmarksinsertcommand.h"
#include "fkbookmarknode.h"
#include "fkbookmarksmgr.h"
//-----------------------------------------------------
CFKBookmarksInsertCommand::CFKBookmarksInsertCommand(CFKBookmarksMgr* pBookmarksMgr,
    CFKBookmarkNode* pParent, CFKBookmarkNode* pNode, int nRow)
    : CFKBookmarksRemoveCommand(pBookmarksMgr, pParent, nRow)
{
    setText(CFKBookmarksMgr::tr("Insert Bookmark"));
    m_pNode = pNode;
}
//-----------------------------------------------------
void CFKBookmarksInsertCommand::undo()
{
    CFKBookmarksRemoveCommand::redo();
}
//-----------------------------------------------------
void CFKBookmarksInsertCommand::redo()
{
    CFKBookmarksRemoveCommand::undo();
}
//-----------------------------------------------------
