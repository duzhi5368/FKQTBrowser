/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>

#include <QtCore/QBuffer>
#include <QtCore/QMimeData>
//-----------------------------------------------------
#include "fkbookmarksmodel.h"
#include "fkbookmarknode.h"
#include "fkbookmarksmgr.h"
#include "fkbrowserapp.h"
#include "fkfreader.h"
#include "fkfwriter.h"
//-----------------------------------------------------
CFKBookmarksModel::CFKBookmarksModel(CFKBookmarksMgr* pBookmarkMgr, QObject* pParent)
    : QAbstractItemModel(pParent)
    , m_bIsEndMacro(false)
    , m_pBookmarkMgr(pBookmarkMgr)
{
    connect(pBookmarkMgr, SIGNAL(SIGNAL_EntryAdded(CFKBookmarkNode*)),
            this, SLOT(SLOT_EntryAdded(CFKBookmarkNode*)));
    connect(pBookmarkMgr, SIGNAL(SIGNAL_EntryRemoved(CFKBookmarkNode*,int,CFKBookmarkNode*)),
            this, SLOT(SLOT_EntryRemoved(CFKBookmarkNode*,int,CFKBookmarkNode*)));
    connect(pBookmarkMgr, SIGNAL(SIGNAL_EntryChanged(CFKBookmarkNode*)),
            this, SLOT(SLOT_EntryChanged(CFKBookmarkNode*)));
}
//-----------------------------------------------------
CFKBookmarksMgr* CFKBookmarksModel::GetBookmarkMgr() const
{
    return m_pBookmarkMgr;
}
//-----------------------------------------------------
CFKBookmarkNode* CFKBookmarksModel::GetNode(const QModelIndex& index)const
{
    CFKBookmarkNode *pItemNode = static_cast<CFKBookmarkNode*>(index.internalPointer());
    if (!pItemNode)
        return m_pBookmarkMgr->GetBookmarks();
    return pItemNode;
}
//-----------------------------------------------------
QModelIndex CFKBookmarksModel::GetIndex(CFKBookmarkNode* pNode)const
{
    CFKBookmarkNode *parent = pNode->GetParent();
    if (!parent)
        return QModelIndex();
    return createIndex(parent->GetChildrens().indexOf(pNode), 0, pNode);
}
//-----------------------------------------------------
void CFKBookmarksModel::SLOT_EntryAdded(CFKBookmarkNode* pItem)
{
    Q_ASSERT(pItem && pItem->GetParent());

    int nRow = pItem->GetParent()->GetChildrens().indexOf(pItem);
    CFKBookmarkNode* pParent = pItem->GetParent();
    pParent->Remove(pItem);

    beginInsertRows(GetIndex(pParent), nRow, nRow);
    pParent->Add(pItem, nRow);
    endInsertRows();
}
//-----------------------------------------------------
void CFKBookmarksModel::SLOT_EntryRemoved(CFKBookmarkNode* pParent, int nRow, CFKBookmarkNode* pItem)
{
    pParent->Add(pItem, nRow);
    beginRemoveRows(GetIndex(pParent), nRow, nRow);
    pParent->Remove(pItem);
    endRemoveRows();
}
//-----------------------------------------------------
void CFKBookmarksModel::SLOT_EntryChanged(CFKBookmarkNode* pItem)
{
    QModelIndex idx = GetIndex(pItem);
    emit dataChanged(idx, idx);
}
//-----------------------------------------------------
QVariant CFKBookmarksModel::headerData(int nSection, Qt::Orientation eOrientation, int nRole) const
{
    if (eOrientation == Qt::Horizontal
            && nRole == Qt::DisplayRole)
    {
        switch (nSection) {
            case 0: return tr("Title");
            case 1: return tr("Address");
        }
    }
    return QAbstractItemModel::headerData(nSection, eOrientation, nRole);
}
//-----------------------------------------------------
QVariant CFKBookmarksModel::data(const QModelIndex &index, int nRole) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    const CFKBookmarkNode* pBookmarkNode = GetNode(index);
    switch (nRole)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
        if (pBookmarkNode->GetType() == CFKBookmarkNode::eNT_Separator)
        {
            switch (index.column())
            {
            case 0:
                return QString(50, 0xB7);
            case 1:
                return QString();
            }
        }

        switch (index.column())
        {
        case 0:
            return pBookmarkNode->m_strTitle;
        case 1:
            return pBookmarkNode->m_strUrl;
        }
        break;
    case CFKBookmarksModel::eUrlRole:
        return QUrl(pBookmarkNode->m_strUrl);
        break;
    case CFKBookmarksModel::eUrlStringRole:
        return pBookmarkNode->m_strUrl;
        break;
    case CFKBookmarksModel::eTypeRole:
        return pBookmarkNode->GetType();
        break;
    case CFKBookmarksModel::eSeparatorRole:
        return (pBookmarkNode->GetType() == CFKBookmarkNode::eNT_Separator);
        break;
    case Qt::DecorationRole:
        if (index.column() == 0)
        {
            if (pBookmarkNode->GetType() == CFKBookmarkNode::eNT_Folder)
            {
                return QApplication::style()->standardIcon(QStyle::SP_DirIcon);
            }
            return CFKBrowserApp::GetAppInstancePtr()->GetIcon(pBookmarkNode->m_strUrl);
        }
    }

    return QVariant();
}
//-----------------------------------------------------
int CFKBookmarksModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : 2;
}
//-----------------------------------------------------
int CFKBookmarksModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return m_pBookmarkMgr->GetBookmarks()->GetChildrens().count();

    const CFKBookmarkNode* pItem = static_cast<CFKBookmarkNode*>(parent.internalPointer());
    return pItem->GetChildrens().count();
}
//-----------------------------------------------------
QModelIndex CFKBookmarksModel::index(int nRow, int nColumn, const QModelIndex& parent) const
{
    if (nRow < 0 || nColumn < 0
            || nRow >= rowCount(parent)
            || nColumn >= columnCount(parent))
    {
        return QModelIndex();
    }

    CFKBookmarkNode* pParentNode = GetNode(parent);
    return createIndex(nRow, nColumn, pParentNode->GetChildrens().at(nRow));
}
//-----------------------------------------------------
QModelIndex CFKBookmarksModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    CFKBookmarkNode* pItemNode      = GetNode(index);
    CFKBookmarkNode* pParentNode    = (pItemNode ? pItemNode->GetParent() : NULL);
    if (!pParentNode || pParentNode == m_pBookmarkMgr->GetBookmarks())
    {
        return QModelIndex();
    }

    CFKBookmarkNode* pGrandParentNode = pParentNode->GetParent();
    int nParentRow = pGrandParentNode->GetChildrens().indexOf(pParentNode);
    Q_ASSERT(nParentRow >= 0);
    return createIndex(nParentRow, 0, pParentNode);
}
//-----------------------------------------------------
Qt::ItemFlags CFKBookmarksModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags eFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    CFKBookmarkNode* pBookmarkNode = GetNode(index);

    if (pBookmarkNode != m_pBookmarkMgr->GetMenu()
        && pBookmarkNode != m_pBookmarkMgr->GetToolbar())
    {
        eFlags |= Qt::ItemIsDragEnabled;
        if (pBookmarkNode->GetType() != CFKBookmarkNode::eNT_Separator)
        {
            eFlags |= Qt::ItemIsEditable;
        }
    }
    if (hasChildren(index))
    {
        eFlags |= Qt::ItemIsDropEnabled;
    }
    return eFlags;
}
//-----------------------------------------------------
Qt::DropActions CFKBookmarksModel::supportedDropActions () const
{
    return Qt::CopyAction | Qt::MoveAction;
}
//-----------------------------------------------------
bool CFKBookmarksModel::removeRows(int nRow, int nCount, const QModelIndex &parent)
{
    if (nRow < 0 || nCount <= 0 ||
            nRow + nCount > rowCount(parent))
    {
        return false;
    }

    CFKBookmarkNode* pBookmarkNode = GetNode(parent);
    for (int i = nRow + nCount - 1; i >= nRow; --i)
    {
        CFKBookmarkNode* pNode = pBookmarkNode->GetChildrens().at(i);
        if (pNode == m_pBookmarkMgr->GetMenu()
            || pNode == m_pBookmarkMgr->GetToolbar())
        {
            continue;
        }

        m_pBookmarkMgr->RemoveBookmark(pNode);
    }
    if (m_bIsEndMacro)
    {
        m_pBookmarkMgr->GetUndoRedoStack()->endMacro();
        m_bIsEndMacro = false;
    }
    return true;
}
//-----------------------------------------------------
bool CFKBookmarksModel::setData(const QModelIndex &index, const QVariant &value, int nRole)
{
    if (!index.isValid() || (flags(index) & Qt::ItemIsEditable) == 0)
        return false;

    CFKBookmarkNode* pItem = GetNode(index);

    switch (nRole)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
        if (index.column() == 0)
        {
            m_pBookmarkMgr->SetTitle(pItem, value.toString());
            break;
        }
        if (index.column() == 1)
        {
            m_pBookmarkMgr->SetUrl(pItem, value.toString());
            break;
        }
        return false;
        break;
    case CFKBookmarksModel::eUrlRole:
        m_pBookmarkMgr->SetUrl(pItem, value.toUrl().toString());
        break;
    case CFKBookmarksModel::eUrlStringRole:
        m_pBookmarkMgr->SetUrl(pItem, value.toString());
        break;
    default:
        break;
        return false;
    }

    return true;
}
//-----------------------------------------------------
QMimeData* CFKBookmarksModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* pMimeData = new QMimeData();
    QByteArray strData;
    QDataStream stream(&strData, QIODevice::WriteOnly);
    foreach (QModelIndex index, indexes)
    {
        if (index.column() != 0 || !index.isValid())
            continue;

        QByteArray strEncodedData;
        QBuffer buffer(&strEncodedData);
        buffer.open(QBuffer::ReadWrite);

        CFKFWriter writer;
        const CFKBookmarkNode* pParentNode = GetNode(index);
        writer.Write(&buffer, pParentNode);
        stream << strEncodedData;
    }
    pMimeData->setData(QLatin1String("application/bookmarks.fkf")
, strData);
    return pMimeData;
}
//-----------------------------------------------------
QStringList CFKBookmarksModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/bookmarks.fkf");
    return types;
}
//-----------------------------------------------------
bool CFKBookmarksModel::dropMimeData(const QMimeData* pData,
    Qt::DropAction eAction, int nRow, int nColumn, const QModelIndex &parent)
{
    if (eAction == Qt::IgnoreAction)
        return true;

    if (!pData->hasFormat(QLatin1String("application/bookmarks.fkf"))
        || nColumn > 0)
    {
        return false;
    }

    QByteArray strTmp = pData->data(QLatin1String("application/bookmarks.fkf"));
    QDataStream stream(&strTmp, QIODevice::ReadOnly);
    if (stream.atEnd())
    {
        return false;
    }

    QUndoStack* pUndoStack = m_pBookmarkMgr->GetUndoRedoStack();
    pUndoStack->beginMacro(QLatin1String("Move Bookmarks"));

    while (!stream.atEnd())
    {
        QByteArray strEncodedData;
        stream >> strEncodedData;
        QBuffer buffer(&strEncodedData);
        buffer.open(QBuffer::ReadOnly);

        CFKFReader reader;
        CFKBookmarkNode* pRootNode = reader.Read(&buffer);
        QList<CFKBookmarkNode*> ListChildrens = pRootNode->GetChildrens();
        for (int i = 0; i < ListChildrens.count(); ++i)
        {
            CFKBookmarkNode* pBookmarkNode = ListChildrens.at(i);
            pRootNode->Remove(pBookmarkNode);
            nRow = qMax(0, nRow);
            CFKBookmarkNode* pParentNode = GetNode(parent);
            m_pBookmarkMgr->AddBookmark(pParentNode, pBookmarkNode, nRow);
            m_bIsEndMacro = true;
        }
        delete pRootNode;
    }
    return true;
}
//-----------------------------------------------------
bool CFKBookmarksModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return true;
    const CFKBookmarkNode* pParentNode = GetNode(parent);
    return (pParentNode->GetType() == CFKBookmarkNode::eNT_Folder);
}
//-----------------------------------------------------
