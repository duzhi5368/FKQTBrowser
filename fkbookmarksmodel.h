/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBOOKMARKSMODEL_H
#define FKBOOKMARKSMODEL_H
//-----------------------------------------------------
#include <QtCore/QAbstractItemModel>
//-----------------------------------------------------
class CFKBookmarkNode;
class CFKBookmarksMgr;
//-----------------------------------------------------
class CFKBookmarksModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum ENUM_Roles{
        eTypeRole       = Qt::UserRole + 1,
        eUrlRole        = Qt::UserRole + 2,
        eUrlStringRole  = Qt::UserRole + 3,
        eSeparatorRole  = Qt::UserRole + 4
    };
public:
    CFKBookmarksModel(CFKBookmarksMgr* pBookmarkMgr, QObject* pParent = NULL);
public:
    CFKBookmarksMgr*        GetBookmarkMgr() const;
    CFKBookmarkNode*        GetNode(const QModelIndex& index)const;
    QModelIndex             GetIndex(CFKBookmarkNode* pNode)const;
public slots:
    void                    SLOT_EntryAdded(CFKBookmarkNode* pItem);
    void                    SLOT_EntryRemoved(CFKBookmarkNode* pParent, int nRow, CFKBookmarkNode* pItem);
    void                    SLOT_EntryChanged(CFKBookmarkNode* pItem);
public:
    QVariant                headerData(int nSection, Qt::Orientation eOrientation, int nRole = Qt::DisplayRole) const;
    QVariant                data(const QModelIndex &index, int nRole = Qt::DisplayRole) const;
    int                     columnCount(const QModelIndex &parent = QModelIndex()) const;
    int                     rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex             index(int nRow, int nColumn, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex             parent(const QModelIndex& index= QModelIndex()) const;
    Qt::ItemFlags           flags(const QModelIndex& index) const;
    Qt::DropActions         supportedDropActions () const;
    bool                    removeRows(int nRow, int nCount, const QModelIndex &parent = QModelIndex());
    bool                    setData(const QModelIndex &index, const QVariant &value, int nRole = Qt::EditRole);
    QMimeData*              mimeData(const QModelIndexList &indexes) const;
    QStringList             mimeTypes() const;
    bool                    dropMimeData(const QMimeData* pData,
        Qt::DropAction eAction, int nRow, int nColumn, const QModelIndex &parent);
    bool                    hasChildren(const QModelIndex &parent = QModelIndex()) const;
private:
    CFKBookmarksMgr*        m_pBookmarkMgr;
    bool                    m_bIsEndMacro;
};
//-----------------------------------------------------
#endif // FKBOOKMARKSMODEL_H
