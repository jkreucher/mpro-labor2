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
const uint8_t patternWarning[] = {0xF, 0x0};
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
        int8_t blink_counter;
    public:
        Blinker(BusOut* left, BusOut* right) {
            // initialize leds
            ledsLeft = left;
            ledsRight = right;
            mode = blinkerStop;
            index = 0;
            blink_counter = -1;
            // start local timer
            timer.start();
        }
        uint8_t isBlinking() {
            return mode;
        }
        void stop() {
            // finishes current blink pattern and stops when blink_counter gets decremented
            blink_counter = 1;
        }
        void blinkLeft(int8_t count=-1) {
            mode = blinkerLeft;
            blink_counter = count;
        }
        void blinkRight(int8_t count=-1) {
            mode = blinkerRight;
            blink_counter = count;
        }
        void warning() {
            mode = blinkerWarning;
            blink_counter = -1;
        }
        void loop() {
            if(timer.elapsed_time() >= chrono::milliseconds(patternBlinkTime)) {
                timer.reset();
                // check blink mode
                if(mode == blinkerStop) {
                    // off
                    index = 0;
                } else if(mode == blinkerLeft) {
                    // blink left
                    *ledsRight = 0;
                    *ledsLeft = patternBlinker[index];
                    index++;
                    // check if end of pattern reached
                    if(index >= sizeof(patternBlinker)) {
                        index = 0;
                        // decrement blinker counter
                        blink_counter--;
                    }
                } else if(mode == blinkerRight) {
                    // blink right
                    *ledsLeft = 0;
                    *ledsRight = patternBlinker[index];
                    index++;
                    // check if end of pattern reached
                    if(index >= sizeof(patternBlinker)) {
                        index = 0;
                        // decrement blinker counter
                        blink_counter--;
                    }
                } else if(mode == blinkerWarning) {
                    // blink both
                    *ledsLeft = *ledsRight = patternWarning[index];
                    index++;
                    // check if end of pattern reached
                    if(index >= sizeof(patternWarning)) index = 0;
                }
                // manage blink counter
                if(blink_counter == 0) {
                    mode = blinkerStop;
                } else if(blink_counter < 0) {
                    blink_counter = -1;
                }
            }
        }
};




enum BlinkerEvent {EventNone, EventStop, EventShortLeft, EventLongLeft, EventShortRight, EventLongRight};

class BlinkerInput {
    private:
        DigitalIn *buttonLeft, *buttonRight;
        uint8_t leftPressed, rightPressed;
        Timer timer;
    public:
        BlinkerInput(DigitalIn *left, DigitalIn *right) {
            buttonLeft = left;
            buttonRight = right;
            leftPressed = 0;
            rightPressed = 0;
        }
        uint8_t check_event() {
            // set default event to none
            uint8_t event = EventNone;
            // check which button is pressed
            if((*buttonLeft == 1) && (leftPressed == 0)) {
                leftPressed = 1;
                // left button pressed
                timer.reset();
                timer.start();
            } else if((*buttonLeft == 0) && (leftPressed == 1)) {
                leftPressed = 0;
                // left button released
                if(timer.elapsed_time() < chrono::milliseconds(600)) {
                    // short press
                    event = EventShortLeft;
                } else {
                    // long press
                    event = EventLongLeft;
                }
                // stop timer
                timer.stop();
            } else if((*buttonRight == 1) && (rightPressed == 0)) {
                rightPressed = 1;
                // right button pressed
                timer.reset();
                timer.start();
            } else if((*buttonRight == 0) && (rightPressed == 1)) {
                rightPressed = 0;
                // right button released
                if(timer.elapsed_time() < chrono::milliseconds(600)) {
                    // short press
                    event = EventShortRight;
                } else {
                    // long press
                    event = EventLongRight;
                }
            }
            // return current event
            return event;
        }
};




/*** MAIN FUNCTION ***/
int main() {

    Blinker car_blinker(&ledsLeft, &ledsRight);
    BlinkerInput car_input(&buttonLeft, &buttonRight);

    // main loop
    while(1) {
        // check buttons
        uint8_t buttonEvent = car_input.check_event();
        // check event
        switch(buttonEvent) {
            case EventNone:
                break;
            case EventStop:
                car_blinker.stop();
                break;
            case EventShortLeft:
                car_blinker.blinkLeft(4);
                break;
            case EventLongLeft:
                car_blinker.blinkLeft();
                break;
            case EventShortRight:
                car_blinker.blinkRight(4);
                break;
            case EventLongRight:
                car_blinker.blinkRight();
                break;
        }
        // led loop
        car_blinker.loop();
    }
}

