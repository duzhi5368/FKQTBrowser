/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkbookmarksmenu.h"
#include "fkbookmarksmgr.h"
#include "fkbookmarksmodel.h"
#include "fkbrowserapp.h"
//-----------------------------------------------------
CFKBookmarksMenu::CFKBookmarksMenu(QWidget* pParent)
    : CFKUI_ModelMenu(pParent)
    , m_pBookmarkMgr(NULL)
{
    connect(this, SIGNAL(SIGNAL_Activated(QModelIndex)),
            this, SLOT(__SLOT_Activated(QModelIndex)));
    SetMaxRows(-1);
    SetHoverRole(CFKBookmarksModel::eUrlStringRole);
    SetSeparatorRole(CFKBookmarksModel::eSeparatorRole);
}
//-----------------------------------------------------
void CFKBookmarksMenu::SetInitialActions(QList<QAction*> ListActions)
{
    m_InitActions = ListActions;
    for (int i = 0; i < m_InitActions.count(); ++i)
    {
        addAction(m_InitActions.at(i));
    }
}
//-----------------------------------------------------
bool CFKBookmarksMenu::prePopulated()
{
    m_pBookmarkMgr = CFKBrowserApp::GetBookmarksMgrPtr();
    SetModel(m_pBookmarkMgr->GetBookmarksModel());
    SetRootIndex(m_pBookmarkMgr->GetBookmarksModel()->index(1, 0));
    // 初始化动作
    for (int i = 0; i < m_InitActions.count(); ++i)
    {
        addAction(m_InitActions.at(i));
    }
    if (!m_InitActions.isEmpty())
    {
        addSeparator();
    }
    _CreateMenu(GetModel()->index(0, 0), 1, this);
    return true;
}
//-----------------------------------------------------
void CFKBookmarksMenu::__SLOT_Activated(const QModelIndex &index)
{
    emit SIGNAL_OpenUrl(index.data(CFKBookmarksModel::eUrlRole).toUrl());
}
//-----------------------------------------------------
