/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QSettings>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include <QtGui/QDesktopServices>
//-----------------------------------------------------
#include "fkdownloadwidget.h"
//-----------------------------------------------------
#include "ui_downloaditem.h"
#include "ui_downloads.h"
//-----------------------------------------------------
CFKDownloadWidget::CFKDownloadWidget(QWebEngineDownloadItem* pDownloadItem, QWidget* pParent)
    : QWidget(pParent)
    , m_pDownloadItem(pDownloadItem)
    , m_llBytesReceived(0)
{
    setupUi(this);

    QPalette tagPaletter = downloadInfoLabel->palette();
    tagPaletter.setColor(QPalette::Text, Qt::darkGray);
    downloadInfoLabel->setPalette(tagPaletter);
    progressBar->setMaximum(0);
    connect(stopButton, SIGNAL(clicked()), this, SLOT(__SLOT_Stop()));
    connect(openButton, SIGNAL(clicked()), this, SLOT(__SLOT_Open()));

    if (m_pDownloadItem)
    {
        m_FileInfo.setFile(m_pDownloadItem->path());
        m_Url = m_pDownloadItem->url();

        connect(m_pDownloadItem.data(), SIGNAL(downloadProgress(qint64,qint64)),
                this, SLOT(__SLOT_DownloadProgress(qint64,qint64)));
        connect(m_pDownloadItem.data(), SIGNAL(finished()),
                this, SLOT(__SLOT_Finished()));
    }

    // 数据重置
    downloadInfoLabel->clear();
    progressBar->setValue(0);
    __GetFileName();

    // 启动定时器进行下载时间评估
    m_tagDownloadTime.start();
}
//-----------------------------------------------------
bool CFKDownloadWidget::IsDownloading() const
{
    return (progressBar->isVisible());
}
//-----------------------------------------------------
bool CFKDownloadWidget::IsDownloadSuccessfully() const
{
    bool bIsCompleted =
            ( m_pDownloadItem
            && m_pDownloadItem->isFinished()
            && m_pDownloadItem->state() == QWebEngineDownloadItem::DownloadCompleted );
    return bIsCompleted || !stopButton->isVisible();
}
//-----------------------------------------------------
void CFKDownloadWidget::__SLOT_Stop()
{
    setUpdatesEnabled(false);
    stopButton->setEnabled(false);
    stopButton->hide();
    setUpdatesEnabled(true);
    if (m_pDownloadItem)
        m_pDownloadItem->cancel();

    // 通知状态更变了
    emit SIGNAL_StatusChanged();
}
//-----------------------------------------------------
void CFKDownloadWidget::__SLOT_Open()
{
    QUrl url = QUrl::fromLocalFile(m_FileInfo.absoluteFilePath());
    QDesktopServices::openUrl(url);
}
//-----------------------------------------------------
void CFKDownloadWidget::__SLOT_DownloadProgress(
        qint64 llBytesReceived, qint64 llBytesTotal)
{
    m_llBytesReceived = llBytesReceived;
    if (llBytesTotal == -1)
    {
        progressBar->setValue(0);
        progressBar->setMaximum(0);
    }
    else
    {
        progressBar->setValue(llBytesReceived);
        progressBar->setMaximum(llBytesTotal);
    }

    __UpdateInfoLabel();
}
//-----------------------------------------------------
void CFKDownloadWidget::__SLOT_Finished()
{
    if (m_pDownloadItem)
    {
        QWebEngineDownloadItem::DownloadState eState = m_pDownloadItem->state();
        QString strMessage;
        bool bIsInterrupted = false;

        switch (eState)
        {
        case QWebEngineDownloadItem::DownloadRequested:
        case QWebEngineDownloadItem::DownloadInProgress:
            Q_UNREACHABLE();
            break;
        case QWebEngineDownloadItem::DownloadCompleted:
            break;
        case QWebEngineDownloadItem::DownloadCancelled:
            strMessage = QStringLiteral("Download cancelled");
            bIsInterrupted = true;
            break;
        case QWebEngineDownloadItem::DownloadInterrupted:
            strMessage = QStringLiteral("Download interrupted");
            bIsInterrupted = true;
            break;
        }

        if (bIsInterrupted)
        {
            downloadInfoLabel->setText(strMessage);
            return;
        }
    }

    progressBar->hide();
    stopButton->setEnabled(false);
    stopButton->hide();

    __UpdateInfoLabel();
    // 发送状态更变消息
    emit SIGNAL_StatusChanged();
}
//-----------------------------------------------------
bool CFKDownloadWidget::__GetFileName(bool bPromptForFileName)
{
    QSettings tagSettings;
    tagSettings.beginGroup(QLatin1String("downloadmanager"));
    QString strDefaultLocation = QStandardPaths::writableLocation(
                QStandardPaths::DesktopLocation);
    if (m_FileInfo.absoluteDir().exists())
    {
        strDefaultLocation = m_FileInfo.absolutePath();
    }
    QString strDownloadDirectory = tagSettings.value(QLatin1String("downloadDirectory"), strDefaultLocation).toString();
    if (!strDownloadDirectory.isEmpty())
    {
        strDownloadDirectory += QLatin1Char('/');
    }

    QString strDefaultFileName  = QFileInfo(strDownloadDirectory, m_FileInfo.fileName()).absoluteFilePath();
    QString strFileName         = strDefaultFileName;
    if (bPromptForFileName)
    {
        strFileName = QFileDialog::getSaveFileName(this, tr("Save File"), strDefaultFileName);
        if (strFileName.isEmpty())
        {
            if (m_pDownloadItem)
            {
                m_pDownloadItem->cancel();
            }
            fileNameLabel->setText(tr("Download canceled: %1").arg(QFileInfo(strDefaultFileName).fileName()));
            return false;
        }
    }
    m_FileInfo.setFile(strFileName);

    if (m_pDownloadItem && m_pDownloadItem->state() == QWebEngineDownloadItem::DownloadRequested)
    {
        m_pDownloadItem->setPath(m_FileInfo.absoluteFilePath());
    }

    fileNameLabel->setText(m_FileInfo.fileName());
    return true;
}
//-----------------------------------------------------
void CFKDownloadWidget::__UpdateInfoLabel()
{
    qint64 llBytesTotal = progressBar->maximum();

    // 更新信息标签
    double dSpeed = m_llBytesReceived * 1000.0 / m_tagDownloadTime.elapsed();
    double dTimeRemaining = ((double)(llBytesTotal - m_llBytesReceived)) / dSpeed;
    QString strTimeRemainingString = tr("seconds");
    if (dTimeRemaining > 60)
    {
        dTimeRemaining = dTimeRemaining / 60;
        strTimeRemainingString = tr("minutes");
    }
    dTimeRemaining = floor(dTimeRemaining);

    // 修正，下载时间不可能为0
    if (dTimeRemaining == 0)
        dTimeRemaining = 1;

    QString strInfo;
    if (!IsDownloadSuccessfully())
    {
        QString strRemaining;
        if (llBytesTotal != 0)
            strRemaining = tr("- %4 %5 remaining")
            .arg(dTimeRemaining)
            .arg(strTimeRemainingString);
        strInfo = tr("%1 of %2 (%3/sec) %4")
            .arg(__GetDataString(m_llBytesReceived))
            .arg(llBytesTotal == 0 ? tr("?") : __GetDataString(llBytesTotal))
            .arg(__GetDataString((int)dSpeed))
            .arg(strRemaining);
    } else {
        if (m_llBytesReceived != llBytesTotal) {
            strInfo = tr("%1 of %2 - Stopped")
                .arg(__GetDataString(m_llBytesReceived))
                .arg(__GetDataString(llBytesTotal));
        } else
            strInfo = __GetDataString(m_llBytesReceived);
    }
    downloadInfoLabel->setText(strInfo);
}
//-----------------------------------------------------
QString CFKDownloadWidget::__GetDataString(int nSize) const
{
    QString strUnit;
    if (nSize < 1024)
    {
        strUnit = tr("Bytes");
    }
    else if (nSize < 1024*1024)
    {
        nSize /= 1024;
        strUnit = tr("KB");
    }
    else
    {
        nSize /= 1024*1024;
        strUnit = tr("MB");
    }
    return QString(QLatin1String("%1 %2")).arg(nSize).arg(strUnit);
}
//-----------------------------------------------------
