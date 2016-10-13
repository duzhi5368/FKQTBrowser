/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_TABBAR_H
#define FKUI_TABBAR_H
//-----------------------------------------------------
#include <QtWidgets/QTabBar>
#include <QtWidgets/QShortcut>
//-----------------------------------------------------
class CFKUI_TabWidget;
//-----------------------------------------------------
class CFKUI_TabBar : public QTabBar
{
    friend class CFKUI_TabWidget;
    Q_OBJECT
public:
    CFKUI_TabBar(QWidget* pParent = NULL);
signals:
    void SIGNAL_NewTab();
    void SIGNAL_CloneTab(int nIndex);
    void SIGNAL_CloseTab(int nIndex);
    void SIGNAL_CloseOtherTabs(int nIndex);
    void SIGNAL_ReloadTab(int nIndex);
    void SIGNAL_MuteTab(int nIndex, bool bIsMute);
    void SIGNAL_ReloadAllTabs();
    void SIGNAL_TabMoveRequested(int nFromIndex, int nToIndex);
protected:
    void mousePressEvent(QMouseEvent* pEvent);
    void mouseMoveEvent(QMouseEvent* pEvent);
private slots:
    void __SLOT_SelectTabAction();
    void __SLOT_CloneTab();
    void __SLOT_CloseTab();
    void __SLOT_CloseOtherTabs();
    void __SLOT_ReloadTab();
    void __SLOT_MuteTab();
    void __SLOT_UnmuteTab();
    void __SLOT_ContextMenuRequested(const QPoint &position);
private:
    QList<QShortcut*>       m_ListTabShortcuts;
    QPoint                  m_tagDragStartPos;
    int                     m_nCurrntDargIndex;
};
//-----------------------------------------------------
#endif // FKUI_TABBAR_H
