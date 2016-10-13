/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_SEARCHBUTTON_H
#define FKUI_SEARCHBUTTON_H
//-----------------------------------------------------
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QMenu>
//-----------------------------------------------------
class CFKUI_SearchButton : public QAbstractButton
{
    friend class CFKUI_SearchLineEdit;
public:
    CFKUI_SearchButton(QWidget* pParent = NULL);
public:
    void    paintEvent(QPaintEvent* pEvent);
protected:
    void    mousePressEvent(QMouseEvent* pEvent);
private:
    QMenu*  m_pMenu;
};
//-----------------------------------------------------
#endif // FKUI_SEARCHBUTTON_H
