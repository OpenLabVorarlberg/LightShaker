#include "main.h"
#include "drvMMA8653.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_i2c.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_exti.h"

#include "stm32f0xx_tim.h"

#define I2C_MMA8653		I2C1
#define ADDR_MMA8653	0x3A

//define MMA8653 registers
#define REG_STATUS		0x00
#define REG_OUT_X_MSB	0x01
#define REG_OUT_X_LSB	0x02
#define REG_OUT_Y_MSB	0x03
#define REG_OUT_Y_LSB	0x04
#define REG_OUT_Z_MSB	0x05
#define REG_OUT_Z_LSB	0x06
#define REG_SYSMOD		0x0B
#define REG_INT_SOURCE	0x0C
#define REG_WHO_AM_I	0x0D
#define REG_XYZ_DATA_CFG 0x0E
#define REG_PL_STATUS	0x10
#define REG_PL_CFG		0x11
#define REG_PL_COUNT	0x12
#define REG_PL_BF_ZCOMNP 0x13
#define REG_PL_THS_REG	0x14
#define REG_FF_MT_CFG	0x15
#define REG_FF_MT_SRC	0x16
#define REG_FF_MT_THS	0x17
#define REG_FF_MT_COUNT	0x18
#define REG_ASLP_COUNT	0x29
#define REG_CTRL_REG_1	0x2A
#define REG_CTRL_REG_2	0x2B
#define REG_CTRL_REG_3	0x2C
#define REG_CTRL_REG_4	0x2D
#define REG_CTRL_REG_5	0x2E
#define REG_OFF_X		0x2F
#define REG_OFF_Y		0x30
#define REG_OFF_Z		0x31




uint8_t isActive = 0;

uint8_t rxBuf[4];

/**
 * Origin of I2C_RdReg and I2C_WrReg:
 * https://hsel.co.uk/2014/08/13/stm32f0-mini-tutorial-using-the-i2c-peripheral-to-communicate-with-a-hmc5883l-digital-compass-ic/
 */
uint8_t I2C_RdReg(uint8_t Reg, uint8_t *Data, uint8_t DCnt)
{
	uint8_t Cnt, SingleData = 0;

	//Wait until I2C isn't busy
	while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_BUSY) == SET);

	//first communication (try reading the status register)
	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2C_MMA8653,ADDR_MMA8653,1,I2C_SoftEnd_Mode,I2C_Generate_Start_Write);
	//Ensure the transmit interrupted flag is set
	while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_TXIS) == RESET);
	//send reg address
	I2C_SendData(I2C_MMA8653, (uint8_t)Reg);
	//Wait until transfer is complete!
	while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_TC) == RESET);
	//initiate read
	I2C_TransferHandling(I2C_MMA8653, ADDR_MMA8653, DCnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);


	//Read in DCnt pieces of data
	for(Cnt = 0; Cnt<DCnt; Cnt++)
	{
        //Wait until the RX register is full of luscious data!
        while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_RXNE) == RESET);
        //If we're only reading one byte, place that data direct into the
        //SingleData variable. If we're reading more than 1 piece of data
        //store in the array "Data" (a pointer from main)
        if(DCnt > 1)
        {
        	Data[Cnt] = I2C_ReceiveData(I2C_MMA8653);
        }
        else SingleData = I2C_ReceiveData(I2C_MMA8653);
     }

    //Wait for the stop condition to be sent
    while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_STOPF) == RESET);

    //Clear the stop flag for next transfers
    I2C_ClearFlag(I2C_MMA8653, I2C_FLAG_STOPF);

    //Return a single piece of data if DCnt was
    //less than 1, otherwise 0 will be returned.
	return SingleData;
}

void I2C_WrReg(uint8_t Reg, uint8_t Val){

	//Wait until I2C isn't busy
	while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_BUSY) == SET);

	//"Handle" a transfer - The STM32F0 series has a shocking
	//I2C interface... Regardless! Send the address of the HMC
	//sensor down the I2C Bus and generate a start saying we're
	//going to write one byte. I'll be completely honest,
	//the I2C peripheral doesn't make too much sense to me
	//and a lot of the code is from the Std peripheral library
	I2C_TransferHandling(I2C_MMA8653, ADDR_MMA8653, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

	//Ensure the transmit interrupted flag is set
	while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_TXIS) == RESET);

	//Send the address of the register we wish to write to
	I2C_SendData(I2C_MMA8653, Reg);

	//Ensure that the transfer complete reload flag is
	//set, essentially a standard TC flag
	while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_TCR) == RESET);

	//Now that the HMC5883L knows which register
	//we want to write to, send the address again
	//and ensure the I2C peripheral doesn't add
	//any start or stop conditions
	I2C_TransferHandling(I2C_MMA8653, ADDR_MMA8653, 1, I2C_AutoEnd_Mode, I2C_No_StartStop);

	//Again, wait until the transmit interrupted flag is set
	while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_TXIS) == RESET);

	//Send the value you wish you write to the register
	I2C_SendData(I2C_MMA8653, Val);

	//Wait for the stop flag to be set indicating
	//a stop condition has been sent
	while(I2C_GetFlagStatus(I2C_MMA8653, I2C_FLAG_STOPF) == RESET);

	//Clear the stop flag for the next potential transfer
	I2C_ClearFlag(I2C_MMA8653, I2C_FLAG_STOPF);
}

void I2C_BitHandling(uint8_t reg, uint8_t bitmask, uint8_t set)
{
	uint8_t tempReg = I2C_RdReg(reg,rxBuf,1u);
	if(set)
	{
		//set bits
		tempReg |= bitmask;
	}
	else
	{
		//reset bits
		tempReg &= ~bitmask;
	}
	//write back the register
	I2C_WrReg(reg,tempReg);
}

uint8_t mma8653_goStandby()
{
	if(!isActive)
	{
		return 0;
	}
	//reset the Active-bit
	I2C_BitHandling(REG_CTRL_REG_1, 1<<0, 0);
	delay(100);
	//check, if the sensor is inactive
	if(I2C_RdReg(REG_SYSMOD,rxBuf,1) != 0)
	{
		return 1;
	}
	isActive = 0;
	return 0;
}
uint8_t mma8653_goActive()
{

	if(isActive)
	{
		return 0;
	}
	//set the Active-bit
	I2C_BitHandling(REG_CTRL_REG_1, 1<<0, 1);
	delay(100);
	//check, if the sensor is active
	if(I2C_RdReg(REG_SYSMOD,rxBuf,1) != 1)
	{

		return 1;
	}
	isActive = 1;
	return 0;
}

uint8_t mma8653_init()
{

	//enable clock for periphery modules
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	//gpio init
	GPIO_InitTypeDef GPIO_InitStruct;
	//configure PINs PF0 and PF1 as AF OD out
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	//remap
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_1);

	//configure PINs PA2 and PA3 as inputs for the interrupt lines
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);


	//i2c init
	//I2C_DeInit(I2C1);
	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStruct.I2C_OwnAddress1 = 0xAB;
	I2C_InitStruct.I2C_DigitalFilter = 0;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_Timing = 0x0010020A; //should be 400kHz for the 8Mhz HSI
	I2C_Init(I2C_MMA8653,&I2C_InitStruct);

	I2C_Cmd(I2C_MMA8653, ENABLE);

	delay(50);

	if(I2C_RdReg(REG_WHO_AM_I,rxBuf, 1) != 0x5A)	//read the device ID to verify communication
	{
		return 1;
	}

	//set to highres mode
	I2C_BitHandling(REG_CTRL_REG_2,1<<1,SET);

	//set the sensor as always active (disable auto sleep) <- default
	//set to active and 10 bit mode, DataRate to 12.5Hz
	I2C_BitHandling(REG_CTRL_REG_1,(1<<0 | 1<<3 | 1<<5),SET);

	//check, if the sensor is active
	if(I2C_RdReg(REG_SYSMOD,rxBuf,1) != 1)
	{
		isActive = 0;
		return 2;
	}
	isActive = 1;
	return 0;
}

uint8_t mma8653_setRange(mma8653_Ranges range)
{
	//deactivate the sensor
	if(mma8653_goStandby())
	{
		return 1;
	}
	//clear bits in register to change
	I2C_BitHandling(REG_XYZ_DATA_CFG, 0x03, Bit_RESET);
	//set range
	I2C_BitHandling(REG_XYZ_DATA_CFG, range, Bit_SET);

	if(mma8653_goActive())
	{
		return 3;
	}
	return 0;
}

uint8_t mma8653_setDataRate(mma8653_dataRates dataRate)
{
	//deactivate the sensor
	if(mma8653_goStandby())
	{
		return 1;
	}
	//clear bits in register to change
	I2C_BitHandling(REG_CTRL_REG_1, 0x38, Bit_RESET);
	//set range
	I2C_BitHandling(REG_CTRL_REG_1, dataRate<<3, Bit_SET);

	if(mma8653_goActive())
	{
		return 3;
	}
	return 0;
}

uint8_t mma8653_setDataWidth(uint8_t DataWidth)
{
	//deactivate the sensor
	if(mma8653_goStandby())
	{
		return 1;
	}
	//clear bits in register to change
	I2C_BitHandling(REG_CTRL_REG_1, 0x3E, Bit_RESET);

	//decide what to do
	switch(DataWidth)
	{
	case MMA8653_DATAWIDTH_8:
		//set F_READ and leave highest data rate
		I2C_BitHandling(REG_CTRL_REG_1, 1<<1, Bit_SET);
		break;
	case MMA8653_DATAWIDTH_10:
		//limit output data rate to 12.5Hz and leave F_READ reset
		I2C_BitHandling(REG_CTRL_REG_1,(1<<3 | 1<<5), Bit_SET);
		break;
	default:
		return 2;
		break;
	}
	if(mma8653_goActive())
	{
		return 3;
	}
	return 0;
}

//read when in fast (8-bit) mode
//needs the fast-read mode enabled as well
//max. sample frequency (because of communication) ~ 7kHz
acc8_t mma8653_read8()
{
	uint8_t rxBuf[3];
	acc8_t acc_result;

	I2C_RdReg(REG_OUT_X_MSB,rxBuf,3);
	//rxbuf[0] contains full 8 bit of data.
	//the first bit defines the sign!
	acc_result.x = rxBuf[0];
	acc_result.y = rxBuf[1];
	acc_result.z = rxBuf[2];
	return acc_result;
}


//read when in full-resolution mode
acc16_t mma8653_read10()
{

	uint8_t rxBuf[6];
	acc16_t acc_result;

	I2C_RdReg(REG_OUT_X_MSB,rxBuf,6);
	//rxbuf[0] is MSB, and contains full 8 bit of data.
	//the first bit defines the sign!
	acc_result.x = ((int16_t)rxBuf[0] << 2) | (rxBuf[1]&0x60>>6);
	acc_result.y = ((int16_t)rxBuf[2] << 2) | (rxBuf[3]&0x60>>6);
	acc_result.z = ((int16_t)rxBuf[4] << 2) | (rxBuf[5]&0x60>>6);
	return acc_result;
}

//TODO: don't forget to implement and register the interrupt handler in startup code
void mma8653_initIrq()
{
	mma8653_goStandby();

	//interrupt pins are defined for push/pull low active by default -> leave that

	//enable the wanted interrupt source
	I2C_BitHandling(REG_CTRL_REG_4,1<<0,Bit_SET);
	//and route it to the corresponding pin
	I2C_BitHandling(REG_CTRL_REG_5,1<<0,Bit_SET);

	//set up the Interrupt in the STM32!
	//INT1 is connected to PA2 -> EXTI2 line
	//enable the clock for the syscfg module
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//enable PA2 interrupt source
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);
	//configure and enable the exti2 line
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line2;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	//enable and set exti2 interrupt
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI2_3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	//activate sensor again
	mma8653_goActive();
}
