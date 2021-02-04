#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <cyan/hwcam/plugin.h>
#include <cyan/hwcam/modes.h>
#include <cyan/common/error.h>

#include "gvcp_thread.h"
#include "gvsp_thread.h"
#include "msg_thread.h"

typedef struct {

	gvcp_thread_t*	gvcp_channel ;
	gvsp_thread_t*	gvsp_channel ;
	msg_thread_t*	msg_channel ;
	
} cam_gigevision_t ;

int init( void** cam_handle, va_list args ) ;
int deinit( void* cam_handle) ;
int get_available_modes( void* cam_handle, hw_mode_t** modes, int* nb_modes ) ;
int get_serial( void* cam_handle, char** serial, size_t* serial_size ) ;
int set_mode( void* cam_handle, int mode ) ;
int get_mode( void* cam_handle, int* mode ) ;
int start_acqui ( void* cam_handle ) ;
int stop_acqui ( void* cam_handle ) ;
int get_frame ( void* cam_handle, image_t* img ) ; 

int load_config( cam_gigevision_t* camera, char* config_file, char* config_prefix ) ;

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
  
	
    printf("yop\n") ;
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

	return ERR_OK ;
}



int deinit( void* cam_handle){
    // TODO
    return 0 ;
}

int get_available_modes( void* cam_handle, hw_mode_t** modes, int* nb_modes ) {
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

int load_config( cam_gigevision_t* camera, char* config_file, char* config_prefix ) {
    // TODO
    return 0 ; 
}
