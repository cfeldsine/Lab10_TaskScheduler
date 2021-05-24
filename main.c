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

unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;
unsigned char keypadOutput;

// enum pauseButtonSM_States {pauseButton_wait, pauseButton_press, pauseButton_release};
//
// int pauseButtonSMTick(int state) {
//   unsigned char press = ~PINA & 0x01;
//
//   switch (state) {
//     case pauseButton_wait:
//       state = press == 0x01? pauseButton_press: pauseButton_wait; break;
//     case pauseButton_press:
//       state = pauseButton_release; break;
//     case pauseButton_release:
//       state = press == 0x00? pauseButton_wait: pauseButton_press; break;
//     default: state = pauseButton_wait; break;
//   }
//
//   switch (state) {
//     case pauseButton_wait: break;
//     case pauseButton_press:
//       pause = (pause == 0)? 1:0;
//       break;
//     case pauseButton_release: break;
//   }
//   return state;
// }
//
// enum toggleLED0_State {toggleLED0_wait, toggleLED0_blink};
//
// int toggleLED0SMTick(int state){
//   switch (state) {
//     case toggleLED0_wait: state = !pause? toggleLED0_blink: toggleLED0_wait; break;
//     case toggleLED0_blink: state = pause? toggleLED0_wait: toggleLED0_blink; break;
//     default: state = toggleLED0_wait; break;
//   }
//
//   switch (state) {
//     case toggleLED0_wait: break;
//     case toggleLED0_blink:
//       led0_output = (led0_output == 0x00)? 0x01 : 0x00;
//       break;
//   }
//   return state;
// }
//
// enum toggleLED1_State {toggleLED1_wait, toggleLED1_blink};
//
// int toggleLED1SMTick(int state){
//   switch (state) {
//     case toggleLED1_wait: state = !pause? toggleLED1_blink: toggleLED1_wait; break;
//     case toggleLED1_blink: state = pause? toggleLED1_wait: toggleLED1_blink; break;
//     default: state = toggleLED1_wait; break;
//   }
//
//   switch (state) {
//     case toggleLED1_wait: break;
//     case toggleLED1_blink:
//       led1_output = (led1_output == 0x00)? 0x01 : 0x00;
//       break;
//   }
//   return state;
// }


enum keypad_States {keypad_INIT, keypad_WAIT, keypad_1, keypad_2, keypad_3, keypad_4, keypad_5, keypad_6, keypad_7, keypad_8, keypad_9, keypad_0, keypad_num, keypad_star, keypad_A, keypad_B, keypad_C, keypad_D};

int keypadSMTick(int state){
  unsigned char input = GetKeypadKey();
  switch (input) {
    case '\0': state = keypad_WAIT; break; //All 5 LEDS On
    case '1': state = keypad_1; break; //Hex equivalent
    case '2': state = keypad_2; break;
    case '3': state = keypad_3; break;
    case '4': state = keypad_4; break;
    case '5': state = keypad_5; break;
    case '6': state = keypad_6; break;
    case '7': state = keypad_7; break;
    case '8': state = keypad_8; break;
    case '9': state = keypad_9; break;
    case 'A': state = keypad_A; break;
    case 'B': state = keypad_B; break;
    case 'C': state = keypad_C; break;
    case 'D': state = keypad_D; break;
    case '*': state = keypad_star; break;
    case '0': state = keypad_0; break;
    case '#': state = keypad_num; break;
  }

  switch(state){
    case keypad_WAIT: keypadOutput = 0x1F; break; //All 5 LEDS On
    case keypad_1: keypadOutput = 0x01 | 0x80; break; //Hex equivalent
    case keypad_2: keypadOutput = 0x02 | 0x80; break;
    case keypad_3: keypadOutput = 0x03 | 0x80; break;
    case keypad_4: keypadOutput = 0x04 | 0x80; break;
    case keypad_5: keypadOutput = 0x05 | 0x80; break;
    case keypad_6: keypadOutput = 0x06 | 0x80; break;
    case keypad_7: keypadOutput = 0x07 | 0x80; break;
    case keypad_8: keypadOutput = 0x08 | 0x80; break;
    case keypad_9: keypadOutput = 0x09 | 0x80; break;
    case keypad_A: keypadOutput = 0x0A | 0x80; break;
    case keypad_B: keypadOutput = 0x0B | 0x80; break;
    case keypad_C: keypadOutput = 0x0C | 0x80; break;
    case keypad_D: keypadOutput = 0x0D | 0x80; break;
    case keypad_star: keypadOutput = 0x0E | 0x80; break;
    case keypad_0: keypadOutput = 0x00 | 0x80; break;
    case keypad_num: keypadOutput = 0x0F | 0x80; break;
    default: PORTB = 0x1B; break; //Should never occur. Middle LED off.
  }

  return state;
}


enum display_States {display_display};

int displaySMTick(int state) {
  unsigned char output;

  switch (state) {
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
  DDRA = 0x00; PORTA = 0xFF;
  DDRB = 0xFF; PORTB = 0x00;

  static task task1, task2;
  task *tasks[] = {&task1, &task2};
  const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

  const char start = -1;

  task1.state = start;
  task1.period = 50;
  task1.elapsedTime = task1.period;
  task1.TickFct = &keypadSMTick;

  task2.state = start;
  task2.period = 10;
  task2.elapsedTime = task2.period;
  task2.TickFct = &displaySMTick;

  unsigned short i;

  unsigned long GCD = tasks[0]->period;
  for (i=1;i<numTasks;i++){
    GCD = findGCD(GCD, tasks[i]->period);
  }

  TimerSet(GCD);
  TimerOn();

  while (1){
    for (i=0; i<numTasks; i++) {
      if (tasks[i]->elapsedTime == tasks[i]->period) {
        tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
        tasks[i]->elapsedTime = 0;
      }
      tasks[i]->elapsedTime += 10;
    }
    while (!TimerFlag);
    TimerFlag = 0;
  }
  return 0;
}
