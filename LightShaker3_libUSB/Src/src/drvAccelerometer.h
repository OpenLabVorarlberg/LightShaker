#include "stdint.h"

#define ACC_DATAWIDTH_8		0
#define ACC_DATAWIDTH_10	1

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} acc16_t;
typedef struct {
	int8_t x;
	int8_t y;
	int8_t z;
} acc8_t;

typedef enum {
	RANGE_2G = 0, RANGE_4G = 1, RANGE_8G = 2
} Accelerometer_Ranges;

typedef enum {						//OS in Highres mode
	RATE_800Hz = 0,		//Oversampling = 2
	RATE_400Hz = 1,		//Oversampling = 4
	RATE_200Hz = 2,		//Oversampling = 8
	RATE_100Hz = 3,		//Oversampling = 16
	RATE_50Hz = 4,		//Oversampling = 32
	RATE_12Hz5 = 5,
	RATE_6Hz25 = 6,
	RATE_1Hz56 = 7
} Accelerometer_dataRates;

uint8_t Accelerometer_init();
//we only use the interrupt for data-ready irq, and only interrupt line 1
//the sensors embedded functions are not usable for this application
void Accelerometer_initIrq();
void Accelerometer_deInit();

uint8_t Accelerometer_setRange(Accelerometer_Ranges range);
uint8_t Accelerometer_setDataRate(Accelerometer_dataRates dataRate);
uint8_t Accelerometer_setDataWidth(uint8_t DataWidth);
acc8_t Accelerometer_read8();
acc16_t Accelerometer_read10();
void Accelerometer_IrqHandler();

