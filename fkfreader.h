/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKFREADER_H
#define FKFREADER_H
//-----------------------------------------------------
#include <QtCore/QXmlStreamReader>
//-----------------------------------------------------
class CFKBookmarkNode;
//-----------------------------------------------------
class CFKFReader : public QXmlStreamReader
{
public:
    CFKFReader();
public:
    CFKBookmarkNode* Read(const QString& strFileName);
    CFKBookmarkNode* Read(QIODevice* pDevice);
private:
    void            __ReadFKF(CFKBookmarkNode* pParent);
    void            __ReadTitle(CFKBookmarkNode* pParent);
    void            __ReadDescription(CFKBookmarkNode* pParent);
    void            __ReadSeparator(CFKBookmarkNode* pParent);
    void            __ReadFolder(CFKBookmarkNode* pParent);
    void            __ReadBookmarkNode(CFKBookmarkNode* pParent);
};
//-----------------------------------------------------
#endif // FKFREADER_H
