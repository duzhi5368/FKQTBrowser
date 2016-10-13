/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkdownloadmodel.h"
#include "fkdownloaddialog.h"
#include "fkdownloadwidget.h"
#include "fkautosaver.h"
//-----------------------------------------------------
CFKDownloadModel::CFKDownloadModel(CFKDownloadDialog* pDownloadDialog, QObject* pParent)
    : QAbstractListModel(pParent)
    , m_pDownloadDialog(pDownloadDialog)
{

}
//-----------------------------------------------------
QVariant CFKDownloadModel::data(const QModelIndex &index, int nRole) const
{
    if (index.row() < 0 || index.row() >= rowCount(index.parent()))
        return QVariant();
    if (nRole == Qt::ToolTipRole)
    {
        if (!m_pDownloadDialog->m_ListDownloads.at(index.row())->IsDownloadSuccessfully())
        {
            return m_pDownloadDialog->m_ListDownloads.at(index.row())->downloadInfoLabel->text();
        }
    }
    return QVariant();
}
//-----------------------------------------------------
int CFKDownloadModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : m_pDownloadDialog->m_ListDownloads.count();
}
//-----------------------------------------------------
bool CFKDownloadModel::removeRows(int nRow, int nCount, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    int nLastRow = nRow + nCount - 1;
    for (int i = nLastRow; i >= nRow; --i)
    {
        if (m_pDownloadDialog->m_ListDownloads.at(i)->IsDownloadSuccessfully())
        {
            beginRemoveRows(parent, i, i);
            m_pDownloadDialog->m_ListDownloads.takeAt(i)->deleteLater();
            endRemoveRows();
        }
    }

    // 通知更变
    m_pDownloadDialog->m_pAutoSaver->SLOT_OnChange();
    return true;
}
//-----------------------------------------------------
