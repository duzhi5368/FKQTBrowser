/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_POPUPWINDOW_H
#define FKUI_POPUPWINDOW_H
//-----------------------------------------------------
#include <QtWidgets/QtWidgets>

#include <QWebEngineProfile>
#include <QWebEnginePage>
//-----------------------------------------------------
class CFKUI_WebView;
//-----------------------------------------------------
class CFKUI_PopupWindow : public QWidget
{
    Q_OBJECT
public:
    CFKUI_PopupWindow(QWebEngineProfile* pProfile);
public:
    QWebEnginePage*     GetPage() const;
private slots:
    void                __SLOT_SetUrl(const QUrl& url);
    void                __SLOT_AdjustGeometry(const QRect& newGeometry);
private:
    QLineEdit*          m_pAddressBar;
    CFKUI_WebView*      m_pWebView;
};
//-----------------------------------------------------
#endif // FKUI_POPUPWINDOW_H
