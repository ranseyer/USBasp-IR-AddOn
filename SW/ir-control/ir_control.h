/*
 * ir_control.h
 *
 *  Created on: 20.05.2012
 *      Author: muebau
 */

#ifndef IR_CONTROL_H_
#define IR_CONTROL_H_

#define PACKAGE    "ir_control"
#define VERSION    "1.0.0 (20.05.2012)"

#define USBDEV_SHARED_VENDOR    0x16C0  /* VOTI */
#define USBDEV_SHARED_PRODUCT   0x27d9  /* Obdev's free shared PID */
/* Use obdev's generic shared VID/PID pair and follow the rules outlined
 * in firmware/usbdrv/USBID-License.txt.
 */
typedef struct {
	uint8_t repeat; // count of repeats
	uint8_t protocol; // protocol, i.e. NEC_PROTOCOL
	uint16_t address; // address
	uint16_t command; // command
	uint8_t flags; // flags, e.g. repetition
} IR_DATA;

void usage(int exval);

//READ
int readPowerOnEnabled(void);
void readTrainedIRCode(IR_DATA *code);
int readIrmpVersion(char* buffer);
void readLogData(void);

//WRITE
int writePowerOnEnabled(int status);
int writeTrainedIRCode(void);
int writeMinRepeats(unsigned char count);
int deleteTrainedIRCode(void);

void printIrmpVersion(void);

static int usbGetStringAscii(usb_dev_handle *dev, int index, int langid, char *buf, int buflen);
static usb_dev_handle *findDevice(void);

#endif /* IR_CONTROL_H_ */
