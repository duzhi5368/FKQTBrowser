/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkbookmarksdialog.h"
#include "fkbookmarknode.h"
#include "fkbookmarksmodel.h"
#include "fkbookmarksmgr.h"
#include "fktreeproxymodel.h"
#include "fkbrowserapp.h"
//-----------------------------------------------------
CFKBookmarksDialog::CFKBookmarksDialog(QWidget* pParent, CFKBookmarksMgr* pMgr)
    : QDialog(pParent)
    , m_pBookmarksMgr(pMgr)
{
    if (!m_pBookmarksMgr)
    {
        m_pBookmarksMgr = CFKBrowserApp::GetBookmarksMgrPtr();
    }
    setupUi(this);

    tree->setUniformRowHeights(true);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree->setSelectionMode(QAbstractItemView::ContiguousSelection);
    tree->setTextElideMode(Qt::ElideMiddle);

    m_pBookmarksModel = m_pBookmarksMgr->GetBookmarksModel();
    m_pProxyModel = new CFKTreeProxyModel(this);
    connect(search, SIGNAL(SIGNAL_TextChanged(QString)),
            m_pProxyModel, SLOT(setFilterFixedString(QString)));
    connect(removeButton, SIGNAL(clicked()),
            tree, SLOT(SLOT_RemoveOne()));
    m_pProxyModel->setSourceModel(m_pBookmarksModel);
    tree->setModel(m_pProxyModel);
    tree->setDragDropMode(QAbstractItemView::InternalMove);
    tree->setExpanded(m_pProxyModel->index(0, 0), true);
    tree->setAlternatingRowColors(true);

    QFontMetrics fm(font());
    int nHeader = fm.width(QLatin1Char('m')) * 40;
    tree->header()->resizeSection(0, nHeader);
    tree->header()->setStretchLastSection(true);
    connect(tree, SIGNAL(activated(QModelIndex)),
            this, SLOT(open()));
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(__SLOT_CustomContextMenuRequested(QPoint)));
    connect(addFolderButton, SIGNAL(clicked()),
            this, SLOT(__SLOT_NewFolder()));
    __ExpandNodes(m_pBookmarksMgr->GetBookmarks());

    setAttribute(Qt::WA_DeleteOnClose);
}
//-----------------------------------------------------
CFKBookmarksDialog::~CFKBookmarksDialog()
{
    if (__SaveExpandedNodes(tree->rootIndex()))
    {
        m_pBookmarksMgr->ChangeExpanded();
    }
}
//-----------------------------------------------------
void CFKBookmarksDialog::open()
{
    QModelIndex index = tree->currentIndex();
    if (!index.parent().isValid())
        return;
    emit SIGNAL_OpenUrl(index.sibling(index.row(), 1).data(CFKBookmarksModel::eUrlRole).toUrl());
}
//-----------------------------------------------------
void CFKBookmarksDialog::__SLOT_CustomContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
    QModelIndex index = tree->indexAt(pos);
    index = index.sibling(index.row(), 0);
    if (index.isValid() &&
            !tree->model()->hasChildren(index))
    {
        menu.addAction(tr("Open"), this, SLOT(open()));
        menu.addSeparator();
    }
    menu.addAction(tr("Delete"), tree, SLOT(SLOT_RemoveOne()));
    menu.exec(QCursor::pos());
}
//-----------------------------------------------------
void CFKBookmarksDialog::__SLOT_NewFolder()
{
    QModelIndex currentIndex = tree->currentIndex();
    QModelIndex idx = currentIndex;
    if (idx.isValid() && !idx.model()->hasChildren(idx))
    {
        idx = idx.parent();
    }
    if (!idx.isValid())
    {
        idx = tree->rootIndex();
    }
    idx = m_pProxyModel->mapToSource(idx);
    CFKBookmarkNode *pParent = m_pBookmarksMgr->GetBookmarksModel()->GetNode(idx);
    CFKBookmarkNode *pNode = new CFKBookmarkNode(CFKBookmarkNode::eNT_Folder);
    pNode->m_strTitle = tr("New Folder");
    m_pBookmarksMgr->AddBookmark(pParent, pNode, currentIndex.row() + 1);
}
//-----------------------------------------------------
void CFKBookmarksDialog::__ExpandNodes(CFKBookmarkNode* pNode)
{
    for (int i = 0; i < pNode->GetChildrens().count(); ++i)
    {
        CFKBookmarkNode* pChildNode = pNode->GetChildrens()[i];
        if (pChildNode->m_bIsExpanded)
        {
            QModelIndex idx = m_pBookmarksModel->GetIndex(pChildNode);
            idx = m_pProxyModel->mapFromSource(idx);
            tree->setExpanded(idx, true);
            __ExpandNodes(pChildNode);
        }
    }
}
//-----------------------------------------------------
bool CFKBookmarksDialog::__SaveExpandedNodes(const QModelIndex& parent)
{
    bool bIsChanged = false;
    for (int i = 0; i < m_pProxyModel->rowCount(parent); ++i)
    {
        QModelIndex child           = m_pProxyModel->index(i, 0, parent);
        QModelIndex sourceIndex     = m_pProxyModel->mapToSource(child);
        CFKBookmarkNode* pChildNode = m_pBookmarksModel->GetNode(sourceIndex);
        bool bWasExpanded           = pChildNode->m_bIsExpanded;
        if (tree->isExpanded(child))
        {
            pChildNode->m_bIsExpanded = true;
            bIsChanged |= __SaveExpandedNodes(child);
        }
        else
        {
            pChildNode->m_bIsExpanded = false;
        }
        bIsChanged |= (bWasExpanded != pChildNode->m_bIsExpanded);
    }
    return bIsChanged;
}
//-----------------------------------------------------
