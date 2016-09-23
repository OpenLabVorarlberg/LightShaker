
#define MMA8653_DATAWIDTH_8		0
#define MMA8653_DATAWIDTH_10	1

typedef enum{
	IRQ_DataReady = 0,
	IRQ_MotionFreefall = 2,
	IRQ_Orientation = 4,
	IRQ_AutoSleep = 7
}IRQTypes;

typedef enum{
	IRQ_Line1 = 1,
	IRQ_Line2 = 0
}IRQLines;

typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
}acc16_t;
typedef struct
{
	int8_t x;
	int8_t y;
	int8_t z;
}acc8_t;
uint8_t mma8653_init();

uint8_t mma8653_setDataWidth(uint8_t DataWidth);
acc8_t mma8653_read8();
acc16_t mma8653_read10();
void mma8653_IrqHandler();
