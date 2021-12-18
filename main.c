#include <reg51.h>

//	Global variables
unsigned char counter_timer0 = 0;
unsigned char counter_timer1 = 0;

unsigned char current_render_digit = 0;

unsigned char seconds = 0;

unsigned char minutes_units = 0;
unsigned char minutes_tens = 0;
unsigned char hours_units = 2;
unsigned char hours_tens = 1;

const unsigned char numbers[] = {
	0xC0,
	0xF9,
	0xA4,
	0xB0,
	0x99,
	0x92,
	0x82,
	0xF8,
	0x80,
	0x90
};

//	Special Function Registers
sbit button_minutes_units = P2^0;
sbit button_minutes_tens = P2^1;
sbit button_hours_units = P2^2;
sbit button_hours_tens = P2^3;

void timer0_isr() interrupt 1 {
	TH0 = 0x4B;        //	Reload timer0 high value
	TL0 = 0xFD;        // Reload timer0 low value
	counter_timer0++;
	
	if (counter_timer0 > 19) {
		seconds++;       // 1 second has been passed
		counter_timer0 = 0;
	}
	
	/*
		Verify digit limits
	*/
	if (seconds > 59) {
		seconds = 0;
		minutes_units++;
	}
	if(minutes_units > 9) {
		minutes_units = 0;
		minutes_tens++;
	}
	if(minutes_tens > 5) {
		minutes_tens = 0;
		hours_units++;
	}
	if((hours_tens < 2 && hours_units > 9) || (hours_tens == 2 && hours_units > 3)) {
		hours_units = 0;
		hours_tens++;
	}
	if(hours_tens > 2) {
		hours_tens = 0;
	}
}

void timer1_isr() interrupt 3	{
	counter_timer1++;
	if(counter_timer1 >= 4) {
		/*
			Render digits one by one every 4ms, taking 16ms to render the entire display (60fps?)
		*/
		P0 = 0xFF; 		 // Clear the display
		switch (current_render_digit)
    {
    	case 0:
				P1 = 0xF7;
				P0 = numbers[minutes_units];
    		break;
    	case 1:
				P1 = 0xFB;
				P0 = numbers[minutes_tens];
    		break;
			case 2:
				P1 = 0xFD;
				P0 = numbers[hours_units];
    		break;
			case 3:
				P1 = 0xFE;
				P0 = numbers[hours_tens];
    		break;
    	default:
    		break;
    }
		
		current_render_digit++;
		if(current_render_digit > 3)
		{
			current_render_digit = 0;
		}
		
		counter_timer1 = 0;
	}
}

void main() {
	/*
		Timer configuration
			timer0 - mode 1 (16-bit timer mode)
			timer1 - mode 2 (8-bit auto-reload mode) 
	*/
	TMOD = 0x21;
	
	/*
		Timer1 calculation:
			fosc = 11.0592Mhz
			period = 1ms
		
		Solving for 8bit Timer1:
			255 - (period / (fosc / 12)) = 255 = 0xFF
			Timer1 high/low: 0xFF
	*/
	TH1 = 0xFF;        // Set timer1 high value
	TL1 = 0xFF;				 // Set timer1 low value
	TR1 = 1;           // Enable timer1
	
	/*
		Timer0 calculation:
			fosc = 11.0592Mhz
			period = 50ms
		
		Solving for 16bit Timer0:
			65536 - (period / (fosc / 12)) = 19453 = 0x4BFD
			Timer0 high: 0x4B
			Timer0 low:  0xFD
	*/
	TH0 = 0x4B;        // Set timer0 high value
	TL0 = 0xFD;				 // Set timer0 low value
	TR0 = 1;           // Enable timer0
	
	/*
		Enable timer interrupts
	*/
	ET0 = 1;           // Enable timer0 interrupt
	ET1 = 1;           // Enable timer1 interrupt
	EA = 1;            // Enable global interrupt
	
	/*
		Port configuration
	*/
	button_minutes_units = 1;
	button_minutes_tens = 1;
	button_hours_units = 1;
	button_hours_tens = 1;
	
	while(1)
	{
		/*
			Input pooling
		*/
		if(button_minutes_units == 0) {
			minutes_units++;
			if(minutes_units > 9) {
				minutes_units = 0;
			}
			while(button_minutes_units == 0);
		}
		if(button_minutes_tens == 0) {
			minutes_tens++;
			if(minutes_tens > 5) {
				minutes_tens = 0;
			}
			while(button_minutes_tens == 0);
		}
		if(button_hours_units == 0) {
			hours_units++;
			if((hours_tens < 2 && hours_units > 9) || (hours_tens == 2 && hours_units > 3)) {
				hours_units = 0;
			}
			while(button_hours_units == 0);
		}
		if(button_hours_tens == 0) {
			hours_tens++;
			if(hours_tens > 2) {
				hours_tens = 0;
			}
			while(button_hours_tens == 0);
		}
	} 
}