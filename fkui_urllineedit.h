/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_URLLINEEDIT_H
#define FKUI_URLLINEEDIT_H
//-----------------------------------------------------
#include <QtGui/QBrush>
#include <QtGui/QColor>
//-----------------------------------------------------
#include "fkui_exlineedit.h"
//-----------------------------------------------------
class CFKUI_WebView;
class CFKUI_UrlIconLabel;
//-----------------------------------------------------
class CFKUI_UrlLineEdit : public CFKUI_ExLineEdit
{
    Q_OBJECT
public:
    CFKUI_UrlLineEdit(QWidget* pParent = NULL);
public:
    void                SetWebView(CFKUI_WebView* pWebView);
    QLinearGradient     GenerateGradient(const QColor &color) const;
protected:
    void                paintEvent(QPaintEvent* pEvent);
    void                focusOutEvent(QFocusEvent* pEvent);
private slots:
    void                __SLOT_WebViewUrlChanged(const QUrl &url);
    void                __SLOT_WebViewIconChanged(const QIcon &icon);
private:
    CFKUI_WebView*      m_pWebView;
    CFKUI_UrlIconLabel* m_pIconLabel;
    QColor              m_DefaultBaseColor;
};
//-----------------------------------------------------
#endif // FKUI_URLLINEEDIT_H
