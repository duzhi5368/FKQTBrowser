/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QStyle>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStyleOption>

#include <QtGui/QPainter>
//-----------------------------------------------------
#include "fkui_searchbutton.h"
#include "fkui_searchlineedit.h"
//-----------------------------------------------------
CFKUI_SearchLineEdit::CFKUI_SearchLineEdit(QWidget* pParent)
    : CFKUI_ExLineEdit(pParent)
{
    m_pSearchButton = new CFKUI_SearchButton(this);

    connect(GetLineEdit(), SIGNAL(textChanged(QString)),
            this, SIGNAL(SIGNAL_TextChanged(QString)));
    SetLeftWidget(m_pSearchButton);
    m_strInactiveText = tr("Search");

    QSizePolicy policy = sizePolicy();
    setSizePolicy(QSizePolicy::Preferred, policy.verticalPolicy());
}
//-----------------------------------------------------
QString CFKUI_SearchLineEdit::GetInactiveText() const
{
    return m_strInactiveText;
}
//-----------------------------------------------------
void CFKUI_SearchLineEdit::SetInactiveText(const QString& text)
{
    m_strInactiveText = text;
}
//-----------------------------------------------------
QMenu* CFKUI_SearchLineEdit::GetMenu() const
{
    if (!m_pSearchButton->m_pMenu)
    {
        m_pSearchButton->m_pMenu = new QMenu(m_pSearchButton);
        if (isVisible())
        {
            (const_cast<CFKUI_SearchLineEdit*>(this))->__UpdateGeometries();
        }
    }
    return m_pSearchButton->m_pMenu;
}
//-----------------------------------------------------
void CFKUI_SearchLineEdit::SetMenu(QMenu* pMenu)
{
    if (m_pSearchButton->m_pMenu)
    {
        m_pSearchButton->m_pMenu->deleteLater();
    }
    m_pSearchButton->m_pMenu = pMenu;

    __UpdateGeometries();
}
//-----------------------------------------------------
void CFKUI_SearchLineEdit::resizeEvent(QResizeEvent* pEvent)
{
    __UpdateGeometries();
    CFKUI_ExLineEdit::resizeEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_SearchLineEdit::paintEvent(QPaintEvent* pEvent)
{
    if (GetLineEdit()->text().isEmpty()
            && !hasFocus()
            && !m_strInactiveText.isEmpty())
    {
        CFKUI_ExLineEdit::paintEvent(pEvent);
        QStyleOptionFrame panel;
        _InitStyleOption(&panel);

        QRect tagRect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
        QFontMetrics fm = fontMetrics();
        int nHorizontalMargin = GetLineEdit()->x();
        QRect tagLineRect(nHorizontalMargin + tagRect.x(), tagRect.y() + (tagRect.height() - fm.height() + 1) / 2,
                       tagRect.width() - 2 * nHorizontalMargin, fm.height());
        QPainter painter(this);
        painter.setPen(palette().brush(QPalette::Disabled, QPalette::Text).color());
        painter.drawText(tagLineRect, Qt::AlignLeft|Qt::AlignVCenter, m_strInactiveText);
    }
    else
    {
        CFKUI_ExLineEdit::paintEvent(pEvent);
    }
}
//-----------------------------------------------------
void CFKUI_SearchLineEdit::__UpdateGeometries()
{
    int nMenuHeight = height();
    int nMenuWidth  = nMenuHeight + 1;
    if (!m_pSearchButton->m_pMenu)
    {
        nMenuWidth = (nMenuHeight / 5) * 4;
    }
    m_pSearchButton->resize(QSize(nMenuWidth, nMenuHeight));
}
//-----------------------------------------------------
