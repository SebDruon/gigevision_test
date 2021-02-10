#ifndef CAM_GIGEVISION_H
#define CAM_GIGEVISION_H

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

    char*   config_file ;
    char*   config_prefix ;
    char*   cam_serial ;

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


#endif
