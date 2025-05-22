#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "os.h"

#define LED_ON      PORTB |=  _BV(PINB7)
#define LED_OFF     PORTB &= ~_BV(PINB7) 
#define LED_TOGGLE  PINB  |=  _BV(PINB7)

#define tail_col MEDIUM_ORCHID
#define head_col LAVENDER_BLUSH
#define bck_col CORNFLOWER_BLUE
#define block_width 10
#define max_length 256


struct Point {
    int x, y;
};

void new_goal();
void repaint_tail();
void check_goal();
int check_collisions();
int move_snake(int);
int check_switches(int state);
int main(void);
