/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_URLICONLABEL_H
#define FKUI_URLICONLABEL_H
//-----------------------------------------------------
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QLabel>
//-----------------------------------------------------
class CFKUI_WebView;
//-----------------------------------------------------
class CFKUI_UrlIconLabel : public QLabel
{
    friend class CFKUI_UrlLineEdit;
public:
    CFKUI_UrlIconLabel(QWidget* pParent);
protected:
    void mousePressEvent(QMouseEvent* pEvent);
    void mouseMoveEvent(QMouseEvent* pEvent);
private:
    CFKUI_WebView*          m_pWebView;
    QPoint                  m_tagDragStartPos;
};
//-----------------------------------------------------
#endif // FKUI_URLICONLABEL_H
