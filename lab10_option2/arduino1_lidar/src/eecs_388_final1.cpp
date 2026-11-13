/******************************************************************************
 *   Header Includes
 *******************************************************************************/
#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>

#include "eecs_388_lib.h"

/******************************************************************************
 *   Function: setup() - Initializes the Arduino System
 *      Pre condition: 
 *          Hardware must be properly connected (sensors, LEDS, etc.)
 *      Post condition: 
 *          Runs initialization calls one time on power up
 *          Input/Output (IO) pins are configured
 *******************************************************************************/
void setup() 
{
uart_init();

return;
}

/******************************************************************************
 *   Function: loop() - Main execution loop
 *      Pre condition: 
 *          setup() has been executed and system is initialized
 *      Post condition: 
 *          Performs a single iteration of the system's function
 *          Repeates indefinetely unless the board is reset or powered off
 *******************************************************************************/
void loop() 
{
uint16_t dist = 0;              /* LIDAR distance data is 16 bits. */

ser_printline("Setup completed.");
ser_write( '\n' );
    //gpio_write(GPIO_11, ON);
    //gpio_write(GPIO_12, ON);
    //gpio_write(GPIO_13, ON);
while( 1 )
    { 
    /* 
        - read a data frame from the TFmini sensor
        
        - from Table 6, we know each LIDAR measurement sends 9 bytes
        - the function `ser_read()` can process 1 byte per function call

        - This if-statement below will read (and check) the first two bytes of a valid data frame
        - You will need to write code to read the subsequent bytes to get the distance data
        - You will need to process the data received from the sensor to get the correct distance value
    */
    if ('Y' == ser_read() && 'Y' == ser_read()) 
        {
        /* 

            - print the measured value to the console (i.e. the serial monitor) 
            - You can use ser_printf() or ser_printline() or ser_write()
            
            - Helpful guide for printing with variadic function similiar to printf()
            - https://en.cppreference.com/w/cpp/io/c/fprintf


			- Send the distance to Arduino2
        */
        uint8_t dist_L = ser_read();
        uint8_t dist_H = ser_read();
        uint16_t dist = static_cast<uint16_t>(dist_L) | (static_cast<uint16_t>(uint16_t(dist_H) << 8));
        uint8_t strength_L = ser_read();
        int8_t strength_H = ser_read();
        uint8_t rsvd = ser_read();
        uint8_t quality = ser_read();
        uint8_t checksum = ser_read();
        uint16_t min = 30;
        uint16_t threshold = 75;
        uint16_t max = 1200;

        uint16_t sum = 'Y' + 'Y' + dist_L + dist_H + strength_L + strength_H + rsvd + quality;
        uint16_t calculated_checksum = sum & (0x00FF);

        if (calculated_checksum == checksum) {
            //ser_printline("Checksum matched!\n");
            ser_write('G');
        } else {
            //ser_printline("Checksum NOT matched!\n");
            ser_write('Z');
        }

        if (dist == min) {
            gpio_write(GPIO_11, OFF);
            gpio_write(GPIO_12, OFF);
            gpio_write(GPIO_13, OFF);
        } else if (dist < threshold){
            gpio_write(GPIO_11, OFF);
            gpio_write(GPIO_12, OFF);
            gpio_write(GPIO_13, ON);
        } else if (dist < max) {
            gpio_write(GPIO_11, OFF);
            gpio_write(GPIO_12, ON);
            gpio_write(GPIO_13, OFF);
        } else {
            gpio_write(GPIO_11, ON);
            gpio_write(GPIO_12, OFF);
            gpio_write(GPIO_13, OFF);
        }
        }
    }
    return;
}