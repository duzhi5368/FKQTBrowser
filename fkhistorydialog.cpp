/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QClipboard>
//-----------------------------------------------------
#include "fkhistorydialog.h"
#include "fkhistorymgr.h"
#include "fkhistorymodel.h"
#include "fkbrowserapp.h"
#include "fktreeproxymodel.h"
#include "fkhistoryfiltermodel.h"
//-----------------------------------------------------
CFKHistoryDialog::CFKHistoryDialog(QWidget* pParent, CFKHistoryMgr* pHistoryMgr)
    : QDialog(pParent)
{
    CFKHistoryMgr *pHistory = pHistoryMgr;
    if (!pHistory)
    {
        pHistory = CFKBrowserApp::GetHistoryMgrPtr();
    }
    setupUi(this);

    tree->setUniformRowHeights(true);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree->setTextElideMode(Qt::ElideMiddle);

    QAbstractItemModel* pModel      = pHistory->GetHistoryFilterModel();
    CFKTreeProxyModel* pProxyModel  = new CFKTreeProxyModel(this);
    connect(search, SIGNAL(textChanged(QString)),
            pProxyModel, SLOT(setFilterFixedString(QString)));
    connect(removeButton, SIGNAL(clicked()),
            tree, SLOT(SLOT_RemoveOne()));
    connect(removeAllButton, SIGNAL(clicked()),
            tree, SLOT(SLOT_RemoveAll()));
    pProxyModel->setSourceModel(pModel);
    pProxyModel->setFilterKeyColumn(1);

    tree->setModel(pProxyModel);
    tree->setSortingEnabled(true);
    tree->setExpanded(pProxyModel->index(0, 0), true);
    tree->setAlternatingRowColors(true);

    QFontMetrics fm(font());
    int nReader = fm.width(QLatin1Char('m')) * 40;
    tree->header()->resizeSection(0, nReader);
    tree->header()->setStretchLastSection(true);
    connect(tree, SIGNAL(activated(QModelIndex)),
            this, SLOT(__SLOT_Open()));
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(__SLOT_CustomContextMenuRequested(QPoint)));
}
//-----------------------------------------------------
void CFKHistoryDialog::__SLOT_CustomContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
    QModelIndex index = tree->indexAt(pos);
    index = index.sibling(index.row(), 0);
    if (index.isValid()
            && !tree->model()->hasChildren(index))
    {
        menu.addAction(tr("Open"), this, SLOT(__SLOT_Open()));
        menu.addSeparator();
        menu.addAction(tr("Copy"), this, SLOT(__SLOT_Copy()));
    }
    menu.addAction(tr("Delete"), tree, SLOT(SLOT_RemoveOne()));
    menu.exec(QCursor::pos());
}
//-----------------------------------------------------
void CFKHistoryDialog::__SLOT_Open()
{
    QModelIndex index = tree->currentIndex();
    if (!index.parent().isValid())
        return;
    emit SIGNAL_OpenUrl(index.data(CFKHistoryModel::eUrlRole).toUrl());
}
//-----------------------------------------------------
void CFKHistoryDialog::__SLOT_Copy()
{
    QModelIndex index = tree->currentIndex();
    if (!index.parent().isValid())
    {
        return;
    }
    QString strUrl = index.data(CFKHistoryModel::eUrlStringRole).toString();
    QClipboard* pClipboard = QApplication::clipboard();
    pClipboard->setText(strUrl);
}
//-----------------------------------------------------
