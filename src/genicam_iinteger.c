#include "genicam_xml.h"

int64_t getvalue_integer(genicam_t*, xmlNode*) ;
int64_t getvalue_intreg(genicam_t*, xmlNode*) ;


int64_t IInteger_GetValue( genicam_t* genicam, const char* name ) {
    xmlNode* node ;
    node = genicam_search( genicam, name ) ;
    if ( node == NULL ) {
        fprintf(stderr, "Could not find genicam node \"%s\"\n", name ) ;
        return 0 ;
    }
    if (!xmlStrcmp(node->name,"Integer")) {
        return getvalue_integer( genicam, node ) ;
    } else if (!xmlStrcmp(node->name,"IntReg")) {
        return getvalue_intreg( genicam, node ) ;
        return 0 ;
    } else if (!xmlStrcmp(node->name,"MaskedIntReg")) {
        fprintf(stderr, "MaskedIntReg Not Implemented Yet\n") ; // TODO
        return 0 ;
    } 

    fprintf(stderr, "Unknown IInteger type: %s\n", name ) ;
    return 0 ;
}

int64_t getvalue_integer(genicam_t* genicam, xmlNode* node) {
   xmlNode* valuenode ;
   valuenode = node_search( node->children, "Value" ) ;
   if ( valuenode != NULL ) {
       int64_t value ;
       xmlChar *key; 
       key = xmlNodeGetContent( valuenode );
       sscanf(key,"%ld",&value) ;
       return value ;
   }
   valuenode = node_search( node->children, "pValue" ) ;
   if ( valuenode != NULL ) {
       xmlChar *key; 
       key = xmlNodeGetContent( valuenode );
       return IInteger_GetValue( genicam, key )  ;
   }
   fprintf(stderr, "No Value node found\n") ;
   return 0 ;
}

int64_t getvalue_intreg(genicam_t* genicam, xmlNode* node) {
   uint32_t Address ;
   size_t   Length ;
   int32_t value ; 
   xmlNode* subnode ;

   subnode = node_search( node->children, "Address" ) ;
   if ( subnode != NULL ) {
       xmlChar *key; 
       key = xmlNodeGetContent( subnode );
       sscanf(key,"%x",&Address) ;
   } else {
        fprintf(stderr, "Address was not found \n") ;
        return 0 ;
   }

   subnode = node_search( node->children, "Length" ) ;
   if ( subnode != NULL ) {
       xmlChar *key; 
       key = xmlNodeGetContent( subnode );
       sscanf(key,"%d",(int*)&Length) ;
   } else {
        fprintf(stderr, "Length was not found \n") ;
        return 0 ;
   }
    
   if ( gvcp_readreg( genicam->cam->gvcp_channel, 1, &Address, &value ) ) {
        fprintf(stderr, "Could not read register \n") ;
        return 0 ;
   }

   return value ;
}

