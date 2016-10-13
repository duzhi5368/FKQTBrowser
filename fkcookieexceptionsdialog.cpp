/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtWidgets/QCompleter>
//-----------------------------------------------------
#include "fkcookiejarmgr.h"
#include "fkcookiemodel.h"
#include "fkcookieexceptionsmodel.h"
#include "fkcookieexceptionsdialog.h"
//-----------------------------------------------------
CFKCookieExceptionsDialog::CFKCookieExceptionsDialog(CFKCookieJarMgr* pMgr, QWidget* pParent)
    : QDialog(pParent)
    , m_pCookieJarMgr(pMgr)
{
    setupUi(this);
    setWindowFlags(Qt::Sheet);

    // 创建控件内部消息关联
    connect(removeButton, SIGNAL(clicked()), exceptionTable, SLOT(removeOne()));
    connect(removeAllButton, SIGNAL(clicked()), exceptionTable, SLOT(removeAll()));

    // 组件关联绑定
    m_pExceptionsModel  = new CFKCookieExceptionsModel(pMgr, this);
    m_pProxyModel       = new QSortFilterProxyModel(this);

    m_pProxyModel->setSourceModel(m_pExceptionsModel);  // 绑定关联

    connect(search, SIGNAL(textChanged(QString)),
            m_pProxyModel, SLOT(setFilterFixedString(QString)));
    exceptionTable->setModel(m_pProxyModel);            // 绑定关联

    CFKCookieModel* pCookieModel = new CFKCookieModel(pMgr, this);
    domainLineEdit->setCompleter(new QCompleter(pCookieModel, domainLineEdit));

    connect(domainLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(SLOT_TextChanged(QString)));
    connect(blockButton, SIGNAL(clicked()), this, SLOT(SLOT_Block()));
    connect(allowButton, SIGNAL(clicked()), this, SLOT(SLOT_Allow()));
    connect(allowForSessionButton, SIGNAL(clicked()), this, SLOT(SLOT_AllowForSession()));

    // 修改部分控件属性
    exceptionTable->verticalHeader()->hide();
    exceptionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    exceptionTable->setAlternatingRowColors(true);
    exceptionTable->setTextElideMode(Qt::ElideMiddle);
    exceptionTable->setShowGrid(false);
    exceptionTable->setSortingEnabled(true);

    QFont tagFont = font();
    tagFont.setPointSize(10);
    QFontMetrics tagFontMetrics(tagFont);
    int height = tagFontMetrics.height() + tagFontMetrics.height()/3;
    exceptionTable->verticalHeader()->setDefaultSectionSize(height);
    exceptionTable->verticalHeader()->setMinimumSectionSize(-1);
    for (int i = 0; i < m_pExceptionsModel->columnCount(); ++i)
    {
        int nHeaderLen = exceptionTable->horizontalHeader()->sectionSizeHint(i);
        switch (i)
        {
        case 0:
            nHeaderLen = tagFontMetrics.width(QLatin1String("averagebiglonghost.domain.com"));
            break;
        case 1:
            nHeaderLen = tagFontMetrics.width(QLatin1String("Allow For Session"));
            break;
        }
        int nBufferLen = tagFontMetrics.width(QLatin1String("xx"));
        nHeaderLen += nBufferLen;
        exceptionTable->horizontalHeader()->resizeSection(i, nHeaderLen);
    }
}
//-----------------------------------------------------
void CFKCookieExceptionsDialog::SLOT_Block()
{
    if (domainLineEdit->text().isEmpty())
        return;

    m_pExceptionsModel->m_ListBlockedCookies.append(domainLineEdit->text());
    m_pCookieJarMgr->SetBlockedCookies(m_pExceptionsModel->m_ListBlockedCookies);
    m_pExceptionsModel->beginResetModel();
    m_pExceptionsModel->endResetModel();
}
//-----------------------------------------------------
void CFKCookieExceptionsDialog::SLOT_Allow()
{
    if (domainLineEdit->text().isEmpty())
        return;

    m_pExceptionsModel->m_ListAllowedCookies.append(domainLineEdit->text());
    m_pCookieJarMgr->SetBlockedCookies(m_pExceptionsModel->m_ListAllowedCookies);
    m_pExceptionsModel->beginResetModel();
    m_pExceptionsModel->endResetModel();
}
//-----------------------------------------------------
void CFKCookieExceptionsDialog::SLOT_AllowForSession()
{
    if (domainLineEdit->text().isEmpty())
        return;

    m_pExceptionsModel->m_ListSessionCookies.append(domainLineEdit->text());
    m_pCookieJarMgr->SetBlockedCookies(m_pExceptionsModel->m_ListSessionCookies);
    m_pExceptionsModel->beginResetModel();
    m_pExceptionsModel->endResetModel();
}
//-----------------------------------------------------
void CFKCookieExceptionsDialog::SLOT_TextChanged(const QString& text)
{
    bool bIsEnabled = !text.isEmpty();
    blockButton->setEnabled(bIsEnabled);
    allowButton->setEnabled(bIsEnabled);
    allowForSessionButton->setEnabled(bIsEnabled);
}
//-----------------------------------------------------
