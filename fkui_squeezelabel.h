/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_SQUEEZELABEL_H
#define FKUI_SQUEEZELABEL_H
//-----------------------------------------------------
#include <QtWidgets/QLabel>
//-----------------------------------------------------
class CFKUI_SqueezeLabel : public QLabel
{
    Q_OBJECT
public:
    CFKUI_SqueezeLabel(QWidget* pParent = NULL);
protected:
    void paintEvent(QPaintEvent* pEvent);
};
//-----------------------------------------------------
#endif // FKUI_SQUEEZELABEL_H
