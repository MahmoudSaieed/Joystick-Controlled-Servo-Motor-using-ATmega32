#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "lcd.h"
#include <string.h>

#define SERVO_PIN PD5

// --------- Constants ---------
#define SERVO_MIN     520         // Pulse width for -90° (0.52ms)
#define SERVO_CENTER  1400        // Pulse width for 0° (1.4ms)
#define SERVO_MAX     2400        // Pulse width for +90° (2.4ms)
#define ADC_MAX       1023        // 10-bit ADC resolution

// --------- ADC Setup ---------
void initADC() {
    DDRA = 0x00;                  // Make ADC port as input
    ADCSRA = 0x87;                // Enable ADC, prescaler 128
    ADMUX = 0x00;                 // Select ADC0, AVCC ref
}

uint16_t readADC(uint8_t channel) {
    ADMUX = 0x40 | (channel & 0x07); // Select ADC channel, AVCC ref
    ADCSRA |= (1 << ADSC);          // Start conversion
    while (!(ADCSRA & (1 << ADIF))); // Wait for conversion complete
    ADCSRA |= (1 << ADIF);          // Clear flag
    _delay_us(10);
    return ADCW;
}

// --------- PWM Setup ---------
void initPWM() {
    DDRD |= (1 << SERVO_PIN);    // PD4 (OC1B) as output

    // Fast PWM Mode 14: ICR1 is TOP, non-inverting on OC1B
    TCCR1A = (1 << WGM11) | (1 << COM1A1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler = 8

    ICR1 = 19999;                // TOP value for 20ms (50Hz) period
    OCR1A = SERVO_CENTER;        // Start at center position
}

// --------- Main ---------
int main(void) {
    char buffer[16];
    initADC();
    initPWM();
    LCD_init();

    while (1) {
        uint16_t adcValue = readADC(0);  // Read joystick X-axis on ADC0

        // Determine direction
        char direction[2];  // Short string for direction
        if (adcValue < 470) {
            strcpy(direction, "L");
        } else if (adcValue > 580) {
            strcpy(direction, "R");
        } else {
            strcpy(direction, "C");
        }

        // Inverted mapping to correct servo direction
        uint16_t pulseWidth = SERVO_MAX - ((uint32_t)adcValue * (SERVO_MAX - SERVO_MIN)) / ADC_MAX;

        // Clamp pulse width
        if (pulseWidth < SERVO_MIN) pulseWidth = SERVO_MIN;
        if (pulseWidth > SERVO_MAX) pulseWidth = SERVO_MAX;

        OCR1A = pulseWidth;  // Write to OCR1B (PD4)

        // LCD Display
        LCD_clear();

        LCD_setCursor(2, 0);
        snprintf(buffer, sizeof(buffer), "  PWM: %u", pulseWidth);
        LCD_print(buffer);

        LCD_setCursor(1, 0);
        snprintf(buffer, sizeof(buffer), "ADC:%u Dir:%s", adcValue, direction);
        LCD_print(buffer);

        _delay_ms(100);
    }
}