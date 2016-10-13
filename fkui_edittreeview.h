/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_EDITTREEVIEW_H
#define FKUI_EDITTREEVIEW_H
//-----------------------------------------------------
#include <QtWidgets/QTreeView>
//-----------------------------------------------------
class CFKUI_EditTreeView : public QTreeView
{
    Q_OBJECT
public:
    CFKUI_EditTreeView(QWidget* pParent = NULL);
public:
    void keyPressEvent(QKeyEvent* pEvent);
public slots:
    void SLOT_RemoveOne();
    void SLOT_RemoveAll();
};
//-----------------------------------------------------
#endif // FKUI_EDITTREEVIEW_H
