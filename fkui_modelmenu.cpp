/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QDebug>

#include "fkui_modelmenu.h"
//-----------------------------------------------------
Q_DECLARE_METATYPE(QModelIndex)
//-----------------------------------------------------
CFKUI_ModelMenu::CFKUI_ModelMenu(QWidget* pParent)
    : QMenu(pParent)
    , m_nMaxRows(7)
    , m_nFirstSeparator(-1)
    , m_nMaxWidth(-1)
    , m_nHoverRole(0)
    , m_nSeparatorRole(0)
    , m_pModel(NULL)
{
    connect(this, SIGNAL(aboutToShow()),
            this, SLOT(__SLOT_AboutToShow()));
}
//-----------------------------------------------------
void CFKUI_ModelMenu::SetModel(QAbstractItemModel* pModel)
{
    m_pModel = pModel;
}
//-----------------------------------------------------
QAbstractItemModel* CFKUI_ModelMenu::GetModel() const
{
    return m_pModel;
}
//-----------------------------------------------------
void CFKUI_ModelMenu::SetMaxRows(int nMaxRows)
{
    m_nMaxRows = nMaxRows;
}
//-----------------------------------------------------
int CFKUI_ModelMenu::GetMaxRows() const
{
    return m_nMaxRows;
}
//-----------------------------------------------------
void CFKUI_ModelMenu::SetFirstSeparator(int nOffset)
{
    m_nFirstSeparator = nOffset;
}
//-----------------------------------------------------
int CFKUI_ModelMenu::GetFirstSeparator() const
{
    return m_nFirstSeparator;
}
//-----------------------------------------------------
void CFKUI_ModelMenu::SetRootIndex(const QModelIndex& index)
{
    m_Root = index;
}
//-----------------------------------------------------
QModelIndex CFKUI_ModelMenu::GetRootIndex() const
{
    return m_Root;
}
//-----------------------------------------------------
void CFKUI_ModelMenu::SetHoverRole(int nRole)
{
    m_nHoverRole = nRole;
}
//-----------------------------------------------------
int CFKUI_ModelMenu::GetHoverRole() const
{
    return m_nHoverRole;
}
//-----------------------------------------------------
void CFKUI_ModelMenu::SetSeparatorRole(int nRole)
{
    m_nSeparatorRole = nRole;
}
//-----------------------------------------------------
int CFKUI_ModelMenu::GetSeparatorRole() const
{
    return m_nSeparatorRole;
}
//-----------------------------------------------------
QAction* CFKUI_ModelMenu::MakeAction(const QIcon& icon,
                        const QString& text, QObject* pParent)
{
    QFontMetrics fm(font());
    if (-1 == m_nMaxWidth)
    {
        m_nMaxWidth = fm.width(QLatin1Char('m')) * 30;
    }
    QString strSmallText = fm.elidedText(text, Qt::ElideMiddle, m_nMaxWidth);
    return new QAction(icon, strSmallText, pParent);
}
//-----------------------------------------------------
QAction* CFKUI_ModelMenu::MakeAction(const QModelIndex& index)
{
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QAction* pAction = MakeAction(icon, index.data().toString(), this);
    QVariant v;
    v.setValue(index);
    pAction->setData(v);
    return pAction;
}
//-----------------------------------------------------
bool CFKUI_ModelMenu::prePopulated()
{
    return false;
}
//-----------------------------------------------------
void CFKUI_ModelMenu::postPopulated()
{

}
//-----------------------------------------------------
void CFKUI_ModelMenu::_CreateMenu(const QModelIndex &parent,
                      int nMax, QMenu* pParentMenu, QMenu* pMenu)
{
    if (!pMenu)
    {
        QString strTitle = parent.data().toString();
        pMenu = new QMenu(strTitle, this);
        QIcon icon = qvariant_cast<QIcon>(parent.data(Qt::DecorationRole));
        pMenu->setIcon(icon);
        pParentMenu->addMenu(pMenu);

        QVariant v;
        v.setValue(parent);
        pMenu->menuAction()->setData(v);
        connect(pMenu, SIGNAL(aboutToShow()),
                this, SLOT(__SLOT_AboutToShow()));
        return;
    }

    int nEnd = m_pModel->rowCount(parent);
    if (nMax != -1)
    {
        nEnd = qMin(nMax, nEnd);
    }

    connect(pMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(__SLOT_Triggered(QAction*)));
    connect(pMenu, SIGNAL(hovered(QAction*)),
            this, SLOT(__SLOT_Hovered(QAction*)));

    for (int i = 0; i < nEnd; ++i)
    {
        QModelIndex idx = m_pModel->index(i, 0, parent);
        if (m_pModel->hasChildren(idx))
        {
            _CreateMenu(idx, -1, pMenu);
        }
        else
        {
            if (m_nSeparatorRole != 0
                && idx.data(m_nSeparatorRole).toBool())
            {
                addSeparator();
            }
            else
            {
                pMenu->addAction(MakeAction(idx));
            }
        }
        if (pMenu == this && i == m_nFirstSeparator - 1)
        {
            addSeparator();
        }
    }
}
//-----------------------------------------------------
void CFKUI_ModelMenu::__SLOT_AboutToShow()
{
    if (QMenu *menu = qobject_cast<QMenu*>(sender()))
    {
        QVariant v = menu->menuAction()->data();
        if (v.canConvert<QModelIndex>())
        {
            QModelIndex idx = qvariant_cast<QModelIndex>(v);
            _CreateMenu(idx, -1, menu, menu);
            disconnect(menu, SIGNAL(aboutToShow()),
                       this, SLOT(__SLOT_AboutToShow()));
            return;
        }
    }

    clear();

    if (prePopulated())
    {
        addSeparator();
    }
    int nMaxRows = m_nMaxRows;
    if (nMaxRows != -1)
    {
        nMaxRows += m_nFirstSeparator;
    }
    _CreateMenu(m_Root, nMaxRows, this, this);
    postPopulated();
}
//-----------------------------------------------------
void CFKUI_ModelMenu::__SLOT_Triggered(QAction* pAction)
{
    QVariant v = pAction->data();
    if (v.canConvert<QModelIndex>())
    {
        QModelIndex idx = qvariant_cast<QModelIndex>(v);
        emit SIGNAL_Activated(idx);
    }
}
//-----------------------------------------------------
void CFKUI_ModelMenu::__SLOT_Hovered(QAction* pAction)
{
    QVariant v = pAction->data();
    if (v.canConvert<QModelIndex>())
    {
        QModelIndex idx = qvariant_cast<QModelIndex>(v);
        QString strHoveredString = idx.data(m_nHoverRole).toString();
        if (!strHoveredString.isEmpty())
        {
            emit SIGNAL_Hovered(strHoveredString);
        }
    }
}
//-----------------------------------------------------
