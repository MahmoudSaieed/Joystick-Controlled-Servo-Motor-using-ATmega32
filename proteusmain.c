#define F_CPU 8000000UL         // Define CPU Frequency (8MHz)
#include <avr/io.h>             // Include AVR std library
#include <stdio.h>              // Include std library
#include <util/delay.h>         // Include delay header file
#include "lcd.h"                // Include LCD library
#define SERVO_PIN PD5

// --------- Constants ---------
#define SERVO_MIN     520         // Pulse width for -90° (0.52ms)
#define SERVO_CENTER  1400        // Pulse width for 0° (1.4ms)
#define SERVO_MAX     2400        // Pulse width for +90° (2.4ms)
#define ADC_MAX       1023        // 10-bit ADC resolution

// --------- ADC Setup ---------
void initADC() {
    DDRA = 0x00;            /* Make ADC port as input */
    ADCSRA = 0x87;          /* Enable ADC, with freq/128 */
    ADMUX = 0b00000000;
}

uint16_t readADC(uint8_t channel) {
    ADMUX = 0x40 | (channel & 0x07); /* set input channel to read */
    ADCSRA |= (1<<ADSC);             /* Start ADC conversion */
    while(!(ADCSRA & (1<<ADIF)));    /* Wait until end of conversion by polling ADC interrupt flag */
    ADCSRA |= (1<<ADIF);             /* Clear ADC interrupt flag */
    _delay_us(10);                   /* Wait a short time for stable reading */
    return (ADCW);
}

// --------- PWM Setup ---------
void initPWM() {
    DDRD |= (1 << SERVO_PIN);

    // Set Timer1 for Fast PWM, 10-bit resolution, non-inverting mode
    TCCR1A |= (1 << WGM11) | (1 << COM1A1);
    TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS10); // Clock source: No prescaling

    ICR1 = 19999;  // Set TOP value for 50Hz frequency

    OCR1A = SERVO_CENTER; // Initialize at the center for smoother initial movement
}

// --------- Main ---------
int main(void) {
    char buffer[16];
    initADC();             // Initialize ADC
    initPWM();             // Initialize PWM
    LCD_init();            // Initialize LCD

    while (1) {
        uint16_t adcValue = readADC(0);  // Read joystick X-axis on ADC0 (PA0)

        // Check ADC value to determine the direction
        char direction[3];  // 1 letter for direction
        if (adcValue < 470) {       // Left direction (0 - 341)
            strcpy(direction, "L");
        } else if (adcValue > 580) { // Right direction (682 - 1023)
            strcpy(direction, "R");
        } else {                    // Center (470 - 580)
            strcpy(direction, "C");
        }

        // Map ADC value to servo PWM range (SERVO_MIN–SERVO_MAX µs)
        uint16_t pulseWidth = SERVO_MIN + ((uint32_t)adcValue * (SERVO_MAX - SERVO_MIN)) / ADC_MAX;

        // Ensure pulse width stays within the defined range
        if (pulseWidth < SERVO_MIN) {
            pulseWidth = SERVO_MIN;  // Ensure pulse width does not go below SERVO_MIN
        } else if (pulseWidth > SERVO_MAX) {
            pulseWidth = SERVO_MAX;  // Ensure pulse width does not go above SERVO_MAX
        }

        // Set the servo position
        OCR1A = pulseWidth;

        // Display the direction on the first line
        LCD_clear();
        LCD_setCursor(2, 0);
        snprintf(buffer, sizeof(buffer), "PWM: %u" , pulseWidth);
        LCD_print(buffer);
        // Display the ADC and PWM values on the second line
        LCD_setCursor(1, 0);
        snprintf(buffer, sizeof(buffer), "ADC:%u Dir:%s", adcValue,direction);
        LCD_print(buffer);

        _delay_ms(100);  // Delay for smoother control loop
    }
}
