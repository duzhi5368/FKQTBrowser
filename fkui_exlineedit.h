/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_EXLINEEDIT_H
#define FKUI_EXLINEEDIT_H
//-----------------------------------------------------
#include <QtWidgets/QWidget>
//-----------------------------------------------------
QT_BEGIN_NAMESPACE
class QLineEdit;
class QStyleOptionFrame;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKUI_ClearButton;
//-----------------------------------------------------
class CFKUI_ExLineEdit : public QWidget
{
    Q_OBJECT
public:
    CFKUI_ExLineEdit(QWidget* pParent = NULL);
public:
    QLineEdit*          GetLineEdit() const;

    void                SetLeftWidget(QWidget* pWidget);
    QWidget*            GetLeftWidget() const;
protected:
    QSize               sizeHint() const;
    QVariant            inputMethodQuery(Qt::InputMethodQuery eProperty) const;
    void                focusInEvent(QFocusEvent* pEvent);
    void                focusOutEvent(QFocusEvent* pEvent);
    void                keyPressEvent(QKeyEvent* pEvent);
    void                paintEvent(QPaintEvent* pEvent);
    void                resizeEvent(QResizeEvent* pEvent);
    void                inputMethodEvent(QInputMethodEvent* pEvent);
    bool                event(QEvent* pEvent);
protected:
    void                _UpdateGeometries();
    void                _InitStyleOption(QStyleOptionFrame* pOption) const;
protected:
    QWidget*            m_pLeftWidget;
    QLineEdit*          m_pLineEdit;
    CFKUI_ClearButton*  m_pClearButton;
};
//-----------------------------------------------------
#endif // FKUI_EXLINEEDIT_H
