//XMLConfig.cpp
#include <iostream>

#include "common/xml/XMLConfig.h"

#include "common/comm/Error.h"
#include "common/log/log.h"

using namespace std;

XMLConfig::XMLConfig() :
    m_pDoc( NULL ),
    m_pRoot( NULL ),
    m_pBuf( NULL ),
    m_xmlType( UNINIT )
{
}

XMLConfig::XMLConfig( const char* fileName ) :
    m_pDoc( NULL ),
    m_pRoot( NULL ),
    m_pBuf( NULL ),
    m_xmlType( UNINIT )
{
    if ( NULL == loadFile( fileName ) )
    {
        ERROR_LOG( "loadFile error: %s" , fileName );
    }
}

XMLConfig::~XMLConfig()
{
    try
    {
        if ( m_pDoc != NULL )
        {
            xmlFreeDoc( m_pDoc );
        }
        if ( m_pBuf != NULL )
        {
            xmlBufferFree( m_pBuf );
        }
        vector<xmlChar*>::iterator vit;
        for ( vit = m_attrs.begin(); vit != m_attrs.end(); ++vit )
        {
            xmlFree( *vit );
        }
    }
    catch ( ... )
    {
    }
}

XMLNode* XMLConfig::loadMemory(
    const char* buf,
    unsigned int size )
{
    //free doc handle
    if ( m_pDoc != NULL )
    {
        xmlFreeDoc( m_pDoc );
    }

    m_xmlType = XMLMEM;
    m_pDoc = xmlReadMemory(
                 buf, size, NULL, "UTF-8", int( XML_PARSE_RECOVER ) );
    if ( NULL == m_pDoc )
    {
        ERROR_LOG( "read file error" );
        return NULL;
    }
    xmlNodePtr node = xmlDocGetRootElement( m_pDoc );
    if ( NULL == node )
    {
        ERROR_LOG( "empty file!" );
        return NULL;
    }
    m_pRoot = node;
    return m_pRoot;
}

char* XMLConfig::getXMLBuffer()
{
    return getXMLBuffer( m_pRoot );
}

char* XMLConfig::getXMLBuffer( XMLNode* node )
{
    if ( m_pBuf != NULL )
    {
        xmlBufferFree( m_pBuf );
        m_pBuf = NULL;
    }

    m_pBuf = xmlBufferCreate();
    if ( m_pBuf == NULL )
    {
        ERROR_LOG( "xmlBufferCreate error" );
        return NULL;
    }

    int bytes = xmlNodeDump( m_pBuf, m_pDoc, node, 0, 1 );
    if ( bytes < 0 )
    {
        ERROR_LOG( "xmlNodeDump failed" );
        return NULL;
    }

    return ( char* )m_pBuf->content;
}

XMLNode* XMLConfig::loadFile( const char* xname )
{
    //free doc handle
    if ( m_pDoc != NULL )
    {
        xmlFreeDoc( m_pDoc );
    }

    m_fileName = xname;
    m_xmlType = XMLFILE;
    m_pDoc = xmlReadFile( 
        xname, "UTF-8", int( XML_PARSE_RECOVER ) );
    if ( NULL == m_pDoc )
    {
        ERROR_LOG( "read file error" );
        return NULL;
    }
    xmlNodePtr node = xmlDocGetRootElement( m_pDoc );
    if ( NULL == node )
    {
        ERROR_LOG( "empty file!" );
        return NULL;
    }
    m_pRoot = node;
    return m_pRoot;
}

XMLNode* XMLConfig::getParent( const XMLNode* curnode ) const
{
    return curnode->parent;
}

int XMLConfig::getFirstNode( 
    const char* xpath, XMLNode*& node )
{
    xmlXPathContextPtr context = xmlXPathNewContext( m_pDoc );
    if ( NULL == context )
    {
        ERROR_LOG( "content is NULL" );
        return FAILED;
    }
    xmlXPathObjectPtr result =
        xmlXPathEvalExpression( BAD_CAST xpath, context );
    xmlXPathFreeContext( context );
    if ( NULL == result )
    {
        ERROR_LOG( "xmlXPathEvalExpression return NULL" );
        xmlXPathFreeObject( result );
        return FAILED;
    }

    xmlNodeSetPtr pNodeset = result->nodesetval;
    if ( pNodeset->nodeNr >= 1 )
    {
        node = pNodeset->nodeTab[0];
    }
    else
    {
        ERROR_LOG( "no such path: %s", xpath );
        return FAILED;
    }
    xmlXPathFreeObject( result ); //new add
    return SUCCESSFUL;
}

int XMLConfig::getNodeSet(
    const char* xpath,
    vector<XMLNode*>& nodes )
{
    xmlXPathContextPtr context = xmlXPathNewContext( m_pDoc );
    if ( NULL == context )
    {
        ERROR_LOG( "content is NULL" );
        return FAILED;
    }
    xmlXPathObjectPtr result =
        xmlXPathEvalExpression( BAD_CAST xpath, context );
    xmlXPathFreeContext( context );
    if ( NULL == result )
    {
        ERROR_LOG( "xmlXPathEvalExpression return NULL" );
        xmlXPathFreeObject( result );
        return FAILED;
    }

    xmlNodeSetPtr pNodeset = result->nodesetval;
    for ( int i = 0; i != pNodeset->nodeNr; ++i )
    {
        /*  lint -e(534) */
        nodes.push_back( pNodeset->nodeTab[i] ); //no return value
    }
    xmlXPathFreeObject( result ); //new add

    return SUCCESSFUL;
}

XMLNode* XMLConfig::getFirstChild( XMLNode* curnode ) const
{
    vector<XMLNode*> nodeset;
    XMLNode* node = NULL;
    if ( getChildren( curnode, nodeset ) < 0 )
    {
        ERROR_LOG( "getChildren error" );
        return NULL;
    }

    if ( nodeset.size() > 0 )
    {
        node = *( nodeset.begin() );
    }
    else
    {
        ERROR_LOG( "this node has no child" );
        node = NULL;
    }

    return node;
}


XMLNode* XMLConfig::getSpecificChild(
    XMLNode* curnode,
    const char* nodeName ) const
{
    vector<XMLNode*> nodeset;
    if ( getChildren( curnode, nodeset ) < 0 )
    {
        ERROR_LOG( "getChildren error" );
        return NULL;
    }

    vector<XMLNode*>::iterator vit;
    for ( vit = nodeset.begin(); vit != nodeset.end(); ++vit )
    {
        if ( !xmlStrcmp( ( *vit )->name, BAD_CAST nodeName ) )
        {
            return *vit;
        }
    }
    ERROR_LOG( "this node has no child named %s", nodeName );

    return NULL;
}

int XMLConfig::getSpecificChildren(
    XMLNode* curnode,
    const char* nodeName,
    std::vector<XMLNode*>& nodes ) const
{
    vector<XMLNode*> children;
    if ( getChildren( curnode, children ) != SUCCESSFUL )
    {
        ERROR_LOG( "getChildren error" );
        return FAILED;
    }

    vector<XMLNode*>::iterator vit;
    for ( vit = children.begin(); vit != children.end(); ++vit )
    {
        if ( !xmlStrcmp( ( *vit )->name, BAD_CAST nodeName ) )
        {
            /*  lint -e(534) */
            nodes.push_back( *vit ); //no return value
        }
    }

    return SUCCESSFUL;
}

int XMLConfig::getChildren(
    const XMLNode* curnode,
    vector<XMLNode*>& children ) const
{
    if ( NULL == curnode )
    {
        return FAILED;
    }
    xmlNodePtr child = curnode->xmlChildrenNode;
    while ( child != NULL )
    {
        if ( xmlStrcmp( child->name, BAD_CAST "text" ) ) //如果不是text
        {
            children.push_back( child );
        }
        child = child->next;
    }

    return SUCCESSFUL;
}

int XMLConfig::getProperty(
    XMLNode* node,
    const char* property,
    std::string& value ) const //new
{
    xmlChar* attr = NULL;
    if ( xmlHasProp( node, BAD_CAST property ) )
    {
        attr = xmlGetProp( node, BAD_CAST property );
        value = ( const char* )attr;
        xmlFree( attr );
    }
    else
    {
        ERROR_LOG( "no such property:  %s", property );
        return FAILED;
    }

    return SUCCESSFUL;
    //need to xmlFree()
}
const char* XMLConfig::getProperty(
    XMLNode* node,
    const char* property )
{
    xmlChar* attr = NULL;
    if ( xmlHasProp( node, BAD_CAST property ) )
    {
        attr = xmlGetProp( node, BAD_CAST property );
        /*  lint -e(534) */
        m_attrs.push_back( attr ); //no return value
    }
    else
    {
        ERROR_LOG( "no such property:  %s", property );
        attr = NULL;
    }

    return ( const char* )attr;
    //need to xmlFree()
}

int XMLConfig::setProperty(
    XMLNode* node,
    const char* property,
    const char* value )
{
    if ( xmlHasProp( node, BAD_CAST property ) )
    {
        if ( NULL == xmlSetProp(
                    node, BAD_CAST property, BAD_CAST value ) )
        {
            ERROR_LOG( "xmlSetProp error" );
        }
    }
    else
    {
        if ( NULL == xmlNewProp(
                    node, BAD_CAST property, BAD_CAST value ) )
        {
            ERROR_LOG( "xmlNewProp error" );
        }
    }

    return SUCCESSFUL;
}

int XMLConfig::delProperty( 
    XMLNode* node, const char* property )
{
    xmlAttrPtr attr = NULL;
    if ( ( attr = xmlHasProp( node, BAD_CAST property ) ) != NULL )
    {
        return xmlRemoveProp( attr ); //0 if success and -1 in case of error
    }
    else
    {
        ERROR_LOG( "no such property:  %s", property );
        return FAILED;
    }
}

XMLNode* XMLConfig::createXMLBuffer( const char* rootname )
{
    m_xmlType = XMLMEM;
    m_pDoc = xmlNewDoc( BAD_CAST "1.0" );
    m_pRoot = xmlNewNode( NULL, BAD_CAST rootname );
    /* lint -e(534) */
    xmlDocSetRootElement( m_pDoc, m_pRoot ); //return the old root element if any was found, NULL if root was NULL

    return m_pRoot;
}

XMLNode* XMLConfig::createFile(
    const char* filename,
    const char* rootname )
{
    m_xmlType = XMLFILE;
    m_fileName = filename;
    m_pDoc = xmlNewDoc( BAD_CAST "1.0" );
    m_pRoot = xmlNewNode( NULL, BAD_CAST rootname );
    /*  lint -e(534) */
    xmlDocSetRootElement( m_pDoc, m_pRoot ); //return the old root element if any was found, NULL if root was NULL

    return m_pRoot;
}

int XMLConfig::saveFile()
{
    return saveFile( m_fileName.c_str() );
}

int XMLConfig::saveFile( const char* filename )
{
    int nwrite = xmlSaveFormatFileEnc( filename, m_pDoc, "UTF-8", 1 );
    if ( nwrite != -1 )
    {
        INFO_LOG( "write %d bytes into %s", nwrite, filename );
    }
    else
    {
        ERROR_LOG( "saveFile error." );
    }

    return nwrite;
}

XMLNode* XMLConfig::addSimpleChild(
    XMLNode* parent,
    const char* nodename,
    const char* value )
{
    //m_changed = true;
    return xmlNewTextChild(
               parent, NULL, BAD_CAST nodename, BAD_CAST value );
}

XMLNode* XMLConfig::addChild(
    XMLNode* parent,
    const char* nodename )
{
    //m_changed = true;
    xmlNodePtr node = xmlNewNode ( NULL, BAD_CAST nodename );
    return xmlAddChild( parent, node );
    //return node;
}

XMLNode* XMLConfig::addChild( 
    XMLNode* parent, XMLNode* cur )
{
    xmlNodePtr node = xmlCopyNode( cur, 1 ); //if 1 do a recursive copy , if 2 copy properties and namespaces
    if ( node != NULL )
    {
        return xmlAddChild( parent, node );    //return the child or NULL in case of error
    }
    else
    {
        ERROR_LOG( "copy node error" );
        return NULL;
    }
}

int XMLConfig::getValue( XMLNode* node, std::string& value ) //new
{
    xmlChar* pValue = xmlNodeGetContent( node );
    if ( pValue != NULL )
    {
        value = ( const char* )pValue;
        xmlFree( pValue );
        return SUCCESSFUL;
    }
    else
    {
        return FAILED;
    }
}
const char* XMLConfig::getValue( XMLNode* node )
{
    xmlChar* p = xmlNodeGetContent( node );
    /*  lint -e(534) */
    m_attrs.push_back( p ); //no return value
    return ( const char* )p;
}

int XMLConfig::setValue( XMLNode* node, const char* value )
{
    xmlNodeSetContent( node, BAD_CAST value ); //no return
    return SUCCESSFUL;
}


int XMLConfig::getFirstNodeValue(
    const char* xpath,
    std::string& value ) //new
{
    XMLNode* node = NULL;
    if ( getFirstNode( xpath, node ) < 0 )
    {
        ERROR_LOG( "getFirstNode error" );
        return FAILED;
    }
    if ( node != NULL )
    {
        return getValue( node, value );
    }
    else
    {
        return FAILED;
    }
}

const char* XMLConfig::getFirstNodeValue( const char* xpath )
{
    XMLNode* node = NULL;
    if ( getFirstNode( xpath, node ) != SUCCESSFUL )
    {
        ERROR_LOG( "getFirstNode error" );
        return NULL;
    }

    return getValue( node );
}

XMLNode* XMLConfig::getRootNode()
{
    return m_pRoot;
}
