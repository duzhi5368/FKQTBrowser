/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtWidgets/QMenu>
#include <QtWidgets/QApplication>

#include <QtCore/QPoint>
#include <QtCore/QMimeData>

#include <QtGui/QtEvents>
#include <QtGui/QDrag>
//-----------------------------------------------------
#include "fkui_tabbar.h"
//-----------------------------------------------------
CFKUI_TabBar::CFKUI_TabBar(QWidget* pParent)
    : QTabBar(pParent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(__SLOT_ContextMenuRequested(QPoint)));

    QString strCtrl = QLatin1String("Ctrl+%1");
    for (int i = 1; i < 10; ++i)
    {
        QShortcut* pShortCut = new QShortcut(strCtrl.arg(i), this);
        m_ListTabShortcuts.append(pShortCut);
        connect(pShortCut, SIGNAL(activated()),
                this, SLOT(__SLOT_SelectTabAction()));
    }
    setTabsClosable(true);
    connect(this, SIGNAL(tabCloseRequested(int)),
            this, SIGNAL(__SLOT_CloseTab(int)));
    setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
    setMovable(true);
}
//-----------------------------------------------------
void CFKUI_TabBar::mousePressEvent(QMouseEvent* pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        m_tagDragStartPos = pEvent->pos();
    }

    QTabBar::mousePressEvent(pEvent);

    // 鼠标中键点击，则关闭选项卡
    if (pEvent->button() == Qt::MiddleButton)
    {
        const QPoint tagPos = pEvent->pos();
        int nIndex = tabAt(tagPos);
        if (nIndex != -1)
        {
            emit SIGNAL_CloseTab(nIndex);
        }
    }
}
//-----------------------------------------------------
void CFKUI_TabBar::mouseMoveEvent(QMouseEvent* pEvent)
{
    if (pEvent->buttons() == Qt::LeftButton) {
        int nDiffX = pEvent->pos().x() - m_tagDragStartPos.x();
        int nDiffY = pEvent->pos().y() - m_tagDragStartPos.y();
        if ((pEvent->pos() - m_tagDragStartPos).manhattanLength() > QApplication::startDragDistance()
            && nDiffX < 3
            && nDiffX > -3
            && nDiffY < -10)
        {
            QDrag* pDrag = new QDrag(this);
            QMimeData* pMimeData = new QMimeData;
            QList<QUrl> ListUrls;
            int nIndex = tabAt(pEvent->pos());
            QUrl url = tabData(nIndex).toUrl();
            ListUrls.append(url);
            pMimeData->setUrls(ListUrls);
            pMimeData->setText(tabText(nIndex));
            pMimeData->setData(QLatin1String("action"), "tab-reordering");
            pDrag->setMimeData(pMimeData);
            pDrag->exec();
        }
    }
    QTabBar::mouseMoveEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_TabBar::__SLOT_SelectTabAction()
{
    if (QShortcut* pShortCut = qobject_cast<QShortcut*>(sender()))
    {
        int nIndex = m_ListTabShortcuts.indexOf(pShortCut);
        setCurrentIndex(nIndex);
    }
}
//-----------------------------------------------------
void CFKUI_TabBar::__SLOT_CloneTab()
{
    if (QAction* pAction = qobject_cast<QAction*>(sender()))
    {
        int nIndex = pAction->data().toInt();
        emit SIGNAL_CloneTab(nIndex);
    }
}
//-----------------------------------------------------
void CFKUI_TabBar::__SLOT_CloseTab()
{
    if (QAction* pAction = qobject_cast<QAction*>(sender()))
    {
        int nIndex = pAction->data().toInt();
        emit SIGNAL_CloseTab(nIndex);
    }
}
//-----------------------------------------------------
void CFKUI_TabBar::__SLOT_CloseOtherTabs()
{
    if (QAction* pAction = qobject_cast<QAction*>(sender()))
    {
        int nIndex = pAction->data().toInt();
        emit SIGNAL_CloseOtherTabs(nIndex);
    }
}
//-----------------------------------------------------
void CFKUI_TabBar::__SLOT_ReloadTab()
{
    if (QAction* pAction = qobject_cast<QAction*>(sender()))
    {
        int nIndex = pAction->data().toInt();
        emit SIGNAL_ReloadTab(nIndex);
    }
}
//-----------------------------------------------------
void CFKUI_TabBar::__SLOT_MuteTab()
{
    if (QAction* pAction = qobject_cast<QAction*>(sender()))
    {
        int nIndex = pAction->data().toInt();
        emit SIGNAL_MuteTab(nIndex, true);
    }
}
//-----------------------------------------------------
void CFKUI_TabBar::__SLOT_UnmuteTab()
{
    if (QAction* pAction = qobject_cast<QAction*>(sender()))
    {
        int nIndex = pAction->data().toInt();
        emit SIGNAL_MuteTab(nIndex, false);
    }
}
//-----------------------------------------------------
void CFKUI_TabBar::__SLOT_ContextMenuRequested(const QPoint &position)
{
    QMenu tagMenu;
    tagMenu.addAction(tr("New &Tab"), this, SIGNAL(SIGNAL_NewTab()), QKeySequence::AddTab);
    int nIndex = tabAt(position);
    if (-1 != nIndex)
    {
        QAction* pAction = tagMenu.addAction(tr("Clone Tab"),
                this, SLOT(__SLOT_CloneTab()));
        pAction->setData(nIndex);

        tagMenu.addSeparator();

        pAction = tagMenu.addAction(tr("&Close Tab"),
                this, SLOT(__SLOT_CloseTab()), QKeySequence::Close);
        pAction->setData(nIndex);

        pAction = tagMenu.addAction(tr("Close &Other Tabs"),
                this, SLOT(__SLOT_CloseOtherTabs()));
        pAction->setData(nIndex);

        tagMenu.addSeparator();

        pAction = tagMenu.addAction(tr("Reload Tab"),
                this, SLOT(__SLOT_ReloadTab()), QKeySequence::Refresh);
        pAction->setData(nIndex);

        pAction = tagMenu.addAction(tr("Mute tab"),
                this, SLOT(__SLOT_MuteTab()));
        pAction->setData(nIndex);

        pAction = tagMenu.addAction(tr("Unmute tab"),
                this, SLOT(__SLOT_UnmuteTab()));
        pAction->setData(nIndex);
    }
    else
    {
        tagMenu.addSeparator();
    }
    tagMenu.addAction(tr("Reload All Tabs"), this, SIGNAL(__SLOT_ReloadAllTabs()));
    tagMenu.exec(QCursor::pos());
}
//-----------------------------------------------------
