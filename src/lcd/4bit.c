#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>

#define TIC()\
	PORTC |= 8;\
	PORTC &= 0xf7;\
	_delay_ms(1);
	
#define SH_LEFT 0x10
#define SH_RIGHT 0x14
#define LINE_LENGTH 16

/** PORTC od najmlodszego bitu:
 * RS
 * R/w
 * LED+
 * E */
 
/** PORTD od najmloszego bitu:
 * D0
 * D1
 * itd. */
 
void cursor_home(){
	PORTC &= 0xfc; // RS=R/W=0
	PORTD = 0;
	TIC()
	
	PORTD = 0x20;
	TIC()
}

void function_set(){
	PORTC &= 0xfc; // RS=R/W=0
	PORTD = 0x20; //interfejs 4bit
	TIC()
	
	PORTD = 0x80; //2 wiersze, znaki 5x7
	TIC()
}

void data_write(char data){
	PORTC |= 1; // RS=1
	PORTC &= 0xfd; // R/W=0
	PORTD = data;
	TIC()
	
	PORTD = PORTD << 4;
	TIC()
}

/**
 *@param direction should be SH_LEFT or SH_RIGHT
 */
void shift_cursor(uint8_t direction){
	PORTD = direction;
	PORTC &= 0xfc; // RS=R/W=0
	TIC()
	
	PORTD = PORTD << 4;
	TIC()
}

void display_on(){
	PORTD = 0;
	PORTC &= 0xfc;
	TIC()
	
	PORTD = 0x60;
	TIC();
}


void display_clear(){
	PORTC &= 0xfc; // RS=R/W=0
	PORTD = 0; //display clear
	TIC()
	
	PORTD = 0x10;
	TIC()
}

void set_entry_mode(){
	PORTC &= 0xfc; // RS=R/W=0
	PORTD = 0;
	TIC()
	
	PORTD = 0x60; // po zapisie kursor przesuwa sie w prawo
	TIC()
}

void shift_cursor_to(uint8_t pos){
	cursor_home();
	
	uint8_t i = 0;
	for(i=0; i<pos; ++i)
		shift_cursor(SH_RIGHT);
}

void clear_line(uint8_t line){
	shift_cursor_to(line*LINE_LENGTH+15);
	uint8_t i;
	for(i=0; i<LINE_LENGTH-1; ++i){ //LINE_LENGTH-1, bo w ostatniej iteracji nie chce przesuwac kursora dwukrotnie
		data_write(' ');
		shift_cursor(SH_LEFT);
		shift_cursor(SH_LEFT);
	}
	data_write(' ');
	shift_cursor(SH_LEFT);
}

void print(char * str){
	while(*str != 0){
		data_write(*str);
		++str;
	}
}

int main(void){
	//ustawienie portow
	DDRD = 0xff;
	DDRC = 0x0f;
	PORTC = 0x30;
	DDRB = 0x00;
	PORTB = 0x02;
	
	display_on();
	function_set();
	display_clear();
	set_entry_mode();
	
	print("To jest");
	_delay_ms(1500);
	clear_line(0);
	
	print("wersja 4-bit");
	_delay_ms(1500);
	clear_line(0);
	
	short offset = 0;
	
	while(1){
	
		if(!(PINC & 0x20)){
			
			++offset;
			data_write('a'+offset);
			shift_cursor(SH_LEFT);
			
			_delay_ms(20); // przeczekaj drgania stykow
			
			uint8_t step = 10, counter = 0;
			while(!(PINC & 0x20)){ //dopoki wcisniety
				if(counter>100){
					step = 3;
					counter = 0;
					
					++offset;
					data_write('a'+offset);
					shift_cursor(SH_LEFT);
				}else
					++counter;
					
				_delay_ms(step);
			}
			_delay_ms(20); // przeczekaj drgania stykow
		}
		
		if(!(PINC & 0x10)){
			
			--offset;
			data_write('a'+offset);
			shift_cursor(SH_LEFT);
			
			_delay_ms(20); // przeczekaj drgania stykow
			
			uint8_t step = 10, counter = 0;
			while(!(PINC & 0x10)){ //dopoki wcisniety
				if(counter>100){
					step = 3;
					counter = 0;
					
					--offset;
					data_write('a'+offset);
					shift_cursor(SH_LEFT);
				}else
					++counter;
					
				_delay_ms(step);
			}
			_delay_ms(20); // przeczekaj drgania stykow
		}
		
		if(!(PINB & 0x2)){
			
			shift_cursor(SH_RIGHT);
			data_write('a');
			shift_cursor(SH_LEFT);
			offset = 0;
			
			_delay_ms(20); // przeczekaj drgania stykow
			while(!(PINB & 0x2)){} //dopoki wcisniety
			_delay_ms(20); // przeczekaj drgania stykow
		}
	}
}