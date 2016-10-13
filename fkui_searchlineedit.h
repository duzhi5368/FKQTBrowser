/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_SEARCHLINEEDIT_H
#define FKUI_SEARCHLINEEDIT_H
//-----------------------------------------------------
#include <QtWidgets/QMenu>
//-----------------------------------------------------
#include "fkui_exlineedit.h"
//-----------------------------------------------------
class CFKUI_SearchButton;
//-----------------------------------------------------
class CFKUI_SearchLineEdit : public CFKUI_ExLineEdit
{
    Q_OBJECT
signals:
    void        SIGNAL_TextChanged(const QString& strText);
public:
    CFKUI_SearchLineEdit(QWidget* pParent = NULL);
public:
    QString     GetInactiveText() const;
    void        SetInactiveText(const QString& text);
    QMenu*      GetMenu() const;
    void        SetMenu(QMenu* pMenu);
protected:
    void        resizeEvent(QResizeEvent* pEvent);
    void        paintEvent(QPaintEvent* pEvent);
private:
    void        __UpdateGeometries();
private:
    QString             m_strInactiveText;
    CFKUI_SearchButton* m_pSearchButton;
};
//-----------------------------------------------------
#endif // FKUI_SEARCHLINEEDIT_H
