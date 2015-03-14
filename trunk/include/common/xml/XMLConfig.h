//XMLConfig.h
#ifndef _XMLCONFIG_H_
#define _XMLCONFIG_H_

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>
#include <string>
#include <vector>

typedef xmlNode XMLNode;
enum XMLType {UNINIT = 0, XMLFILE, XMLMEM};

class XMLConfig
{
public:
    XMLConfig();
    XMLConfig( const char* fileName );
    ~XMLConfig();

    //for xml buffer
    XMLNode* loadMemory( const char* xbuf, unsigned int size );
    XMLNode* createXMLBuffer( const char* rootname );
    char* getXMLBuffer();
    char* getXMLBuffer( XMLNode* node );

    //for xml file
    XMLNode* loadFile( const char* );
    XMLNode* createFile( const char* name, const char* root );
    int saveFile();
    int saveFile( const char* filename );

    int getFirstNode( const char* xpath, XMLNode*& node );
    int getNodeSet( 
        const char* xpath, std::vector<XMLNode*>& nodes );

    XMLNode* getParent( const XMLNode* curnode ) const;
    XMLNode* getFirstChild( XMLNode* curnode ) const;
    int getChildren( 
        const XMLNode* curnode, std::vector<XMLNode*>& children ) const;

    XMLNode* getSpecificChild(
        XMLNode* curnode, const char* nodeName ) const;
    int getSpecificChildren( 
        XMLNode* curnode, 
        const char* nodeName, 
        std::vector<XMLNode*>& nodes ) const;

    const char* getProperty( XMLNode* node, const char* protery ); //to be replaced by the next interface, you'd better use the next
    int getProperty( 
        XMLNode* node, const char* protery, std::string& value ) const; //new
    int setProperty( 
        XMLNode* node, const char* protery, const char* value );
    int delProperty( XMLNode* node, const char* property );

    XMLNode* addSimpleChild( 
        XMLNode* parent, const char* nodename, const char* value );
    XMLNode* addChild( XMLNode* parent, const char* nodename );
    XMLNode* addChild( XMLNode* parent, XMLNode* cur );

    //int moveToNode(XMLNode *node);
    const char* getValue( XMLNode* node ); //to be replaced by the next interface, you'd better use the next
    int getValue( XMLNode* node, std::string& value ); //new
    int setValue( XMLNode* node, const char* value );

    const char* getFirstNodeValue( const char* xpath ); //to be replaced by the next interface, you'd better use the next
    int getFirstNodeValue( const char* xpath, std::string& value ); //new
    //void printProperties(XMLNode *node);

    XMLNode* getRootNode();

private:
    std::string m_fileName;
    xmlDocPtr m_pDoc;
    xmlNodePtr m_pRoot;
    xmlBufferPtr m_pBuf;
    //bool m_changed;
    XMLType m_xmlType;//0 for xmlfile, 1 for xml buffer
    xmlTextReaderPtr m_reader;
    std::vector<xmlChar*> m_attrs;//save to free them
};

#endif
