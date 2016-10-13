/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBOOKMARKSMGR_H
#define FKBOOKMARKSMGR_H
//-----------------------------------------------------
#include <QtCore/QObject>

#include <QtWidgets/QUndoCommand>
//-----------------------------------------------------
class CFKBookmarkNode;
class CFKAutoSaver;
class CFKBookmarksModel;
//-----------------------------------------------------
class CFKBookmarksMgr : public QObject
{
    Q_OBJECT
signals:
    void                SIGNAL_EntryAdded(CFKBookmarkNode* pItem);
    void                SIGNAL_EntryRemoved(CFKBookmarkNode* pParent,
                                            int nRow, CFKBookmarkNode* pItem);
    void                SIGNAL_EntryChanged(CFKBookmarkNode* pItem);
public:
    CFKBookmarksMgr(QObject* pParent = NULL);
    ~CFKBookmarksMgr();
public:
    void                AddBookmark(CFKBookmarkNode* pParent,
                                    CFKBookmarkNode* pNode, int nRow = -1);
    void                RemoveBookmark(CFKBookmarkNode* pNode);
    void                SetTitle(CFKBookmarkNode* pNode, const QString& strNewTitle);
    void                SetUrl(CFKBookmarkNode* pNode, const QString& strNewUrl);
    void                ChangeExpanded();

    CFKBookmarkNode*    GetBookmarks();
    CFKBookmarkNode*    GetMenu();
    CFKBookmarkNode*    GetToolbar();
    CFKBookmarksModel*  GetBookmarksModel();
    QUndoStack*         GetUndoRedoStack();
public slots:
    void                __SLOT_ImportBookmarks();
    void                __SLOT_ExportBookmarks();
private slots:
    void                __SLOT_Save() const;
private:
    void                __Load();
private:
    bool                m_bIsLoaded;
    CFKAutoSaver*       m_pAutoSaver;
    CFKBookmarksModel*  m_pBookmarkModel;
    CFKBookmarkNode*    m_pBookmarkRootNode;
    QUndoStack          m_StackCommands;
};
//-----------------------------------------------------
#endif // FKBOOKMARKSMGR_H
