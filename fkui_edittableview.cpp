/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QtEvents>
//-----------------------------------------------------
#include "fkui_edittableview.h"
//-----------------------------------------------------
CFKUI_EditTableView::CFKUI_EditTableView(QWidget* pParent)
    : QTableView(pParent)
{

}
//-----------------------------------------------------
void CFKUI_EditTableView::keyPressEvent(QKeyEvent* pEvent)
{
    if ((pEvent->key() == Qt::Key_Delete
        || pEvent->key() == Qt::Key_Backspace)
        && model())
    {
        SLOT_RemoveOne();
    }
    else
    {
        QAbstractItemView::keyPressEvent(pEvent);
    }
}
//-----------------------------------------------------
void CFKUI_EditTableView::SLOT_RemoveOne()
{
    if (!model() || !selectionModel())
        return;
    int nRow = currentIndex().row();
    model()->removeRow(nRow, rootIndex());
    QModelIndex tagIndex = model()->index(nRow, 0, rootIndex());
    if (!tagIndex.isValid())
    {
        tagIndex = model()->index(nRow - 1, 0, rootIndex());
    }
    selectionModel()->select(tagIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}
//-----------------------------------------------------
void CFKUI_EditTableView::SLOT_RemoveAll()
{
    if (!model())
        return;
    model()->removeRows(0, model()->rowCount(rootIndex()), rootIndex());
}
//-----------------------------------------------------
