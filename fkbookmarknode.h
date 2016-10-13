/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBOOKMARKNODE_H
#define FKBOOKMARKNODE_H
//-----------------------------------------------------
#include <QtCore/QString>
#include <QtCore/QList>
//-----------------------------------------------------
class CFKBookmarkNode
{
public:
    enum ENUM_NodeType{
        eNT_Root,
        eNT_Folder,
        eNT_Bookmark,
        eNT_Separator
    };
public:
    CFKBookmarkNode(ENUM_NodeType eType = eNT_Root, CFKBookmarkNode* pParent = NULL);
    ~CFKBookmarkNode();
    bool                    operator==(const CFKBookmarkNode& other);
public:
    ENUM_NodeType           GetType() const;
    void                    SetType(ENUM_NodeType eType);
    QList<CFKBookmarkNode*> GetChildrens() const;
    CFKBookmarkNode*        GetParent() const;

    void                    Add(CFKBookmarkNode* pChild, int nOffset = -1);
    void                    Remove(CFKBookmarkNode* pChild);
private:
    CFKBookmarkNode*        m_pParent;
    ENUM_NodeType           m_eNodeType;
    QList<CFKBookmarkNode*> m_ListChildrens;
public:
    QString                 m_strUrl;
    QString                 m_strTitle;
    QString                 m_strDesc;
    bool                    m_bIsExpanded;
};
//-----------------------------------------------------
#endif // FKBOOKMARKNODE_H
