#include "tsl2561.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

volatile sig_atomic_t done = 0 ;

void catch_sigint()
{
    done = 1;
    printf("Received SIGINT\n");
    fflush(stdout);
}

int main(int argc, char ** argv)
{
    if ( argc != 2 )
    {
        printf("Invocation: ./test <address>\n");
        exit(0);
    }
    uint8_t addr = atoi(argv[1]);
    switch(addr)
    {
        case 29:
            addr = 0x29;
            break;
        case 39:
            addr = 0x39;
            break;
        case 49:
            addr = 0x49;
            break;
        default:
            printf("Wrong address for TSL2561 %d, exiting...\n", addr);
            exit(0);
            break;
    }
    tsl2561 *dev = (tsl2561 *)malloc(sizeof(tsl2561));
    if(tsl2561_init(dev, 0x29) > 0)
        printf("Init successful\n");
    struct sigaction sa ;
    sa.sa_handler = &catch_sigint;
    sigaction(SIGINT, &sa, NULL);
    while(!done)
    {
        unsigned int measure ;
        tsl2561_measure(dev, &measure);
        printf("0x%08x | %05d\n", measure, tsl2561_get_lux(measure));
        fflush(stdout);
        usleep(100000);
    }
    printf("\n");
    tsl2561_destroy(dev);
    return 0;
}
