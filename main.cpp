/* PROJECT:  MPRO Laboraufgabe 2
 *
 * DEVELOPERS: Jannik Kreucher
 *             Nick Diendorf
 *             Benedikt Wendling
 */
#include "mbed.h"
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
            mode = blinkerStop;
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
        Timer timer;
    public:
        BlinkerInput(DigitalIn *left, DigitalIn *right) {
            buttonLeft = left;
            buttonRight = right;
        }
        uint8_t check_event() {
            return EventNone;
        }
};




/*** MAIN FUNCTION ***/
int main() {

    Blinker car_blinker(&ledsLeft, &ledsRight);
    BlinkerInput car_input(&buttonLeft, &buttonRight);

    Timer timer;
    timer.start();

    // main loop
    while (true) {
        if(timer.elapsed_time() >= chrono::milliseconds(5000)) {
            timer.reset();
            car_blinker.blinkLeft(2);
        }
        /*
        // check buttons
        uint8_t buttonEvent = car_input.check_event();
        // check event
        switch(buttonEvent) {
            case EventNone:
                break;
            case EventStop:
                car_blinker.stop();
                break;
            case EventLongLeft:
                car_blinker.blinkLeft();
                break;
            case EventLongRight:
                car_blinker.blinkRight();
                break;
        }
        */
        // led loop
        car_blinker.loop();
    }
}

