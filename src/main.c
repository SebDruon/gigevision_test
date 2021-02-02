#include <stdio.h>
#include <unistd.h>

#include "gvcp_thread.h"


int main(int argc, char** argv, char** envv ) {

    gvcp_thread_t* mygvcp ;
    mygvcp = gvcp_create( "172.20.0.18", 3956 ) ;

    sleep(60) ;

    return 0 ;

}
