/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKHISTORYMENU_H
#define FKHISTORYMENU_H
//-----------------------------------------------------
#include <QtCore/QUrl>
//-----------------------------------------------------
#include "fkui_modelmenu.h"
//-----------------------------------------------------
class CFKHistoryMgr;
class CFKHistoryMenuModel;
//-----------------------------------------------------
class CFKHistoryMenu : public CFKUI_ModelMenu
{
    Q_OBJECT
signals:
    void        SIGNAL_OpenUrl(const QUrl& url);
public:
    CFKHistoryMenu(QWidget* pParent = NULL);
public:
    void        SetInitActions(QList<QAction*> listActions);
protected:
    bool        prePopulated();
    void        postPopulated();
private slots:
    void        __SLOT_Activated(const QModelIndex &index);
    void        __SLOT_ShowHistoryDialog();
private:
    CFKHistoryMgr*          m_pHistoryMgr;
    CFKHistoryMenuModel*    m_pHistoryMenuModel;
    QList<QAction*>         m_InitActions;
};
//-----------------------------------------------------
#endif // FKHISTORYMENU_H
