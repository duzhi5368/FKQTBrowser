/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtWidgets/QTreeView>
//-----------------------------------------------------
#include "fkaddbookmarkdialog.h"
#include "fkbookmarksmgr.h"
#include "fkaddbookmarkproxymodel.h"
#include "fkbookmarksmodel.h"
#include "fkbookmarknode.h"
#include "fkbrowserapp.h"
//-----------------------------------------------------
CFKAddBookmarkDialog::CFKAddBookmarkDialog(const QString& url, const QString &title,
       QWidget *pParent, CFKBookmarksMgr* pBookmarkMgr)
    : QDialog(pParent)
    , m_strUrl(url)
    , m_pBookmarksMgr(pBookmarkMgr)
{
    setWindowFlags(Qt::Sheet);
    setupUi(this);
    if (!m_pBookmarksMgr)
    {
        m_pBookmarksMgr = CFKBrowserApp::GetBookmarksMgrPtr();
    }
    QTreeView* pView            = new QTreeView(this);
    m_pProxyModel               = new CFKAddBookmarkProxyModel(this);
    CFKBookmarksModel* pModel   = m_pBookmarksMgr->GetBookmarksModel();
    m_pProxyModel->setSourceModel(pModel);
    pView->setModel(m_pProxyModel);
    pView->expandAll();
    pView->header()->setStretchLastSection(true);
    pView->header()->hide();
    pView->setItemsExpandable(false);
    pView->setRootIsDecorated(false);
    pView->setIndentation(10);
    location->setModel(m_pProxyModel);
    pView->show();
    location->setView(pView);

    CFKBookmarkNode* pMenu = m_pBookmarksMgr->GetMenu();
    QModelIndex idx = m_pProxyModel->mapFromSource(pModel->GetIndex(pMenu));
    pView->setCurrentIndex(idx);
    location->setCurrentIndex(idx.row());
    name->setText(title);
}
//-----------------------------------------------------
void CFKAddBookmarkDialog::accept()
{
    QModelIndex index = location->view()->currentIndex();
    index = m_pProxyModel->mapToSource(index);
    if (!index.isValid())
    {
        index = m_pBookmarksMgr->GetBookmarksModel()->index(0, 0);
    }
    CFKBookmarkNode *pParent = m_pBookmarksMgr->GetBookmarksModel()->GetNode(index);
    CFKBookmarkNode *pBookmark = new CFKBookmarkNode(CFKBookmarkNode::eNT_Bookmark);
    pBookmark->m_strUrl = m_strUrl;
    pBookmark->m_strTitle = name->text();
    m_pBookmarksMgr->AddBookmark(pParent, pBookmark);
    QDialog::accept();
}
//-----------------------------------------------------
