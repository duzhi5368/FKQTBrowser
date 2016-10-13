/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QWebEngineSettings>
#include <QWebEngineProfile>

#include <QtWidgets/QFontDialog>

#include <QtCore/QStandardPaths>
#include <QtCore/QSettings>
//-----------------------------------------------------
#include "fksettingdialog.h"
#include "fkcookiesdialog.h"
#include "fkui_webview.h"
#include "fkcookieexceptionsdialog.h"
#include "fkbrowserapp.h"
#include "fkhistorymgr.h"
#include "fkbrowsermainwindow.h"
//-----------------------------------------------------
static QString GetDefaultAcceptLanguage()
{
    const QStringList ListLangs = QLocale().uiLanguages();
    if (ListLangs.isEmpty())
    {
        return QString();
    }
    QString str         = ListLangs.first();
    const float fQstep  = 1.0f / float(ListLangs.count());
    float fQ            = 1.0f - fQstep;
    for (int i = 1; i < ListLangs.count(); ++i)
    {
        str += QStringLiteral(", ")
                + ListLangs.at(i)
                + QStringLiteral(";q=")
                + QString::number(fQ, 'f', 2);
        fQ -= fQstep;
    }
    return str;
}
//-----------------------------------------------------
CFKSettingDialog::CFKSettingDialog(QWidget* pParent)
    : QDialog(pParent)
{
    setupUi(this);
    connect(setHomeToCurrentPageButton, SIGNAL(clicked()),
            this, SLOT(__SLOT_SetHomeToCurrentPage()));
    connect(standardFontButton, SIGNAL(clicked()),
            this, SLOT(__SLOT_ChooseFont()));
    connect(fixedFontButton, SIGNAL(clicked()),
            this, SLOT(__SLOT_ChooseFixedFont()));

    __SLOT_LoadDefaults();
    __SLOT_LoadFromSettings();
}
//-----------------------------------------------------
void CFKSettingDialog::accept()
{
    __SLOT_SaveToSettings();
    QDialog::accept();
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_LoadDefaults()
{
    QWebEngineSettings *pDefaultSettings    = QWebEngineSettings::globalSettings();
    QString strStandardFontFamily           = pDefaultSettings->fontFamily(QWebEngineSettings::StandardFont);
    int nStandardFontSize                   = pDefaultSettings->fontSize(QWebEngineSettings::DefaultFontSize);
    m_StandardFont                          = QFont(strStandardFontFamily, nStandardFontSize);
    standardLabel->setText(QString(QLatin1String("%1 %2")).arg(m_StandardFont.family()).arg(m_StandardFont.pointSize()));

    QString strFixedFontFamily              = pDefaultSettings->fontFamily(QWebEngineSettings::FixedFont);
    int nFixedFontSize                      = pDefaultSettings->fontSize(QWebEngineSettings::DefaultFixedFontSize);
    m_FixedFont                             = QFont(strFixedFontFamily, nFixedFontSize);
    fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(m_FixedFont.family()).arg(m_FixedFont.pointSize()));

    downloadsLocation->setText(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    enableJavascript->setChecked(pDefaultSettings->testAttribute(QWebEngineSettings::JavascriptEnabled));
    enablePlugins->setChecked(pDefaultSettings->testAttribute(QWebEngineSettings::PluginsEnabled));
    enableScrollAnimator->setChecked(pDefaultSettings->testAttribute(QWebEngineSettings::ScrollAnimatorEnabled));
    persistentDataPath->setText(QWebEngineProfile::defaultProfile()->persistentStoragePath());
    sessionCookiesCombo->setCurrentIndex(QWebEngineProfile::defaultProfile()->persistentCookiesPolicy());
    httpUserAgent->setText(QWebEngineProfile::defaultProfile()->httpUserAgent());
    httpAcceptLanguage->setText(GetDefaultAcceptLanguage());

    if (!pDefaultSettings->testAttribute(QWebEngineSettings::AutoLoadIconsForPage))
        faviconDownloadMode->setCurrentIndex(0);
    else if (!pDefaultSettings->testAttribute(QWebEngineSettings::TouchIconsEnabled))
        faviconDownloadMode->setCurrentIndex(1);
    else
        faviconDownloadMode->setCurrentIndex(2);
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_LoadFromSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    const QString strDefaultHome = QLatin1String(CFKBrowserMainWindow::s_strDefaultHome);
    homeLineEdit->setText(settings.value(QLatin1String("home"), strDefaultHome).toString());
    settings.endGroup();

    settings.beginGroup(QLatin1String("history"));
    int nHistoryExpire = settings.value(QLatin1String("historyExpire")).toInt();
    int nIndex = 0;
    switch (nHistoryExpire)
    {
    case 1: nIndex = 0; break;
    case 7: nIndex = 1; break;
    case 14: nIndex = 2; break;
    case 30: nIndex = 3; break;
    case 365: nIndex = 4; break;
    case -1: nIndex = 5; break;
    default:
        nIndex = 5;
    }
    expireHistory->setCurrentIndex(nIndex);
    settings.endGroup();

    settings.beginGroup(QLatin1String("downloadmanager"));
    QString strDownloadDirectory = settings.value(QLatin1String("downloadDirectory"),
                   downloadsLocation->text()).toString();
    downloadsLocation->setText(strDownloadDirectory);
    settings.endGroup();

    settings.beginGroup(QLatin1String("general"));
    openLinksIn->setCurrentIndex(settings.value(QLatin1String("openLinksIn"),
                   openLinksIn->currentIndex()).toInt());
    settings.endGroup();

    // Appearance
    settings.beginGroup(QLatin1String("websettings"));
    m_FixedFont = qvariant_cast<QFont>(settings.value(QLatin1String("fixedFont"), m_FixedFont));
    m_StandardFont = qvariant_cast<QFont>(settings.value(QLatin1String("standardFont"), m_StandardFont));
    standardLabel->setText(QString(QLatin1String("%1 %2")).arg(m_StandardFont.family()).arg(m_StandardFont.pointSize()));
    fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(m_FixedFont.family()).arg(m_FixedFont.pointSize()));
    enableJavascript->setChecked(settings.value(QLatin1String("enableJavascript"), enableJavascript->isChecked()).toBool());
    enablePlugins->setChecked(settings.value(QLatin1String("enablePlugins"), enablePlugins->isChecked()).toBool());
    userStyleSheet->setPlainText(settings.value(QLatin1String("userStyleSheet")).toString());
    enableScrollAnimator->setChecked(settings.value(QLatin1String("enableScrollAnimator"), enableScrollAnimator->isChecked()).toBool());
    httpUserAgent->setText(settings.value(QLatin1String("httpUserAgent"), httpUserAgent->text()).toString());
    httpAcceptLanguage->setText(settings.value(QLatin1String("httpAcceptLanguage"), httpAcceptLanguage->text()).toString());
    faviconDownloadMode->setCurrentIndex(settings.value(QLatin1String("faviconDownloadMode"), faviconDownloadMode->currentIndex()).toInt());
    settings.endGroup();

    // Privacy
    settings.beginGroup(QLatin1String("cookies"));
    int nPersistentCookiesPolicy = settings.value(QLatin1String("persistentCookiesPolicy"), sessionCookiesCombo->currentIndex()).toInt();
    sessionCookiesCombo->setCurrentIndex(nPersistentCookiesPolicy);
    QString strPersistentDataPath = settings.value(QLatin1String("persistentDataPath"), persistentDataPath->text()).toString();
    persistentDataPath->setText(strPersistentDataPath);
    settings.endGroup();

    // Proxy
    settings.beginGroup(QLatin1String("proxy"));
    proxySupport->setChecked(settings.value(QLatin1String("enabled"), false).toBool());
    proxyType->setCurrentIndex(settings.value(QLatin1String("type"), 0).toInt());
    proxyHostName->setText(settings.value(QLatin1String("hostName")).toString());
    proxyPort->setValue(settings.value(QLatin1String("port"), 1080).toInt());
    proxyUserName->setText(settings.value(QLatin1String("userName")).toString());
    proxyPassword->setText(settings.value(QLatin1String("password")).toString());
    settings.endGroup();
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_SaveToSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    settings.setValue(QLatin1String("home"), homeLineEdit->text());
    settings.endGroup();

    settings.beginGroup(QLatin1String("general"));
    settings.setValue(QLatin1String("openLinksIn"), openLinksIn->currentIndex());
    settings.endGroup();

    settings.beginGroup(QLatin1String("history"));
    int nHistoryExpire = expireHistory->currentIndex();
    int nIndex = -1;
    switch (nHistoryExpire)
    {
    case 0: nIndex = 1; break;
    case 1: nIndex = 7; break;
    case 2: nIndex = 14; break;
    case 3: nIndex = 30; break;
    case 4: nIndex = 365; break;
    case 5: nIndex = -1; break;
    }
    settings.setValue(QLatin1String("historyExpire"), nIndex);
    settings.endGroup();

    // Appearance
    settings.beginGroup(QLatin1String("websettings"));
    settings.setValue(QLatin1String("fixedFont"),           m_FixedFont);
    settings.setValue(QLatin1String("standardFont"),        m_StandardFont);
    settings.setValue(QLatin1String("enableJavascript"),    enableJavascript->isChecked());
    settings.setValue(QLatin1String("enablePlugins"),       enablePlugins->isChecked());
    settings.setValue(QLatin1String("enableScrollAnimator"), enableScrollAnimator->isChecked());
    settings.setValue(QLatin1String("userStyleSheet"),      userStyleSheet->toPlainText());
    settings.setValue(QLatin1String("httpUserAgent"),       httpUserAgent->text());
    settings.setValue(QLatin1String("httpAcceptLanguage"),  httpAcceptLanguage->text());
    settings.setValue(QLatin1String("faviconDownloadMode"), faviconDownloadMode->currentIndex());
    settings.endGroup();

    //Privacy
    settings.beginGroup(QLatin1String("cookies"));
    int nPersistentCookiesPolicy    = sessionCookiesCombo->currentIndex();
    settings.setValue(QLatin1String("persistentCookiesPolicy"), nPersistentCookiesPolicy);
    QString strPersistentdataPath   = persistentDataPath->text();
    settings.setValue(QLatin1String("persistentDataPath"), strPersistentdataPath);
    settings.endGroup();

    // proxy
    settings.beginGroup(QLatin1String("proxy"));
    settings.setValue(QLatin1String("enabled"),     proxySupport->isChecked());
    settings.setValue(QLatin1String("type"),        proxyType->currentIndex());
    settings.setValue(QLatin1String("hostName"),    proxyHostName->text());
    settings.setValue(QLatin1String("port"),        proxyPort->text());
    settings.setValue(QLatin1String("userName"),    proxyUserName->text());
    settings.setValue(QLatin1String("password"),    proxyPassword->text());
    settings.endGroup();

    CFKBrowserApp::GetAppInstancePtr()->LoadSettingConfig();
    CFKBrowserApp::GetHistoryMgrPtr()->SLOT_LoadSettings();
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_SetHomeToCurrentPage()
{
    CFKBrowserMainWindow *mw = static_cast<CFKBrowserMainWindow*>(parent());
    CFKUI_WebView *pWebView = mw->GetCurrentTab();
    if (pWebView)
    {
        homeLineEdit->setText(pWebView->GetUrl().toString());
    }
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_ShowCookies()
{
    CFKCookiesDialog *pDialog = new CFKCookiesDialog(CFKBrowserApp::GetCookieJarMgrPtr(), this);
    pDialog->exec();
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_ShowExceptions()
{
    CFKCookieExceptionsDialog *pDialog = new CFKCookieExceptionsDialog(CFKBrowserApp::GetCookieJarMgrPtr(), this);
    pDialog->exec();
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_ChooseFont()
{
    bool bIsOk;
    QFont font = QFontDialog::getFont(&bIsOk, m_StandardFont, this);
    if ( bIsOk )
    {
        m_StandardFont = font;
        standardLabel->setText(QString(QLatin1String("%1 %2")).arg(font.family()).arg(font.pointSize()));
    }
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_ChooseFixedFont()
{
    bool bIsOk;
    QFont font = QFontDialog::getFont(&bIsOk, m_FixedFont, this);
    if ( bIsOk )
    {
        m_FixedFont = font;
        fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(font.family()).arg(font.pointSize()));
    }
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_OnHttpUserAgentEditingFinished()
{
    QWebEngineProfile::defaultProfile()->setHttpUserAgent(httpUserAgent->text());
}
//-----------------------------------------------------
void CFKSettingDialog::__SLOT_OnHttpAcceptLanguageEditingFinished()
{
    QWebEngineProfile::defaultProfile()->setHttpAcceptLanguage(httpAcceptLanguage->text());
}
//-----------------------------------------------------
