/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkbookmarksremovecommand.h"
#include "fkbookmarksmgr.h"
#include "fkbookmarknode.h"
//-----------------------------------------------------
CFKBookmarksRemoveCommand::CFKBookmarksRemoveCommand(CFKBookmarksMgr* pBookmarksMgr,
                          CFKBookmarkNode* pParent, int nRow)
    : QUndoCommand(CFKBookmarksMgr::tr("Remove Bookmark"))
    , m_nRow(nRow)
    , m_bIsDone(false)
    , m_pBookmarksMgr(pBookmarksMgr)
    , m_pNode(pParent->GetChildrens().value(nRow))
    , m_pParent(pParent)
{

}
//-----------------------------------------------------
CFKBookmarksRemoveCommand::~CFKBookmarksRemoveCommand()
{
    if (m_bIsDone && !m_pNode->GetParent())
    {
        delete m_pNode;
    }
}
//-----------------------------------------------------
void CFKBookmarksRemoveCommand::undo()
{
    m_pParent->Add(m_pNode, m_nRow);
    emit m_pBookmarksMgr->SIGNAL_EntryAdded(m_pNode);
    m_bIsDone = false;
}
//-----------------------------------------------------
void CFKBookmarksRemoveCommand::redo()
{
    m_pParent->Remove(m_pNode);
    emit m_pBookmarksMgr->SIGNAL_EntryRemoved(m_pParent, m_nRow, m_pNode);
    m_bIsDone = true;
}
//-----------------------------------------------------
