#include "genicam_xml.h"

double getvalue_float(genicam_t*, xmlNode*) ;
double getvalue_converter(genicam_t*, xmlNode*) ;

double IFloat_GetValue( genicam_t* genicam, const char* name ) {
    xmlNode* node ;
    node = genicam_search( genicam, name ) ;
    if ( node == NULL ) {
        fprintf(stderr, "Could not find genicam node \"%s\"\n", name ) ;
        return 0 ;
    }
    if (!xmlStrcmp(node->name,"Float")) {
        return getvalue_float( genicam, node ) ;
    } 
    if (!xmlStrcmp(node->name,"Converter")) {
        return getvalue_converter( genicam, node ) ;
    } 

    fprintf(stderr, "Unknown IFloat type: %s (%s)\n", name, node->name ) ;
    return 0 ;
}
double getvalue_float(genicam_t* genicam, xmlNode* node) {
   xmlNode* valuenode ;
   valuenode = node_search( node->children, "Value" ) ;
   if ( valuenode != NULL ) {
       double value ;
       xmlChar *key; 
       key = xmlNodeGetContent( valuenode );
       sscanf(key,"%lf",&value) ;
       return value ;
   }
   valuenode = node_search( node->children, "pValue" ) ;
   if ( valuenode != NULL ) {
       xmlChar *key; 
       key = xmlNodeGetContent( valuenode );
       return IFloat_GetValue( genicam, key )  ;
   }
   fprintf(stderr, "No Value node found\n") ;
   return 0 ;
}

double getvalue_converter(genicam_t* genicam, xmlNode* node) {
   xmlNode* valuenode ;
   xmlChar *key; 
   uint64_t unscaled_value = 0 ;
   float scaled_value = 0.0 ;

   valuenode = node_search( node->children, "pValue" ) ;
   if ( valuenode == NULL ) {
        fprintf(stderr, "[Converter] No pValue found\n") ;
        return 0.0 ;
   }
   
   key = xmlNodeGetContent( valuenode );
   unscaled_value = IInteger_GetValue( genicam, key )  ;
 
   fprintf(stderr," --> %ld \n", unscaled_value ) ;

   // TODO
   // reste a parser le FormulaFrom

   return scaled_value ;
}
