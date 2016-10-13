/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_SAVEPAGEDIALOG_H
#define FKUI_SAVEPAGEDIALOG_H
//-----------------------------------------------------
#include <QtWidgets/QDialog>
#include <QtWebEngineWidgets/QWebEngineDownloadItem>
//-----------------------------------------------------
class Ui_SavePageDialog;
//-----------------------------------------------------
class CFKUI_SavePageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CFKUI_SavePageDialog(QWidget* pParent,
            QWebEngineDownloadItem::SavePageFormat eFormat,
            const QString& strFilePath );
    ~CFKUI_SavePageDialog();
public:
    QWebEngineDownloadItem::SavePageFormat  GetPageFormat() const;
    QString                                 GetFilePath() const;
private slots:
    void        __SLOT_OnChooseFilePathBtnClicked();
    void        __SLOT_OnFormatComboBoxCurrentIndexChanged(int nIndex);
private:
    static int  FormatToIndex(QWebEngineDownloadItem::SavePageFormat eFormat);
    static QWebEngineDownloadItem::SavePageFormat IndexToFormat(int nIndex);
    static QString SuffixOfFormat(QWebEngineDownloadItem::SavePageFormat eFormat);
private:
    void        __SetFilePath(const QString& strFilePath);
    void        __EnsureFileSuffix(QWebEngineDownloadItem::SavePageFormat eFormat);
private:
    Ui_SavePageDialog*      m_pUIDlg;
    static const QWebEngineDownloadItem::SavePageFormat m_eIndexToFormatTable[];
};
//-----------------------------------------------------
#endif // FKUI_SAVEPAGEDIALOG_H
