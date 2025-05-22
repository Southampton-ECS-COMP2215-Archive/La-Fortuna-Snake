/*       -*- mode: c;  -*-        */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "os.h"
#include "main.h"

int step;
const rectangle start_head = {50, 60, 50, 60};
const rectangle score_cover = {80, 180, 0, 10};
const rectangle end_game = {100, 220, 80, 140};
const rectangle menu = {0, 320, 0, 10};
rectangle head, last_head;
int direction;
int obs [24][32];
struct Point tail [max_length];
int tail_tail, tail_head;
int grow;
unsigned int goalx, goaly;
int score;
char* scorestring;
char scorebuffer [5];
FIL File;
char highscore[5];

void get_high_score(){
	f_mount(&FatFs, "", 0);
	cli();
	if (f_open(&File, "high.txt", FA_WRITE | FA_READ | FA_OPEN_ALWAYS) == FR_OK) {
		f_gets(&highscore[0], 5, &File);
		if(atoi(highscore) < score){
			f_lseek(&File, 0);
			f_printf(&File, scorestring);
		}
	}else{
		display_string("Could not read high score");
	}
	f_close(&File);
	sei();
	
	fill_rectangle(end_game, NAVY);
	display_string_xy("High Score:", 110, 90);
	display_string_xy("Your Score:", 110, 125);
	if(atoi(highscore) < score){
		display_string_xy(scorestring, 200, 90);
	}else{
		display_string_xy(highscore, 200, 90);
	}
	display_string_xy(scorestring, 200, 125);
}

void new_goal(void){
	score += 1;
	scorestring = itoa(score, scorebuffer, 10);
	fill_rectangle(score_cover, bck_col);
	display_string_xy(scorestring, 80, 0);	
	
	grow = 1;
	
	goalx = rand() % 32;
	goaly = (rand() % 23) + 1;
	
	while (obs[goaly][goalx] != 0){
		goalx = rand() % 32;
		goaly = (rand() % 23) + 1;
	}
	
	rectangle goal = {goalx*10, goalx*10 + block_width, goaly*10, goaly*10 + block_width};
	fill_rectangle(goal, YELLOW);
	
	if (score > 5){
		
		if(score % 2 == 0){
			unsigned int badx, bady;
			
			badx = rand() % 32;
			bady = (rand() % 23) + 1;
	
			while (obs[bady][badx] != 0 || (goalx == badx && goaly == bady)){
				badx = rand() % 32;
				bady = (rand() % 23) + 1;
			}
	
			rectangle bad = {badx*10, badx*10 + block_width, bady*10, bady*10 + block_width};
			fill_rectangle(bad, RED);
			obs[bady][badx] = 1;
		}
	}
}

void repaint_tail(void){
	cli();
	rectangle tailend = {(tail[tail_tail].x), (tail[tail_tail].x)+block_width, (tail[tail_tail].y), (tail[tail_tail].y)+block_width};
	
	struct Point newPoint;
	newPoint.x = head.left;
	newPoint.y = head.top;
	tail[tail_head] = newPoint;
		
	fill_rectangle(last_head, tail_col);
	fill_rectangle(head, head_col);
	fill_rectangle(tailend, bck_col);
	
	obs[tail[tail_tail].y/10][tail[tail_tail].x/10] = 0;
	
	if(tail_head < max_length -1){
		tail_head += 1;
	}else{
		tail_head = 0;
	}
	
	if(grow == 0){
		if(tail_tail < max_length -1){
			tail_tail += 1;
		}else{
			tail_tail = 0;
		}
	}else{
		grow = 0;
	}
	sei();
}

void check_goal(){
	if(head.top == goaly*10){
		if(head.left == goalx*10){
			new_goal();
		}
	}
}

int check_collisions(){
	if(obs[head.top/10][head.left/10] == 1){
		step = 0;
		fill_rectangle(head, RED);
		get_high_score();
		return 1;
	}else{
		return 0;
	}
}

int move_snake(int){

	if (direction == 2) {
		last_head = head;
		if(head.top < 230){
			head.top +=step;
			head.bottom +=step;
		}else{
			head.top = 10;
			head.bottom = 20;
		}
	}else if (direction == 0) {
		last_head = head;
		if(head.top > 10){
			head.top -=step;
			head.bottom -=step;
		}else{
			head.top = 230;
			head.bottom = 240;
		}
	}else if (direction == 1) {
		last_head = head;
		if(head.right < 320){
			head.right +=step;
			head.left +=step;
		}else{
			head.right = 10;
			head.left = 0;
		}
	}else if (direction == 3) {
		last_head = head;
		if(head.right > 10){
			head.right -=step;
			head.left -=step;
		}else{
			head.right = 320;
			head.left = 310;
		}		
	}
	if(step > 0){
		repaint_tail();
		check_goal();
		check_collisions();
		obs[head.top/10][head.left/10] = 1;
	}
	return 0;
}

void reset_game(void){
	cli();
	last_head = start_head;
	head = start_head;
	
	int i, j;
	
	for( i = 0; i < 32; i++){
		for( j = 0; j < 24; j++){
			obs[j][i] = 0;
		}
	}
	
	clear_screen();
	display_string_xy("SCORE:", 30, 0);
	new_goal();
	score = 0;
	scorestring = "0";
	fill_rectangle(score_cover, bck_col);
	display_string_xy(scorestring, 80, 0);	
	direction = 1;
	grow = 1;
	step = 10;
	tail_tail = 0;
	tail_head = 1;
	sei();
}

int check_switches(int state) {
	if (get_switch_press(_BV(SWC))) {
		reset_game();
		repaint_tail();
	}
	if (get_switch_press(_BV(SWS))) {
		if(direction != 0){
			direction = 2;
		}
	}
	if (get_switch_press(_BV(SWN))) {
		if(direction !=2){
			direction = 0;
		}
	}
	if (get_switch_press(_BV(SWE))) {
		if (direction !=3){
			direction = 1;
		}
	}
	if (get_switch_press(_BV(SWW))) {
		if (direction !=1){
			direction = 3;
		}
	}
	return state;
}

int main(void) {

	os_init();
	set_frame_rate_hz(61); /* > 60 Hz  (KPZ 30.01.2015) */

	os_add_task( check_switches,  100, 1);
	os_add_task( move_snake, 100, 1);
	
	display_color(YELLOW, bck_col);
	reset_game();
	
	sei();
    for(;;){}
	return 0;
}
