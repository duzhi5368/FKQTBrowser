/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtWidgets/QStyleOptionFrame>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCompleter>

#include <QtGui/QPainter>
#include <QtGui/QFocusEvent>

#include <QtCore/QEvent>
//-----------------------------------------------------
#include "fkui_exlineedit.h"
#include "fkui_clearbutton.h"
//-----------------------------------------------------
CFKUI_ExLineEdit::CFKUI_ExLineEdit(QWidget* pParent)
    : QWidget(pParent)
    , m_pLeftWidget( NULL )
{
    m_pLineEdit =  new QLineEdit(this);

    setFocusPolicy(m_pLineEdit->focusPolicy());
    setAttribute(Qt::WA_InputMethodEnabled);
    setSizePolicy(m_pLineEdit->sizePolicy());
    setBackgroundRole(m_pLineEdit->backgroundRole());
    setMouseTracking(true);
    setAcceptDrops(true);
    setAttribute(Qt::WA_MacShowFocusRect, true);
    QPalette tagLineEidtPalatte = m_pLineEdit->palette();
    setPalette(tagLineEidtPalatte);

    // 设置LineEdit属性
    m_pLineEdit->setFrame(false);
    m_pLineEdit->setFocusProxy(this);
    m_pLineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    QPalette tagClearPalette = m_pLineEdit->palette();
    tagClearPalette.setBrush(QPalette::Base, QBrush(Qt::transparent));
    m_pLineEdit->setPalette(tagClearPalette);

    // 设置clearButton属性
    m_pClearButton = new CFKUI_ClearButton(this);
    connect(m_pClearButton, SIGNAL(clicked()),
            m_pLineEdit, SLOT(clear()));
    connect(m_pLineEdit, SIGNAL(textChanged(QString)),
            m_pClearButton, SLOT(__SLOT_TextChanged(QString)));
}
//-----------------------------------------------------
QLineEdit* CFKUI_ExLineEdit::GetLineEdit() const
{
    return m_pLineEdit;
}
//-----------------------------------------------------
void CFKUI_ExLineEdit::SetLeftWidget(QWidget* pWidget)
{
    m_pLeftWidget = pWidget;
}
//-----------------------------------------------------
QWidget* CFKUI_ExLineEdit::GetLeftWidget() const
{
    return m_pLeftWidget;
}
//-----------------------------------------------------
QSize CFKUI_ExLineEdit::sizeHint() const
{
    m_pLineEdit->setFrame(true);
    QSize tagSize = m_pLineEdit->sizeHint();
    m_pLineEdit->setFrame(false);
    return tagSize;
}
//-----------------------------------------------------
QVariant CFKUI_ExLineEdit::inputMethodQuery(Qt::InputMethodQuery eProperty) const
{
    return m_pLineEdit->inputMethodQuery(eProperty);
}
//-----------------------------------------------------
void CFKUI_ExLineEdit::focusInEvent(QFocusEvent* pEvent)
{
    m_pLineEdit->event(pEvent);
    QWidget::focusInEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_ExLineEdit::focusOutEvent(QFocusEvent* pEvent)
{
    m_pLineEdit->event(pEvent);

    if (m_pLineEdit->completer())
    {
        connect(m_pLineEdit->completer(), SIGNAL(activated(QString)),
                         m_pLineEdit, SLOT(setText(QString)));
        connect(m_pLineEdit->completer(), SIGNAL(highlighted(QString)),
                         m_pLineEdit, SLOT(_q_completionHighlighted(QString)));
    }
    QWidget::focusOutEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_ExLineEdit::keyPressEvent(QKeyEvent* pEvent)
{
    m_pLineEdit->event(pEvent);
}
//-----------------------------------------------------
void CFKUI_ExLineEdit::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);

    QPainter p(this);
    QStyleOptionFrame panel;
    _InitStyleOption(&panel);
    style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &p, this);
}
//-----------------------------------------------------
void CFKUI_ExLineEdit::resizeEvent(QResizeEvent* pEvent)
{
    Q_ASSERT(m_pLeftWidget != NULL);
    _UpdateGeometries();
    QWidget::resizeEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_ExLineEdit::inputMethodEvent(QInputMethodEvent* pEvent)
{
    m_pLineEdit->event(pEvent);
}
//-----------------------------------------------------
bool CFKUI_ExLineEdit::event(QEvent* pEvent)
{
    if (pEvent->type() == QEvent::ShortcutOverride)
    {
        return m_pLineEdit->event(pEvent);
    }
    return QWidget::event(pEvent);
}
//-----------------------------------------------------
void CFKUI_ExLineEdit::_UpdateGeometries()
{
    QStyleOptionFrame tagPanel;
    _InitStyleOption(&tagPanel);
    QRect tagRect = style()->subElementRect(QStyle::SE_LineEditContents,
                                         &tagPanel, this);

    int nHeight             = tagRect.height();
    int nWidth              = tagRect.width();
    int nLeftWidgetHeight   = m_pLeftWidget->height();
    m_pLeftWidget->setGeometry(tagRect.x() + 2,
                              tagRect.y() + (nHeight - nLeftWidgetHeight)/2,
                              m_pLeftWidget->width(), m_pLeftWidget->height());

    int nClearButtonWidth = this->height();
    m_pLineEdit->setGeometry(m_pLeftWidget->x() + m_pLeftWidget->width(),        0,
                            nWidth - nClearButtonWidth - m_pLeftWidget->width(),
                            this->height());

    m_pClearButton->setGeometry(this->width() - nClearButtonWidth,
                               0,
                               nClearButtonWidth, this->height());
}
//-----------------------------------------------------
void CFKUI_ExLineEdit::_InitStyleOption(QStyleOptionFrame* pOption) const
{
    pOption->initFrom(this);
    pOption->rect = contentsRect();
    pOption->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, pOption, this);
    pOption->midLineWidth = 0;
    pOption->state |= QStyle::State_Sunken;
    if (m_pLineEdit->isReadOnly())
    {
        pOption->state |= QStyle::State_ReadOnly;
    }
    pOption->features = QStyleOptionFrame::None;
}
//-----------------------------------------------------
