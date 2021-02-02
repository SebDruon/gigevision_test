#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <poll.h>

#include <cyan/common/error.h>

#include "gvcp_thread.h"
#include "gvcp.h"

static void* heartbeat_fct( void* arg ) ;

int gvcp_transaction( int socket_fd , int retries, int timeout,
                      uint8_t flags, uint16_t cmd, uint16_t cmd_length, uint16_t req_id, void* cmd_data, 
                      uint16_t* status, uint16_t* ack, uint16_t* ack_length, uint16_t* ack_id, void* ack_data ) ;

gvcp_thread_t* gvcp_create( const char* cam_ip, uint16_t port) {
    
    gvcp_thread_t* tmp = malloc( sizeof(gvcp_thread_t) ) ;
    if ( tmp == NULL ) {
        CYAN_ERROR( ERR_MALLOC ) ;
        return NULL ;
    }

    // Socket creation

    tmp->sockfd = socket( AF_INET, SOCK_DGRAM, 0) ; 
    tmp->cam_addr.sin_family = AF_INET; 
    tmp->cam_addr.sin_port = htons(port); 
    if(inet_pton(AF_INET, cam_ip, &(tmp->cam_addr.sin_addr))<=0) { 
        CYAN_ERROR( ERR_INVALID_ARG ) ; 
        return NULL ; 
    } 
    if(connect(tmp->sockfd, (struct sockaddr *)&(tmp->cam_addr), sizeof(tmp->cam_addr)) < 0) {
        CYAN_ERROR( ERR_NOPE ) ; 
        return NULL ;
    } 

    // Mutex creation
    
    if(pthread_mutex_init(&(tmp->lock), NULL) != 0) { 
        CYAN_ERROR( ERR_NOPE ) ; 
        return NULL ; 
    } 
    // Init var

    tmp->request_id = 1 ;
    tmp->timeout = 200 ;
    tmp->retries = 3 ;
    tmp->heartbeat_period = 200e6 ;
    tmp->heartbeat_looptime = 20e3 ;
    tmp->heartbeat_done = 0 ;
    clock_gettime( CLOCK_REALTIME, &(tmp->timestamp) ) ;

    // Init and start heartbeat thread
    if ( pthread_create ( &(tmp->heartbeat), NULL,
                          heartbeat_fct, (void *) tmp ) == 0 ) {
        CYAN_ERROR( ERR_NOPE ) ; 
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
    uint32_t ccp_register = htonl(0x0A00) ;

    gvcp = (gvcp_thread_t*) arg ;
    while( !gvcp->heartbeat_done ) {
        usleep(gvcp->heartbeat_looptime) ;
        clock_gettime( CLOCK_REALTIME, &now ) ;
        pthread_mutex_lock( &(gvcp->lock) );
        elapsed_time = now.tv_nsec - gvcp->timestamp.tv_nsec ;
        if( elapsed_time < 0 )
            elapsed_time += 1e9 ;
        if ( elapsed_time > gvcp->heartbeat_period ) {
            
            // Send READREG
              
             
             uint16_t status ;
             uint16_t ack ;
             uint16_t ack_length = 4 ;
             uint16_t ack_id ;
             unsigned char ack_data[255] ;

             gvcp_transaction( gvcp->sockfd, gvcp->retries, gvcp->timeout,
                                 0x01, READREG_CMD, sizeof(ccp_register), gvcp->request_id, &ccp_register, 
                                 &status, &ack, &ack_length, &ack_id, ack_data ) ;

            clock_gettime( CLOCK_REALTIME, &(gvcp->timestamp));
            gvcp->request_id++ ;
            if ( gvcp->request_id == 0 )
                gvcp->request_id = 1 ;
        }
        pthread_mutex_unlock( &(gvcp->lock) );
    }
    return NULL ;
}



int gvcp_send_cmd( int socket_fd, 
                    uint8_t flags, uint16_t command, uint16_t length, uint16_t req_id, void* data ) {
        unsigned char packet[548] ;
        gvcp_header_t* header ;
        ssize_t ret ;
        header = (gvcp_header_t*) packet ;
        header->magic = 0x42 ;
        header->flag  = flags ;
        header->command = htons(command) ;
        header->length = htons(length) ;
        header->req_id = htons(req_id) ;
        if ( length != 0 ) {
            memcpy( packet+sizeof(gvcp_header_t), data, length ) ;
        }
        ret = send( socket_fd, packet, sizeof(gvcp_header_t)+length, 0 ) ;
        if ( ret != sizeof(gvcp_header_t)+length ) {
                CYAN_ERROR( ERR_NOPE ) ; 
                return -1 ;  
        }
        return 0 ;
}

int gvcp_rcv_ack( int socket_fd, int timeout,
                    uint16_t* status, uint16_t* ack, uint16_t* length, uint16_t* ack_id, void* data ) {

        unsigned char packet_ack[548] ;
        gvcp_header_ack_t* header_ack ;
        struct pollfd fd;
        fd.fd = socket_fd;
        fd.events = POLLIN;
        int res ;
        ssize_t ret ;
        
        res = poll(&fd, 1, timeout);   
        if (res == 0) {                     // timeout reached
            CYAN_ERROR( ERR_NOPE ) ;
            return -1 ;
        }
        if (res == -1) {                    // Error in poll
            CYAN_ERROR( ERR_NOPE ) ;
            return -2 ;
        }  
       
        ret = recv( socket_fd, packet_ack, sizeof(gvcp_header_ack_t) + *length, 0 ) ;
        if ( ret != sizeof(gvcp_header_ack_t)+ *length ) {
            CYAN_ERROR( ERR_NOPE ) ;
            return -3 ;
        }
            
        header_ack = ( gvcp_header_ack_t* ) packet_ack ;
        
        *status = ntohs( header_ack->status) ;
        *ack = ntohs( header_ack->acknowledge) ;
        *length = ntohs( header_ack->length ) ;
        *ack_id = ntohs( header_ack->ack_id ) ;

        if ( length != 0 ) 
            memcpy( data, header_ack + sizeof(gvcp_header_ack_t), *length ) ;

        return 0 ;
}


int gvcp_transaction( int socket_fd , int retries, int timeout,
                      uint8_t flags, uint16_t cmd, uint16_t cmd_length, uint16_t req_id, void* cmd_data, 
                      uint16_t* status, uint16_t* ack, uint16_t* ack_length, uint16_t* ack_id, void* ack_data ) {

        int res ;
        int success = 0;
        int counter = 0;
        int done = 0 ;
        int ack_desired = flags & 0x01 ;
       
        do {
            
            // envoi du paquet
            
            if ( gvcp_send_cmd( socket_fd, flags, cmd, cmd_length, req_id, cmd_data ) )  {
                CYAN_ERROR( ERR_NOPE ) ;
                return -1 ;
            }
            counter++ ;

            // Reception de l'ACK

            if ( !ack_desired ) { 
                success = 1 ;
                done = 1 ;
            } else {
                    
                    int pending_ack ;
                    pending_ack = 1 ;

                    while( pending_ack ) {

                        res = gvcp_rcv_ack( socket_fd, timeout, status, ack, ack_length, ack_id, ack_data ) ;
                
                        if( res == 0 ) {
                            if (*ack_id == req_id ) {
                                if ( *ack == PENDING_ACK ) {           // Recu un PENDING_ACK
                                    pending_ack = 1 ;
                                    timeout = 200 ;    //FIXME
                                } else {                              // Recu un ack valide
                                    pending_ack = 0 ;
                                    success = 1 ;
                                    done = 1 ;
                                }
                           } else {                                   // Recu un ack non valide
                                pending_ack = 0 ;
                                success = 0 ;
                                done = 0 ;
                           }
                       } else if ( res == -1 ) {                      // Timeout sans reception
                                pending_ack = 0 ;
                                success = 0 ;
                                done = 0 ;
                       } else {                                       // Erreur
                                CYAN_ERROR( ERR_NOPE ) ;
                                return -2;
                       }
                 }

            }
                
            if ( counter == retries ) 
                done = 1 ;

        } while (!done) ;   

        return success ; 
}
