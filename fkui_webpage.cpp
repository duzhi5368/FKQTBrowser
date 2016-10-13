/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QAuthenticator>
//-----------------------------------------------------
#include "ui_passworddialog.h"
#include "ui_proxy.h"
//-----------------------------------------------------
#include "fkui_webpage.h"
#include "fkui_webview.h"
#include "fkui_popupwindow.h"
#include "fkui_tabwidget.h"
#include "fkbrowserapp.h"
#include "fkbrowsermainwindow.h"
//-----------------------------------------------------
CFKUI_WebPage::CFKUI_WebPage(QWebEngineProfile* pProfile, QObject* pParent)
    : QWebEnginePage(pProfile, pParent)
    , m_eKeyboardModifiers(Qt::NoModifier)
    , m_ePressedButtons(Qt::NoButton)
{
    connect(this, SIGNAL(unsupportedContent(QNetworkReply*)),
            this, SLOT(__SLOT_HandleUnsupportedContext(QNetworkReply*)));
    connect(this, SIGNAL(authenticationRequired(const QUrl &, QAuthenticator*)),
            SLOT(__SLOT_AuthenticationRequired(const QUrl &, QAuthenticator*)));
    connect(this, SIGNAL(proxyAuthenticationRequired(const QUrl &, QAuthenticator *, const QString &)),
            SLOT(__SLOT_ProxyAuthenticationRequired(const QUrl &, QAuthenticator *, const QString &)));
}
//-----------------------------------------------------
CFKBrowserMainWindow* CFKUI_WebPage::GetMainWindow()
{
    QObject* pParent = this->parent();
    while (pParent)
    {
        if (CFKBrowserMainWindow* pMainWindow = qobject_cast<CFKBrowserMainWindow*>(pParent))
        {
            return pMainWindow;
        }
        pParent = pParent->parent();
    }
    return CFKBrowserApp::GetAppInstancePtr()->GetMainWindow();
}
//-----------------------------------------------------
QWebEnginePage* CFKUI_WebPage::createWindow(QWebEnginePage::WebWindowType eType)
{
    switch( eType )
    {
    case QWebEnginePage::WebBrowserTab:
        {
            return GetMainWindow()->GetTabWidget()->SLOT_NewTab()->page();
        }
        break;
    case QWebEnginePage::WebBrowserBackgroundTab:
        {
            return GetMainWindow()->GetTabWidget()->SLOT_NewTab(false)->page();
        }
        break;
    case QWebEnginePage::WebBrowserWindow:
        {
            CFKBrowserApp::GetAppInstancePtr()->CreateMainWindow();
            CFKBrowserMainWindow* pMainWindow = CFKBrowserApp::GetAppInstancePtr()->GetMainWindow();
            return pMainWindow->GetCurrentTab()->page();
        }
        break;
    default:
        {
            CFKUI_PopupWindow* pPopupWindow = new CFKUI_PopupWindow(profile());
            pPopupWindow->setAttribute(Qt::WA_DeleteOnClose);
            pPopupWindow->show();
            return pPopupWindow->GetPage();
        }
        break;
    }
}
//-----------------------------------------------------
bool CFKUI_WebPage::certificateError(const QWebEngineCertificateError &error)
{
    if (error.isOverridable())
    {
        QMessageBox tagMsgBox;
        tagMsgBox.setIcon(QMessageBox::Warning);
        tagMsgBox.setText(error.errorDescription());
        tagMsgBox.setInformativeText(tr("If you wish so, you may continue with an unverified certificate. "
                                     "Accepting an unverified certificate means "
                                     "you may not be connected with the host you tried to connect to.\n"
                                     "Do you wish to override the security check and continue?"));
        tagMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        tagMsgBox.setDefaultButton(QMessageBox::No);
        return tagMsgBox.exec() == QMessageBox::Yes;
    }
    QMessageBox::critical(view(), tr("Certificate Error"),
                          error.errorDescription(), QMessageBox::Ok,
                          QMessageBox::NoButton);
    return false;
}
//-----------------------------------------------------
void CFKUI_WebPage::__SLOT_HandleUnsupportedContext(QNetworkReply* pReply)
{
    Q_UNUSED(pReply);
    /*
    QString strErrorString = pReply->errorString();

    if (m_loadingUrl != pReply->url())
    {
        qWarning() << "Resource" << pReply->url().toEncoded()
                   << "has unknown Content-Type, will be ignored.";
        pReply->deleteLater();
        return;
    }

    if (pReply->error() == QNetworkReply::NoError &&
            !pReply->header(QNetworkRequest::ContentTypeHeader).isValid())
    {
        strErrorString = "Unknown Content-Type";
    }

    QFile tagFile(QLatin1String(":/notfound.html"));
    bool bIsOpened = tagFile.open(QIODevice::ReadOnly);
    Q_ASSERT(bIsOpened);
    Q_UNUSED(bIsOpened);

    QString strTitle = tr("Error loading page: %1").arg(pReply->url().toString());
    QString strHtml = QString(QLatin1String(tagFile.readAll()))
                        .arg(strTitle)
                        .arg(strErrorString)
                        .arg(pReply->url().toString());

    QBuffer pImageBuffer;
    pImageBuffer.open(QBuffer::ReadWrite);
    QIcon tagIcon = view()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, view());
    QPixmap tagPixmap = tagIcon.pixmap(QSize(32,32));
    if (tagPixmap.save(&pImageBuffer, "PNG"))
    {
        strHtml.replace(QLatin1String("IMAGE_BINARY_DATA_HERE"),
                     QString(QLatin1String(pImageBuffer.buffer().toBase64())));
    }

    QList<QWebEngineFrame*> ListFrames;
    ListFrames.append(mainFrame());
    while (!ListFrames.isEmpty())
    {
        QWebEngineFrame* pFrame = ListFrames.takeFirst();
        if (pFrame->url() == pReply->url())
        {
            pFrame->setHtml(strHtml, pReply->url());
            return;
        }
        QList<QWebEngineFrame *> ListChildren = pFrame->childFrames();
        foreach (QWebEngineFrame *pFrame, ListChildren)
        {
            ListFrames.append(pFrame);
        }
    }

    if (m_loadingUrl == pReply->url())
    {
        mainFrame()->setHtml(strHtml, pReply->url());
    }
    */
}
//-----------------------------------------------------
void CFKUI_WebPage::__SLOT_AuthenticationRequired(const QUrl &requestUrl, QAuthenticator *pAuth)
{
    CFKBrowserMainWindow* pMainWindow = CFKBrowserApp::GetAppInstancePtr()->GetMainWindow();

    QDialog dialog(pMainWindow);
    dialog.setWindowFlags(Qt::Sheet);

    Ui_PasswordDialog passwordDialog;
    passwordDialog.setupUi(&dialog);

    passwordDialog.iconLabel->setText(QString());
    passwordDialog.iconLabel->setPixmap(pMainWindow->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, pMainWindow).pixmap(32, 32));

    QString strIntroMessage = tr("<qt>Enter username and password for \"%1\" at %2</qt>");
    strIntroMessage = strIntroMessage.arg(pAuth->realm()).arg(requestUrl.toString().toHtmlEscaped());
    passwordDialog.introLabel->setText(strIntroMessage);
    passwordDialog.introLabel->setWordWrap(true);

    if (dialog.exec() == QDialog::Accepted)
    {
        pAuth->setUser(passwordDialog.userNameLineEdit->text());
        pAuth->setPassword(passwordDialog.passwordLineEdit->text());
    }
    else
    {
        *pAuth = QAuthenticator();
    }
}
//-----------------------------------------------------
void CFKUI_WebPage::__SLOT_ProxyAuthenticationRequired(const QUrl &requestUrl,
                                                           QAuthenticator* pAuth, const QString &proxyHost)
{
    Q_UNUSED(requestUrl);
    CFKBrowserMainWindow* pMainWindow = CFKBrowserApp::GetAppInstancePtr()->GetMainWindow();

    QDialog dialog(pMainWindow);
    dialog.setWindowFlags(Qt::Sheet);

    Ui_ProxyDialog proxyDialog;
    proxyDialog.setupUi(&dialog);

    proxyDialog.iconLabel->setText(QString());
    proxyDialog.iconLabel->setPixmap(pMainWindow->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, pMainWindow).pixmap(32, 32));

    QString strIntroMessage = tr("<qt>Connect to proxy \"%1\" using:</qt>");
    strIntroMessage = strIntroMessage.arg(proxyHost.toHtmlEscaped());
    proxyDialog.introLabel->setText(strIntroMessage);
    proxyDialog.introLabel->setWordWrap(true);

    if (dialog.exec() == QDialog::Accepted)
    {
        pAuth->setUser(proxyDialog.userNameLineEdit->text());
        pAuth->setPassword(proxyDialog.passwordLineEdit->text());
    }
    else
    {
        *pAuth = QAuthenticator();
    }
}
//-----------------------------------------------------
