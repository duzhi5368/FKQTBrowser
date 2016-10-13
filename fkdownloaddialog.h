/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKDOWNLOADDIALOG_H
#define FKDOWNLOADDIALOG_H
//-----------------------------------------------------
#include <QtWidgets/QFileIconProvider>
#include <QWebEngineDownloadItem>
//-----------------------------------------------------
#include "ui_downloads.h"
//-----------------------------------------------------
class CFKAutoSaver;
class CFKDownloadWidget;
class CFKDownloadModel;
//-----------------------------------------------------
class CFKDownloadDialog : public QDialog, public Ui_DownloadDialog
{
    friend class CFKDownloadModel;
    Q_OBJECT
public:
    enum ENUM_RemovePolicy{
        eRP_Never,
        eRP_Exit,
        eRP_SuccessFullDownload,
    };
    Q_ENUM(ENUM_RemovePolicy)
public:
    CFKDownloadDialog(QWidget* pParent = NULL);
    ~CFKDownloadDialog();
public:
    ENUM_RemovePolicy   GetRemovePolicy() const;
    void                SetRemovePolicy(ENUM_RemovePolicy ePolicy);

    int                 GetActiveDownloadsCount() const;
public slots:
    void                SLOT_Cleanup();
    void                SLOT_Download(QWebEngineDownloadItem* pDownload);
private slots:
    void                __SLOT_Save() const;
    void                __SLOT_UpdateRow();
private:
    void                __LoadSettings();
    void                __UpdateItemCount();
    void                __AddItem(CFKDownloadWidget* pItem);
private:
    CFKAutoSaver*               m_pAutoSaver;
    QList<CFKDownloadWidget*>   m_ListDownloads;
    CFKDownloadModel*           m_pModel;
    QFileIconProvider*          m_pIconProvider;
    ENUM_RemovePolicy           m_eRemovePolicy;
};
//-----------------------------------------------------
#endif // FKDOWNLOADDIALOG_H
