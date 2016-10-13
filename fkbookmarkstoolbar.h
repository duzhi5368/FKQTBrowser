/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBOOKMARKSTOOLBAR_H
#define FKBOOKMARKSTOOLBAR_H
//-----------------------------------------------------
#include <QtWidgets/QToolBar>

#include <QtCore/QPersistentModelIndex>
//-----------------------------------------------------
class CFKBookmarksModel;
//-----------------------------------------------------
class CFKBookmarksToolBar : public QToolBar
{
    Q_OBJECT
signals:
    void            SIGNAL_OpenUrl(const QUrl& url);
public:
    CFKBookmarksToolBar(CFKBookmarksModel* pModel, QWidget* pParent = NULL);
public:
    void            SetRootIndex(const QModelIndex& index);
    QModelIndex     GetRootIndex() const;
protected:
    void            dragEnterEvent(QDragEnterEvent* pEvent);
    void            dropEvent(QDropEvent* pEvent);
private slots:
    void            __SLOT_Triggered(QAction* pAction);
    void            __SLOT_Activated(const QModelIndex& index);
    void            __SLOT_Build();
private:
    QPersistentModelIndex       m_Root;
    CFKBookmarksModel*          m_pBookmarksModel;
};
//-----------------------------------------------------
#endif // FKBOOKMARKSTOOLBAR_H
