/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKSETTINGDIALOG_H
#define FKSETTINGDIALOG_H
//-----------------------------------------------------
#include <QtWidgets/QDialog>
//-----------------------------------------------------
#include "ui_settings.h"
//-----------------------------------------------------
class CFKSettingDialog : public QDialog, public Ui_Settings
{
    Q_OBJECT
public:
    CFKSettingDialog(QWidget* pParent = NULL);
public:
    void        accept();
private slots:
    void        __SLOT_LoadDefaults();
    void        __SLOT_LoadFromSettings();
    void        __SLOT_SaveToSettings();

    void        __SLOT_SetHomeToCurrentPage();
    void        __SLOT_ShowCookies();
    void        __SLOT_ShowExceptions();

    void        __SLOT_ChooseFont();
    void        __SLOT_ChooseFixedFont();

    void        __SLOT_OnHttpUserAgentEditingFinished();
    void        __SLOT_OnHttpAcceptLanguageEditingFinished();
private:
    QFont       m_StandardFont;
    QFont       m_FixedFont;
};
//-----------------------------------------------------
#endif // FKSETTINGDIALOG_H
