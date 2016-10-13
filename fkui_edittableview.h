/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_EDITTABLEVIEW_H
#define FKUI_EDITTABLEVIEW_H
//-----------------------------------------------------
#include <QtWidgets/QTableView>
//-----------------------------------------------------
class CFKUI_EditTableView : public QTableView
{
    Q_OBJECT
public:
    CFKUI_EditTableView(QWidget* pParent = NULL);
public:
    void    keyPressEvent(QKeyEvent* pEvent);
public slots:
    void    SLOT_RemoveOne();
    void    SLOT_RemoveAll();
};
//-----------------------------------------------------
#endif // FKUI_EDITTABLEVIEW_H
