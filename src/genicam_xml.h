#ifndef GENICAM_XML_H
#define GENICAM_XML_H

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "cam_gigevision.h"

typedef struct{
   cam_gigevision_t* cam ;
   xmlDocPtr doc;
   xmlNode* root;
} genicam_t ;

genicam_t* genicam_new( cam_gigevision_t* cam, char* filename ) ;
void genicam_del( genicam_t* ) ;

xmlNode* genicam_search( genicam_t*, const char* Name ) ;
xmlNode* node_search   ( xmlNode* node, const char* Name ) ;

// IInteger Interface

int64_t IInteger_GetValue( genicam_t* genicam, const char* name ) ;
int64_t IInteger_GetInc( genicam_t* genicam, const char* name ) ;
int64_t IInteger_GetMax( genicam_t* genicam, const char* name ) ;
int64_t IInteger_GetMin( genicam_t* genicam, const char* name ) ;

// IEnumeration interface

int64_t IEnumeration_GetIntValue( genicam_t*, const char* name ) ;

// IFloat Interface

double IFloat_GetValue( genicam_t*, const char* name ) ;

#endif
