/* $Id: ToneTimer2.cpp 119 2010-07-17 18:56:36Z bhagman@roguerobotics.com $

  A Tone Generator Library

  Written by Brett Hagman
  http://www.roguerobotics.com/
  bhagman@roguerobotics.com
  
  Modified by Larry LeBron (larrylebron@gmail.com) to eliminate ISR definition conflicts
  
  Only tested with Arduino Uno/Demuinellenove

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <arduino.h>
#include <pins_arduino.h>
#include "ToneTimer2.h"


volatile int32_t timer0_toggle_count;
volatile uint8_t *timer0_pin_port;
volatile uint8_t timer0_pin_mask;
volatile int32_t timer1_toggle_count;
volatile uint8_t *timer1_pin_port;
volatile uint8_t timer1_pin_mask;
volatile int32_t timer2_toggle_count;
volatile uint8_t *timer2_pin_port;
volatile uint8_t timer2_pin_mask;

void ToneTimer2::begin(uint8_t tonePin)
{
    _pin = tonePin;

	// Initialize timer 2
	TCCR2A = 0;
	TCCR2B = 0;
	bitWrite(TCCR2A, WGM21, 1);
	bitWrite(TCCR2B, CS20, 1);
	timer2_pin_port = portOutputRegister(digitalPinToPort(_pin));
	timer2_pin_mask = digitalPinToBitMask(_pin);
}



// frequency (in hertz) and duration (in milliseconds).

void ToneTimer2::play(uint16_t frequency, uint32_t duration)
{
  uint8_t prescalarbits = 0b001;
  int32_t toggle_count = 0;
  uint32_t ocr = 0;

	// Set the pinMode as OUTPUT
    pinMode(_pin, OUTPUT);
    
// we are using an 8 bit timer, scan through prescalars to find the best fit
  ocr = F_CPU / frequency / 2 - 1;
  prescalarbits = 0b001;  // ck/1: same for both timers
  if (ocr > 255)
  {
	ocr = F_CPU / frequency / 2 / 8 - 1;
	prescalarbits = 0b010;  // ck/8: same for both timers

	if (ocr > 255)
	{
	  ocr = F_CPU / frequency / 2 / 32 - 1;
	  prescalarbits = 0b011;
	}

	if (ocr > 255)
	{
	  ocr = F_CPU / frequency / 2 / 64 - 1;
	  prescalarbits = 0b100;

	  if (ocr > 255)
	  {
		ocr = F_CPU / frequency / 2 / 128 - 1;
		prescalarbits = 0b101;
	  }

	  if (ocr > 255)
	  {
		ocr = F_CPU / frequency / 2 / 256 - 1;
		prescalarbits = 0b110;
		if (ocr > 255)
		{
		  // can't do any better than /1024
		  ocr = F_CPU / frequency / 2 / 1024 - 1;
		  prescalarbits = 0b111;
		}
	  }
	}
  }

    TCCR2B = (TCCR2B & 0b11111000) | prescalarbits;

   

    // Calculate the toggle count
    if (duration > 0)
    {
      toggle_count = 2 * frequency * duration / 1000;
    }
    else
    {
      toggle_count = -1;
    }

    // Set the OCR for timer 2
    // set the toggle count,
    // then turn on the interrupts

	OCR2A = ocr;
	timer2_toggle_count = toggle_count;
	bitWrite(TIMSK2, OCIE2A, 1);
  
}


void ToneTimer2::stop()
{
	TIMSK2 &= ~(1 << OCIE2A);
	digitalWrite(_pin, 0);
}


bool ToneTimer2::isPlaying(void)
{
	return (TIMSK2 & (1 << OCIE2A));
}


ISR(TIMER2_COMPA_vect)
{
  int32_t temp_toggle_count = timer2_toggle_count;

  if (temp_toggle_count != 0)
  {
    // toggle the pin
    *timer2_pin_port ^= timer2_pin_mask;

    if (temp_toggle_count > 0)
      temp_toggle_count--;
  }
  else
  {
    TIMSK2 &= ~(1 << OCIE2A);                 // disable the interrupt
    *timer2_pin_port &= ~(timer2_pin_mask);   // keep pin low after stop
  }
  
  timer2_toggle_count = temp_toggle_count;
}