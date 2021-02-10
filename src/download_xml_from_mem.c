#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <cyan/hwcam/plugin.h>
#include <cyan/hwcam/modes.h>
#include <cyan/common/error.h>

#include "cam_gigevision.h"
#include "xml_file.h"

int download_xml_from_mem( cam_gigevision_t* camera, char* url ){

    unsigned char* c_address ;
    unsigned char* c_length ;
    unsigned char* filename ;
    unsigned char* filetype ;
    unsigned char* method ;
    int address ;
    int length ;
    unsigned char* file ;
    char* save_ptr ;
    int i ;
    int blocksize ;

    printf("url: %s\n", url ) ;

    method = strtok_r(url,":", &save_ptr);
    filename = strtok_r(NULL,".", &save_ptr);
    filetype = strtok_r(NULL,";", &save_ptr);
    c_address = strtok_r(NULL,";", &save_ptr);
    c_length = strtok_r(NULL,";", &save_ptr);

    length  = (int)strtol(c_length, NULL, 16);
    address = (int)strtol(c_address, NULL, 16);

    file = malloc( length ) ;
    if ( file == NULL ) {
            fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
            fprintf(stderr, "%s\n",strerror(errno)) ;
            return -1 ;
    }
    
    i=0 ;
    while ( i<length ) {
        blocksize = (100>length-i)?length-i:100 ;
        gvcp_readmem( camera->gvcp_channel, address+i, blocksize, file+i ) ;
        i+=blocksize ; 
    }

    if ((strcmp(filetype,"ZIP")==0)||
        (strcmp(filetype,"zip")==0)){
        save_xml_file( camera, filename, file, length,"zip" ) ;
        extract_xml_file( camera, filename, file, length ) ;
    } else {
//        save_xml_file(camera,filename,file,length) ;
    }

    free( file ) ;
    return 0 ;
}
