#ifndef GVCP_THREAD_H
#define GVCP_THREAD_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h> 
#include <signal.h> 
#include <time.h> 

#include "gev.h"

typedef struct gvcp_thread_s {
    
    // GVCP settings

    int request_id ;    
    int timeout ;           // ACK timeout in ms 
    int retries ;           // send retries before failing    

    // Semaphore Operation en cours   
    pthread_mutex_t lock;  

    // UDP socket
    int sockfd ;
    struct sockaddr_in cam_addr ; 

    // heartbeat thread
    pthread_t       heartbeat ;
    int             heartbeat_done ;
    int             heartbeat_period ;     // in nsec
    useconds_t      heartbeat_looptime ;   // in usec

    // Last message sent
    struct timespec timestamp ;

} gvcp_thread_t ;

gvcp_thread_t* gvcp_create( const char* cam_ip, uint16_t port) ; 
void gvcp_destroy( gvcp_thread_t * ) ;

//gev_status_t gvcp_discovery( gvcp_thread_t* ) ;
//gev_status_t gvcp_forceip( gvcp_thread_t* ) ;
//gev_status_t gvcp_readreg( gvcp_thread_t* ) ;
//gev_status_t gvcp_writereg( gvcp_thread_t* ) ;
//gev_status_t gvcp_readmem( gvcp_thread_t* ) ;
//gev_status_t gvcp_writemem( gvcp_thread_t* ) ;

// PACKETRESEND ?
// PENDING ?
// ACTION ?


#endif
