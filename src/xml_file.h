#ifndef XML_FILE_H
#define XML_FILE_H


int save_xml_file( cam_gigevision_t* camera, unsigned char* filename, unsigned char* file, size_t length, const char* extension ) ;
int extract_xml_file( cam_gigevision_t* camera, unsigned char* filename, unsigned char* file, size_t length ) ;
#endif
