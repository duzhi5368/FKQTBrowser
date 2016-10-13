/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QSettings>
#include <QtCore/QMetaEnum>
//-----------------------------------------------------
#include "fkautosaver.h"
#include "fkdownloadmodel.h"
#include "fkdownloaddialog.h"
#include "fkdownloadwidget.h"
#include "fkbrowserapp.h"
//-----------------------------------------------------
CFKDownloadDialog::CFKDownloadDialog(QWidget* pParent)
    : QDialog(pParent)
    , m_pIconProvider( NULL )
    , m_eRemovePolicy( eRP_Never )

{
    setupUi(this);

    m_pAutoSaver    = new CFKAutoSaver(this);
    m_pModel        = new CFKDownloadModel(this);

    downloadsView->setShowGrid(false);
    downloadsView->verticalHeader()->hide();
    downloadsView->horizontalHeader()->hide();
    downloadsView->setAlternatingRowColors(true);
    downloadsView->horizontalHeader()->setStretchLastSection(true);
    downloadsView->setModel(m_pModel);

    connect(cleanupButton, SIGNAL(clicked()), this, SLOT(SLOT_Cleanup()));

    __LoadSettings();
}
//-----------------------------------------------------
CFKDownloadDialog::~CFKDownloadDialog()
{
    m_pAutoSaver->SLOT_OnChange();
    m_pAutoSaver->SaveIfNeccessary();
    if(m_pIconProvider != NULL)
    {
        delete m_pIconProvider;
        m_pIconProvider = NULL;
    }
}
//-----------------------------------------------------
CFKDownloadDialog::ENUM_RemovePolicy CFKDownloadDialog::GetRemovePolicy() const
{
    return m_eRemovePolicy;
}
//-----------------------------------------------------
void CFKDownloadDialog::SetRemovePolicy(ENUM_RemovePolicy ePolicy)
{
    if (ePolicy == m_eRemovePolicy)
        return;
    m_eRemovePolicy = ePolicy;
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
int CFKDownloadDialog::GetActiveDownloadsCount() const
{
    int nCount = 0;
    for (int i = 0; i < m_ListDownloads.count(); ++i)
    {
        if (m_ListDownloads.at(i)->stopButton->isEnabled())
        {
            ++nCount;
        }
    }
    return nCount;
}
//-----------------------------------------------------
void CFKDownloadDialog::SLOT_Cleanup()
{
    if (m_ListDownloads.isEmpty())
        return;

    m_pModel->removeRows(0, m_ListDownloads.count());
    __UpdateItemCount();

    if (m_ListDownloads.isEmpty() && m_pIconProvider) {
        delete m_pIconProvider;
        m_pIconProvider = NULL;
    }

    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
void CFKDownloadDialog::SLOT_Download(QWebEngineDownloadItem* pDownload)
{
    CFKDownloadWidget* pWidget = new CFKDownloadWidget(pDownload, this);
    __AddItem(pWidget);
}
//-----------------------------------------------------
void CFKDownloadDialog::__SLOT_Save() const
{
    QSettings tagSettings;
    tagSettings.beginGroup(QLatin1String("downloadmanager"));
    QMetaEnum eRemovePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
    tagSettings.setValue(QLatin1String("removeDownloadsPolicy"), QLatin1String(eRemovePolicyEnum.valueToKey(m_eRemovePolicy)));
    tagSettings.setValue(QLatin1String("size"), size());
    if (m_eRemovePolicy == eRP_Exit)
        return;

    for (int i = 0; i < m_ListDownloads.count(); ++i)
    {
        QString strKey = QString(QLatin1String("download_%1_")).arg(i);
        tagSettings.setValue(strKey + QLatin1String("url"),        m_ListDownloads[i]->m_Url);
        tagSettings.setValue(strKey + QLatin1String("location"),   m_ListDownloads[i]->m_FileInfo.filePath());
        tagSettings.setValue(strKey + QLatin1String("done"),       m_ListDownloads[i]->IsDownloadSuccessfully());
    }
    int i = m_ListDownloads.count();
    QString strKey = QString(QLatin1String("download_%1_")).arg(i);
    while (tagSettings.contains(strKey + QLatin1String("url")))
    {
        tagSettings.remove(strKey + QLatin1String("url"));
        tagSettings.remove(strKey + QLatin1String("location"));
        tagSettings.remove(strKey + QLatin1String("done"));
        strKey = QString(QLatin1String("download_%1_")).arg(++i);
    }
}
//-----------------------------------------------------
void CFKDownloadDialog::__SLOT_UpdateRow()
{
    CFKDownloadWidget* pWidget = qobject_cast<CFKDownloadWidget*>(sender());
    int nRow = m_ListDownloads.indexOf(pWidget);
    if (-1 == nRow)
    {
        return;
    }
    if (NULL == m_pIconProvider)
    {
        m_pIconProvider = new QFileIconProvider();
    }
    QIcon tagIcon = m_pIconProvider->icon(pWidget->m_FileInfo);
    if (tagIcon.isNull())
    {
        tagIcon = style()->standardIcon(QStyle::SP_FileIcon);
    }
    pWidget->fileIcon->setPixmap(tagIcon.pixmap(48, 48));
    downloadsView->setRowHeight(nRow, pWidget->minimumSizeHint().height());

    bool bIsRemove = false;
    if (!pWidget->IsDownloading()
        && CFKBrowserApp::GetAppInstancePtr()->IsPrivateBrowsing())
    {
        bIsRemove = true;
    }

    if (pWidget->IsDownloadSuccessfully()
        && GetRemovePolicy() == CFKDownloadDialog::eRP_SuccessFullDownload)
    {
        bIsRemove = true;
    }

    if (bIsRemove)
    {
        m_pModel->removeRow(nRow);
    }

    cleanupButton->setEnabled(m_ListDownloads.count() - GetActiveDownloadsCount() > 0);
}
//-----------------------------------------------------
void CFKDownloadDialog::__LoadSettings()
{
    QSettings tagSettings;
    tagSettings.beginGroup(QLatin1String("downloadmanager"));
    QSize tagSize = tagSettings.value(QLatin1String("size")).toSize();
    if (tagSize.isValid()){
        resize(tagSize);
    }
    QByteArray strValue = tagSettings.value(QLatin1String("removeDownloadsPolicy"), QLatin1String("Never")).toByteArray();
    QMetaEnum eRemovePolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("RemovePolicy"));
    m_eRemovePolicy = eRemovePolicyEnum.keyToValue(strValue) == -1 ?
                        eRP_Never :
                        static_cast<ENUM_RemovePolicy>(eRemovePolicyEnum.keyToValue(strValue));

    int i = 0;
    QString strKey = QString(QLatin1String("download_%1_")).arg(i);
    while (tagSettings.contains(strKey + QLatin1String("url")))
    {
        QUrl url = tagSettings.value(strKey + QLatin1String("url")).toUrl();
        QString strFileName = tagSettings.value(strKey + QLatin1String("location")).toString();
        bool bIsDone = tagSettings.value(strKey + QLatin1String("done"), true).toBool();
        if (bIsDone && !url.isEmpty() && !strFileName.isEmpty())
        {
            CFKDownloadWidget* pWidget = new CFKDownloadWidget(0, this);
            pWidget->m_FileInfo.setFile(strFileName);
            pWidget->fileNameLabel->setText(pWidget->m_FileInfo.fileName());
            pWidget->m_Url = url;
            pWidget->stopButton->setVisible(false);
            pWidget->stopButton->setEnabled(false);
            pWidget->progressBar->hide();

            __AddItem(pWidget);
        }
        strKey = QString(QLatin1String("download_%1_")).arg(++i);
    }
    cleanupButton->setEnabled(m_ListDownloads.count() - GetActiveDownloadsCount() > 0);
}
//-----------------------------------------------------
void CFKDownloadDialog::__UpdateItemCount()
{
    int nCount = m_ListDownloads.count();
    itemCount->setText(nCount == 1 ? tr("1 Download")
                                   : tr("%1 Downloads").arg(nCount));
}
//-----------------------------------------------------
void CFKDownloadDialog::__AddItem(CFKDownloadWidget* pItem)
{
    connect(pItem, SIGNAL(SIGNAL_StatusChanged()), this, SLOT(__SLOT_UpdateRow()));

    int nRow = m_ListDownloads.count();
    m_pModel->beginInsertRows(QModelIndex(), nRow, nRow);
    m_ListDownloads.append(pItem);
    m_pModel->endInsertRows();

    __UpdateItemCount();
    if (nRow == 0)
    {
        show();
    }
    downloadsView->setIndexWidget(m_pModel->index(nRow, 0), pItem);
    QIcon icon = style()->standardIcon(QStyle::SP_FileIcon);
    pItem->fileIcon->setPixmap(icon.pixmap(48, 48));
    downloadsView->setRowHeight(nRow, pItem->sizeHint().height());
}
//-----------------------------------------------------
