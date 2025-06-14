#include "stm8s.h"
#include "stm8_hd44780.h"
#include "stm8s_gpio.h"
#include "delay.h"
#include "stdio.h"
#define SPINAC GPIOE, GPIO_PIN_4

unsigned int adc_val = 0;
uint16_t prevod;
uint16_t procenta;
double lux;
uint16_t i;
int stav_tlacitka=0;
unsigned int result = 0;
void LUX_PROCENTA(void);
void lcd_print(unsigned int value);
void ADC_Read(void);

void LUX_PROCENTA(void){
		if (stav_tlacitka==0) {
			procenta = 106.4516-(prevod/31); 
			if (procenta > 100) procenta = 100;
			if (procenta < 0) procenta = 0;
			lcd_gotoxy(10,1);
			lcd_puts(" ");
			lcd_gotoxy(11,1);
			lcd_print(procenta);
			lcd_putchar('%');
		} else if (stav_tlacitka==1) {
			lcd_gotoxy(11,1);
			lux = 46000.0 - 95.7 * prevod + 0.068 * prevod * prevod - 0.000016 * prevod * prevod * prevod;
			if (lux > 20000) lux = 20000;
			if (lux < 0) lux = 0;
			lcd_gotoxy(10,1);
			lcd_print(lux);
			lcd_puts("lx");
		}
}
	
void delay_mss(uint16_t ms) {
			for (i = 0; i < ms; i++) {
        _delay_us(1000);
    }
}

void lcd_print(unsigned int value)//int na str
{
    char str[6];
    sprintf(str, "%4u", value);
		lcd_puts(str);
}

void main()//hlavni funkce
{
GPIO_Init(GPIOE, GPIO_PIN_4,GPIO_MODE_IN_FL_NO_IT);	
lcd_init();
lcd_gotoxy(0,0);
lcd_puts("Intenzita svetla:");
lcd_gotoxy(0,1);
lcd_puts("ADC:");
lcd_gotoxy(8,1);
lcd_puts("->");

while (1)
{
		ADC_Read();
    adc_val = result;
    prevod = adc_val*3.2258;//3.3/1023-1023 10 bitù
    lcd_gotoxy(4,1);
    lcd_print(prevod);
    LUX_PROCENTA();
    if (GPIO_ReadInputPin(SPINAC) == RESET) {//tlacitko na procenta/lux
        delay_mss(20);
        if (GPIO_ReadInputPin(SPINAC) == RESET) {
            stav_tlacitka = (stav_tlacitka + 1) % 2;
            while (GPIO_ReadInputPin(SPINAC) == RESET);
            delay_mss(20);
        }
    }
    delay_mss(100);
}
}
//ADC prevodník 
void ADC_Read(void)
 {
	ADC1_DeInit();
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
             ADC1_CHANNEL_7,
             ADC1_PRESSEL_FCPU_D18, 
             ADC1_EXTTRIG_TIM, 
             DISABLE, 
             ADC1_ALIGN_RIGHT, 
             ADC1_SCHMITTTRIG_ALL, 
             DISABLE);
	ADC1_Cmd(ENABLE);
	ADC1_StartConversion();
	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
  result = ADC1_GetConversionValue();
  ADC1_ClearFlag(ADC1_FLAG_EOC);
	ADC1_DeInit();
 }

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


