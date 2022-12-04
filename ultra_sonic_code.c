#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"

#define _XTAL_FREQ 8000000

// Set the pins for the ultrasonic sensor
#define TRIGGER_PIN PORTBbits.RB0
#define ECHO_PIN PORTBbits.RB1

// Set the maximum distance (in centimeters) that the sensor can measure
#define MAX_DISTANCE 200

// Set the timeout period (in microseconds) for the sensor
#define TIMEOUT_PERIOD 30000

// Function prototypes
void triggerSensor(void);
unsigned int readEcho(void);
unsigned int getDistance(void);

void main(void)
{
    // Initialize the TRIS registers
    TRISB = 0b00000010;

    // Initialize the UART module
    SPBRG = 51;
    TXSTA = 0b00100100;
    RCSTA = 0b10010000;
    BAUDCON = 0b00001000;

    // Enable interrupts
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;

    // Main loop
    while (true)
    {
        // Trigger the ultrasonic sensor and read the echo pulse
        triggerSensor();
        unsigned int echo_time = readEcho();

        // Calculate the distance based on the echo time
        unsigned int distance = getDistance(echo_time);

        // Print the distance to the UART
        printf("Distance: %dcm\r\n", distance);

        // Delay for 1 second
        __delay_ms(1000);
    }
}

// Function to trigger the ultrasonic sensor
void triggerSensor(void)
{
    // Set the trigger pin high for 10 microseconds
    TRIGGER_PIN = 1;
    __delay_us(10);
    TRIGGER_PIN = 0;
}

// Function to read the echo pulse from the sensor
unsigned int readEcho(void)
{
    // Wait for the echo pin to go high
    while (ECHO_PIN == 0)
        ;

    // Start the timer
    TMR1H = 0;
    TMR1L = 0;
    T1CONbits.TMR1ON = 1;

    // Wait for the echo pin to go low or for the timeout period to expire
    unsigned int timeout = 0;
    while (ECHO_PIN == 1)
    {
        if (timeout >= TIMEOUT_PERIOD)
        {
            break;
        }
        timeout++;
    }

    // Stop the timer and read the value
    T1CONbits.TMR1ON = 0;
    unsigned int timer_value = (TMR1H << 8) + TMR1L;

    // Return the timer value
    return timer_value;
}

// Function to calculate the distance based on the echo time
unsigned int getDistance(unsigned int echo_time)
{
    // Calculate the distance in centimeters
    unsigned int distance = (echo_time / 58);

    // Return the distance, limited to the maximum distance
    if (distance > MAX_DISTANCE)
    {
        return MAX_DISTANCE;
    }
    else
    {
        return distance;
    }
}
