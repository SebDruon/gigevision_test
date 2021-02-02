#ifndef GEV_H
#define GEV_H

typedef enum {
     GEV_STATUS_SUCCESS         = 0x0000 ,
     GEV_STATUS_PACKET_RESEND   = 0x0100 ,
     GEV_STATUS_NOT_IMPLEMENTED = 0x8001 ,
     GEV_STATUS_INVALID_PARAMETER = 0x8002 ,
     GEV_STATUS_INVALID_ADDRESS = 0x8003 ,
     GEV_STATUS_WRITE_PROTECT = 0x8004 ,
     GEV_STATUS_BAD_ALIGNMENT = 0x8005 ,
     GEV_STATUS_ACCESS_DENIED = 0x8006 ,
     GEV_STATUS_STATUS_BUSY = 0x8007 ,
     GEV_STATUS_PACKET_UNAVAILABLE = 0x800C ,
     GEV_STATUS_DATA_OVERRUN = 0x800D ,
     GEV_STATUS_INVALID_HEADER = 0x800E ,
     GEV_STATUS_PACKET_NOT_YET_AVAILABLE = 0x8010 ,
     GEV_STATUS_PACKET_AND_PREV_REMOVED_FROM_MEMORY = 0x8011 ,
     GEV_STATUS_PACKET_REMOVED_FROM_MEMORY = 0x8012 ,
     GEV_STATUS_NO_REF_TIME = 0x8013 ,
     GEV_STATUS_PACKET_TEMPORARY_UNAVAILABLE = 0x8014 ,
     GEV_STATUS_OVERFLOW = 0x8015 ,
     GEV_STATUS_ACTION_LATE = 0x8016 ,
     GEV_STATUS_LEADER_TRAILER_OVERFLOW = 0x8017 ,
     GEV_STATUS_ERROR = 0x8FFF ,
     GEV_STATUS_DEPRECATED_0x8008= 0x8008 ,
     GEV_STATUS_DEPRECATED_0x8009= 0x8009 ,
     GEV_STATUS_DEPRECATED_0x800A= 0x800A ,
     GEV_STATUS_DEPRECATED_0x800B= 0x800B ,
     GEV_STATUS_DEPRECATED_0x800F= 0x800F 
} gev_status_t ;

#endif
