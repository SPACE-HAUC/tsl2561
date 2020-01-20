#include "tsl2561.h"

/*
TSL2561 DEVICE INITIALIZATION 
*/

int tsl2561_init(tsl2561 *dev, uint8_t s_address)
{
    int32_t dev_id = 0;
    tsl2561_command m_con;
    uint8_t write_data = 0;

    // Create the file descriptor handle to the device
    dev->fd = open(I2C_BUS, O_RDWR);
    if (dev->fd < 0)
    {
        perror("[ERROR] TSL2561 Could not create a file descriptor.");
        return -1;
    }

    // Configure file descriptor handle as a slave device w/input address
    if (ioctl(dev->fd, I2C_SLAVE, s_address) < 0)
    {
        perror("[ERROR] TSL2561 Could not assign device.");
        return -1;
    }

    // Power the device - write to control register
    uint8_t cmd[2];
    cmd[0] = 0x80;
    cmd[1] = 0x03;
    if (write(dev->fd, cmd, 2) < 2)
    {
        perror("[ERROR] Could not power on the sensor.");
        return -1;
    }
    usleep(1000);
    // Device is powered --> verify device is TSL2561 sensor - read from ID reg

    // 1) Configure the command register
    cmd[0] = 0x8a;
    // 2) Initiate the read command and read byte from ID register
    dev_id = write(dev->fd, cmd, 1);
    usleep(100);
    dev_id = read(dev->fd, cmd, 1);
    if (dev_id < 1)
    {
        perror("[ERROR] Could not read device ID.");
        return -1;
    }

    // Extract the part number and the revision number
    uint8_t partno = (cmd[0] & 0xF0) >> 4;
    uint8_t revno = (cmd[0] & 0x0F);

    printf("PARTNO: [%X], REVNO: [%04X]\n", partno, revno);
    cmd[0] = 0x80;
    dev_id = write(dev->fd, cmd, 1);
    if ((dev_id = read(dev->fd, cmd, 1)) < 0)
    {
        perror("[ERROR] Could not read device config.");
        return -1;
    }
    printf("[DEBUG] TSL2561 Init: Read config register: 0x%x\n", cmd[0]);
    if ((uint8_t)cmd[0] == 0x03)
    {
        printf("TSL2561 Initialization failed: 0x%x\n\n", 0x000000ff & (uint8_t)cmd[0]);
    }
    cmd[0] = 0x81;
    cmd[1] = 0x00;
    if (write(dev->fd, cmd, 2) < 2)
    {
        perror("[ERROR] Could not write the integration time register.");
    }
    if (read(dev->fd, cmd, 1) < 1)
    {
        perror("[ERROR] Could not read the integration time register.");
    }
    printf("[DEBUG] TSL2561 config timing reg: 0x%x\n", cmd[0]);
    return 1;
}

int tsl2561_configure(tsl2561 *dev)
{

    uint8_t cmd[2];
    cmd[0] = 0x81;
    cmd[1] = 0x00;
    if (write(dev->fd, cmd, 2) < 2)
    {
        perror("[ERROR] Could not write the integration time register.");
    }
    if (read(dev->fd, cmd, 1) < 1)
    {
        perror("[ERROR] Could not read the integration time register.");
    }
    printf("[DEBUG] TSL2561 config timing reg: 0x%x\n", cmd[0]);
    return 1;
}

int tsl2561_write(tsl2561 *dev, uint8_t reg_addr, uint8_t data)
{
    tsl2561_command m_con;

    // Configure the command register with the input register address
    m_con.cmd = 1;
    m_con.clear = 0;
    m_con.word = 0;
    m_con.block = 0;
    m_con.address = reg_addr;

    // Write the input data to the target register
    if (i2c_smbus_write_byte_data(dev->fd, m_con.raw, data) < 0)
    {
        perror("[ERROR] Could not write to device.");
        return -1;
    }

    return 1;
}

int tsl2561_read_block_data(tsl2561 *dev, uint8_t *data)
{
    if (i2c_smbus_read_i2c_block_data(dev->fd, 0x9b, 4, data) < 0)
    {
        perror("[ERROR] Could not perform a block read from the data registers.");
        return -1;
    }

    // Return the read data
    return 1;

    // You should not allocate memory in a function call and return a pointer to that to the caller,
    // this is a very easy way to create a memory leak.
}

int tsl2561_read_word_data(tsl2561 *dev, uint8_t *data)
{
    // Read the Ch0 register
    int ch0 = 0x00;
    if ((ch0 = i2c_smbus_read_word_data(dev->fd, 0xac)) < 0)
    {
        perror("[ERROR] Could not perform a word read from the ch0.");
        return -1;
    }
    *((uint16_t *)data) = 0x0000ffff & ch0;
    printf("[DEBUG] Read Ch0: 0x%x\n", ch0);
    if ((ch0 = i2c_smbus_read_word_data(dev->fd, 0xae)) < 0)
    {
        perror("[ERROR] Could not perform a word read from the ch1.");
        return -1;
    }
    *((uint16_t *)&(data[2])) = 0x0000ffff & ch0;
    printf("[DEBUG] Read Ch0: 0x%x\n", ch0);
    return 1;
}

int tsl2561_read_byte_data(tsl2561 *dev, uint8_t *data)
{
    uint8_t cmd = 0x8c;
    int status = 1;
    for (int i = 0; i < 4; i++)
    {
        uint8_t tmp = 0x00;
        int ch = i2c_smbus_read_byte_data(dev->fd, cmd);
        cmd++;
        if (ch < 0)
        {
            perror("[ERROR] Could not read byte data");
            status = 0;
        }
        data[i] = ch;
        printf("[DEBUG] TSL2561 read byte: %d 0x%x -> 0x%x\n", ch, cmd - 1, tmp);
    }
    return status;
}

int tsl2561_read_i2c_data(tsl2561 *dev, uint8_t *data)
{
    char cmd = 0x9b;
    int status = 1;
    if (write(dev->fd, &cmd, 1) < 1)
        status = -1;
    if (read(dev->fd, data, 4) < 4)
        status = 0;
    return status;
}

int tsl2561_read_config(tsl2561 *dev, uint8_t *data)
{
    tsl2561_command m_con;
    int32_t configuration = 0;

    // Configure command register for block reading of all the data registers
    m_con.cmd = 1;
    m_con.clear = 0;
    m_con.word = 0;
    m_con.block = 0;
    m_con.address = TSL2561_REGISTER_TIMING;

    // Perform block reading of all 4 data registers - 4 bytes read
    configuration = i2c_smbus_read_byte_data(dev->fd, m_con.raw);
    if (configuration < 0)
    {
        perror("[ERROR] Could not read from the data registers.");
        return -1;
    }

    // Return the read data
    *data = (configuration & 0x000000FF);
    return 1;
}

uint32_t tsl2561_get_lux(tsl2561 *dev)
{
    uint8_t data[4];
    int status = tsl2561_read_i2c_data(dev, data);
    unsigned long chScale;
    unsigned long channel1;
    unsigned long channel0;

    /* Make sure the sensor isn't saturated! */
    uint16_t clipThreshold = TSL2561_CLIPPING_13MS;
    uint16_t broadband = ((uint16_t *)data)[0];
    uint16_t ir = ((uint16_t *)data)[1];
    /* Return 65536 lux if the sensor is saturated */
    if ((broadband > clipThreshold) || (ir > clipThreshold))
    {
        return 65536;
    }

    /* Get the correct scale depending on the intergration time */

    chScale = TSL2561_LUX_CHSCALE_TINT0;

    /* Scale for gain (1x or 16x) */
    // if (!_tsl2561Gain)
    //     chScale = chScale << 4;

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

void tsl2561_destroy(tsl2561 *dev)
{
    uint8_t cmd[2];
    cmd[0] = 0x80;
    cmd[1] = 0x00;
    write(dev->fd, cmd, 2);
    close(dev->fd);
    free(dev);
}