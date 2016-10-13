/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKAUTOSAVER_H
#define FKAUTOSAVER_H
//-----------------------------------------------------
#include <QtCore/QObject>
#include <QtCore/QBasicTimer>
#include <QtCore/QTime>
//-----------------------------------------------------
// 本类将会在父类发生更变时自动调用父类的 AutoSave() 槽函数。
// 注意：该函数不是立刻调用 AutoSave，而是在 Changed() 之后过几秒后再进行 AutoSave()
//      延迟的目的是等待多重修改完成，然后再写入硬盘。
class CFKAutoSaver : public QObject
{
    Q_OBJECT
public:
    CFKAutoSaver(QObject* pParent);
    ~CFKAutoSaver();
public slots:
    // 父类发生了某种更变
    void        SLOT_OnChange();
public:
    // 尝试保存
    void        SaveIfNeccessary();
protected:
    // 重载的定时器回调函数
    void        timerEvent(QTimerEvent* pEvent);
private:
    QBasicTimer m_tagTimer;
    QTime       m_tagFirstChangeTime;
};
//-----------------------------------------------------
#endif // FKAUTOSAVER_H
