/*      Author: Cote Feldsine
 *  Partner(s) Name: none
 *      Lab Section: 023
 *      Assignment: Lab #10  Exercise #1
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "keypad.h"
#include "task.h"
#include "gcd.h"
#endif

unsigned char keypadOutput;
unsigned char lock;

enum keypad_States {keypad_init, keypad_wait, keypad_press};

int keypadSMTick(int state) {

  unsigned char x = GetKeypadKey();
  switch(x){
    case '\0': keypadOutput = 0x1F; break; //All 5 LEDS On
    case '1': keypadOutput = 0x81; break; //Hex equivalent
    case '2': keypadOutput = 0x82; break;
    case '3': keypadOutput = 0x83; break;
    case '4': keypadOutput = 0x84; break;
    case '5': keypadOutput = 0x85; break;
    case '6': keypadOutput = 0x86; break;
    case '7': keypadOutput = 0x87; break;
    case '8': keypadOutput = 0x88; break;
    case '9': keypadOutput = 0x89; break;
    case 'A': keypadOutput = 0x8A; break;
    case 'B': keypadOutput = 0x8B; break;
    case 'C': keypadOutput = 0x8C; break;
    case 'D': keypadOutput = 0x8D; break;
    case '*': keypadOutput = 0x8E; break;
    case '0': keypadOutput = 0x80; break;
    case '#': keypadOutput = 0x8F; break;
    default: keypadOutput = 0x1B; break; //Should never occur. Middle LED off.
  }
  return state;
}

enum display_States {display_display};

int displaySMTick(int state){
  unsigned char output;

  switch(state) {
    case display_display: state = display_display; break;
    default: state = display_display; break;
  }

  switch(state) {
    case display_display:
      output = keypadOutput;
      break;
  }

  PORTB = output;
  return state;
}

int main(void) {
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;

    static task task1, task2;

    task* tasks[] = {&task1, &task2};

    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;

    task1.state = start;
    task1.period = 20;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadSMTick;

    task2.state = start;
    task2.period = 20;
    task2.elapsedTime = task2.period;
    task2.TickFct = &displaySMTick;


    unsigned short i;

    unsigned long GCD = tasks[0]->period;
    for (i=1; i<numTasks; i++){
      GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();

    while(1) {

      for (i=0;i<numTasks;i++){
        if (tasks[i]->elapsedTime == tasks[i]->period) {
          tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
          tasks[i]->elapsedTime = 0;
        }
        tasks[i]->elapsedTime += GCD;
      }
      while (!TimerFlag);
      TimerFlag = 0;
    }
    return 0;
}
