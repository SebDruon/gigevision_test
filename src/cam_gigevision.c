#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <zip.h>

#include "cam_gigevision.h"
#include "genicam_xml.h"

int download_xml_from_mem( cam_gigevision_t* camera, char* url ) ;


int init( void** cam_handle, va_list args ){
    cam_gigevision_t* camera ;
    char* cam_ip ;
    uint16_t port ;
 	char* config_file ;
 	char* config_prefix ;
   
	cam_ip = va_arg(args, char*) ;
    port = (uint16_t) va_arg(args, int) ;
	config_file = va_arg(args, char*) ;
	config_prefix = va_arg(args, char*) ;
  
    camera = (cam_gigevision_t*) malloc(sizeof(cam_gigevision_t)) ;
	if (camera==NULL) {
            fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
            fprintf(stderr, "%s\n",strerror(errno)) ;
            return -1 ;
	}

	*cam_handle = (void*) camera ;

    camera->gvcp_channel = NULL ;
    camera->gvsp_channel = NULL ;
    camera->msg_channel  = NULL ;

    // Create control channel and start heartbeat

    camera->gvcp_channel = gvcp_create( cam_ip, port) ; 
	if (camera->gvcp_channel==NULL) {
            fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
            fprintf(stderr, "Could not create control channel\n") ;
            return -1 ;
	}
    
    // Get exclusive access rights

    uint32_t reg = 0x0A00 ;
    uint32_t value ;
    uint16_t result ;
    value = 0x0002;
    if ( gvcp_writereg( camera->gvcp_channel, 1, &reg, &value, &result ) ) {
            fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
            fprintf(stderr, "Could not create control channel\n") ;
            return -1 ;
    }
    
    // Apply config if given
/*
    if ( config_file != NULL ) {
        
        if( load_config( camera, config_file, config_prefix )) {
            fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
            fprintf(stderr, "Could not apply config\n") ;
            fprintf(stderr, "Config file: %s\n", config_file ) ;
            if ( config_prefix == NULL )
                fprintf(stderr, "Config prefix: NULL\n" ) ;
            else
                fprintf(stderr, "Config prefix: %s\n", config_prefix ) ;
            return -1 ;
        }
    }
*/
	return ERR_OK ;
}



int deinit( void* cam_handle){
    // TODO
    return 0 ;
}

int get_available_modes( void* cam_handle, hw_mode_t** modes, int* nb_modes ) {

    cam_gigevision_t* camera ;
    uint32_t reg ;
    uint32_t value ;
    uint16_t result ;
    uint8_t  url[512] ;
    unsigned char method[8] ;
    int i ;

    camera = (cam_gigevision_t*) cam_handle ;

    reg = 0x0200 ;
    if ( gvcp_readmem( camera->gvcp_channel, reg, 512, url ) ) {
            fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
            fprintf(stderr, "Could not retrieve config URL\n") ;
            return -1 ;
    }
    
    for (i=0;i<8;i++) {
        if (url[i]==':')
            method[i]='\0' ;
        else
            method[i] = toupper( (unsigned char) url[i] ) ;
    }
    
    if ((strcmp(method,"HTTP")==0)||(strcmp(method,"HTTPS")==0)) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "Retrieving xml file over http is not yet implemented\n") ;
        return -1 ; // TODO
    } else if (strcmp(method,"FILE")==0) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "xml local file over is not yet implemented\n") ;
        return -1 ; // TODO
    } else if (strcmp(method,"LOCAL")==0) {
       download_xml_from_mem( camera, url ) ; 
    } else {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "Could not retrieve parse configuration URL\n") ;
        return -1 ; 
    }
    
    // FIXME
    genicam_t* genicam ;
    int64_t width ; 
    int64_t height ; 
    int64_t pixelformat ; 
    double  framerate ;

    genicam = genicam_new( camera, "/home/seb/.cyan/cam_gigevision/config_xml/IMX290_M_AB631911A79AC8AC3F0960EA5C1332F5.xml" ) ;
    width  = IInteger_GetValue( genicam, "Width") ;
    height = IInteger_GetValue( genicam, "Height") ;
    pixelformat = IEnumeration_GetIntValue( genicam, "PixelFormat") ;
    framerate = IFloat_GetValue( genicam, "AcquisitionFrameRate" ) ;

    printf("Width : %ld \n",width) ; 
    printf("Height: %ld \n",height) ; 
    printf("PixelFormat: %lx \n",pixelformat) ; 
    printf("Framerate: %lf \n",framerate) ; 

    genicam_del( genicam ) ;

    // TODO
    return 0 ;
}

int get_serial( void* cam_handle, char** serial, size_t* serial_size )  {
    // TODO
    return 0 ;
}

int set_mode( void* cam_handle, int mode )  {
    // TODO
    return 0 ;
}

int get_mode( void* cam_handle, int* mode ) {
    // TODO
    return 0 ;
}

int start_acqui ( void* cam_handle ) {
    // TODO
    return 0 ;
}

int stop_acqui ( void* cam_handle ) {
    // TODO
    return 0 ;
}

int get_frame ( void* cam_handle, image_t* img ) {
    // TODO
    return 0 ;
}


