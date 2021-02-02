#ifndef GVCP_H
#define GVCP_H

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

#define DISCOVERY_CMD 0x0002
#define DISCOVERY_ACK 0x0003
#define FORCEIP_CMD   0x0004
#define FORCEIP_ACK   0x0005
#define READREG_CMD   0x0080
#define READREG_ACK   0x0081
#define WRITEREG_CMD  0x0082
#define WRITEREG_ACK  0x0083
#define READMEM_CMD   0x0084
#define READMEM_ACK   0x0085
#define WRITEMEM_CMD  0x0086
#define WRITEMEM_ACK  0x0087
#define PENDING_ACK   0x0089
#define EVENT_CMD     0x00C0
#define EVENT_ACK     0x00C1
#define EVENTDATA_CMD 0x00C2
#define EVENTDATA_ACK 0x00C3
#define ACTION_CMD    0x0100
#define ACTION_ACK    0x0101

#endif