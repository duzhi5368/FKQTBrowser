/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_FULLSCREENNOTIFICATION_H
#define FKUI_FULLSCREENNOTIFICATION_H
//-----------------------------------------------------
#include <QWidget>
//-----------------------------------------------------
QT_BEGIN_NAMESPACE
class QLabel;
class QPropertyAnimation;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKUI_FullScreenNotification : public QWidget
{
    Q_OBJECT
public:
    CFKUI_FullScreenNotification(QWidget* pParent = NULL);
    ~CFKUI_FullScreenNotification();
public:
    void                Show();
    void                Hide();
private slots:
    void                __SLOT_FadeOut();
    void                __SLOT_FadeOutFinished();
private:
    QLabel*             m_pLabel;
    QPropertyAnimation* m_pAnim;
    int                 m_nWidth;
    int                 m_nHeight;
    int                 m_nX;
    int                 m_nY;
};
//-----------------------------------------------------
#endif // FKUI_FULLSCREENNOTIFICATION_H
