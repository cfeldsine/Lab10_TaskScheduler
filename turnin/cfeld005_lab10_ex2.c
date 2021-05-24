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
    case '1': keypadOutput = 0x01; break; //Hex equivalent
    case '2': keypadOutput = 0x02; break;
    case '3': keypadOutput = 0x03; break;
    case '4': keypadOutput = 0x04; break;
    case '5': keypadOutput = 0x05; break;
    case '6': keypadOutput = 0x06; break;
    case '7': keypadOutput = 0x07; break;
    case '8': keypadOutput = 0x08; break;
    case '9': keypadOutput = 0x09; break;
    case 'A': keypadOutput = 0x0A; break;
    case 'B': keypadOutput = 0x0B; break;
    case 'C': keypadOutput = 0x0C; break;
    case 'D': keypadOutput = 0x0D; break;
    case '*': keypadOutput = 0x0E; break;
    case '0': keypadOutput = 0x00; break;
    case '#': keypadOutput = 0x0F; break;
    default: keypadOutput = 0x1B; break; //Should never occur. Middle LED off.
  }
  return state;
}

enum combination_States{combination_init, combination_wait, combination_p1, combination_w1, combination_p2, combination_w2, combination_p3, combination_w3, combination_p4, combination_w4, combination_p5, combination_w5, combination_failure, combination_pnum};

int combinationSMTick(int state){
  switch (state) {
    case combination_init:
      PORTA = 0xFF;
      state = combination_wait;
      break;
    case combination_wait:
      PORTA = 0x01;
      if (keypadOutput ==0x01){
        state = combination_p1;
      } else if (keypadOutput == 0x1F) {
        state = combination_wait;
      } else {
        state = combination_failure;
      }
      break;
    case combination_p1:
      PORTA = 0x02;
      if (keypadOutput ==0x01){
        state = combination_p1;
      } else {
        state = combination_w1;
      }
      break;
    case combination_w1:
      PORTA = 0x03;
      if (keypadOutput ==0x02){
        state = combination_p2;
      } else if (keypadOutput == 0x1F) {
        state = combination_w1;
      } else {
        state = combination_failure;
      }
      break;
    case combination_p2:
      PORTA = 0x04;
      if (keypadOutput ==0x02){
        state = combination_p2;
      } else {
        state = combination_w2;
      }
      break;
    case combination_w2:
      PORTA = 0x05;
      if (keypadOutput ==0x03){
        state = combination_p3;
      } else if (keypadOutput == 0x1F) {
        state = combination_w2;
      } else {
        state = combination_failure;
      }
      break;
    case combination_p3:
      PORTA = 0x06;
      if (keypadOutput ==0x03){
        state = combination_p3;
      } else {
        state = combination_w3;
      }
      break;
    case combination_w3:
      PORTA = 0x07;
      if (keypadOutput ==0x04){
        state = combination_p4;
      } else if (keypadOutput == 0x1F) {
        state = combination_w3;
      } else {
        state = combination_failure;
      }
      break;
    case combination_p4:
      PORTA = 0x08;
      if (keypadOutput ==0x04){
        state = combination_p4;
      } else {
        state = combination_w4;
      }
      break;
    case combination_w4:
      PORTA = 0x09;
      if (keypadOutput ==0x05){
        state = combination_p5;
      } else if (keypadOutput == 0x1F) {
        state = combination_w4;
      } else {
        state = combination_failure;
      }
      break;
    case combination_p5:
      PORTA = 0x10;
      if (keypadOutput ==0x05){
        state = combination_p5;
      } else {
        state = combination_w5;
      }
      break;
    case combination_w5:
      PORTA = 0x11;
      if (keypadOutput == 0x1F) {
        state = combination_w5;
      } else {
        state = combination_failure;
      }
      break;
    case combination_failure:
      PORTA = 0x12;
      if (keypadOutput == 0x0F) {
        state = combination_pnum;
      } else {
        state = combination_failure;
      }
      break;
    case combination_pnum:
      PORTA = 0x13;
      if (keypadOutput == 0x0F) {
        state = combination_pnum;
      } else {
        state = combination_wait;
      }
      break;
    default:
      PORTA = 0x14;
      break;
  }


  switch (state) {
    case combination_p5:
      lock = 1;
      break;
    case combination_w5:
      lock = 1;
      break;
    default:
      lock = 0;
      break;
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
      output = lock;
      break;
  }

  PORTB = output;
  return state;
}

int main(void) {
    DDRA = 0xFF; PORTA = 0x00;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;

    static task task1, task2, task3;

    task* tasks[] = {&task1, &task2, &task3};

    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;

    task1.state = start;
    task1.period = 20;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadSMTick;

    task2.state = combination_init;
    task2.period = 20;
    task2.elapsedTime = task2.period;
    task2.TickFct = &combinationSMTick;

    task3.state = start;
    task3.period = 20;
    task3.elapsedTime = task3.period;
    task3.TickFct = &displaySMTick;


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
