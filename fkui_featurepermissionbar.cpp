/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QPropertyAnimation>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
//-----------------------------------------------------
#include "fkui_featurepermissionbar.h"
//-----------------------------------------------------
static const int s_nDefaultHeight = 30;
//-----------------------------------------------------
static QString s_TextForPermissionType(QWebEnginePage::Feature eType)
{
    switch (eType)
    {
    case QWebEnginePage::Notifications:
        return QObject::tr("use desktop notifications");
    case QWebEnginePage::Geolocation:
        return QObject::tr("use your position");
    case QWebEnginePage::MediaAudioCapture:
        return QObject::tr("use your microphone");
    case QWebEnginePage::MediaVideoCapture:
        return QObject::tr("use your camera");
    case QWebEnginePage::MediaAudioVideoCapture:
        return QObject::tr("use your camera and microphone");
    case QWebEnginePage::MouseLock:
        return QObject::tr("lock your mouse");
    default:
        Q_UNREACHABLE();
    }
    return QString();
}
//-----------------------------------------------------
CFKUI_FeaturePermissionBar::CFKUI_FeaturePermissionBar(QWidget* pParent)
    : QWidget(pParent)
{
    m_pMessageLabel = new QLabel(this);

    setAutoFillBackground(true);
    QHBoxLayout *pLayout = new QHBoxLayout;
    setLayout(pLayout);
    pLayout->setContentsMargins(s_nDefaultHeight, 0, 0, 0);
    pLayout->addWidget(m_pMessageLabel);
    pLayout->addStretch();

    QPushButton *pAllowButton   = new QPushButton(tr("Allow"), this);
    QPushButton *pDenyButton    = new QPushButton(tr("Deny"), this);
    QPushButton *pDiscardButton = new QPushButton(QIcon(QStringLiteral(":closetab.png")), QString(), this);
    connect(pAllowButton,   &QPushButton::clicked, this, &CFKUI_FeaturePermissionBar::__SLOT_PermissionGranted);
    connect(pDenyButton,    &QPushButton::clicked, this, &CFKUI_FeaturePermissionBar::__SLOT_PermissionDenied);
    connect(pDiscardButton, &QPushButton::clicked, this, &CFKUI_FeaturePermissionBar::__SLOT_PermissionUnknown);
    connect(pAllowButton,   &QPushButton::clicked, this, &QObject::deleteLater);
    connect(pDenyButton,    &QPushButton::clicked, this, &QObject::deleteLater);
    connect(pDiscardButton, &QPushButton::clicked, this, &QObject::deleteLater);
    pLayout->addWidget(pDenyButton);
    pLayout->addWidget(pAllowButton);
    pLayout->addWidget(pDiscardButton);

    setGeometry(0, -s_nDefaultHeight, pParent->width(), s_nDefaultHeight);
}
//-----------------------------------------------------
void CFKUI_FeaturePermissionBar::RequestPermission(const QUrl& url, QWebEnginePage::Feature eFeature)
{
    m_SecurityOrigin    = url;
    m_eFeature          = eFeature;
    m_pMessageLabel->setText(tr("%1 wants to %2.").arg(url.host()).arg(s_TextForPermissionType(eFeature)));
    show();
    // 淡入
    QPropertyAnimation* pAnimation = new QPropertyAnimation(this);
    pAnimation->setTargetObject(this);
    pAnimation->setPropertyName(QByteArrayLiteral("pos"));
    pAnimation->setDuration(300);
    pAnimation->setStartValue(QVariant::fromValue(pos()));
    pAnimation->setEndValue(QVariant::fromValue(QPoint(0,0)));
    pAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    pAnimation->start(QPropertyAnimation::DeleteWhenStopped);
}
//-----------------------------------------------------
void CFKUI_FeaturePermissionBar::__SLOT_PermissionDenied()
{
    emit SIGNAL_FeaturePermissionProvided(m_SecurityOrigin, m_eFeature,
                                   QWebEnginePage::PermissionDeniedByUser);
}
//-----------------------------------------------------
void CFKUI_FeaturePermissionBar::__SLOT_PermissionGranted()
{
    emit SIGNAL_FeaturePermissionProvided(m_SecurityOrigin, m_eFeature,
                                   QWebEnginePage::PermissionGrantedByUser);
}
//-----------------------------------------------------
void CFKUI_FeaturePermissionBar::__SLOT_PermissionUnknown()
{
    emit SIGNAL_FeaturePermissionProvided(m_SecurityOrigin, m_eFeature,
                                   QWebEnginePage::PermissionUnknown);
}
//-----------------------------------------------------
