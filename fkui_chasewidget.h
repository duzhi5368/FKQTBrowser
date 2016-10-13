/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_CHASEWIDGET_H
#define FKUI_CHASEWIDGET_H
//-----------------------------------------------------
#include <QtWidgets/QWidget>

#include <QtGui/QPixmap>
//-----------------------------------------------------
class CFKUI_ChaseWidget : public QWidget
{
    Q_OBJECT
public:
    CFKUI_ChaseWidget(QWidget* pParent = NULL,
            QPixmap pixmap = QPixmap(), bool bIsPixmapEnabled = false);
public:
    void            SetAnimated(bool bValue);
    void            SetPixmapEnabled(bool bEnable);
public:
    QSize           sizeHint() const;
protected:
    void            paintEvent(QPaintEvent* pEvent);
    void            timerEvent(QTimerEvent* pEvent);
private:
    int             __GetSegmentCount() const;
    QColor          __ColorForSegment(int nSegment) const;
private:
    int             m_nSegment;
    int             m_nDelay;
    int             m_nStep;
    int             m_nTimerID;
    bool            m_bIsAnimated;
    bool            m_bIsPixmapEnabled;
    QPixmap         m_tagPixmap;
};
//-----------------------------------------------------
#endif // FKUI_CHASEWIDGET_H
