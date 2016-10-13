/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBOOKMARKSREMOVECOMMAND_H
#define FKBOOKMARKSREMOVECOMMAND_H
//-----------------------------------------------------
#include <QtWidgets/QUndoCommand>
//-----------------------------------------------------
class CFKBookmarksMgr;
class CFKBookmarkNode;
//-----------------------------------------------------
class CFKBookmarksRemoveCommand : public QUndoCommand
{
public:
    CFKBookmarksRemoveCommand(CFKBookmarksMgr* pBookmarksMgr,
                              CFKBookmarkNode* pParent, int nRow);
    ~CFKBookmarksRemoveCommand();
public:
    void                undo();
    void                redo();
protected:
    int                 m_nRow;
    CFKBookmarksMgr*    m_pBookmarksMgr;
    CFKBookmarkNode*    m_pNode;
    CFKBookmarkNode*    m_pParent;
    bool                m_bIsDone;
};
//-----------------------------------------------------
#endif // FKBOOKMARKSREMOVECOMMAND_H
