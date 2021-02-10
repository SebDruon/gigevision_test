#include "genicam_xml.h"

int64_t getintvalue_enumeration(genicam_t*, xmlNode*) ;

int64_t IEnumeration_GetIntValue( genicam_t* genicam, const char* name ) {
    xmlNode* node ;
    node = genicam_search( genicam, name ) ;
    if ( node == NULL ) {
        fprintf(stderr, "Could not find genicam node \"%s\"\n", name ) ;
        return 0 ;
    }
    if (!xmlStrcmp(node->name,"Enumeration")) {
        return getintvalue_enumeration( genicam, node ) ;
    } 

    fprintf(stderr, "Unknown IEnumeration type: %s\n", name ) ;
    return 0 ;
}

int64_t getintvalue_enumeration(genicam_t* genicam, xmlNode* node) {
   xmlNode* valuenode ;
   valuenode = node_search( node->children, "Value" ) ;
   if ( valuenode != NULL ) {
       int64_t value ;
       xmlChar *key; 
       key = xmlNodeGetContent( valuenode );
       sscanf(key,"%ld",&value) ;
       fprintf(stderr, "--> %ld\n", value ) ;
       return value ;
   }
   valuenode = node_search( node->children, "pValue" ) ;
   if ( valuenode != NULL ) {
       xmlChar *key; 
       key = xmlNodeGetContent( valuenode );
       fprintf(stderr, "-- %s\n", key ) ;
       return IInteger_GetValue( genicam, key )  ;
   }
   fprintf(stderr, "No Value node found\n") ;
   return 0 ;
}

