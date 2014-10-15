// Encoder02.cpp


#include <Arduino.h>
#include "A1Main.h"
#include <util/atomic.h>

#ifdef USE_ENCODER02

#define NO_PORTD_PINCHANGES
#define NO_PORTB_PINCHANGES
#define NO_PORTC_PINCHANGES

// One of the following "NO_PORTX_PINCHANGES" will be Undefined, Note: ENC_A_PIN must be a number.
#if defined (ENC_A_PIN)
    #if (ENC_A_PIN) >= 0 && (ENC_A_PIN) <= 7
        #undef NO_PORTD_PINCHANGES
    #endif
    #if (ENC_A_PIN) >= 8 && (ENC_A_PIN) <= 13
        #undef NO_PORTB_PINCHANGES
    #endif
    #if (ENC_A_PIN) >= 14 && (ENC_A_PIN) <= 19
        #undef NO_PORTC_PINCHANGES
    #endif
#endif


#define NO_PIN_STATE
#define NO_PIN_NUMBER
#define DISABLE_PCINT_MULTI_SERVICE

#include "PinChangeInt.h"

#include "Encoder02.h"
#include "debug.h"

volatile int knob;


// ###############################################################################
// ###############################################################################
void encoderISR() {
    int pin = ENC_B_PIN;
    static unsigned long startTime = 0;
    unsigned long tigermillis;
  
    uint8_t oldSREG = SREG;

    cli();
    tigermillis = millis();
    SREG = oldSREG;
    
    if (tigermillis-startTime <= ISR_DEBOUNCE_TIME_OUT) return;
    startTime=tigermillis;
    
    knob += digitalRead(ENC_B_PIN) ? -1 : +1;
    
    //debug("%s/%d: Pin= %d, Knob= %d", __func__, __LINE__, pin, knob);
}


// ###############################################################################
void initEncoder() {
    int pin = ENC_A_PIN;
     
    //debug("%s/%d: Pin= %d", __func__, __LINE__, pin);
    
    pinMode(ENC_A_PIN, INPUT_PULLUP);
    pinMode(ENC_B_PIN, INPUT_PULLUP);
    
    #ifdef USE_ENCODER02
        PCintPort::attachInterrupt(pin, &encoderISR, FALLING);
    #endif // USE_ENCODER02
}


// ###############################################################################
int getEncoderDir() {
    char tmp = knob;
      
    if (tmp>0) {uint8_t oldSREG = SREG; cli(); knob--; SREG = oldSREG; return +1;}
    if (tmp<0) {uint8_t oldSREG = SREG; cli(); knob++; SREG = oldSREG; return -1;}
    return 0;
} 

#endif // USE_ENCODER02
// End