#include <avr/io.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) 
#endif

int readADC(uint8_t pin)
{
  int result = 0;

  if ( pin >= 14 )
    pin -= 14;

  ADMUX = _BV(REFS1) | _BV(REFS0) | ( pin & 7 ) ;
  ADCSRA = _BV(ADEN) | _BV(ADPS0) | _BV(ADPS2) ;
  
  sbi(ADCSRA,ADSC);
  loop_until_bit_is_clear(ADCSRA,ADSC);

  result = ADC; 
  
  ADCSRA=0;                 //disable ADC
  ADMUX=0;                  //turn off internal vref  

  return result; 
}

/**
 * \return Internal temperature in 0.01C, e.g. 25C is 2500
 */
int readInternalTemp(void)
{
  int reading = 0;
  int result = 0;

  ADCSRB |= _BV(MUX5);
  ADMUX = _BV(REFS1) | _BV(REFS0) | 0b1001 ;
  ADCSRA = _BV(ADEN) | _BV(ADPS0) | _BV(ADPS2) ;
  
  sbi(ADCSRA,ADSC);
  loop_until_bit_is_clear(ADCSRA,ADSC);
  
  reading = ADC;

  ADCSRA=0;                 //disable ADC
  ADCSRB=0;                 //disable ADC
  ADMUX=0;                  //turn off internal vref  

  return reading * 113 - 27280; 
}
