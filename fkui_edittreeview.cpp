/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QtEvents>
//-----------------------------------------------------
#include "fkui_edittreeview.h"
#include "fkbrowserapp.h"
#include "fkhistorymodel.h"
#include "fkhistorymgr.h"
//-----------------------------------------------------
CFKUI_EditTreeView::CFKUI_EditTreeView(QWidget* pParent)
    : QTreeView(pParent)
{

}
//-----------------------------------------------------
void CFKUI_EditTreeView::keyPressEvent(QKeyEvent* pEvent)
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
void CFKUI_EditTreeView::SLOT_RemoveOne()
{
    if (!model())
        return;
    QModelIndex ci = currentIndex();
    CFKBrowserApp::GetHistoryMgrPtr()->RemoveHistoryEntry(
                model()->data(ci,CFKHistoryModel::eUrlStringRole).toString());
}
//-----------------------------------------------------
void CFKUI_EditTreeView::SLOT_RemoveAll()
{
    if (!model())
        return;
    CFKBrowserApp::GetHistoryMgrPtr()->SLOT_Clear();
}
//-----------------------------------------------------
