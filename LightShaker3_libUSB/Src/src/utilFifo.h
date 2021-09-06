#define FIFO_FAIL     1
#define FIFO_SUCCESS  0

#define FIFO_SIZE 128 // muss 2^n betragen (8, 16, 32, 64 ...)
#define FIFO_MASK (FIFO_SIZE-1) // Klammern auf keinen Fall vergessen

typedef struct Buffers {
	uint8_t data[FIFO_SIZE];
	uint32_t read; // zeigt auf das Feld mit dem �ltesten Inhalt
	uint32_t write; // zeigt immer auf leeres Feld
} FifoBuffer;

uint8_t FifoIn(FifoBuffer *buf, uint8_t byte);
uint8_t FifoOut(FifoBuffer *buf, uint8_t *pByte);
uint32_t FifoCount(FifoBuffer *buf);
