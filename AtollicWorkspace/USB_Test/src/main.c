#include <stdio.h>
#include "stm32f0xx.h"
#include "usb.h"

// The current clock frequency
uint32_t SystemCoreClock=48000000;

// Counts milliseconds
volatile uint32_t systick_count=0;

// Interrupt handler
void SysTick_Handler(void)
{
    systick_count++;
}

// Delay some milliseconds
void delay_ms(int ms)
{
    uint32_t start=systick_count;
    while (systick_count-start<ms);
}

// Redirect standard output to the USB port
int _write(int file, char *ptr, int len)
{
    for (int i=0; i<len; i++)
    {
        UsbCharOut(*ptr++);
    }
    return len;
}


// Change system clock to 48Mhz using 8Mhz crystal
// Called by Assembler startup code
void SystemInit(void)
{
    // Because the debugger switches PLL on, we first
    // need to switch back to the HSI oscillator without PLL

    // Switch to HSI oscillator
	RCC->CFGR &= ~RCC_CFGR_SW;

    // Wait until the switch is done
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    // Disable the PLL, then we can configure it
    RCC->CR &= ~RCC_CR_PLLON;

    /* SYSCLK, HCLK, PCLK configuration ----------------------------------------*/
     /* At this stage the HSI is already enabled */

     /* Enable Prefetch Buffer and set Flash Latency */
     FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

     /* HCLK = SYSCLK */
     RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

     /* PCLK = HCLK */
     RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

    /* Enable HSI48 */
     RCC->CR2 |= RCC_CR2_HSI48ON;
   /* Wait till HSI48RDY is set */
     while((RCC->CR2 & RCC_CR2_HSI48RDY) == 0)
     { }
   /* Select HSI48 as system clock source */
     RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
     RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI48;

    // Set USB clock to HSI48 -> default

    // Update variable
    SystemCoreClock=48000000;
}

void init_io()
{
    // Enable USB
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    // Enable Port A
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    //For the STM32F042F6, we need to reroute Pin 17 and 18 to PA11 and PA12 in order to use USB
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_PA12_RMP;

    //USB D+ and D- are additional functions on the GPIB-> the pin is always connected to the USB-Periphery
    //Just set it to analog to switch off all other gpio functions
    GPIOA->MODER |= GPIO_MODER_MODER11 | GPIO_MODER_MODER12;
}

int main(void)
{
    init_io();
    UsbSetup();

    // Initialize system timer
    SysTick_Config(SystemCoreClock/1000);

    while (1)
    {

        puts("Hello World!");
    }
}
