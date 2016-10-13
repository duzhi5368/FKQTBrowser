/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QDateTime>
//-----------------------------------------------------
#include "fkcookiesdialog.h"
#include "fkcookiemodel.h"
//-----------------------------------------------------
CFKCookiesDialog::CFKCookiesDialog(CFKCookieJarMgr* pMgr, QWidget* pParent)
    : QDialog(pParent)
{
    setupUi(this);
    setWindowFlags(Qt::Sheet);

    // 创建Cookie的列表显示
    CFKCookieModel* pModel = new CFKCookieModel(pMgr, this);
    // 多列排序器
    m_pProxyModel = new QSortFilterProxyModel(this);

    // 设置消息函数关联
    connect(search, SIGNAL(textChanged(QString)),
            m_pProxyModel, SLOT(setFilterFixedString(QString)));
    connect(removeButton, SIGNAL(clicked()),
            cookiesTable, SLOT(removeOne()));
    connect(removeAllButton, SIGNAL(clicked()),
            cookiesTable, SLOT(removeAll()));
    m_pProxyModel->setSourceModel(pModel);

    // 设置表单样式
    cookiesTable->verticalHeader()->hide();
    cookiesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    cookiesTable->setModel(m_pProxyModel);
    cookiesTable->setAlternatingRowColors(true);
    cookiesTable->setTextElideMode(Qt::ElideMiddle);
    cookiesTable->setShowGrid(false);
    cookiesTable->setSortingEnabled(true);

    QFont tagFont = font();
    tagFont.setPointSize(10);
    QFontMetrics tagFontMetrics(tagFont);
    int nHeight = tagFontMetrics.height() + tagFontMetrics.height()/3;
    cookiesTable->verticalHeader()->setDefaultSectionSize(nHeight);
    cookiesTable->verticalHeader()->setMinimumSectionSize(-1);
    for (int i = 0; i < pModel->columnCount(); ++i)
    {
        int nHeaderLength = cookiesTable->horizontalHeader()->sectionSizeHint(i);
        switch (i)
        {
        case 0:
            nHeaderLength = tagFontMetrics.width(QLatin1String("averagehost.domain.com"));
            break;
        case 1:
            nHeaderLength = tagFontMetrics.width(QLatin1String("_session_id"));
            break;
        case 4:
            nHeaderLength = tagFontMetrics.width(QDateTime::currentDateTime().toString(Qt::LocalDate));
            break;
        }
        int nBufferLen = tagFontMetrics.width(QLatin1String("xx"));
        nHeaderLength += nBufferLen;
        cookiesTable->horizontalHeader()->resizeSection(i, nHeaderLength);
    }
    cookiesTable->horizontalHeader()->setStretchLastSection(true);
}
//-----------------------------------------------------
