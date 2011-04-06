#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>

#define TIC()\
	PORTC |= 8;\
	PORTC &= 0xf7;\
	_delay_ms(1);
	
#define SH_LEFT 0x10
#define SH_RIGHT 0x14

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
	PORTC &= 0xfc; // RS=R/W=0
	PORTD = 0x38; //interfejs 8bit, 2 wiersze, znaki 5x7
	
	TIC()
}

void data_write(char data){
	PORTC |= 1; // RS=1
	PORTC &= 0xfd; // R/W=0
	PORTD = data;
	
	TIC()
}

/**
 *@param direction should be SH_LEFT or SH_RIGHT
 */
void shift_cursor(short direction){
	PORTD = direction;
	PORTC &= 0xfc; // RS=R/W=0
	
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
	DDRC = 0x0f;
	PORTC = 0x30;
	DDRB = 0x00;
	PORTB = 0x02;
	
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
	data_write('a');
	shift_cursor(SH_LEFT);
	
	short offset = 0;
	
	while(1){
		if(!(PINC & 0x20)){
			
			++offset;
			data_write('a'+offset);
			shift_cursor(SH_LEFT);
			
			while(!(PINC & 0x20)){}
			_delay_ms(50);
		}
		if(!(PINC & 0x10)){
			
			--offset;
			data_write('a'+offset);
			shift_cursor(SH_LEFT);
			
			while(!(PINC & 0x10)){}
			_delay_ms(50);
		}
		if(!(PINB & 0x2)){
			
			shift_cursor(SH_RIGHT);
			data_write('a');
			shift_cursor(SH_LEFT);
			offset = 0;
			while(!(PINB & 0x2)){}
			_delay_ms(50);
		}
	}
	
	/*while(1){
		PORTC |= 4;
		_delay_ms(660);
		PORTC &= 255-4;
		_delay_ms(660);
	}*/
}