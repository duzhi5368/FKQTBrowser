/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKCOOKIESDIALOG_H
#define FKCOOKIESDIALOG_H
//-----------------------------------------------------
#include <QtCore/QSortFilterProxyModel>
//-----------------------------------------------------
#include "ui_cookies.h"
//-----------------------------------------------------
class CFKCookieJarMgr;
//-----------------------------------------------------
class CFKCookiesDialog : public QDialog, public Ui_CookiesDialog
{
    Q_OBJECT
public:
    CFKCookiesDialog(CFKCookieJarMgr* pMgr, QWidget* pParent = NULL);
private:
    QSortFilterProxyModel*  m_pProxyModel;
};
//-----------------------------------------------------
#endif // FKCOOKIESDIALOG_H
