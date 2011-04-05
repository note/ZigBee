#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>

#define TIC()\
	PORTC |= 8;\
	PORTC &= 0xf7;\
	_delay_ms(1);

/** PORTC od najmlodszego bitu:
 * RS
 * R/w
 * LED+
 * E */
 
/** PORTD od najmloszego bitu:
 * D0
 * D1
 * itd. */

void function_set(){
	PORTD = 0x38; //interfejs 8bit, 2 wiersze, znaki 5x7
	//PORTC &= 0xfe;
	
	TIC()
}

void data_write(char data){
	PORTC |= 1; // RS=1
	PORTC &= 0xfd; // R/W=0
	PORTD = data;
	
	TIC()
}

void shift_cursor(){
	PORTD = 0x14;
	PORTC &= 0xfe;
	
	TIC()
}

void display_on(){
	PORTD = 0xf;
	PORTC &= 0xfe;
	
	TIC()
}


void display_clear(){
	PORTC &= 0xfc; // RS=R/W=0
	PORTD = 1; //display clear
	
	TIC()
}

void set_entry_mode(){
	PORTC &= 0xfc; // RS=R/W=0
	PORTD = 6; // po zapisie kursor przesuwa sie w prawo
	
	TIC()
}

int main(void){
	//ustawienie portow
	DDRD = 0xff;
	DDRC = 0xf;
	
	display_on();
	
	display_clear();
	
	set_entry_mode();
	
	function_set();
	
	data_write('H');
	data_write('e');
	data_write('l');
	data_write('l');
	data_write('o');
	data_write(' ');
	data_write('w');
	data_write('o');
	data_write('r');
	data_write('l');
	data_write('d');
	
	PORTC |= 4;
	
	/*while(1){
		PORTC |= 4;
		_delay_ms(660);
		PORTC &= 255-4;
		_delay_ms(660);
	}*/
}