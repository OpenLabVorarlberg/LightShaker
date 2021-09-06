/*
 * @file			usb.h
 * @brief			checks the version of the used controller and defines, which device stack must be used
 * @author			SteDun00
 * @version			V1.0
 * @review date		2021-02-26
 * OMICRON electronics GmbH, Austria
 */

#ifndef USB_INC_USB_H_
#define USB_INC_USB_H_



#if defined(__cplusplus)
	extern "C"{
#endif


#include "usbd_core.h"
#if !defined(__ASSEMBLER__)
#include "usb_std.h"
#endif


#if defined(STM32F042x6)
	#define USBD_STM32L052

#if !defined(__ASSEMBLER__)
	extern const struct usbd_driver usbd_devfs;
	extern const struct usbd_driver usbd_devfs_asm;
#if defined(USBD_ASM_DRIVER)
#define usbd_hw usbd_devfs_asm
#else
#define usbd_hw usbd_devfs
#endif
#endif

#else
	#error Unsupported STM32 family
#endif



#if defined (__cplusplus)
	}
#endif



#endif /* USB_INC_USB_H_ */
