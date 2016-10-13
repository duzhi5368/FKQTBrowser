/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKADDBOOKMARKDIALOG_H
#define FKADDBOOKMARKDIALOG_H
//-----------------------------------------------------
#include "ui_addbookmarkdialog.h"
//-----------------------------------------------------
class CFKBookmarksMgr;
class CFKAddBookmarkProxyModel;
//-----------------------------------------------------
class CFKAddBookmarkDialog : public QDialog, public Ui_AddBookmarkDialog
{
    Q_OBJECT
public:
    CFKAddBookmarkDialog(const QString& url, const QString &title,
           QWidget *pParent = NULL, CFKBookmarksMgr* pBookmarkMgr = NULL);
private slots:
    void                        accept();
private:
    QString                     m_strUrl;
    CFKBookmarksMgr*            m_pBookmarksMgr;
    CFKAddBookmarkProxyModel*   m_pProxyModel;
};
//-----------------------------------------------------
#endif // FKADDBOOKMARKDIALOG_H
