/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKFWRITER_H
#define FKFWRITER_H
//-----------------------------------------------------
#include <QtCore/QXmlStreamWriter>
//-----------------------------------------------------
class CFKBookmarkNode;
//-----------------------------------------------------
class CFKFWriter : public QXmlStreamWriter
{
public:
    CFKFWriter();
public:
    bool Write(const QString& strFileName, const CFKBookmarkNode* pRoot);
    bool Write(QIODevice* pDevice, const CFKBookmarkNode* pRoot);

private:
    void __WriteItem(const CFKBookmarkNode* pParent);
};
//-----------------------------------------------------
#endif // FKFWRITER_H
