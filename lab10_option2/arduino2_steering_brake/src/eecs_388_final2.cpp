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

int angle_values[] = {10, 25, 75, 45, 100, 40, 125, 15, 150, 50, 170};

uint8_t angleIndex = 0;

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

volatile uint8_t servoCounter = 0;
volatile bool updateServo = false;

  ISR(USART_RX_vect) {
    dist = UDR0;
}

volatile bool ledState = false;

ISR(TIMER1_COMPA_vect) {
    ledState = !ledState;

    servoCounter++;

    if (servoCounter >= 30) {
        servoCounter = 0;
        updateServo = true;
    }
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

void display_gear(bool drive_gear){
  if (drive_gear){
    ser_write('d');
    gpio_write(GPIO_8, ON);
    gpio_write(GPIO_7, ON);
    gpio_write(GPIO_5, ON);
    gpio_write(GPIO_4, ON);
    gpio_write(GPIO_10, ON);
    gpio_write(GPIO_3, OFF);
    gpio_write(GPIO_9, OFF);
  } else {
    ser_write('P');
    gpio_write(GPIO_8, ON);
    gpio_write(GPIO_7, OFF);
    gpio_write(GPIO_5, OFF);
    gpio_write(GPIO_4, ON);
    gpio_write(GPIO_10, ON);
    gpio_write(GPIO_3, ON);
    gpio_write(GPIO_9, ON);

  }
  ser_write('\n');
}

void gear_change_setup(void){
  DDRD &= ~(1 << DDD2); // set gpio pin 2 as input
  PORTD |= (1 << PORTD2); // enable internal pull-up resistor
}

void gear_display_setup(void){
  gpio_mode(GPIO_8, GPIO_OUTPUT);
  gpio_mode(GPIO_7, GPIO_OUTPUT);
  gpio_mode(GPIO_5, GPIO_OUTPUT);
  gpio_mode(GPIO_4, GPIO_OUTPUT);
  gpio_mode(GPIO_10, GPIO_OUTPUT);
  gpio_mode(GPIO_3, GPIO_OUTPUT);
  gpio_mode(GPIO_9, GPIO_OUTPUT);

  //PARK
  gpio_write(GPIO_8, ON);
  gpio_write(GPIO_7, OFF);
  gpio_write(GPIO_5, OFF);
  gpio_write(GPIO_4, ON);
  gpio_write(GPIO_10, ON);
  gpio_write(GPIO_3, ON);
  gpio_write(GPIO_9, ON);
}

bool gear_change(void){
  static bool drive_gear = false;
  static bool previous_pressed = false;

  bool currently_pressed = !(PIND & (1 << PIND2));

  if (currently_pressed && !previous_pressed){
    drive_gear = !drive_gear; //switch gears
    display_gear(drive_gear);
  }

  previous_pressed = currently_pressed;
  return drive_gear;
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
static void servo (int gpio, int angle)
{

uint32_t on_pulse_width = (uint32_t(angle)*(SERVO_PULSE_MAX-SERVO_PULSE_MIN))/MAX_ANGLE+SERVO_PULSE_MIN;
uint32_t off_pulse_width = SERVO_PERIOD-on_pulse_width;
gpio_write(gpio,ON);
delay_us(on_pulse_width);
gpio_write(gpio,OFF);
delay_ms(off_pulse_width/1000);
delay_us(off_pulse_width % 1000);

return;
}

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

//setup gear change 
gear_change_setup();
gear_display_setup();

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
  static bool drive_gear;
  drive_gear = gear_change();

  if (updateServo) {
      updateServo = false;

      angleIndex++;

      if (angleIndex >= 11) {
          angleIndex = 0;
      }
  }

/*
char buffer[20];
sprintf(buffer, "Angle: %d", angle_values[angleIndex]);
ser_printline(buffer);
*/

  if(drive_gear){
    servo(GPIO_6, angle_values[angleIndex]);
  }

  if (dist >= distmax || !drive_gear) {
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
}

