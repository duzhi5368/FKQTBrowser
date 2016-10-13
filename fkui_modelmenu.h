/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_MODELMENU_H
#define FKUI_MODELMENU_H
//-----------------------------------------------------
#include <QtWidgets/QMenu>

#include <QtCore/QAbstractItemModel>
//-----------------------------------------------------
class CFKUI_ModelMenu : public QMenu
{
    Q_OBJECT
signals:
    void                    SIGNAL_Activated(const QModelIndex& index);
    void                    SIGNAL_Hovered(const QString& text);
public:
    CFKUI_ModelMenu(QWidget* pParent = NULL);
public:
    void                    SetModel(QAbstractItemModel* pModel);
    QAbstractItemModel*     GetModel() const;
    void                    SetMaxRows(int nMaxRows);
    int                     GetMaxRows() const;
    void                    SetFirstSeparator(int nOffset);
    int                     GetFirstSeparator() const;
    void                    SetRootIndex(const QModelIndex& index);
    QModelIndex             GetRootIndex() const;
    void                    SetHoverRole(int nRole);
    int                     GetHoverRole() const;
    void                    SetSeparatorRole(int nRole);
    int                     GetSeparatorRole() const;
public:
    QAction*                MakeAction(const QIcon& icon, const QString& text, QObject* pParent);
    QAction*                MakeAction(const QModelIndex& index);
protected:
    virtual bool            prePopulated();
    virtual void            postPopulated();
    void                    _CreateMenu(const QModelIndex &parent, int nMax, QMenu* pParentMenu = NULL, QMenu* pMenu = NULL);
private slots:
    void                    __SLOT_AboutToShow();
    void                    __SLOT_Triggered(QAction* pAction);
    void                    __SLOT_Hovered(QAction* pAction);
private:
    int                     m_nMaxRows;
    int                     m_nFirstSeparator;
    int                     m_nMaxWidth;
    int                     m_nHoverRole;
    int                     m_nSeparatorRole;
    QAbstractItemModel*     m_pModel;
    QPersistentModelIndex   m_Root;
};
//-----------------------------------------------------
#endif // FKUI_MODELMENU_H
