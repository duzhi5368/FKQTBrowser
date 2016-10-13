/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
/*
#ifndef QT_NO_PRINTER
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPageSetupDialog>
#endif
*/

#include <QtCore/QFileInfo>

#include <QtWidgets/QFileDialog>
//-----------------------------------------------------
#include "fkprinttopdfdialog.h"
#include "ui_printtopdfdialog.h"
//-----------------------------------------------------
CFKPrintToPdfDialog::CFKPrintToPdfDialog(const QString& strFilePath,
                                         QWidget* pParent)
    : QDialog(pParent)
{
    m_pUI = new Ui_PrintToPdfDialog();
    m_CurrentPageLayout = QPageLayout(QPageSize(QPageSize::A4),
                                      QPageLayout::Portrait,
                                      QMarginsF(0.0, 0.0, 0.0, 0.0));

    m_pUI->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(m_pUI->chooseFilePathButton, &QToolButton::clicked,
            this, &CFKPrintToPdfDialog::__SLOT_OnChooseFilePathButtonClicked);
#ifndef QT_NO_PRINTER
    /*
    connect(m_pUI->choosePageLayoutButton, &QToolButton::clicked,
            this, &CFKPrintToPdfDialog::__SLOT_OnChoosePageLayoutButtonClicked);
    */
#else
    m_pUI->choosePageLayoutButton->hide();
#endif
    __UpdatePageLayoutLabel();
    __SetFilePath(strFilePath);
}
//-----------------------------------------------------
CFKPrintToPdfDialog::~CFKPrintToPdfDialog()
{
    delete m_pUI;
}
//-----------------------------------------------------
QString CFKPrintToPdfDialog::GetFilePath() const
{
    return QDir::fromNativeSeparators(m_pUI->filePathLineEdit->text());
}
//-----------------------------------------------------
QPageLayout CFKPrintToPdfDialog::GetPageLayout() const
{
    return m_CurrentPageLayout;
}
//-----------------------------------------------------
void CFKPrintToPdfDialog::__SLOT_OnChoosePageLayoutButtonClicked()
{
#ifndef QT_NO_PRINTER
    /*
    QPrinter printer;
    printer.setPageLayout(m_CurrentPageLayout);

    QPageSetupDialog dlg(&printer, this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    m_CurrentPageLayout.setPageSize(printer.pageLayout().pageSize());
    m_CurrentPageLayout.setOrientation(printer.pageLayout().orientation());
    __UpdatePageLayoutLabel();
    */
#endif
}
//-----------------------------------------------------
void CFKPrintToPdfDialog::__SLOT_OnChooseFilePathButtonClicked()
{
    QFileInfo fi(GetFilePath());
    QFileDialog dlg(this, tr("Save PDF as"), fi.absolutePath());
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setDefaultSuffix(QStringLiteral(".pdf"));
    dlg.selectFile(fi.absoluteFilePath());
    if (dlg.exec() != QDialog::Accepted)
        return;
    __SetFilePath(dlg.selectedFiles().first());
}
//-----------------------------------------------------
void CFKPrintToPdfDialog::__SetFilePath(const QString& strFilePath)
{
    m_pUI->filePathLineEdit->setText(QDir::toNativeSeparators(strFilePath));
}
//-----------------------------------------------------
void CFKPrintToPdfDialog::__UpdatePageLayoutLabel()
{
    m_pUI->pageLayoutLabel->setText(QString("%1, %2").arg(
                                   m_CurrentPageLayout.pageSize().name()).arg(
                                   m_CurrentPageLayout.orientation() == QPageLayout::Portrait
                                   ? tr("Portrait") : tr("Landscape")
                                   ));
}
//-----------------------------------------------------
