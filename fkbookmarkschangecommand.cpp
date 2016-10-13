/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkbookmarkschangecommand.h"
#include "fkbookmarknode.h"
#include "fkbookmarksmgr.h"
//-----------------------------------------------------
CFKBookmarksChangeCommand::CFKBookmarksChangeCommand(CFKBookmarksMgr* pBookmarkMgr,
    CFKBookmarkNode* pNode, const QString& strNewValue, bool bIsTitle)
    : QUndoCommand()
    , m_pBookmarkMgr(pBookmarkMgr)
    , m_bIsTitle(bIsTitle)
    , m_strNewValue(strNewValue)
    , m_pNode(pNode)
{
    if (m_bIsTitle)
    {
        m_strOldValue = m_pNode->m_strTitle;
        setText(CFKBookmarksMgr::tr("Name Change"));
    }
    else
    {
        m_strOldValue = m_pNode->m_strUrl;
        setText(CFKBookmarksMgr::tr("Address Change"));
    }
}
//-----------------------------------------------------
void CFKBookmarksChangeCommand::undo()
{
    if (m_bIsTitle)
        m_pNode->m_strTitle = m_strOldValue;
    else
        m_pNode->m_strUrl   = m_strOldValue;
    emit m_pBookmarkMgr->SIGNAL_EntryChanged(m_pNode);
}
//-----------------------------------------------------
void CFKBookmarksChangeCommand::redo()
{
    if (m_bIsTitle)
        m_pNode->m_strTitle = m_strNewValue;
    else
        m_pNode->m_strUrl   = m_strNewValue;
    emit m_pBookmarkMgr->SIGNAL_EntryChanged(m_pNode);
}
//-----------------------------------------------------
