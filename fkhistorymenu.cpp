/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkhistorymenu.h"
#include "fkhistorymodel.h"
#include "fkhistorymgr.h"
#include "fkbrowserapp.h"
#include "fkhistorymenumodel.h"
#include "fkhistorydialog.h"
//-----------------------------------------------------
CFKHistoryMenu::CFKHistoryMenu(QWidget* pParent)
    : CFKUI_ModelMenu(pParent)
    , m_pHistoryMgr(NULL)
{
    connect(this, SIGNAL(SIGNAL_Activated(QModelIndex)),
            this, SLOT(__SLOT_Activated(QModelIndex)));
    SetHoverRole(CFKHistoryModel::eUrlStringRole);
}
//-----------------------------------------------------
void CFKHistoryMenu::SetInitActions(QList<QAction*> listActions)
{
    m_InitActions = listActions;
    for (int i = 0; i < m_InitActions.count(); ++i)
    {
        addAction(m_InitActions.at(i));
    }
}
//-----------------------------------------------------
bool CFKHistoryMenu::prePopulated()
{
    if (!m_pHistoryMgr)
    {
        m_pHistoryMgr       = CFKBrowserApp::GetHistoryMgrPtr();
        m_pHistoryMenuModel = new CFKHistoryMenuModel(m_pHistoryMgr->GetHistoryTreeModel(), this);
        SetModel(m_pHistoryMenuModel);
    }
    // 初始化动作
    for (int i = 0; i < m_InitActions.count(); ++i)
    {
        addAction(m_InitActions.at(i));
    }
    if (!m_InitActions.isEmpty())
    {
        addSeparator();
    }
    SetFirstSeparator(m_pHistoryMenuModel->GetBumpedRows());

    return false;
}
//-----------------------------------------------------
void CFKHistoryMenu::postPopulated()
{
    if (m_pHistoryMgr->GetHistorys().count() > 0)
    {
        addSeparator();
    }

    QAction* pShowAllAction = new QAction(tr("Show All History"), this);
    connect(pShowAllAction, SIGNAL(triggered()),
            this, SLOT(__SLOT_ShowHistoryDialog()));
    addAction(pShowAllAction);

    QAction* pClearAction = new QAction(tr("Clear History"), this);
    connect(pClearAction, SIGNAL(triggered()),
            m_pHistoryMgr, SLOT(SLOT_Clear()));
    addAction(pClearAction);
}
//-----------------------------------------------------
void CFKHistoryMenu::__SLOT_Activated(const QModelIndex &index)
{
    emit SIGNAL_OpenUrl(index.data(CFKHistoryModel::eUrlRole).toUrl());
}
//-----------------------------------------------------
void CFKHistoryMenu::__SLOT_ShowHistoryDialog()
{
    CFKHistoryDialog* pDialog = new CFKHistoryDialog(this);
    connect(pDialog, SIGNAL(openUrl(QUrl)),
            this, SIGNAL(openUrl(QUrl)));
    pDialog->show();
}
//-----------------------------------------------------
