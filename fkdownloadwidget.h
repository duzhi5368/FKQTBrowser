/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKDOWNLOADWIDGET_H
#define FKDOWNLOADWIDGET_H
//-----------------------------------------------------
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QUrl>

#include <QWebEngineDownloadItem>
//-----------------------------------------------------
#include "ui_downloaditem.h"
//-----------------------------------------------------
class CFKDownloadDialog;
//-----------------------------------------------------
class CFKDownloadWidget : public QWidget, public Ui_DownloadItem
{
    friend class CFKDownloadDialog;
    Q_OBJECT
public:
    CFKDownloadWidget(QWebEngineDownloadItem* pDownloadItem, QWidget* pParent = NULL);
signals:
    void            SIGNAL_StatusChanged();
public:
    bool            IsDownloading() const;
    bool            IsDownloadSuccessfully() const;
private slots:
    void            __SLOT_Stop();
    void            __SLOT_Open();
    void            __SLOT_DownloadProgress(
            qint64 llBytesReceived, qint64 llBytesTotal);
    void            __SLOT_Finished();
private:
    bool            __GetFileName(bool bPromptForFileName = false);
    void            __UpdateInfoLabel();
    QString         __GetDataString(int nSize) const;
private:
    qint64                                  m_llBytesReceived;      // 已接收的文件数据字节大小
    QScopedPointer<QWebEngineDownloadItem>  m_pDownloadItem;
    QFileInfo                               m_FileInfo;
    QUrl                                    m_Url;
    QTime                                   m_tagDownloadTime;
};
//-----------------------------------------------------
#endif // FKDOWNLOADWIDGET_H
