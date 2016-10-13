/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBOOKMARKSDIALOG_H
#define FKBOOKMARKSDIALOG_H
//-----------------------------------------------------
#include <QtWidgets/QDialog>
//-----------------------------------------------------
#include "ui_bookmarks.h"
//-----------------------------------------------------
class CFKBookmarksMgr;
class CFKBookmarkNode;
class CFKBookmarksModel;
class CFKTreeProxyModel;
//-----------------------------------------------------
class CFKBookmarksDialog : public QDialog, public Ui_BookmarksDialog
{
    Q_OBJECT
signals:
    void                SIGNAL_OpenUrl(const QUrl& url);
public:
    CFKBookmarksDialog(QWidget* pParent = NULL, CFKBookmarksMgr* pMgr = NULL);
    ~CFKBookmarksDialog();
private slots:
    void                open();
    void                __SLOT_CustomContextMenuRequested(const QPoint &pos);
    void                __SLOT_NewFolder();
private:
    void                __ExpandNodes(CFKBookmarkNode* pNode);
    bool                __SaveExpandedNodes(const QModelIndex& parent);
private:
    CFKBookmarksMgr*    m_pBookmarksMgr;
    CFKBookmarksModel*  m_pBookmarksModel;
    CFKTreeProxyModel*  m_pProxyModel;
};
//-----------------------------------------------------
#endif // FKBOOKMARKSDIALOG_H
