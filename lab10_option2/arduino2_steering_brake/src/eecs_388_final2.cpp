/******************************************************************************
 *   Header Includes
 *******************************************************************************/
#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>

#include "eecs_388_lib2.h"

/******************************************************************************
 *   Constant definitions
 *******************************************************************************/

/*ServoMotor constants*/
#define SERVO_PULSE_MAX (2400)    /* 2400 us */
#define SERVO_PULSE_MIN (544)     /* 544 us */
#define SERVO_PERIOD    (20000)   /* 20000 us (20ms) */
#define MIN_ANGLE       (0)       /* degrees */
#define MAX_ANGLE       (180)     /* degrees */
#define SAFE_DIST (201)
#define CLOSE_DIST (100)
#define VERY_CLOSE_DIST (60) 
#define TOO_CLOSE_DIST (0)
  uint8_t distmax = 201;
  uint8_t distmid = 100;
  uint8_t distmin = 60;

  volatile uint8_t dist = 0;

  ISR(USART_RX_vect) {
    dist = UDR0;
}


volatile bool ledState = false;

ISR(TIMER1_COMPA_vect) {
    ledState = !ledState;
}

/******************************************************************************
 *   Local function prototypes
 *******************************************************************************/
static void auto_brake();
static void steering(int pos);

/******************************************************************************
 *   Function: auto_brake() - Auto Brake
 *      Pre condition: 
 *          None
 *      Post condition: 
 *          Checks the LiDAR distance and configures LEDs
 *******************************************************************************/
static void auto_brake()
{
    // Task-2: 
    // Your code goes here (Use Lab 2 & 4 for reference)
    // Check the project document to understand the task
}


/******************************************************************************
 *   Function: steering() - Steering
 *      Pre condition: 
 *          None
 *      Post condition: 
 *          Control the servomotor with GPIO_6
 *******************************************************************************/
static void steering(int pos)
{
    // Task-4: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task
}

/******************************************************************************
 *   Function: setup() - Initializes the Arduino System
 *      Pre condition: 
 *          Hardware must be properly connected (BMP180 sensors, etc.)
 *      Post condition: 
 *          Runs initialization calls one time on power up
 *          UART is initialized for ser_printf()
 *          I2C is initialized for communication with BMP180
 *          calib_data is filled with calibration data from BMP180
 *******************************************************************************/
void setup() 
{
uart_init();                 // Initialize UART first

UCSR0B |= (1 << RXCIE0);    // Enable receive interrupt

sei();                      // Enable global interrupts


cli();  // Temporarily disable interrupts

TCCR1A = 0;
TCCR1B = 0;
TCNT1 = 0;

OCR1A = 12499;  // 50 ms at 16 MHz, prescaler 64

TCCR1B |= (1 << WGM12);   // CTC mode
TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 64

TIMSK1 |= (1 << OCIE1A);  // Enable Timer1 compare interrupt

sei();  // Enable global interrupts


//Setup Auto-break LEDs for Distance
gpio_mode(GPIO_13, GPIO_OUTPUT); //RED
gpio_mode(GPIO_12, GPIO_OUTPUT); //GREEN
gpio_mode(GPIO_11, GPIO_OUTPUT); //BLUE

//Setup GPIO_6 for PWM output
gpio_mode(GPIO_6, GPIO_OUTPUT);

ser_printf("System Initialized");
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
// Task-4: 
// Setup simulated code for the angles from the lab sheet
int angle;

// Task-1&2:
// Receive data from arudino1 for the distance and setup the LEDs
auto_brake();

//  Task-4:
for (int i = 0; i < 10; i++)
  {
  // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
  // and 0 if the prediction is a negative angle.
  // This is so that it is easier to see the movement of the servo.
  // You are welcome to pass the angle values directly to the steering function.
  // If the servo function is written correctly, it should still work,
  // only the movements of the servo will be more subtle
  if( angle > 0 )
    {
    steering(180);
    }
  else 
    {
    steering(0);
    }
           
  // Uncomment the line below to see the actual angles on the servo.
  // Remember to comment out the if-else statement above!
  // steering(angle);



  char buffer[20];
  sprintf(buffer, "Distance: %u", (unsigned int)dist);
  ser_printline(buffer);


  }
  if (dist >= distmax) {
    //ser_printline("A");
    gpio_write(GPIO_13, ON);
    gpio_write(GPIO_11, OFF);
  } else if (dist > distmid) {
    //ser_printline("B");
    gpio_write(GPIO_13, ON);
    gpio_write(GPIO_11, ON);
  } else if (dist > distmin){
    //ser_printline("C");
    gpio_write(GPIO_13, OFF);
    gpio_write(GPIO_11, ON);
  } else {
    //ser_printline("D");
    gpio_write(GPIO_11, ledState ? ON : OFF);
    gpio_write(GPIO_13, OFF);
  }
  ;
}

