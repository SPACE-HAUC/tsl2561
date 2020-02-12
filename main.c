#include "tsl2561.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

volatile sig_atomic_t done = 0 ;

void catch_sigint()
{
    done = 1;
}

int main()
{
    tsl2561 *dev = (tsl2561 *)malloc(sizeof(tsl2561));
    if(tsl2561_init(dev, 0x39) > 0)
        printf("Init successful\n");
    struct sigaction sa ;
    sa.sa_handler = &catch_sigint;
    sigaction(SIGINT, &sa, NULL);
    while(!done)
    {
        unsigned int measure ;
        tsl2561_measure(dev, &measure);
        printf("0x%08x | %05d\r", measure, tsl2561_get_lux(measure));
        fflush(stdout);
        usleep(100000);
    }
    printf("\n");
    tsl2561_destroy(dev);
    return 0;
}
