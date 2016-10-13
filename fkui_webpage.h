/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_WEBPAGE_H
#define FKUI_WEBPAGE_H
//-----------------------------------------------------
#include <QWebEnginePage>
//-----------------------------------------------------
class CFKBrowserMainWindow;
class CFKUI_WebView;

QT_BEGIN_NAMESPACE
class QWebEngineProfile;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKUI_WebPage : public QWebEnginePage
{
    friend class CFKUI_WebView;
    Q_OBJECT
public:
    CFKUI_WebPage(QWebEngineProfile* pProfile, QObject* pParent = NULL);
public:
    CFKBrowserMainWindow*   GetMainWindow();
protected:
    QWebEnginePage*         createWindow(QWebEnginePage::WebWindowType eType) Q_DECL_OVERRIDE;
    virtual bool            certificateError(const QWebEngineCertificateError &error) Q_DECL_OVERRIDE;
private slots:
    void                    __SLOT_HandleUnsupportedContext(QNetworkReply* pReply);
    void                    __SLOT_AuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth);
    void                    __SLOT_ProxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth, const QString &proxyHost);
private:
    Qt::KeyboardModifiers   m_eKeyboardModifiers;
    Qt::MouseButtons        m_ePressedButtons;
};
//-----------------------------------------------------
#endif // FKUI_WEBPAGE_H
