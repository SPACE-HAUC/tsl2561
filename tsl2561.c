/**
 * @file tsl2561.c
 * @author Sunip K. Mukherjee (sunipkmukherjee@gmail.com)
 * @brief TSL2561 I2C driver function definitions
 * @version 0.1
 * @date 2020-03-19
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "tsl2561.h"
#include <i2cbus/i2cbus.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

int tsl2561_init(tsl2561 *dev, int id, int addr, int ctx)
{
    // Create the file descriptor handle to the device
    if (i2cbus_open(dev, id, addr) < 0)
    {
        eprintf("%s: Error: Failed to open I2C Bus: ", __func__);
        perror(__FUNCTION__);
        return -1;
    }

    // Power the device - write to control register
    unsigned char cmd_pwup[] = {0x80, 0x03};
    if (i2cbus_write(dev, cmd_pwup, sizeof(cmd_pwup)) < 0)
    {
        eprintf("%s: Error: Failed to send power up command: ", __func__);
        perror(__FUNCTION__);
        return -1;
    }
    usleep(100000);
    // Verify that device is powered
    if (i2cbus_xfer(dev, cmd_pwup, 1, cmd_pwup + 1, 1, 0) < 0)
    {
        eprintf("%s: Error: Could not read the power up register: ", __func__);
        perror(__FUNCTION__);
        return -1;
    }
    if (cmd_pwup[1] & 0x3 != 0x3)
    {
        eprintf("%s: Power up failed, returned 0x02x\n", __func__, cmd_pwup[1]);
        return -1;
    }
    /* DO NOT READ THE DEVICE REGISTER */
#ifdef CSS_LOW_GAIN
    // Set the timing and gain
    unsigned char cmd_gain[] = {0x81, 0x0};
    if (i2cbus_write(dev, cmd_gain, sizeof(cmd_gain)) < 0)
    {
        eprintf("%s: Error: Failed to send gain command: ", __func__);
        perror(__FUNCTION__);
        return -1;
    }
    usleep(100000);
    if (i2cbus_xfer(dev, cmd_gain, 1, cmd_gain + 1, 1, 0) < 0)
    {
        eprintf("%s: Error: Could not read the gain register: ", __func__);
        perror(__FUNCTION__);
        return -1;
    }
    if (cmd_gain[1])
    {
        eprintf("%s: Could not set timing and gain\n", __func__);
        return -1;
    }
#endif
    return 1;
}

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

int tsl2561_measure(tsl2561 *dev, uint32_t *measure)
{
    if (unlikely(dev == NULL))
    {
        return -1;
    }
    *measure = 0x0;
    uint8_t cmd_buf[] = {0xac, 0x0};
    if (unlikely(i2cbus_xfer(dev, cmd_buf, 1, cmd_buf, 2, 0) < 0))
    {
        eprintf("%s: Error reading first set of bytes\n", __func__);
        return -1;
    }
    *measure |= cmd_buf[0] << 8 | cmd_buf[1];
    *measure <<= 16;
#ifdef TSL2561_DEBUG
    eprintf("%s: Step 1: 0x%02x%02x -> 0x%08x\t", __fund__, cmd_buf[0], cmd_buf[1], *measure);
#endif
    cmd_buf[0] = 0xae;
    cmd_buf[1] = 0x0;
    if (unlikely(i2cbus_xfer(dev, cmd_buf, 1, cmd_buf, 2, 0) < 0))
    {
        eprintf("%s: Error reading first set of bytes\n", __func__);
        return -1;
    }
    *measure |= cmd_buf[0] << 8 | cmd_buf[1];
#ifdef TSL2561_DEBUG
    eprintf("Step 2: 0x%02x%02x -> 0x%08x\n", cmd_buf[0], cmd_buf[1], *measure);
#endif
    return 1;
}

/**
 * @brief Calculate lux using value measured using tsl2561_measure()
 * 
 * @param measure 
 * @return Lux value 
 */
uint32_t tsl2561_get_lux(uint32_t measure)
{
    unsigned long chScale;
    unsigned long channel1;
    unsigned long channel0;

    /* Make sure the sensor isn't saturated! */
    uint16_t clipThreshold = TSL2561_CLIPPING_13MS;
    uint16_t broadband = measure >> 16;
    uint16_t ir = measure;
    /* Return 65536 lux if the sensor is saturated */
    if ((broadband > clipThreshold) || (ir > clipThreshold))
    {
        return 65536;
    }

    /* Get the correct scale depending on the intergration time */

    chScale = TSL2561_LUX_CHSCALE_TINT0;

    /* Scale for gain (1x or 16x) */
    // if (!_tsl2561Gain)
    // Gain 1x -> _tsl2561Gain == 0 so the if statement evaluates true
    chScale = chScale << 4;

    /* Scale the channel values */
    channel0 = (broadband * chScale) >> TSL2561_LUX_CHSCALE;
    channel1 = (ir * chScale) >> TSL2561_LUX_CHSCALE;

    /* Find the ratio of the channel values (Channel1/Channel0) */
    unsigned long ratio1 = 0;
    if (channel0 != 0)
        ratio1 = (channel1 << (TSL2561_LUX_RATIOSCALE + 1)) / channel0;

    /* round the ratio value */
    unsigned long ratio = (ratio1 + 1) >> 1;

    unsigned int b, m;

#ifdef TSL2561_PACKAGE_CS
    if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1C))
    {
        b = TSL2561_LUX_B1C;
        m = TSL2561_LUX_M1C;
    }
    else if (ratio <= TSL2561_LUX_K2C)
    {
        b = TSL2561_LUX_B2C;
        m = TSL2561_LUX_M2C;
    }
    else if (ratio <= TSL2561_LUX_K3C)
    {
        b = TSL2561_LUX_B3C;
        m = TSL2561_LUX_M3C;
    }
    else if (ratio <= TSL2561_LUX_K4C)
    {
        b = TSL2561_LUX_B4C;
        m = TSL2561_LUX_M4C;
    }
    else if (ratio <= TSL2561_LUX_K5C)
    {
        b = TSL2561_LUX_B5C;
        m = TSL2561_LUX_M5C;
    }
    else if (ratio <= TSL2561_LUX_K6C)
    {
        b = TSL2561_LUX_B6C;
        m = TSL2561_LUX_M6C;
    }
    else if (ratio <= TSL2561_LUX_K7C)
    {
        b = TSL2561_LUX_B7C;
        m = TSL2561_LUX_M7C;
    }
    else if (ratio > TSL2561_LUX_K8C)
    {
        b = TSL2561_LUX_B8C;
        m = TSL2561_LUX_M8C;
    }
#else
    if ((ratio >= 0) && (ratio <= TSL2561_LUX_K1T))
    {
        b = TSL2561_LUX_B1T;
        m = TSL2561_LUX_M1T;
    }
    else if (ratio <= TSL2561_LUX_K2T)
    {
        b = TSL2561_LUX_B2T;
        m = TSL2561_LUX_M2T;
    }
    else if (ratio <= TSL2561_LUX_K3T)
    {
        b = TSL2561_LUX_B3T;
        m = TSL2561_LUX_M3T;
    }
    else if (ratio <= TSL2561_LUX_K4T)
    {
        b = TSL2561_LUX_B4T;
        m = TSL2561_LUX_M4T;
    }
    else if (ratio <= TSL2561_LUX_K5T)
    {
        b = TSL2561_LUX_B5T;
        m = TSL2561_LUX_M5T;
    }
    else if (ratio <= TSL2561_LUX_K6T)
    {
        b = TSL2561_LUX_B6T;
        m = TSL2561_LUX_M6T;
    }
    else if (ratio <= TSL2561_LUX_K7T)
    {
        b = TSL2561_LUX_B7T;
        m = TSL2561_LUX_M7T;
    }
    else if (ratio > TSL2561_LUX_K8T)
    {
        b = TSL2561_LUX_B8T;
        m = TSL2561_LUX_M8T;
    }
#endif

    unsigned long temp;
    channel0 = channel0 * b;
    channel1 = channel1 * m;

    temp = 0;
    /* Do not allow negative lux value */
    if (channel0 > channel1)
        temp = channel0 - channel1;

    /* Round lsb (2^(LUX_SCALE-1)) */
    temp += (1 << (TSL2561_LUX_LUXSCALE - 1));

    /* Strip off fractional portion */
    uint32_t lux = temp >> TSL2561_LUX_LUXSCALE;

    /* Signal I2C had no errors */
    return lux;
}

int tsl2561_destroy(tsl2561 *dev)
{
    static unsigned char cmd_buf[] = {0x80, 0x0};
    if (i2cbus_write(dev, cmd_buf, 2) < 0)
    {
        eprintf("%s: Could not send power down command\n", __func__);
        return -1;
    }
    return i2cbus_close(dev);
}

#ifdef UNIT_TEST
#include <signal.h>
#include <stdio.h>
#include <string.h>
volatile sig_atomic_t done = 0;

void sighandler(int sig)
{
    done = 1;
#ifdef TSL2561_DEBUG
    eprintf("%s: Received signal %d\n", __func__, sig);
#endif
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invocation: ./%s <Bus ID> <Address (in hex)>\n\n", argv[0]);
        return 0;
    }
    int id = atoi(argv[0]);
    int addr = (int)strtol(argv[2], NULL, 16); // convert to hex
    if (addr != 0x29 || addr != 0x39 || addr != 0x49)
    {
        printf("0x%02x is not appropriate for TSL2561, exiting...\n", addr);
        return -1;
    }
    tsl2561 *dev = (tsl2561 *)malloc(sizeof(tsl2561));
    if (tsl2561_init(dev, id, addr, 0x0) < 0)
    {
        printf("Could not initialize device, exiting...\n");
        goto end;
    }
    signal(SIGINT, &sighandler);
    while(!done)
    {
        unsigned int measure = 0x0;
        if (tsl2561_measure(dev, &measure) < 0)
        {
            printf("main: Error taking measurement, exiting...\n");
            break;
        }
        int num_char = printf("0x%08x | %05d", measure, tsl2561_get_lux(measure));
        fflush(stdout);
        usleep(100000);
        printf("\r");
        while(num_char--)
            printf(" ");
        printf("\r");
    }
    printf("\n");
end:
    free(dev);
}
#endif