#include "stdint.h"
#include "utilFifo.h"

//
// Stellt 1 Byte in den Ringbuffer
//
// Returns:
//     FIFO_FAIL       der Ringbuffer ist voll. Es kann kein weiteres Byte gespeichert werden
//     FIFO_SUCCESS    das Byte wurde gespeichert
//
uint8_t FifoIn(FifoBuffer *buf, uint8_t byte) {
	uint32_t next = ((buf->write + 1) & FIFO_MASK);

	if (buf->read == next)
		return FIFO_FAIL; // voll

	buf->data[buf->write] = byte;
	// buffer.data[buffer.write & BUFFER_MASK] = byte; // absolut Sicher
	buf->write = next;

	return FIFO_SUCCESS;
}

//
// Holt 1 Byte aus dem Ringbuffer, sofern mindestens eines abholbereit ist
//
// Returns:
//     FIFO_FAIL       der Ringbuffer ist leer. Es kann kein Byte geliefert werden.
//     FIFO_SUCCESS    1 Byte wurde geliefert
//
uint8_t FifoOut(FifoBuffer *buf, uint8_t *pByte) {
	if (buf->read == buf->write)
		return FIFO_FAIL;

	*pByte = buf->data[buf->read];

	buf->read = (buf->read + 1) & FIFO_MASK;

	return FIFO_SUCCESS;
}

uint32_t FifoCount(FifoBuffer *buf) {
	if (buf->write >= buf->read) {
		return buf->write - buf->read;
	} else {
		return FIFO_SIZE - buf->read + buf->write;
	}
}
