/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkbookmarknode.h"
//-----------------------------------------------------
CFKBookmarkNode::CFKBookmarkNode(ENUM_NodeType eType, CFKBookmarkNode* pParent)
    : m_bIsExpanded(false)
    , m_pParent(pParent)
    , m_eNodeType(eType)
{
    if(m_pParent)
    {
        m_pParent->Add(this);
    }
}
//-----------------------------------------------------
CFKBookmarkNode::~CFKBookmarkNode()
{
    if (m_pParent)
    {
        m_pParent->Remove(this);
    }
    qDeleteAll(m_ListChildrens);
    m_pParent = NULL;
    m_eNodeType = CFKBookmarkNode::eNT_Root;
}
//-----------------------------------------------------
bool CFKBookmarkNode::operator==(const CFKBookmarkNode& other)
{
    if (m_strUrl != other.m_strUrl
        || m_strTitle != other.m_strTitle
        || m_strDesc != other.m_strDesc
        || m_bIsExpanded != other.m_bIsExpanded
        || m_eNodeType != other.m_eNodeType
        || m_ListChildrens.count() != other.m_ListChildrens.count())
    {
        return false;
    }

    for (int i = 0; i < m_ListChildrens.count(); ++i)
    {
        if (!((*(m_ListChildrens[i])) == (*(other.m_ListChildrens[i]))))
        {
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------
CFKBookmarkNode::ENUM_NodeType CFKBookmarkNode::GetType() const
{
    return m_eNodeType;
}
//-----------------------------------------------------
void CFKBookmarkNode::SetType(ENUM_NodeType eType)
{
    m_eNodeType = eType;
}
//-----------------------------------------------------
QList<CFKBookmarkNode*> CFKBookmarkNode::GetChildrens() const
{
    return m_ListChildrens;
}
//-----------------------------------------------------
CFKBookmarkNode* CFKBookmarkNode::GetParent() const
{
    return m_pParent;
}
//-----------------------------------------------------
void CFKBookmarkNode::Add(CFKBookmarkNode* pChild, int nOffset)
{
    Q_ASSERT(pChild->m_eNodeType != eNT_Root);

    if (pChild->m_pParent)
    {
        pChild->m_pParent->Remove(pChild);
    }
    pChild->m_pParent = this;
    if (-1 == nOffset)
    {
        nOffset = m_ListChildrens.size();
    }
    m_ListChildrens.insert(nOffset, pChild);
}
//-----------------------------------------------------
void CFKBookmarkNode::Remove(CFKBookmarkNode* pChild)
{
    pChild->m_pParent = NULL;
    m_ListChildrens.removeAll(pChild);
}
//-----------------------------------------------------
