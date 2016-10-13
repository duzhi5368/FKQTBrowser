/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKPRINTTOPDFDIALOG_H
#define FKPRINTTOPDFDIALOG_H
//-----------------------------------------------------
#include <QtWidgets/QDialog>
#include <QtGui/QPageLayout>
//-----------------------------------------------------
QT_BEGIN_NAMESPACE
class Ui_PrintToPdfDialog;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKPrintToPdfDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CFKPrintToPdfDialog(const QString& strFilePath, QWidget* pParent = NULL);
    ~CFKPrintToPdfDialog();
public:
    QString                 GetFilePath() const;
    QPageLayout             GetPageLayout() const;
private slots:
    void                    __SLOT_OnChoosePageLayoutButtonClicked();
    void                    __SLOT_OnChooseFilePathButtonClicked();
private:
    void                    __SetFilePath(const QString& strFilePath);
    void                    __UpdatePageLayoutLabel();
private:
    QPageLayout             m_CurrentPageLayout;
    Ui_PrintToPdfDialog*    m_pUI;
};
//-----------------------------------------------------
#endif // FKPRINTTOPDFDIALOG_H
