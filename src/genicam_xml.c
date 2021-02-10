#include <errno.h>
#include <string.h>
#include "genicam_xml.h"

xmlNode* node_search_rec( xmlNode* node, const char* Name ) ;

genicam_t* genicam_new( cam_gigevision_t* camera, char* filename ) {
    genicam_t *tmp = NULL ;
    tmp = malloc(sizeof(genicam_t)) ;
    if (tmp==NULL) {
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return NULL ;
    }
    tmp->cam = camera ;
    xmlInitParser();
    LIBXML_TEST_VERSION

    tmp->doc = xmlParseFile(filename);
    if (tmp->doc == NULL) {
        fprintf(stderr, "Error: unable to parse file \"%s\"\n", filename);
        return NULL;
    }

    tmp->root = xmlDocGetRootElement(tmp->doc);
    return tmp ;
}

void genicam_del( genicam_t* genicam ) {
    xmlCleanupParser() ;   // FIXME -> THREAD SAFE ????????
    free( genicam ) ;
}

xmlNode* genicam_search( genicam_t* genicam, const char* Name ) {
    return node_search_rec( genicam->root, Name ) ;
}

xmlNode* node_search_rec( xmlNode* node, const char* Name ) {
    xmlChar* nom ;
    xmlNode* ret ;
    if ( node == NULL )
        return NULL ;
    if ( node->type != XML_ELEMENT_NODE )
        return NULL ;

    nom = xmlGetProp( node, "Name" ) ;
    if ( nom != NULL )
        if (!xmlStrcmp(nom,Name))
            return node ;
    ret = node_search_rec( node->children, Name ) ;
    if ( ret != NULL )
        return ret ;
    return node_search_rec( node->next, Name ) ;
}

xmlNode* node_search ( xmlNode* node, const char* Name ) {
    xmlChar* nom ;
    xmlNode* ret ;
    if ( node == NULL )
        return NULL ;
    if ( node->type != XML_ELEMENT_NODE )
        return NULL ;
    if (!xmlStrcmp(node->name,Name))
            return node ;
    return node_search( node->next, Name ) ;
}

