/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_FEATUREPERMISSIONBAR_H
#define FKUI_FEATUREPERMISSIONBAR_H
//-----------------------------------------------------
#include <QWidget>
#include <QWebEnginePage>

#include <QtCore/QUrl>
//-----------------------------------------------------
QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKUI_FeaturePermissionBar : public QWidget
{
    Q_OBJECT
public:
    CFKUI_FeaturePermissionBar(QWidget* pParent = NULL);
    void    RequestPermission(const QUrl& url, QWebEnginePage::Feature eFeature);

signals:
    void    SIGNAL_FeaturePermissionProvided(const QUrl &securityOrigin,
                   QWebEnginePage::Feature, QWebEnginePage::PermissionPolicy);

private slots:
    void    __SLOT_PermissionDenied();
    void    __SLOT_PermissionGranted();
    void    __SLOT_PermissionUnknown();
private:
    QWebEnginePage::Feature     m_eFeature;
    QLabel*                     m_pMessageLabel;
    QUrl                        m_SecurityOrigin;
};
//-----------------------------------------------------
#endif // FKUI_FEATUREPERMISSIONBAR_H
