#ifndef GVCP_THREAD_H
#define GVCP_THREAD_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h> 
#include <time.h> 
#include <unistd.h> 


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

int gvcp_readreg( gvcp_thread_t*, int nb, uint32_t* registers, uint32_t* values ) ;
int gvcp_writereg( gvcp_thread_t* gvcp, int nb, uint32_t* registers, uint32_t* values, uint16_t* result ) ;
int gvcp_readmem( gvcp_thread_t* gvcp, uint32_t address, uint16_t bytes, uint8_t* data_ack ) ;
int gvcp_writemem( gvcp_thread_t* gvcp, uint32_t address, uint16_t bytes, uint8_t* data, uint16_t* result ) ;

// DISCOVERY
// FORCEIP
// 
// PACKETRESEND NOT IMPLEMENTED
// ACTION ?


#endif
