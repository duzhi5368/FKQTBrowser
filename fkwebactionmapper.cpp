/*
 * Author:Frankie.W
 * 这是一个代理对象，负责将单一浏览器动作和同一时间的其子页面动作进行连接
 * 例如：我们将MainWindows暂停时，我们要求当前的选项卡页面也执行暂停动作。
*/
//-----------------------------------------------------
#include <QtWidgets/QAction>
//-----------------------------------------------------
#include "fkwebactionmapper.h"
//-----------------------------------------------------
CFKWebActionMapper::CFKWebActionMapper(QAction* pRoot,
         QWebEnginePage::WebAction eWebAction, QObject* pParent)
    : QObject(pParent)
    , m_pCurrentParent(NULL)
    , m_pRoot(pRoot)
    , m_eWebAction(eWebAction)
{
    if(m_pRoot == NULL)
        return;

    connect(pRoot, SIGNAL(triggered()),
            this, SLOT(__SLOT_RootTriggered()));
    connect(pRoot, SIGNAL(destroyed(QObject*)),
            this, SLOT(__SLOT_RootDestroyed()));
    pRoot->setEnabled(false);
}
//-----------------------------------------------------
QWebEnginePage::WebAction CFKWebActionMapper::GetWebAction() const
{
    return m_eWebAction;
}
//-----------------------------------------------------
void CFKWebActionMapper::AddChild(QAction* pAction)
{
    if (pAction == NULL)
        return;
    connect(pAction, SIGNAL(changed()),
            this, SLOT(__SLOT_ChildChanged()));
}
//-----------------------------------------------------
void CFKWebActionMapper::UpdateCurrent(QWebEnginePage* pCurrentParent)
{
    if (m_pCurrentParent)
        disconnect(m_pCurrentParent, SIGNAL(destroyed(QObject*)),
                   this, SLOT(__SLOT_CurrentDestroyed()));
    m_pCurrentParent = pCurrentParent;

    if (NULL == m_pRoot)
        return;
    if (NULL == m_pCurrentParent)
    {
        m_pRoot->setEnabled(false);
        m_pRoot->setChecked(false);
        return;
    }

    QAction* pSource = m_pCurrentParent->action(m_eWebAction);
    m_pRoot->setChecked(pSource->isChecked());
    m_pRoot->setEnabled(pSource->isEnabled());
    connect(m_pCurrentParent, SIGNAL(destroyed(QObject*)),
            this, SLOT(__SLOT_CurrentDestroyed()));
}
//-----------------------------------------------------
void CFKWebActionMapper::__SLOT_RootTriggered()
{
    if (m_pCurrentParent)
    {
        QAction* pGotoAction = m_pCurrentParent->action(m_eWebAction);
        pGotoAction->trigger();
    }
}
//-----------------------------------------------------
void CFKWebActionMapper::__SLOT_ChildChanged()
{
    if (QAction* pSource = qobject_cast<QAction*>(sender()))
    {
        if (m_pRoot
            && m_pCurrentParent
            && pSource->parent() == m_pCurrentParent)
        {
            m_pRoot->setChecked(pSource->isChecked());
            m_pRoot->setEnabled(pSource->isEnabled());
        }
    }
}
//-----------------------------------------------------
void CFKWebActionMapper::__SLOT_RootDestroyed()
{
    m_pRoot = NULL;
}
//-----------------------------------------------------
void CFKWebActionMapper::__SLOT_CurrentDestroyed()
{
    UpdateCurrent(NULL);
}
//-----------------------------------------------------
