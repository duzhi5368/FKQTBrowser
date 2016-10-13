/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QTimer>
#include <QtCore/QPropertyAnimation>

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
//-----------------------------------------------------
#include "fkui_fullscreennotification.h"
//-----------------------------------------------------
CFKUI_FullScreenNotification::CFKUI_FullScreenNotification(QWidget* pParent)
    : QWidget(pParent)
    , m_nWidth(400)
    , m_nHeight(80)
    , m_nX((pParent->geometry().width() - m_nWidth) / 2)
    , m_nY(80)
{
    setVisible(false);
    setWindowFlags(Qt::ToolTip | Qt::WindowDoesNotAcceptFocus);

    QGridLayout* pLayout = new QGridLayout(this);

    m_pLabel = new QLabel(tr("You are now in fullscreen mode. Press ESC to quit!"), this);
    pLayout->addWidget(m_pLabel, 0, 0, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);

    setGeometry(m_nX, m_nY, m_nWidth, m_nHeight);

    setStyleSheet("background-color: white;\
        color: black;");

    m_pAnim = new QPropertyAnimation(this, "windowOpacity");
    connect(m_pAnim, SIGNAL(finished()), this, SLOT(__SLOT_FadeOutFinished()));
}
//-----------------------------------------------------
CFKUI_FullScreenNotification::~CFKUI_FullScreenNotification()
{

}
//-----------------------------------------------------
void CFKUI_FullScreenNotification::Show()
{
    setWindowOpacity(1.0);

    QTimer::singleShot(300, [&] {
        QWidget* pParent = parentWidget();
        m_nX = (pParent->geometry().width() - m_nWidth) / 2;
        QPoint tagTopLeft = pParent->mapToGlobal(QPoint(m_nX, m_nY));
        QWidget::move(tagTopLeft.x(), tagTopLeft.y());
        QWidget::show();
        QWidget::raise();
    });
    QTimer::singleShot(5000, this, SLOT(__SLOT_FadeOut()));
}
//-----------------------------------------------------
void CFKUI_FullScreenNotification::Hide()
{
    QWidget::hide();
    m_pAnim->stop();
}
//-----------------------------------------------------
void CFKUI_FullScreenNotification::__SLOT_FadeOut()
{
    m_pAnim->setDuration(800);
    m_pAnim->setStartValue(1.0);
    m_pAnim->setEndValue(0.0);
    m_pAnim->setEasingCurve(QEasingCurve::OutQuad);
    m_pAnim->start();
}
//-----------------------------------------------------
void CFKUI_FullScreenNotification::__SLOT_FadeOutFinished()
{
    QWidget::hide();
    setWindowOpacity(1.0);
}
//-----------------------------------------------------
