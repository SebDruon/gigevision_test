#include <errno.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zip.h>

#include "cam_gigevision.h"
#include "xml_file.h"

int save_xml_file( cam_gigevision_t* camera, unsigned char* filename, unsigned char* file, size_t length, const char* extension ) {

struct passwd pw ; 
struct passwd* result ; 
char* buf ;
long buflen ;
char filepath[255] ;
struct stat st = {0};

buflen = sysconf(_SC_GETPW_R_SIZE_MAX) ;
if ( buflen == -1 ) {
    fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
    fprintf(stderr, "Could not retrieve buflen\n") ;
    return -1 ;
}
buf = malloc( buflen ) ;
if ( buf == NULL ) {
    fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
    fprintf(stderr, "%s\n",strerror(errno)) ;
    return -1 ;
}
getpwuid_r(getuid(), &pw, buf, buflen, &result );
if ( result == NULL ) {
    fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
    fprintf(stderr, "Could not find home directory\n") ;
    return -1 ;
}

sprintf(filepath,"%s/.cyan",pw.pw_dir ) ;
if (stat( filepath, &st) == -1) {
        mkdir(filepath, 0755);
}

sprintf(filepath,"%s/.cyan/cam_gigevision",pw.pw_dir ) ;
if (stat( filepath, &st) == -1) {
        mkdir(filepath, 0755);
}

sprintf(filepath,"%s/.cyan/cam_gigevision/config_xml",pw.pw_dir ) ;
if (stat( filepath, &st) == -1) {
        mkdir(filepath, 0755);
}

sprintf(filepath,"%s/.cyan/cam_gigevision/config_xml/%s.%s",pw.pw_dir, filename, extension ) ;
if (stat( filepath, &st) == -1) {
    FILE* fd = fopen(filepath,"w+") ;
    fwrite(file, 1, length, fd ) ;
    fclose(fd) ;
}

free(buf) ;
return 0 ;

}

int extract_xml_file( cam_gigevision_t* camera, unsigned char* filename, unsigned char* file, size_t length ) {

    zip_error_t error;
    zip_source_t *src;
    zip_t *za;
    zip_file_t * zipfile ;
    struct zip_stat zipstat ;
    int size ;

    zip_error_init(&error);

    if ((src = zip_source_buffer_create(file, length, 1, &error)) == NULL) {
        fprintf(stderr, "can't create source: %s\n", zip_error_strerror(&error));
        zip_error_fini(&error);
        return 1;
    }

    if ((za = zip_open_from_source(src, 0, &error)) == NULL) {
        fprintf(stderr, "can't open zip from source: %s\n", zip_error_strerror(&error));
        zip_source_free(src);
        zip_error_fini(&error);
        return 1;
    }

    zip_stat_index( za, 0, 0, &zipstat);
    zipfile = zip_fopen_index( za, 0, 0);
    unsigned char zipbuf[zipstat.size] ;
    size =zip_fread( zipfile, zipbuf, zipstat.size);
    zip_close(za) ;

    save_xml_file( camera, filename, zipbuf, zipstat.size, "xml" );
        
return 0 ;

}


