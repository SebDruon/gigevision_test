#ifndef GIGEVISION_H
#define GIGEVISION_H

#include <stdint.h>

// GVCP ( camera control protocol )


int gvcp_send_cmd( int socket_fd, 
                   uint8_t flags, uint16_t command, uint16_t length, uint16_t req_id, void* data ) ;


int gvcp_rcv_ack( int socket_fd, int timeout,
                    uint16_t* status, uint16_t* ack, uint16_t* length, uint16_t* ack_id, void* data ) ;

int gvcp_transaction( int socket_fd , int retries, int timeout,
                      uint8_t flags, uint16_t cmd, uint16_t cmd_length, uint16_t req_id, void* cmd_data, 
                      uint16_t* status, uint16_t* ack, uint16_t* ack_length, uint16_t* ack_id, void* ack_data ) ;

int send_heartbeat(int sockfd, int retries, int timeout, int req_id ) ;

// GVSP ( camera streaming protocol )


#endif
