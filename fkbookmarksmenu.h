/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBOOKMARKSMENU_H
#define FKBOOKMARKSMENU_H
//-----------------------------------------------------
#include <QtCore/QUrl>
//-----------------------------------------------------
#include "fkui_modelmenu.h"
//-----------------------------------------------------
class CFKBookmarksMgr;
//-----------------------------------------------------
class CFKBookmarksMenu : public CFKUI_ModelMenu
{
    Q_OBJECT
signals:
    void                SIGNAL_OpenUrl(const QUrl& url);
public:
    CFKBookmarksMenu(QWidget* pParent = NULL);
public:
    void                SetInitialActions(QList<QAction*> ListActions);
protected:
    bool                prePopulated();
private slots:
    void                __SLOT_Activated(const QModelIndex &index);
private:
    CFKBookmarksMgr*    m_pBookmarkMgr;
    QList<QAction*>     m_InitActions;
};
//-----------------------------------------------------
#endif // FKBOOKMARKSMENU_H
