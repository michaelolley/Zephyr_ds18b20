/*
  */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/w1.h>
#include "ds18b20.h"

// Commands
#define STARTCONVO 0x44
#define READSCRATCH 0xBE
#define WRITESCRATCH 0x4E

// Scratchpad locations
#define TEMP_LSB 0
#define TEMP_MSB 1

// Device resolution
#define TEMP_9_BIT 0x1F  //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

typedef uint8_t ScratchPad[9];

#define DS_PIN DT_ALIAS(ds)
const struct device *const dev = DEVICE_DT_GET(DS_PIN);

int ds18b20_init(void)
{
 	if (!device_is_ready(dev))
	{
		return 0;
	}
    return 1;
}

/**@brief Function for reading scratchpad value
 */
void ds18b20_readScratchPad(uint8_t *scratchPad, uint8_t fields)
{
    w1_reset_bus(dev);
    w1_write_byte(dev, 0xCC);
    w1_write_byte(dev, READSCRATCH);

    for (uint8_t i = 0; i < fields; i++)
    {
        scratchPad[i] = w1_read_byte(dev);
    }
    w1_reset_bus(dev);
}

/**@brief Function for reading temperature.
 */
float ds18b20_get_temp(void)
{
    int check;
    char temp1 = 0, temp2 = 0;
    float temp;

    check = w1_reset_bus(dev);
    if (check)
    {
 		w1_write_byte(dev, 0xCC);
		w1_write_byte(dev, 0x44);
		while (w1_read_bit(dev) == 0);
		w1_reset_bus(dev);
		w1_write_byte(dev, 0xCC);
		w1_write_byte(dev, 0xBE);
		temp1 = w1_read_byte(dev);
		temp2 = w1_read_byte(dev);
		w1_reset_bus(dev);
		temp = (float)(temp1 + (temp2 * 256)) / 16;
        return temp;
    }
    return 0;
}

/**@brief Function for request temperature reading
 */
void ds18b20_requestTemperatures(void)
{
    w1_reset_bus(dev);
    w1_write_byte(dev, 0xCC);
    w1_write_byte(dev, STARTCONVO);
}



/**@brief Function for reading temperature method 2
 */
float ds18b20_get_temp_method_2(void)
{
    ScratchPad scratchPad;

    ds18b20_requestTemperatures();
    ds18b20_readScratchPad(scratchPad, 2);
    int16_t rawTemperature = (((int16_t)scratchPad[TEMP_MSB]) << 8) | scratchPad[TEMP_LSB];
    float temp = 0.0625 * rawTemperature;
    return temp;
}

/**@brief Function for setting temperature resolution
 */
void ds18b20_setResolution(int resolution)
{
    w1_reset_bus(dev);
    w1_write_byte(dev, 0xCC);
    w1_write_byte(dev, WRITESCRATCH);
    // two dummy values for LOW & HIGH ALARM
    w1_write_byte(dev, 0x00);
    w1_write_byte(dev, 100);
    switch (resolution)
    {
    case 12:
        w1_write_byte(dev, TEMP_12_BIT);
        break;

    case 11:
        w1_write_byte(dev, TEMP_11_BIT);
        break;

    case 10:
        w1_write_byte(dev, TEMP_10_BIT);
        break;

    case 9:
    default:
        w1_write_byte(dev, TEMP_9_BIT);
        break;
    }
    w1_reset_bus(dev);
}
