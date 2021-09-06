/*
 * @file			winusb_loop.h
 * @brief			header-file for the WinUSB loop (winusb_loop.c)
 * @author			SteDun00
 * @version			V1.0
 * @review date		2021-02-26
 * @note			with reference to NASA-gateway OMICRON electronics GmbH, DTPprim (ThoRat00)
 * OMICRON electronics GmbH, Austria
 * */

#ifndef USB_INC_WINUSB_LOOP_H_
#define USB_INC_WINUSB_LOOP_H_

#include <stdbool.h>


//USB device state
struct usb_device_state {
	bool isConfigured;
	bool isConnected;
	bool isInterruptEnabled;
};


extern struct usb_device_state usb_device_state;
extern bool usb_device_interruptEnabled;

#define FIFO_SIZE					64*4
extern uint8_t fifo[FIFO_SIZE];
extern uint32_t fpos;


void winusb_init_usbd(void);
void usb_device_connect(void);
void usb_device_disconnect(void);
void usb_interrupt_send(char *msg);
void USB_Handler(void);

#endif /* USB_INC_WINUSB_LOOP_H_ */
