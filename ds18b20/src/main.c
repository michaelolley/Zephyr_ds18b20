/*
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "ds18b20.h"


#include <stdio.h>

int main(void)
{
	float temp = 15.78;

	printk("TP: %f\n", (double)temp);
	ds18b20_setResolution(12);
	temp = ds18b20_get_temp_method_2();

	printk("Temperature: %f \n", (double)temp);

	while (1)
	{
		k_sleep(K_SECONDS(2));
		temp = ds18b20_get_temp_method_2();
		printk("Temperature: %f \n", (double)temp);
	}

	return 0;
}
