/**
 * @file tsl2561.h
 * @author Sunip K. Mukherjee (sunipkmukherjee@gmail.com)
 * @brief TSL2561 I2C driver function and struct declarations
 * @version 0.2
 * @date 2020-03-19
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef TSL2561_H
#define TSL2561_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

/******************************************************************************/
#define TSL2561_VISIBLE 2      ///< channel 0 - channel 1
#define TSL2561_INFRARED 1     ///< channel 1
#define TSL2561_FULLSPECTRUM 0 ///< channel 0

// I2C address options
#define TSL2561_ADDR_LOW (0x29)   ///< Default address (pin pulled low)
#define TSL2561_ADDR_FLOAT (0x39) ///< Default address (pin left floating)
#define TSL2561_ADDR_HIGH (0x49)  ///< Default address (pin pulled high)

// Lux calculations differ slightly for CS package
//#define TSL2561_PACKAGE_CS                ///< Chip scale package
#define TSL2561_PACKAGE_T_FN_CL ///< Dual Flat No-Lead package

#define TSL2561_COMMAND_BIT (0x80) ///< Must be 1
#define TSL2561_CLEAR_BIT (0x40)   ///< Clears any pending interrupt (write 1 to clear)
#define TSL2561_WORD_BIT (0x20)    ///< 1 = read/write word (rather than byte)
#define TSL2561_BLOCK_BIT (0x10)   ///< 1 = using block read/write

#define TSL2561_CONTROL_POWERON (0x03)  ///< Control register setting to turn on
#define TSL2561_CONTROL_POWEROFF (0x00) ///< Control register setting to turn off

#define TSL2561_LUX_LUXSCALE (14)          ///< Scale by 2^14
#define TSL2561_LUX_RATIOSCALE (9)         ///< Scale ratio by 2^9
#define TSL2561_LUX_CHSCALE (10)           ///< Scale channel values by 2^10
#define TSL2561_LUX_CHSCALE_TINT0 (0x7517) ///< 322/11 * 2^TSL2561_LUX_CHSCALE
#define TSL2561_LUX_CHSCALE_TINT1 (0x0FE7) ///< 322/81 * 2^TSL2561_LUX_CHSCALE

// T, FN and CL package values
#define TSL2561_LUX_K1T (0x0040) ///< 0.125 * 2^RATIO_SCALE
#define TSL2561_LUX_B1T (0x01f2) ///< 0.0304 * 2^LUX_SCALE
#define TSL2561_LUX_M1T (0x01be) ///< 0.0272 * 2^LUX_SCALE
#define TSL2561_LUX_K2T (0x0080) ///< 0.250 * 2^RATIO_SCALE
#define TSL2561_LUX_B2T (0x0214) ///< 0.0325 * 2^LUX_SCALE
#define TSL2561_LUX_M2T (0x02d1) ///< 0.0440 * 2^LUX_SCALE
#define TSL2561_LUX_K3T (0x00c0) ///< 0.375 * 2^RATIO_SCALE
#define TSL2561_LUX_B3T (0x023f) ///< 0.0351 * 2^LUX_SCALE
#define TSL2561_LUX_M3T (0x037b) ///< 0.0544 * 2^LUX_SCALE
#define TSL2561_LUX_K4T (0x0100) ///< 0.50 * 2^RATIO_SCALE
#define TSL2561_LUX_B4T (0x0270) ///< 0.0381 * 2^LUX_SCALE
#define TSL2561_LUX_M4T (0x03fe) ///< 0.0624 * 2^LUX_SCALE
#define TSL2561_LUX_K5T (0x0138) ///< 0.61 * 2^RATIO_SCALE
#define TSL2561_LUX_B5T (0x016f) ///< 0.0224 * 2^LUX_SCALE
#define TSL2561_LUX_M5T (0x01fc) ///< 0.0310 * 2^LUX_SCALE
#define TSL2561_LUX_K6T (0x019a) ///< 0.80 * 2^RATIO_SCALE
#define TSL2561_LUX_B6T (0x00d2) ///< 0.0128 * 2^LUX_SCALE
#define TSL2561_LUX_M6T (0x00fb) ///< 0.0153 * 2^LUX_SCALE
#define TSL2561_LUX_K7T (0x029a) ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B7T (0x0018) ///< 0.00146 * 2^LUX_SCALE
#define TSL2561_LUX_M7T (0x0012) ///< 0.00112 * 2^LUX_SCALE
#define TSL2561_LUX_K8T (0x029a) ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B8T (0x0000) ///< 0.000 * 2^LUX_SCALE
#define TSL2561_LUX_M8T (0x0000) ///< 0.000 * 2^LUX_SCALE

// CS package values
#define TSL2561_LUX_K1C (0x0043) ///< 0.130 * 2^RATIO_SCALE
#define TSL2561_LUX_B1C (0x0204) ///< 0.0315 * 2^LUX_SCALE
#define TSL2561_LUX_M1C (0x01ad) ///< 0.0262 * 2^LUX_SCALE
#define TSL2561_LUX_K2C (0x0085) ///< 0.260 * 2^RATIO_SCALE
#define TSL2561_LUX_B2C (0x0228) ///< 0.0337 * 2^LUX_SCALE
#define TSL2561_LUX_M2C (0x02c1) ///< 0.0430 * 2^LUX_SCALE
#define TSL2561_LUX_K3C (0x00c8) ///< 0.390 * 2^RATIO_SCALE
#define TSL2561_LUX_B3C (0x0253) ///< 0.0363 * 2^LUX_SCALE
#define TSL2561_LUX_M3C (0x0363) ///< 0.0529 * 2^LUX_SCALE
#define TSL2561_LUX_K4C (0x010a) ///< 0.520 * 2^RATIO_SCALE
#define TSL2561_LUX_B4C (0x0282) ///< 0.0392 * 2^LUX_SCALE
#define TSL2561_LUX_M4C (0x03df) ///< 0.0605 * 2^LUX_SCALE
#define TSL2561_LUX_K5C (0x014d) ///< 0.65 * 2^RATIO_SCALE
#define TSL2561_LUX_B5C (0x0177) ///< 0.0229 * 2^LUX_SCALE
#define TSL2561_LUX_M5C (0x01dd) ///< 0.0291 * 2^LUX_SCALE
#define TSL2561_LUX_K6C (0x019a) ///< 0.80 * 2^RATIO_SCALE
#define TSL2561_LUX_B6C (0x0101) ///< 0.0157 * 2^LUX_SCALE
#define TSL2561_LUX_M6C (0x0127) ///< 0.0180 * 2^LUX_SCALE
#define TSL2561_LUX_K7C (0x029a) ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B7C (0x0037) ///< 0.00338 * 2^LUX_SCALE
#define TSL2561_LUX_M7C (0x002b) ///< 0.00260 * 2^LUX_SCALE
#define TSL2561_LUX_K8C (0x029a) ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B8C (0x0000) ///< 0.000 * 2^LUX_SCALE
#define TSL2561_LUX_M8C (0x0000) ///< 0.000 * 2^LUX_SCALE

// Auto-gain thresholds
#define TSL2561_AGC_THI_13MS (4850)   ///< Max value at Ti 13ms = 5047
#define TSL2561_AGC_TLO_13MS (100)    ///< Min value at Ti 13ms = 100
#define TSL2561_AGC_THI_101MS (36000) ///< Max value at Ti 101ms = 37177
#define TSL2561_AGC_TLO_101MS (200)   ///< Min value at Ti 101ms = 200
#define TSL2561_AGC_THI_402MS (63000) ///< Max value at Ti 402ms = 65535
#define TSL2561_AGC_TLO_402MS (500)   ///< Min value at Ti 402ms = 500

// Clipping thresholds
#define TSL2561_CLIPPING_13MS (4900)   ///< Counts that trigger a change in gain/integration
#define TSL2561_CLIPPING_101MS (37000) ///< Counts that trigger a change in gain/integration
#define TSL2561_CLIPPING_402MS (65000) ///< Counts that trigger a change in gain/integration

// Delay for integration times
#define TSL2561_DELAY_INTTIME_13MS (15)   ///< Wait 15ms for 13ms integration
#define TSL2561_DELAY_INTTIME_101MS (120) ///< Wait 120ms for 101ms integration
#define TSL2561_DELAY_INTTIME_402MS (450) ///< Wait 450ms for 402ms integration

/**
 * @brief TSL2561 I2C Registers
 * 
 */
typedef enum
{
    TSL2561_REGISTER_CONTROL = 0x00,          ///< Control/power register
    TSL2561_REGISTER_TIMING = 0x01,           ///< Set integration time register
    TSL2561_REGISTER_THRESHHOLDL_LOW = 0x02,  ///< Interrupt low threshold low-byte
    TSL2561_REGISTER_THRESHHOLDL_HIGH = 0x03, ///< Interrupt low threshold high-byte
    TSL2561_REGISTER_THRESHHOLDH_LOW = 0x04,  ///< Interrupt high threshold low-byte
    TSL2561_REGISTER_THRESHHOLDH_HIGH = 0x05, ///< Interrupt high threshold high-byte
    TSL2561_REGISTER_INTERRUPT = 0x06,        ///< Interrupt settings
    TSL2561_REGISTER_CRC = 0x08,              ///< Factory use only
    TSL2561_REGISTER_ID = 0x0A,               ///< TSL2561 identification setting
    TSL2561_REGISTER_CHAN0_LOW = 0x0C,        ///< Light data channel 0, low byte
    TSL2561_REGISTER_CHAN0_HIGH = 0x0D,       ///< Light data channel 0, high byte
    TSL2561_REGISTER_CHAN1_LOW = 0x0E,        ///< Light data channel 1, low byte
    TSL2561_REGISTER_CHAN1_HIGH = 0x0F        ///< Light data channel 1, high byte
} TSL2561_REGISTER_SET;

/**
 * @brief Three options for how long to integrate readings for
 * 
 */
typedef enum
{
    TSL2561_INTEGRATIONTIME_13MS = 0x00,  ///< 13.7ms
    TSL2561_INTEGRATIONTIME_101MS = 0x01, ///< 101ms
    TSL2561_INTEGRATIONTIME_402MS = 0x02  ///< 402ms
} tsl2561IntegrationTime_t;

/**
 * @brief  TSL2561 offers 2 gain settings
 * 
 */
typedef enum
{
    TSL2561_GAIN_1X = 0x00,  ///< No gain
    TSL2561_GAIN_16X = 0x10, ///< 16x gain
} tsl2561Gain_t;

/******************************************************************************/
#define TSL2561_BLOCK_READ 0x0B ///< Block read mask

#include <i2cbus/i2cbus.h>
/**
 * @brief TSL2561 Device Handle
 * 
 */
typedef i2cbus tsl2561;

/**
 * @brief Opens a TSL2561 Lux sensor on bus given by ID (X in /dev/i2c-X)
 * at address addr, belonging to context ctx.
 * TODO: Fix init + gain, figure out what goes wrong if ID
 * register is read
 * 
 * @param dev tsl2561 device handle, which is an alias to i2cbus handle
 * @param id I2C Bus ID
 * @param addr Device Address
 * @param ctx Device context
 * @return int Status of i2cbus_open call
 */
int tsl2561_init(tsl2561 *dev, int id, int addr, int ctx);
/**
 * @brief Get a measurement and store it in the 
 * 
 * @param dev Handle to tsl2561 device
 * @param measure Pointer to uint32 where measurement is stored
 * 
 * @return int Return status of i2cbus_read
 */
int tsl2561_measure(tsl2561 *dev, uint32_t *measure);
/**
 * @brief Convert a raw TSL2561 measurement to lux
 * 
 * @param measure Measurement using tsl2561_measure
 * @return uint32_t Lux output from measurement
 */
uint32_t tsl2561_get_lux(uint32_t measure);
/**
 * @brief Close I2C bus corresponding to the device
 * 
 * @param dev tsl2561 device handle
 */
int tsl2561_destroy(tsl2561 *dev);
#ifdef __cplusplus
}
#endif
#endif // TSL2561_H