#include "tsl2561.h"
#include "tca9458a/tca9458a.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t done = 0;

void sighandler(int sig)
{
    done = 1;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Invocation: sudo ./testcss.out <I2C Bus Number>\n\n");
        return 0;
    }
    int bus = atoi(argv[1]);
    signal(SIGINT, &sighandler);
    tsl2561 lux[7];
    tca9458a mux[1];
    if (tca9458a_init(mux, bus, 0x70, 0) < 0)
    {
        printf("Could not initialize mux\n");
        return 0;
    }
    // activate mux channel 0
    if (tca9458a_set(mux, 0) < 0)
    {
        printf("Could not set mux channel 0\n");
        goto err_close_mux;
    }
    // activate dev on channel 0
    if (tsl2561_init(&(lux[0]), bus, 0x29, 0) < 0)
    {
        printf("Could not open 0x29 chn 0\n");
    }
    if (tsl2561_init(&(lux[1]), bus, 0x39, 0) < 0)
    {
        printf("Could not open 0x39 chn 0\n");
    }
    if (tsl2561_init(&(lux[2]), bus, 0x49, 0) < 0)
    {
        printf("Could not open 0x49 chn 0\n");
    }
    // activate mux channel 1
    if (tca9458a_set(mux, 1) < 0)
    {
        printf("Could not set mux channel 0\n");
        goto err_close_mux;
    }
    // activate dev on channel 1
    if (tsl2561_init(&(lux[3]), bus, 0x39, 0) < 0)
    {
        printf("Could not open 0x39 chn 0\n");
    }
    if (tsl2561_init(&(lux[4]), bus, 0x49, 0) < 0)
    {
        printf("Could not open 0x49 chn 0\n");
    }
    if (tsl2561_init(&(lux[5]), bus, 0x29, 0) < 0)
    {
        printf("Could not open 0x29 chn 0\n");
    }
    // activate mux channel 1
    if (tca9458a_set(mux, 2) < 0)
    {
        printf("Could not set mux channel 0\n");
        goto err_close_mux;
    }
    if (tsl2561_init(&(lux[6]), bus, 0x39, 0) < 0)
    {
        printf("Could not open 0x39 chn 0\n");
    }
    ssize_t print_char = 0;
    while (!done)
    {
        uint32_t lv[7];
        uint32_t mes;
        // activate mux channel 0
        if (tca9458a_set(mux, 0) < 0)
        {
            printf("Could not set mux channel 0\n");
            goto err_close_mux;
        }
        // read dev on chn 0
        tsl2561_measure(&(lux[0]), &mes);
        lv[0] = tsl2561_get_lux(mes);
        tsl2561_measure(&(lux[1]), &mes);
        lv[1] = tsl2561_get_lux(mes);
        tsl2561_measure(&(lux[2]), &mes);
        lv[2] = tsl2561_get_lux(mes);
        // activate mux channel 1
        if (tca9458a_set(mux, 1) < 0)
        {
            printf("Could not set mux channel 0\n");
            goto err_close_mux;
        }
        tsl2561_measure(&(lux[3]), &mes);
        lv[3] = tsl2561_get_lux(mes);
        tsl2561_measure(&(lux[4]), &mes);
        lv[4] = tsl2561_get_lux(mes);
        tsl2561_measure(&(lux[5]), &mes);
        lv[5] = tsl2561_get_lux(mes);
        // activate mux channel 0
        if (tca9458a_set(mux, 2) < 0)
        {
            printf("Could not set mux channel 0\n");
            goto err_close_mux;
        }
        tsl2561_measure(&(lux[6]), &mes);
        lv[6] = tsl2561_get_lux(mes);
        print_char = printf("%u %u %u %u %u %u %u\r", lv[0], lv[1], lv[2], lv[3], lv[4], lv[5], lv[6]);
        fflush(stdout);
        usleep(100*1000);
        while(print_char--)
            printf(" ");
        printf("\r");
    }
    printf("\n");
    // activate mux channel 0
    if (tca9458a_set(mux, 0) < 0)
    {
        printf("Could not set mux channel 0\n");
        goto err_close_mux;
    }
    // printf("Here\n");
    // fflush(stdout);
    // destroy
    tsl2561_destroy(&(lux[0]));
    tsl2561_destroy(&(lux[1]));
    tsl2561_destroy(&(lux[2]));
    // activate mux channel 1
    if (tca9458a_set(mux, 1) < 0)
    {
        printf("Could not set mux channel 0\n");
        goto err_close_mux;
    }
    tsl2561_destroy(&(lux[3]));
    tsl2561_destroy(&(lux[4]));
    tsl2561_destroy(&(lux[5]));
    // activate mux channel 2
    if (tca9458a_set(mux, 2) < 0)
    {
        printf("Could not set mux channel 0\n");
        goto err_close_mux;
    }
    tsl2561_destroy(&(lux[6]));
err_close_mux:
    tca9458a_destroy(mux);
    return 0;
}