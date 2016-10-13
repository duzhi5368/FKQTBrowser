/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_CLEARBUTTON_H
#define FKUI_CLEARBUTTON_H
//-----------------------------------------------------
#include <QtWidgets/QAbstractButton>
//-----------------------------------------------------
class CFKUI_ClearButton : public QAbstractButton
{
    Q_OBJECT
public:
    CFKUI_ClearButton(QWidget* pParent = NULL);
    void paintEvent(QPaintEvent* pEvent);

public slots:
    void __SLOT_TextChanged(const QString& strText);
};
//-----------------------------------------------------
#endif // FKUI_CLOSEBUTTON_H
