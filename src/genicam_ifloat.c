#include "genicam_xml.h"
#include "genicam_formula.h"

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
   xmlChar *formula; 
   xmlChar *from;
   double unscaled_value = 0 ;
   double scaled_value = 0.0 ;

   // Recherche de la pValue 

   valuenode = node_search( node->children, "pValue" ) ;
   if ( valuenode == NULL ) {
        fprintf(stderr, "[Converter] No pValue found\n") ;
        return 0.0 ;
   }
   
   key = xmlNodeGetContent( valuenode );
   unscaled_value = (double) IInteger_GetValue( genicam, key )  ;
 
   // Recherche du FormulaFrom

   valuenode = node_search( node->children, "FormulaFrom" ) ;
   if ( valuenode == NULL ) {
        fprintf(stderr, "[Converter] No FormulaFrom found\n") ;
        return 0.0 ;
   }
   
   formula = xmlNodeGetContent( valuenode );
 
   fprintf(stderr," Value--> %lf \n", unscaled_value ) ;
   fprintf(stderr," Formula--> %s \n", formula ) ;
   
   // Application de la formule
    from=malloc(5);
    sprintf(from,"TO") ;
    scaled_value = evaluate_formula(formula, 1, &from, &unscaled_value) ;
    free(from);

   return scaled_value ;
}
