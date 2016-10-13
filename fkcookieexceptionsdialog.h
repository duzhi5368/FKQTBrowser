/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKCOOKIEEXCEPTIONSDIALOG_H
#define FKCOOKIEEXCEPTIONSDIALOG_H
//-----------------------------------------------------
#include <QtCore/QSortFilterProxyModel>
//-----------------------------------------------------
#include "ui_cookiesexceptions.h"
//-----------------------------------------------------
class CFKCookieExceptionsModel;
class CFKCookieJarMgr;
//-----------------------------------------------------
class CFKCookieExceptionsDialog : public QDialog, public Ui_CookiesExceptionsDialog
{
    Q_OBJECT
public:
    CFKCookieExceptionsDialog(CFKCookieJarMgr* pMgr, QWidget* pParent = NULL);
private slots:
    void                        SLOT_Block();
    void                        SLOT_Allow();
    void                        SLOT_AllowForSession();
    void                        SLOT_TextChanged(const QString& text);
private:
    CFKCookieExceptionsModel*   m_pExceptionsModel;
    QSortFilterProxyModel*      m_pProxyModel;
    CFKCookieJarMgr*            m_pCookieJarMgr;
};
//-----------------------------------------------------
#endif // FKCOOKIEEXCEPTIONSDIALOG_H
