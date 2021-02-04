#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>

#include "gvcp_thread.h"


int main(int argc, char** argv, char** envv ) {

    gvcp_thread_t* mygvcp ;
    mygvcp = gvcp_create( "172.20.0.18", 3956 ) ;
    
    sleep(2) ;
    
    uint32_t reg = 0x0A00 ;
    uint32_t value ;
    gvcp_readreg( mygvcp, 1, &reg, &value ) ;
    printf("%08X \n", value ) ;
   
    uint16_t result ;
    value = 0x0002;
    gvcp_writereg( mygvcp, 1, &reg, &value, &result ) ;
    printf("%d\n",result) ;

    value = 0 ;
    gvcp_readreg( mygvcp, 1, &reg, &value ) ;
    printf("%08X \n", value ) ;

    // ---------------------------------------------------------

    reg = 0x00010000 ;
    uint8_t msg[50] ;
    gvcp_readmem( mygvcp, reg, 50, msg ) ;
    printf("%s \n", msg ) ;

    // ---------------------------------------------------------
    
    sleep(60) ;

    return 0 ;

}
