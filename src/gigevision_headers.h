#ifndef GIGEVISION_HEADERS_H
#define GIGEVISION_HEADERS_H

typedef struct {
  uint8_t magic ;
  uint8_t flag  ;
  uint16_t command ;
  uint16_t length ;
  uint16_t req_id ;
} gvcp_header_t ;

typedef struct {
    uint16_t status ;
    uint16_t acknowledge ;
    uint16_t length ;
    uint16_t ack_id ;
} gvcp_header_ack_t ;

#endif
