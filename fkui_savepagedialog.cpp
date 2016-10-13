/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <QtWidgets/QFileDialog>
//-----------------------------------------------------
#include "fkui_savepagedialog.h"

#include "ui_savepagedialog.h"
//-----------------------------------------------------
const QWebEngineDownloadItem::SavePageFormat CFKUI_SavePageDialog::m_eIndexToFormatTable[] =
{
    QWebEngineDownloadItem::SingleHtmlSaveFormat,
    QWebEngineDownloadItem::CompleteHtmlSaveFormat,
    QWebEngineDownloadItem::MimeHtmlSaveFormat
};
//-----------------------------------------------------
CFKUI_SavePageDialog::CFKUI_SavePageDialog(QWidget* pParent,
        QWebEngineDownloadItem::SavePageFormat eFormat,
        const QString& strFilePath )
    : QDialog(pParent)
{
    m_pUIDlg = new Ui_SavePageDialog();
    m_pUIDlg->formatComboBox->setCurrentIndex(FormatToIndex(eFormat));
    __SetFilePath(strFilePath);
}
//-----------------------------------------------------
CFKUI_SavePageDialog::~CFKUI_SavePageDialog()
{
    if(m_pUIDlg != NULL)
    {
        delete m_pUIDlg;
        m_pUIDlg = NULL;
    }
}
//-----------------------------------------------------
QWebEngineDownloadItem::SavePageFormat CFKUI_SavePageDialog::GetPageFormat() const
{
    return IndexToFormat(m_pUIDlg->formatComboBox->currentIndex());
}
//-----------------------------------------------------
QString CFKUI_SavePageDialog::GetFilePath() const
{
    return QDir::fromNativeSeparators(m_pUIDlg->filePathLineEdit->text());
}
//-----------------------------------------------------
void CFKUI_SavePageDialog::__SLOT_OnChooseFilePathBtnClicked()
{
    QFileInfo tagFileInfo(GetFilePath());
    QFileDialog dlg(this, tr("Save Page As"), tagFileInfo.absolutePath());
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setDefaultSuffix(SuffixOfFormat(GetPageFormat()));
    dlg.selectFile(tagFileInfo.absoluteFilePath());
    if (dlg.exec() != QDialog::Accepted)
    {
        return;
    }
    __SetFilePath(dlg.selectedFiles().first());
    __EnsureFileSuffix(GetPageFormat());
}
//-----------------------------------------------------
void CFKUI_SavePageDialog::__SLOT_OnFormatComboBoxCurrentIndexChanged(int nIndex)
{
    __EnsureFileSuffix(IndexToFormat(nIndex));
}
//-----------------------------------------------------
int CFKUI_SavePageDialog::FormatToIndex(QWebEngineDownloadItem::SavePageFormat eFormat)
{
    for (auto i : m_eIndexToFormatTable)
    {
        if (m_eIndexToFormatTable[i] == eFormat)
        {
            return i;
        }
    }
    // 执行到这里说明有问题
    Q_UNREACHABLE();
}
//-----------------------------------------------------
QWebEngineDownloadItem::SavePageFormat CFKUI_SavePageDialog::IndexToFormat(int nIndex)
{
    Q_ASSERT(nIndex >= 0
            && size_t(nIndex) < (sizeof(m_eIndexToFormatTable) / sizeof(QWebEngineDownloadItem::SavePageFormat)));
    return m_eIndexToFormatTable[nIndex];
}
//-----------------------------------------------------
QString CFKUI_SavePageDialog::SuffixOfFormat(QWebEngineDownloadItem::SavePageFormat eFormat)
{
    if (eFormat == QWebEngineDownloadItem::MimeHtmlSaveFormat)
        return QStringLiteral(".mhtml");
    return QStringLiteral(".html");
}
//-----------------------------------------------------
void CFKUI_SavePageDialog::__SetFilePath(const QString& strFilePath)
{
    m_pUIDlg->filePathLineEdit->setText(QDir::toNativeSeparators(strFilePath));
}
//-----------------------------------------------------
void CFKUI_SavePageDialog::__EnsureFileSuffix(QWebEngineDownloadItem::SavePageFormat eFormat)
{
    QFileInfo tagFileInfo(GetFilePath());
    __SetFilePath(tagFileInfo.absolutePath() + QLatin1Char('/') + tagFileInfo.completeBaseName()
                + SuffixOfFormat(eFormat));
}
//-----------------------------------------------------
