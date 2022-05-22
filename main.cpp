/* PROJECT:  MPRO Laboraufgabe 2
 *
 * DEVELOPERS: Jannik Kreucher
 *             Nick Diendorf
 *             Benedikt Wendling
 */
#include "mbed.h"
#include <chrono>
#include <cstdint>


/*** PIN DEFINITIONS ***/
// Nucleo On-Board
#define LED_NUCLEO  PA_5
#define SW_NUCLEO   PC_13
// Velleman STEM Shield I/O
#define LED_RED1    PB_0
#define LED_ORANGE1 PC_1
#define LED_GREEN1  PA_10
#define LED_RED2    PB_3
#define LED_ORANGE2 PB_5
#define LED_GREEN2  PB_4
#define SW_1    PA_9
#define SW_2    PC_7
#define SW_3    PB_6
#define SW_4    PA_7
#define SW_5    PA_6


// blink patterns
const uint8_t patternBlinker[] = {0x1, 0x2, 0x4, 0x0, 0x0, 0x0};
const uint8_t patternWarning[] = {0xF, 0xF, 0xF, 0x0, 0x0, 0x0};
uint16_t patternBlinkTime = 100;

// output definition
BusOut ledsLeft(LED_RED1, LED_ORANGE1, LED_GREEN1);
BusOut ledsRight(LED_RED2, LED_ORANGE2, LED_GREEN2);

// input definition
DigitalIn buttonLeft(SW_5);
DigitalIn buttonRight(SW_2);
DigitalIn buttonWarning(SW_1);




enum BlinkerState {blinkerStop, blinkerLeft, blinkerRight, blinkerWarning};

class Blinker {
    private:
        BusOut *ledsLeft, *ledsRight;
        Timer timer;
        uint8_t mode, index;
        int8_t sequenceCounter;
    public:
        Blinker(BusOut* left, BusOut* right) {
            // initialize leds
            ledsLeft = left;
            ledsRight = right;
            mode = blinkerStop;
            index = 0;
            sequenceCounter = -1;

            // start local timer
            timer.start();
        }

        uint8_t getMode() {
            return mode;
        }

        void stop() {
            mode = blinkerStop;
        }

        void finishAndStop() {
            // finishes current blink pattern and stops when blink_counter gets decremented
            sequenceCounter = 1;
        }

        void blinkLeft(int8_t count=-1) {
            mode = blinkerLeft;
            sequenceCounter = count;
        }

        void blinkRight(int8_t count=-1) {
            mode = blinkerRight;
            sequenceCounter = count;
        }

        void warning() {
            mode = blinkerWarning;
            sequenceCounter = -1;
        }

        void loop() {
            if(timer.elapsed_time() >= chrono::milliseconds(patternBlinkTime)) {
                timer.reset();

                // check blink mode
                if(mode == blinkerStop) {
                    // off
                    *ledsRight = 0;
                    *ledsLeft = 0;
                    index = 0;
                } else {
                    switch(mode) {
                        case blinkerLeft:
                            *ledsRight = 0;
                            *ledsLeft = patternBlinker[index];
                            break;
                        case blinkerRight:
                            *ledsLeft = 0;
                            *ledsRight = patternBlinker[index];
                            break;
                        case blinkerWarning:
                            *ledsLeft = *ledsRight = patternWarning[index];
                    }

                    // increment index and if index greater than pattern size than set to zero and decrement sequence number
                    index++;
                    if(index >= sizeof(patternBlinker)) {
                        index = 0;
                        sequenceCounter--;
                    }
                }

                // manage blink counter
                if(sequenceCounter == 0) {
                    mode = blinkerStop;
                } else if(sequenceCounter < 0) {
                    sequenceCounter = -1;
                }
            }
        }
};


enum BlinkerEvent {EventNone, EventStop, EventLeftPressed, EventLeftLong, EventLeftReleased, EventRightPressed, EventRightLong, EventRightReleased};

class BlinkerInput {
    private:
        DigitalIn *buttonLeft, *buttonRight, *buttonWarning;
        uint8_t leftPressed, rightPressed, warningSwitched;
        Timer timer;
    public:
        BlinkerInput(DigitalIn *left, DigitalIn *right, DigitalIn *warning) {
            buttonLeft = left;
            buttonRight = right;
            buttonWarning = warning;
            leftPressed = 0;
            rightPressed = 0;
        }

        uint8_t checkEvent() {
            // set default event to none
            uint8_t event = EventNone;

            // check which button is pressed

            // LEFT BUTTON        
            if((*buttonLeft == 1) && (leftPressed == 0)) {
                // left button pressed
                leftPressed = 1;
                timer.reset();
                timer.start();
                event = EventLeftPressed;
            } else if((*buttonLeft == 0) && (leftPressed == 1)) {
                 // left button released
                leftPressed = 0;
                event = EventLeftReleased;
                // stop timer
                timer.stop();
            } else if((leftPressed == 1) && (timer.elapsed_time() >= chrono::milliseconds(600))) {
                // continuous mode
                event = EventLeftLong;
            }
            
            // RIGHT BUTTON
            else if((*buttonRight == 1) && (rightPressed == 0)) {
                // right button pressed
                rightPressed = 1;
                timer.reset();
                timer.start();
                event = EventRightPressed;
            } else if((*buttonRight == 0) && (rightPressed == 1)) {
                // right button released
                rightPressed = 0;
                event = EventRightReleased;
                // stop timer
                timer.stop();
            } else if((rightPressed == 1) && (timer.elapsed_time() >= chrono::milliseconds(600))) {
                // continuous mode
                event = EventRightLong;
            }

            // return current event
            return event;
        }
};




enum ButtonState {StateNone, StateLeftShort, StateLeftLong, StateRightShort, StateRightLong};

/*** MAIN FUNCTION ***/
int main() {

    Blinker carBlinker(&ledsLeft, &ledsRight);
    BlinkerInput carInput(&buttonLeft, &buttonRight, &buttonWarning);

    uint8_t buttonState = StateNone; // long or short press

    // main loop
    while(1) {
        // check buttons
        uint8_t buttonEvent = carInput.checkEvent();

        // check event
        switch(buttonEvent) {
            case EventNone:
                break;
            
            case EventStop:
                carBlinker.stop();
                break;
            
            // handle left button events
            case EventLeftPressed:
                if(buttonState == StateRightShort) {
                    carBlinker.stop();
                    buttonState = StateNone;
                } else {
                    carBlinker.blinkLeft(4);
                    buttonState = StateLeftShort;
                }
                break;
            
            case EventLeftReleased:
                if(buttonState == StateLeftLong) {
                    carBlinker.finishAndStop();
                }
                break;
            
            case EventLeftLong:
                buttonState = StateLeftLong;
                carBlinker.blinkLeft();
                break;
            
            // handle right button events
            case EventRightPressed:
                if(buttonState == StateLeftShort) {
                    carBlinker.stop();
                    buttonState = StateNone;
                } else {
                    carBlinker.blinkRight(4);
                    buttonState = StateRightShort;
                }
                break;
            
            case EventRightReleased:
                if(buttonState == StateRightLong) {
                    carBlinker.finishAndStop();
                }
                break;
            
            case EventRightLong:
                buttonState = StateRightLong;
                carBlinker.blinkRight();
        }

        // reset button state when nothing is blinking
        if(carBlinker.getMode() == blinkerStop) {
            buttonState = StateNone;
        }

        // led loop
        carBlinker.loop();

        // debounce delay
        ThisThread::sleep_for(10ms);
    }
}

