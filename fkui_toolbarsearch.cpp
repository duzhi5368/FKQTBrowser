/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QStringListModel>
#include <QtCore/QSettings>
#include <QtCore/QUrlQuery>

#include <QtWidgets/QCompleter>
#include <QtWidgets/QLineEdit>
//-----------------------------------------------------
#include "fkui_toolbarsearch.h"
#include "fkbrowserapp.h"
#include "fkautosaver.h"
//-----------------------------------------------------
CFKUI_ToolBarSearch::CFKUI_ToolBarSearch(QWidget* pParent)
    : CFKUI_SearchLineEdit(pParent)
    , m_nMaxSavedSearches(10)
{
    m_pAutoSaver = new CFKAutoSaver(this);
    m_pStringListModel = new QStringListModel(this);

    QMenu* pMenu = GetMenu();
    connect(pMenu, SIGNAL(aboutToShow()),
            this, SLOT(__SLOT_AboutToShowMenu()));
    connect(pMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(__SLOT_TriggeredMenuAction(QAction*)));

    QCompleter* pCompleter = new QCompleter(m_pStringListModel, this);
    pCompleter->setCompletionMode(QCompleter::InlineCompletion);
    GetLineEdit()->setCompleter(pCompleter);

    connect(GetLineEdit(), SIGNAL(returnPressed()),
            SLOT(SLOT_SearchNow()));
    SetInactiveText(tr("Google"));

    __Load();
}
//-----------------------------------------------------
CFKUI_ToolBarSearch::~CFKUI_ToolBarSearch()
{
    m_pAutoSaver->SaveIfNeccessary();
}
//-----------------------------------------------------
void CFKUI_ToolBarSearch::SLOT_Clear()
{
    m_pStringListModel->setStringList(QStringList());
    m_pAutoSaver->SLOT_OnChange();;
}
//-----------------------------------------------------
void CFKUI_ToolBarSearch::SLOT_SearchNow()
{
    QString strSearchText = GetLineEdit()->text();
    QStringList newStringList = m_pStringListModel->stringList();
    if (newStringList.contains(strSearchText))
    {
        newStringList.removeAt(newStringList.indexOf(strSearchText));
    }
    newStringList.prepend(strSearchText);
    if (newStringList.size() >= m_nMaxSavedSearches)
    {
        newStringList.removeLast();
    }

    if (!CFKBrowserApp::GetAppInstancePtr()->IsPrivateBrowsing())
    {
        m_pStringListModel->setStringList(newStringList);
        m_pAutoSaver->SLOT_OnChange();
    }

    QUrl url(QLatin1String("http://www.google.com/search"));
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QLatin1String("q"), strSearchText);
    urlQuery.addQueryItem(QLatin1String("ie"), QLatin1String("UTF-8"));
    urlQuery.addQueryItem(QLatin1String("oe"), QLatin1String("UTF-8"));
    urlQuery.addQueryItem(QLatin1String("client"), QLatin1String("qtdemobrowser"));
    url.setQuery(urlQuery);
    emit SIGNAL_Search(url);
}
//-----------------------------------------------------
void CFKUI_ToolBarSearch::__SLOT_Save()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("toolbarsearch"));
    settings.setValue(QLatin1String("recentSearches"), m_pStringListModel->stringList());
    settings.setValue(QLatin1String("maximumSaved"), m_nMaxSavedSearches);
    settings.endGroup();
}
//-----------------------------------------------------
void CFKUI_ToolBarSearch::__SLOT_AboutToShowMenu()
{
    GetLineEdit()->selectAll();

    QMenu* pMenu = GetMenu();
    pMenu->clear();
    QStringList listStrings = m_pStringListModel->stringList();
    if (listStrings.isEmpty())
    {
        pMenu->addAction(tr("No Recent Searches"));
        return;
    }

    QAction *pRecent = pMenu->addAction(tr("Recent Searches"));
    pRecent->setEnabled(false);
    for (int i = 0; i < listStrings.count(); ++i)
    {
        QString text = listStrings.at(i);
        pMenu->addAction(text)->setData(text);
    }
    pMenu->addSeparator();
    pMenu->addAction(tr("Clear Recent Searches"),
                     this, SLOT(SLOT_Clear()));
}
//-----------------------------------------------------
void CFKUI_ToolBarSearch::__SLOT_TriggeredMenuAction(QAction* pAction)
{
    QVariant v = pAction->data();
    if (v.canConvert<QString>())
    {
        QString strText = v.toString();
        GetLineEdit()->setText(strText);
        SLOT_SearchNow();
    }
}
//-----------------------------------------------------
void CFKUI_ToolBarSearch::__Load()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("toolbarsearch"));
    QStringList listString = settings.value(QLatin1String("recentSearches")).toStringList();
    m_nMaxSavedSearches = settings.value(QLatin1String("maximumSaved"), m_nMaxSavedSearches).toInt();
    m_pStringListModel->setStringList(listString);
    settings.endGroup();
}
//-----------------------------------------------------
