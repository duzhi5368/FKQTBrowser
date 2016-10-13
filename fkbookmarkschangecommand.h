/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBOOKMARKSCHANGECOMMAND_H
#define FKBOOKMARKSCHANGECOMMAND_H
//-----------------------------------------------------
#include <QtWidgets/QUndoCommand>
//-----------------------------------------------------
class CFKBookmarksMgr;
class CFKBookmarkNode;
//-----------------------------------------------------
class CFKBookmarksChangeCommand : public QUndoCommand
{
public:
    CFKBookmarksChangeCommand(CFKBookmarksMgr* pBookmarkMgr,
        CFKBookmarkNode* pNode, const QString& strNewValue, bool bIsTitle);
public:
    void                undo();
    void                redo();
private:
    CFKBookmarksMgr*    m_pBookmarkMgr;
    CFKBookmarkNode*    m_pNode;
    QString             m_strOldValue;
    QString             m_strNewValue;
    bool                m_bIsTitle;
};
//-----------------------------------------------------
#endif // FKBOOKMARKSCHANGECOMMAND_H
