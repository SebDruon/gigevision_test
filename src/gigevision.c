#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>

#include "gigevision.h"
#include "gigevision_headers.h"
#include "gigevision_cmd.h"

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
        if ( ret == -1 ) {
            fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
            fprintf(stderr, "%s\n",strerror(errno)) ;
            return -1 ;
        }
        if ( ret != sizeof(gvcp_header_t)+length ) {
            fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
            fprintf(stderr, "send() return value is less than msg size\n") ;
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
            return -2 ;
        }
        if (res == -1) {                    
            fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
            fprintf(stderr, "%s\n",strerror(errno)) ;
            return -1 ;
        }  
       
        ret = recv( socket_fd, packet_ack, sizeof(gvcp_header_ack_t) + *length, 0 ) ;
        if ( ret == -1 ) {
            fprintf(stderr, "Error %d in %s:%d\n", errno, __FILE__, __LINE__ ) ;
            fprintf(stderr, "%s\n",strerror(errno)) ;
            return -1 ;
        }
        if ( ret != sizeof(gvcp_header_ack_t)+*length ) {
            fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
            fprintf(stderr, "recv() return value is less than msg size\n") ;
            return -1 ;
        }
            
        header_ack = ( gvcp_header_ack_t* ) packet_ack ;
        
        *status = ntohs( header_ack->status) ;
        *ack = ntohs( header_ack->acknowledge) ;
        *length = ntohs( header_ack->length ) ;
        *ack_id = ntohs( header_ack->ack_id ) ;

        if ( length != 0 ) 
            memcpy( data, packet_ack + sizeof(gvcp_header_ack_t), *length ) ;

        return 0 ;
}


int gvcp_transaction( int socket_fd , int retries, int timeout,
                      uint8_t flags, uint16_t cmd, uint16_t cmd_length, uint16_t req_id, void* cmd_data, 
                      uint16_t* status, uint16_t* ack, uint16_t* ack_length, uint16_t* ack_id, void* ack_data ) {

        int res ;
        int return_value = -1;
        int pending_ack = 0 ;
        int counter = 0;
        int done = 0 ;
       
        do {
            
            // envoi du paquet
            
            if ( gvcp_send_cmd( socket_fd, flags, cmd, cmd_length, req_id, cmd_data ) )  {
                fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
                fprintf(stderr, "gvcp_transaction() : Could not send cmd\n") ;
                return -1 ;
            }
            counter++ ;

            // Reception de l'ACK

            if ( !(flags & 0x01) ) { 
                return_value = 1 ;
                done = 1 ;
            } else {
                    do {

                        res = gvcp_rcv_ack( socket_fd, timeout, status, ack, ack_length, ack_id, ack_data ) ;
                
                        if( res == 0 ) {
                            if (*ack_id == req_id ) {
                                if ( *ack == PENDING_ACK ) {          // Recu un PENDING_ACK
                                    pending_ack = 1 ;
                                    timeout = 200 ;    //FIXME
                                } else {                              // Recu un ack valide
                                    pending_ack = 0 ;
                                    return_value = 0 ;
                                    done = 1 ;
                                }
                           } else {                                   // Recu un ack non valide
                                pending_ack = 0 ;
                                done = 0 ;
                           }
                       } else if ( res == -2 ) {                      // Timeout sans reception
                                pending_ack = 0 ;
                                done = 0 ;
                       } else {                                       
                                fprintf(stderr, "Error in %s:%d\n", __FILE__, __LINE__ ) ;
                                fprintf(stderr, "gvcp_transaction() : Could not receive ack\n") ;
                                return -1 ;
                       }
                 
                    } while ( pending_ack ) ;
            } 
                
        } while ( (counter <= retries) && !done) ;   

        return return_value ; 
}


int send_heartbeat(int sockfd, int retries, int timeout, int req_id) {

uint32_t ccp_register = htonl(0x0A00) ;
uint16_t status ;
uint16_t ack ;
uint16_t ack_length = 4 ;
uint16_t ack_id ;
uint32_t ack_data ;
int res ;

res = gvcp_transaction( sockfd, retries, timeout,
                     0x01, READREG_CMD, sizeof(ccp_register), req_id, &ccp_register, 
                     &status, &ack, &ack_length, &ack_id, &ack_data ) ;

if ( res != 0 )
    return -1 ;

if ( ack != READREG_ACK )
    return -2 ;

return 0 ;

}
