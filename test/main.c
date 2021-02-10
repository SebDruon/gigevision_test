#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>

#include <cyan/hwcam/hwcam.h>


int main(int argc, char** argv, char** envv ) {

    hwcam_t* camera ;
    char ip[16] = "172.20.0.18" ;
    // char ip[16] = "10.24.3.105" ;
    int port = 3956 ;
    camera = hwcam_new( "/home/seb/devel/gigevision_test/build/libcam_gigevision.so.0.1.1", ip, port, "test", "yolo") ;

    sleep(60) ;
    return 0 ;

}
