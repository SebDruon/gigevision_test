#include <errno.h>
#include <math.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gvcp_thread.h"
#include "gigevision.h"
#include "gigevision_cmd.h"
#include "gigevision_status.h"

static void* heartbeat_fct( void* arg ) ;

gvcp_thread_t* gvcp_create( const char* cam_ip, uint16_t port) {
    int res ;
    gvcp_thread_t* tmp ;
    tmp = malloc( sizeof(gvcp_thread_t) ) ;
    if ( tmp == NULL ) {
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return NULL ;
    }

    // Socket creation

    tmp->sockfd = socket( AF_INET, SOCK_DGRAM, 0) ; 
    tmp->cam_addr.sin_family = AF_INET; 
    tmp->cam_addr.sin_port = htons(port); 
    
    res=inet_pton(AF_INET, cam_ip, &(tmp->cam_addr.sin_addr));
    if(res==-1) { 
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return NULL ; 
    } 
    res=connect(tmp->sockfd, (struct sockaddr *)&(tmp->cam_addr), sizeof(tmp->cam_addr));
    if(res==-1) {
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return NULL ;
    } 

    // Mutex creation
    
    pthread_mutex_init(&(tmp->lock), NULL) ;
    
    // Init var

    tmp->request_id = 1 ;
    tmp->timeout = 200 ;
    tmp->retries = 3 ;
    tmp->heartbeat_period = 200e6 ;
    tmp->heartbeat_looptime = 20e3 ;
    tmp->heartbeat_done = 0 ;
    res=clock_gettime( CLOCK_REALTIME, &(tmp->timestamp) ) ;
    if(res==-1) {
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return NULL ;
    } 

    // Init and start heartbeat thread
    res = pthread_create ( &(tmp->heartbeat), NULL,
                          heartbeat_fct, (void *) tmp ) ;
    if ( res != 0 ){
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return NULL ; 
    }
    
    return tmp ;
}

void print_current_time_with_ms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }
    printf("[timestamp] %ld.%03ld\n", s,ms);
}

static void* heartbeat_fct( void* arg ) {
    gvcp_thread_t* gvcp ; 
    struct timespec now ;
    long elapsed_time ;
    int res ;

    gvcp = (gvcp_thread_t*) arg ;
    while( !gvcp->heartbeat_done ) {
        usleep(gvcp->heartbeat_looptime) ;
        clock_gettime( CLOCK_REALTIME, &now ) ;
        pthread_mutex_lock( &(gvcp->lock) );
        elapsed_time = now.tv_nsec - gvcp->timestamp.tv_nsec ;
        if( elapsed_time < 0 )
            elapsed_time += 1e9 ;
        if ( elapsed_time > gvcp->heartbeat_period ) {
            res = send_heartbeat( gvcp->sockfd, 0, gvcp->timeout, gvcp->request_id) ;
            if (res == 0 )
                clock_gettime( CLOCK_REALTIME, &(gvcp->timestamp));
            gvcp->request_id++ ;
            if ( gvcp->request_id == 0 )
                gvcp->request_id = 1 ;
        }
        pthread_mutex_unlock( &(gvcp->lock) );
    }
    return NULL ;
}

int gvcp_readreg( gvcp_thread_t* gvcp, int nb, uint32_t* registers, uint32_t* values ) {
    int res ;
    int i ;
    uint16_t status ;
    uint16_t ack ;
    uint16_t ack_length ;
    uint16_t ack_id ;
    uint32_t* data ;
    size_t data_size ;
    data_size = nb*sizeof(uint32_t);
    ack_length = nb*sizeof(uint32_t) ;
    data = malloc( data_size ) ;
    if ( data == NULL ) {
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return -1 ;
    }
    for (i=0;i<nb;i++)
        data[i]=htonl(registers[i]);
    pthread_mutex_lock( &(gvcp->lock) );
    res = gvcp_transaction( gvcp->sockfd , gvcp->retries, gvcp->timeout,
                            0x01, READREG_CMD, data_size, gvcp->request_id, data, 
                            &status, &ack, &ack_length, &ack_id, values ) ;
    if (res == 0 )
        clock_gettime( CLOCK_REALTIME, &(gvcp->timestamp));
        gvcp->request_id++ ;
    if ( gvcp->request_id == 0 )
        gvcp->request_id = 1 ;
   pthread_mutex_unlock( &(gvcp->lock) );
   if ( res != 0 ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_readreg(): No reply.\n") ;
        return -1 ;
   }
   if ( status != GEV_STATUS_SUCCESS ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_readreg(): Ack received with status %d \n", status ) ;
        return -1 ;
   }
   if ( ack != READREG_ACK ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_readreg(): Received ack has wrong type. Received %d \n", ack ) ;
        return -1 ;
   }
   for (i=0;i<nb;i++)
        values[i]=ntohl(values[i]);
   free(data);
   return 0 ;
}

int gvcp_writereg( gvcp_thread_t* gvcp, int nb, uint32_t* registers, uint32_t* values, uint16_t* result ) {
    int res ;
    int i ;
    uint16_t status ;
    uint16_t ack ;
    uint16_t ack_id ;
    uint16_t ack_length ;
    uint16_t ack_data[2] ;
    uint32_t* data ;
    size_t data_size ;
    data_size = 2*nb*sizeof(uint32_t) ;
    data = malloc( data_size ) ;
    if ( data == NULL ) {
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return -1 ;
    }
    for (i=0;i<nb;i++){
        data[2*i]=htonl(registers[i]);
        data[2*i+1]=htonl(values[i]);
    }
    ack_length = sizeof(ack_data);
    pthread_mutex_lock( &(gvcp->lock) );
    res = gvcp_transaction( gvcp->sockfd , gvcp->retries, gvcp->timeout,
                            0x01, WRITEREG_CMD, data_size, gvcp->request_id, data, 
                            &status, &ack, &ack_length, &ack_id, &ack_data ) ;
    if (res == 0 )
        clock_gettime( CLOCK_REALTIME, &(gvcp->timestamp));
        gvcp->request_id++ ;
    if ( gvcp->request_id == 0 )
        gvcp->request_id = 1 ;
   pthread_mutex_unlock( &(gvcp->lock) );
   if ( res != 0 ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_writereg(): No reply.\n") ;
        return -1 ;
   }
   if ( status != GEV_STATUS_SUCCESS ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_writereg(): Ack received with status %d \n", status ) ;
        return -1 ;
   }
   if ( ack != WRITEREG_ACK ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_writereg(): Received ack has wrong type. Received %d \n", ack ) ;
        return -1 ;
   }
   *result=ntohs(ack_data[1]);
   free(data) ;
   return 0 ;
}

int gvcp_readmem( gvcp_thread_t* gvcp, uint32_t address, uint16_t bytes, uint8_t* data ) {
    int res ;
    int i ;
    uint16_t status ;
    uint16_t ack ;
    uint16_t ack_id ;
    uint16_t data_cmd[4] ;
    uint8_t* data_ack ;
    uint16_t data_ack_size ;
    ((uint32_t*)data_cmd)[0]=htonl(address) ;
    data_cmd[2] = 0 ;
    data_cmd[3] = htons(bytes);
    data_ack_size = 4 + bytes ;
    data_ack = malloc( data_ack_size ) ;
    if ( data_ack == NULL ) {
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return -1 ;
    }
    pthread_mutex_lock( &(gvcp->lock) );
    res = gvcp_transaction( gvcp->sockfd , gvcp->retries, gvcp->timeout,
                            0x01, READMEM_CMD, 4*sizeof(uint16_t), gvcp->request_id, data_cmd, 
                            &status, &ack, &data_ack_size, &ack_id, data_ack ) ;
    if (res == 0 )
        clock_gettime( CLOCK_REALTIME, &(gvcp->timestamp));
        gvcp->request_id++ ;
    if ( gvcp->request_id == 0 )
        gvcp->request_id = 1 ;
   pthread_mutex_unlock( &(gvcp->lock) );
   if ( res != 0 ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_readmem(): No reply.\n") ;
        return -1 ;
   }
   if ( status != GEV_STATUS_SUCCESS ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_readmem(): Ack received with status %d \n", status ) ;
        return -1 ;
   }
   if ( ack != READMEM_ACK ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_readmem(): Received ack has wrong type. Received %d \n", ack ) ;
        return -1 ;
   }
   memcpy(data, data_ack+4, bytes ) ;
   free(data_ack) ;
   return 0 ;
}

int gvcp_writemem( gvcp_thread_t* gvcp, uint32_t address, uint16_t bytes, uint8_t* data, uint16_t* result ) {
    int res ; 
    uint16_t status ;
    uint16_t ack ;
    uint16_t ack_id ;
    uint8_t* data_cmd ;
    uint16_t data_cmd_size ;
    uint16_t data_ack[2] ;
    uint16_t data_ack_size ;
    data_cmd_size = bytes + 4 ;
    data_cmd = malloc( data_cmd_size ) ;
    if ( data_cmd == NULL ) {
        fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
        fprintf(stderr, "%s\n",strerror(errno)) ;
        return -1 ;
    }
    ((uint32_t*)data_cmd)[0]=htonl(address) ;
    memcpy(data_cmd+4, data, bytes ) ;
    data_ack_size = 4 ;

    pthread_mutex_lock( &(gvcp->lock) );
    res = gvcp_transaction( gvcp->sockfd , gvcp->retries, gvcp->timeout,
                            0x01, WRITEMEM_CMD, data_cmd_size, gvcp->request_id, data_cmd, 
                            &status, &ack, &data_ack_size, &ack_id, data_ack ) ;
    if (res == 0 )
        clock_gettime( CLOCK_REALTIME, &(gvcp->timestamp));
        gvcp->request_id++ ;
    if ( gvcp->request_id == 0 )
        gvcp->request_id = 1 ;
   pthread_mutex_unlock( &(gvcp->lock) );
   if ( res != 0 ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_writemem(): No reply.\n") ;
        return -1 ;
   }
   if ( status != GEV_STATUS_SUCCESS ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_writemem(): Ack received with status %d \n", status ) ;
        return -1 ;
   }
   if ( ack != WRITEMEM_ACK ) {
        fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
        fprintf(stderr, "gvcp_writemem(): Received ack has wrong type. Received %d \n", ack ) ;
        return -1 ;
   }
   free(data_cmd) ;
   *result = ntohs(data_ack[1]) ;
   return 0 ;
}
