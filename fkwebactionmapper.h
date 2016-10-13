/*
 * Author:Frankie.W
 * 这是一个代理对象，负责将单一浏览器动作和同一时间的其子页面动作进行连接
 * 例如：我们将MainWindows暂停时，我们要求当前的选项卡页面也执行暂停动作。
*/
//-----------------------------------------------------
#ifndef FKWEBACTIONMAPPER_H
#define FKWEBACTIONMAPPER_H
//-----------------------------------------------------
#include <QtCore/QObject>

#include <QWebEnginePage>
//-----------------------------------------------------
QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKWebActionMapper : public QObject
{
    Q_OBJECT
public:
    CFKWebActionMapper(QAction* pRoot, QWebEnginePage::WebAction eWebAction, QObject* pParent);
public:
    QWebEnginePage::WebAction   GetWebAction() const;
    void                        AddChild(QAction* pAction);
    void                        UpdateCurrent(QWebEnginePage* pCurrentParent);
private slots:
    void                        __SLOT_RootTriggered();
    void                        __SLOT_ChildChanged();
    void                        __SLOT_RootDestroyed();
    void                        __SLOT_CurrentDestroyed();
private:
    QWebEnginePage*             m_pCurrentParent;
    QAction*                    m_pRoot;
    QWebEnginePage::WebAction   m_eWebAction;
};
//-----------------------------------------------------
#endif // FKWEBACTIONMAPPER_H
