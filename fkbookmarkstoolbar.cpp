/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QtEvents>

#include <QtCore/QMimeData>

#include <QtWidgets/QToolButton>
//-----------------------------------------------------
#include "fkbookmarkstoolbar.h"
#include "fkbookmarksmodel.h"
#include "fkbookmarknode.h"
#include "fkbookmarksmgr.h"
#include "fkui_modelmenu.h"
//-----------------------------------------------------
CFKBookmarksToolBar::CFKBookmarksToolBar(
        CFKBookmarksModel* pModel, QWidget* pParent)
    : QToolBar(tr("Bookmark"), pParent)
    , m_pBookmarksModel(pModel)
{
    connect(this, SIGNAL(actionTriggered(QAction*)),
            this, SLOT(__SLOT_Triggered(QAction*)));
    SetRootIndex(pModel->index(0, 0));
    connect(m_pBookmarksModel, SIGNAL(modelReset()),
            this, SLOT(__SLOT_Build()));
    connect(m_pBookmarksModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(__SLOT_Build()));
    connect(m_pBookmarksModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(__SLOT_Build()));
    connect(m_pBookmarksModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(__SLOT_Build()));
    setAcceptDrops(true);
}
//-----------------------------------------------------
void CFKBookmarksToolBar::SetRootIndex(const QModelIndex& index)
{
    m_Root = index;
    __SLOT_Build();
}
//-----------------------------------------------------
QModelIndex CFKBookmarksToolBar::GetRootIndex() const
{
    return m_Root;
}
//-----------------------------------------------------
void CFKBookmarksToolBar::dragEnterEvent(QDragEnterEvent* pEvent)
{
    const QMimeData* pMimeData = pEvent->mimeData();
    if (pMimeData->hasUrls())
    {
        pEvent->acceptProposedAction();
    }
    QToolBar::dragEnterEvent(pEvent);
}
//-----------------------------------------------------
void CFKBookmarksToolBar::dropEvent(QDropEvent* pEvent)
{
    const QMimeData* pMimeData = pEvent->mimeData();
    if (pMimeData->hasUrls() && pMimeData->hasText()) {
        QList<QUrl> ListUrls = pMimeData->urls();
        QAction* pAction = actionAt(pEvent->pos());
        QString strDropText;
        if (pAction){
            strDropText = pAction->text();
        }
        int nRow = -1;
        QModelIndex parentIndex = m_Root;
        for (int i = 0; i < m_pBookmarksModel->rowCount(m_Root); ++i)
        {
            QModelIndex idx = m_pBookmarksModel->index(i, 0, m_Root);
            QString strTitle = idx.data().toString();
            if (strTitle == strDropText)
            {
                nRow = i;
                if (m_pBookmarksModel->hasChildren(idx))
                {
                    parentIndex = idx;
                    nRow = -1;
                }
                break;
            }
        }
        CFKBookmarkNode* pBookmark  = new CFKBookmarkNode(CFKBookmarkNode::eNT_Bookmark);
        pBookmark->m_strUrl         = ListUrls.at(0).toString();
        pBookmark->m_strTitle       = pMimeData->text();

        CFKBookmarkNode *pParent        = m_pBookmarksModel->GetNode(parentIndex);
        CFKBookmarksMgr *pBookmarksMgr  = m_pBookmarksModel->GetBookmarkMgr();
        pBookmarksMgr->AddBookmark(pParent, pBookmark, nRow);
        pEvent->acceptProposedAction();
    }
    QToolBar::dropEvent(pEvent);
}
//-----------------------------------------------------
void CFKBookmarksToolBar::__SLOT_Triggered(QAction* pAction)
{
    QVariant v = pAction->data();
    if (v.canConvert<QUrl>())
    {
        emit SIGNAL_OpenUrl(v.toUrl());
    }
}
//-----------------------------------------------------
void CFKBookmarksToolBar::__SLOT_Activated(const QModelIndex& index)
{
    emit SIGNAL_OpenUrl(index.data(CFKBookmarksModel::eUrlRole).toUrl());
}
//-----------------------------------------------------
void CFKBookmarksToolBar::__SLOT_Build()
{
    clear();

    for (int i = 0; i < m_pBookmarksModel->rowCount(m_Root); ++i)
    {
        QModelIndex idx = m_pBookmarksModel->index(i, 0, m_Root);
        if (m_pBookmarksModel->hasChildren(idx))
        {
            QToolButton* pButton = new QToolButton(this);
            pButton->setPopupMode(QToolButton::InstantPopup);
            pButton->setArrowType(Qt::DownArrow);
            pButton->setText(idx.data().toString());
            CFKUI_ModelMenu *pMenu = new CFKUI_ModelMenu(this);
            connect(pMenu, SIGNAL(SIGNAL_Activated(QModelIndex)),
                    this, SLOT(__SLOT_Activated(QModelIndex)));
            pMenu->SetModel(m_pBookmarksModel);
            pMenu->SetRootIndex(idx);
            pMenu->addAction(new QAction(pMenu));
            pButton->setMenu(pMenu);
            pButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
            QAction *pAction = addWidget(pButton);
            pAction->setText(idx.data().toString());
        }
        else
        {
            QAction* pAction = addAction(idx.data().toString());
            pAction->setData(idx.data(CFKBookmarksModel::eUrlRole));
        }
    }
}
//-----------------------------------------------------
