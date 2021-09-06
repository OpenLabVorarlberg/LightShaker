/*
 * @file			definitions.h
 * @brief			global definitions file
 * @author	 		SteDun00
 * @version			V1.0
 * @revision date	2021-02-26
 * OMICRON electronics GmbH, Austria
 */

#ifndef USB_INC_DEFINITIONS_H_
#define USB_INC_DEFINITIONS_H_



//USB_PMA - size equals the MAX-Stack-Size
#define USB_PMASIZE				768


/* modify bitfield */
#define _BMD(reg, msk, val)     (reg) = (((reg) & ~(msk)) | (val))
/* set bitfield */
#define _BST(reg, bits)         (reg) = ((reg) | (bits))
/* clear bitfield */
#define _BCL(reg, bits)         (reg) = ((reg) & ~(bits))
/* wait until bitfield set */
#define _WBS(reg, bits)         while(((reg) & (bits)) == 0)
/* wait until bitfield clear */
#define _WBC(reg, bits)         while(((reg) & (bits)) != 0)
/* wait for bitfield value */
#define _WVL(reg, msk, val)     while(((reg) & (msk)) != (val))
/* bit value */
#define _BV(bit)                (0x01 << (bit))



#endif /* USB_INC_DEFINITIONS_H_ */
