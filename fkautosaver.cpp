/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtDebug>

#include <QtCore>
//-----------------------------------------------------
#include "fkautosaver.h"
//-----------------------------------------------------
#define MAX_WAIT_TIME   1000*3
#define AUTO_SAVE_TIME  1000*15
//-----------------------------------------------------
CFKAutoSaver::CFKAutoSaver(QObject* pParent)
    : QObject(pParent)
{
    Q_ASSERT(pParent);
}
//-----------------------------------------------------
CFKAutoSaver::~CFKAutoSaver()
{
    if(m_tagTimer.isActive())
    {
        qWarning() << "CFKAutoSaver is still active when destroyed, changes not saved.";
    }
}
//-----------------------------------------------------
// 父类发生了某种更变
void CFKAutoSaver::SLOT_OnChange()
{
    if(m_tagFirstChangeTime.isNull())
        m_tagFirstChangeTime.start();

    if(m_tagFirstChangeTime.elapsed() > MAX_WAIT_TIME)
    {
        // 等待够了
        SaveIfNeccessary();
    }
    else
    {
        m_tagTimer.start(AUTO_SAVE_TIME, this);
    }
}
//-----------------------------------------------------
// 尝试保存
void CFKAutoSaver::SaveIfNeccessary()
{
    if(!m_tagTimer.isActive())
    {
        // 定时器无效或已经在进行保存
        return;
    }
    // 开始保存，关闭定时器
    m_tagTimer.stop();
    // 记录保存时间
    m_tagFirstChangeTime = QTime();

    // 调用父类保存函数
    if (!QMetaObject::invokeMethod(parent(), "SLOT_Save", Qt::DirectConnection))
    {
        qWarning() << "CFKAutoSaver: error invoking slot AutoSave() on parent";
    }
}
//-----------------------------------------------------
// 重载的定时器回调函数
void CFKAutoSaver::timerEvent(QTimerEvent* pEvent)
{
    if(pEvent->timerId() == m_tagTimer.timerId())
    {
        // 到达定时，尝试保存
        SaveIfNeccessary();
    }
    else
    {
        // 不是自己的消息，则向底层分发
        QObject::timerEvent(pEvent);
    }
}
//-----------------------------------------------------
