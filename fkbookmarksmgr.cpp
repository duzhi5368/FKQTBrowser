/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QStandardPaths>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>
//-----------------------------------------------------
#include "fkbookmarksmgr.h"
#include "fkbookmarknode.h"
#include "fkautosaver.h"
#include "fkbookmarksmodel.h"
#include "fkbookmarksinsertcommand.h"
#include "fkbookmarksremovecommand.h"
#include "fkbookmarkschangecommand.h"
#include "fkfwriter.h"
#include "fkfreader.h"
//-----------------------------------------------------
CFKBookmarksMgr::CFKBookmarksMgr(QObject* pParent)
    : QObject(pParent)
    , m_bIsLoaded(false)
    , m_pBookmarkRootNode(NULL)
    , m_pBookmarkModel(NULL)
{
    m_pAutoSaver = new CFKAutoSaver(this);

    connect(this, SIGNAL(SIGNAL_EntryAdded(BookmarkNode*)),
            m_pAutoSaver, SLOT(SLOT_OnChange()));
    connect(this, SIGNAL(SIGNAL_EntryRemoved(BookmarkNode*,int,BookmarkNode*)),
            m_pAutoSaver, SLOT(SLOT_OnChange()));
    connect(this, SIGNAL(SIGNAL_EntryChanged(BookmarkNode*)),
            m_pAutoSaver, SLOT(SLOT_OnChange()));
}
//-----------------------------------------------------
CFKBookmarksMgr::~CFKBookmarksMgr()
{
    m_pAutoSaver->SaveIfNeccessary();
}
//-----------------------------------------------------
void CFKBookmarksMgr::AddBookmark(CFKBookmarkNode* pParent, CFKBookmarkNode* pNode, int nRow)
{
    if (!m_bIsLoaded)
        return;

    Q_ASSERT(pParent);
    CFKBookmarksInsertCommand* pCommand = new CFKBookmarksInsertCommand(this, pParent, pNode, nRow);
    m_StackCommands.push(pCommand);
}
//-----------------------------------------------------
void CFKBookmarksMgr::RemoveBookmark(CFKBookmarkNode* pNode)
{
    if (!m_bIsLoaded)
        return;

    Q_ASSERT(pNode);
    CFKBookmarkNode *pParent = pNode->GetParent();
    int nRow = pParent->GetChildrens().indexOf(pNode);
    CFKBookmarksRemoveCommand* pCommand = new CFKBookmarksRemoveCommand(this, pParent, nRow);
    m_StackCommands.push(pCommand);
}
//-----------------------------------------------------
void CFKBookmarksMgr::SetTitle(CFKBookmarkNode* pNode, const QString& strNewTitle)
{
    if (!m_bIsLoaded)
        return;

    Q_ASSERT(pNode);
    CFKBookmarksChangeCommand* pCommand = new CFKBookmarksChangeCommand(this, pNode, strNewTitle, true);
    m_StackCommands.push(pCommand);
}
//-----------------------------------------------------
void CFKBookmarksMgr::SetUrl(CFKBookmarkNode* pNode, const QString& strNewUrl)
{
    if (!m_bIsLoaded)
        return;

    Q_ASSERT(pNode);
    CFKBookmarksChangeCommand* pCommand = new CFKBookmarksChangeCommand(this, pNode, strNewUrl, true);
    m_StackCommands.push(pCommand);
}
//-----------------------------------------------------
void CFKBookmarksMgr::ChangeExpanded()
{
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
CFKBookmarkNode* CFKBookmarksMgr::GetBookmarks()
{
    if (!m_bIsLoaded)
        return NULL;
    return m_pBookmarkRootNode;
}
//-----------------------------------------------------
CFKBookmarkNode* CFKBookmarksMgr::GetMenu()
{
    if (!m_bIsLoaded)
        return NULL;

    for (int i = m_pBookmarkRootNode->GetChildrens().count() - 1;
         i >= 0; --i) {
        CFKBookmarkNode* pNode = m_pBookmarkRootNode->GetChildrens().at(i);
        if (pNode->m_strTitle == tr("Bookmarks Menu"))
        {
            return pNode;
        }
    }

    Q_ASSERT(false);
    return NULL;
}
//-----------------------------------------------------
CFKBookmarkNode* CFKBookmarksMgr::GetToolbar()
{
    if (!m_bIsLoaded)
        return NULL;

    for (int i = m_pBookmarkRootNode->GetChildrens().count() - 1;
         i >= 0; --i) {
        CFKBookmarkNode* pNode = m_pBookmarkRootNode->GetChildrens().at(i);
        if (pNode->m_strTitle == tr("Bookmarks Bar"))
        {
            return pNode;
        }
    }

    Q_ASSERT(false);
    return NULL;
}
//-----------------------------------------------------
CFKBookmarksModel* CFKBookmarksMgr::GetBookmarksModel()
{
    if(m_pBookmarkModel == NULL)
    {
        m_pBookmarkModel = new CFKBookmarksModel(this, this);
    }
    return m_pBookmarkModel;
}
//-----------------------------------------------------
QUndoStack* CFKBookmarksMgr::GetUndoRedoStack()
{
    return &m_StackCommands;
}
//-----------------------------------------------------
void CFKBookmarksMgr::__SLOT_ImportBookmarks()
{
    QString strFileName = QFileDialog::getOpenFileName(0,
                             tr("Open File"), QString(), tr("FKF (*.fkf *.xml)"));
    if (strFileName.isEmpty())
        return;

    CFKFReader reader;
    CFKBookmarkNode* pImportRootNode = reader.Read(strFileName);
    if (reader.error() != QXmlStreamReader::NoError)
    {
        QMessageBox::warning(0, QLatin1String("Loading Bookmark"),
            tr("Error when loading bookmarks on line %1, column %2:\n"
               "%3").arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.errorString()));
    }

    pImportRootNode->SetType(CFKBookmarkNode::eNT_Folder);
    pImportRootNode->m_strTitle = (tr("Imported %1").arg(QDate::currentDate().toString(Qt::SystemLocaleShortDate)));
    AddBookmark(GetMenu(), pImportRootNode);
}
//-----------------------------------------------------
void CFKBookmarksMgr::__SLOT_ExportBookmarks()
{
    QString strFileName = QFileDialog::getSaveFileName(0, tr("Save File"),
                                tr("%1 Bookmarks.fkf").arg(QCoreApplication::applicationName()),
                                tr("FKF (*.fkf *.xml)"));
    if (strFileName.isEmpty())
        return;

    CFKFWriter writer;
    if (!writer.Write(strFileName, m_pBookmarkRootNode))
    {
        QMessageBox::critical(0, tr("Export error"), tr("error saving bookmarks"));
    }
}
//-----------------------------------------------------
void CFKBookmarksMgr::__SLOT_Save() const
{
    if (!m_bIsLoaded)
        return;

    CFKFWriter writer;
    QString strDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QString strBookmarkFile = strDir + QLatin1String("/bookmarks.fkf");
    if (!writer.Write(strBookmarkFile, m_pBookmarkRootNode))
    {
        qWarning() << "BookmarkManager: error saving to" << strBookmarkFile;
    }
}
//-----------------------------------------------------
void CFKBookmarksMgr::__Load()
{
    if (m_bIsLoaded)
        return;
    m_bIsLoaded = true;

    QString strDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QString strBookmarkFile = strDir + QLatin1String("/bookmarks.fkf");
    if (!QFile::exists(strBookmarkFile)){
        strBookmarkFile = QLatin1String(":defaultbookmarks.fkf");
    }

    CFKFReader reader;
    m_pBookmarkRootNode = reader.Read(strBookmarkFile);
    if (reader.error() != QXmlStreamReader::NoError)
    {
        QMessageBox::warning(0, QLatin1String("Loading Bookmark"),
            tr("Error when loading bookmarks on line %1, column %2:\n"
               "%3").arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.errorString()));
    }

    CFKBookmarkNode *pToolbar   = NULL;
    CFKBookmarkNode *pMenu      = NULL;
    QList<CFKBookmarkNode*> others;
    for (int i = m_pBookmarkRootNode->GetChildrens().count() - 1;
         i >= 0; --i)
    {
        CFKBookmarkNode* pNode = m_pBookmarkRootNode->GetChildrens().at(i);
        if (pNode->GetType() == CFKBookmarkNode::eNT_Folder)
        {
            if (pNode->m_strTitle == tr("Toolbar Bookmarks") && !pToolbar)
            {
                pNode->m_strTitle = tr("Bookmarks Bar");
            }
            if (pNode->m_strTitle == tr("Bookmarks Bar") && !pToolbar)
            {
                pToolbar = pNode;
            }

            if (pNode->m_strTitle == tr("Menu") && !pMenu)
            {
                pNode->m_strTitle = tr("Bookmarks Menu");
            }
            if (pNode->m_strTitle == tr("Bookmarks Menu") && !pMenu)
            {
                pMenu = pNode;
            }
        }
        else
        {
            others.append(pNode);
        }
        m_pBookmarkRootNode->Remove(pNode);
    }

    Q_ASSERT(m_pBookmarkRootNode->GetChildrens().count() == 0);
    if (!pToolbar)
    {
        pToolbar = new CFKBookmarkNode(CFKBookmarkNode::eNT_Folder, m_pBookmarkRootNode);
        pToolbar->m_strTitle = tr("Bookmarks Bar");
    }
    else
    {
        m_pBookmarkRootNode->Add(pToolbar);
    }

    if (!pMenu)
    {
        pMenu = new CFKBookmarkNode(CFKBookmarkNode::eNT_Folder, m_pBookmarkRootNode);
        pMenu->m_strTitle = tr("Bookmarks Menu");
    }
    else
    {
        m_pBookmarkRootNode->Add(pMenu);
    }

    for (int i = 0; i < others.count(); ++i)
    {
        pMenu->Add(others.at(i));
    }
}
//-----------------------------------------------------
