/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_TOOLBARSEARCH_H
#define FKUI_TOOLBARSEARCH_H
//-----------------------------------------------------
#include <QtCore/QUrl>
//-----------------------------------------------------
#include "fkui_searchlineedit.h"
//-----------------------------------------------------
class CFKAutoSaver;

QT_BEGIN_NAMESPACE
class QStringListModel;
class QAction;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKUI_ToolBarSearch : public CFKUI_SearchLineEdit
{
    Q_OBJECT
signals:
    void                SIGNAL_Search(const QUrl& url);
public:
    CFKUI_ToolBarSearch(QWidget* pParent = NULL);
    ~CFKUI_ToolBarSearch();
public slots:
    void                SLOT_Clear();
    void                SLOT_SearchNow();
private slots:
    void                __SLOT_Save();
    void                __SLOT_AboutToShowMenu();
    void                __SLOT_TriggeredMenuAction(QAction* pAction);
private:
    void                __Load();
private:
    CFKAutoSaver*       m_pAutoSaver;
    QStringListModel*   m_pStringListModel;
    int                 m_nMaxSavedSearches;
};

//-----------------------------------------------------
#endif // FKUI_TOOLBARSEARCH_H
