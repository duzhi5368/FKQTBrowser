/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKHISTORYDIALOG_H
#define FKHISTORYDIALOG_H
//-----------------------------------------------------
#include "ui_history.h"
//-----------------------------------------------------
class CFKHistoryMgr;
//-----------------------------------------------------
class CFKHistoryDialog : public QDialog, public Ui_HistoryDialog
{
    Q_OBJECT
signals:
    void            SIGNAL_OpenUrl(const QUrl& url);
public:
    CFKHistoryDialog(QWidget* pParent = NULL, CFKHistoryMgr* pHistoryMgr = NULL);
private slots:
    void            __SLOT_CustomContextMenuRequested(const QPoint &pos);
    void            __SLOT_Open();
    void            __SLOT_Copy();
};
//-----------------------------------------------------
#endif // FKHISTORYDIALOG_H
