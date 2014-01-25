/*
 * main.c
 *
 *  Created on: 20.05.2012
 *      Author: muebau
 *      Modified 04.01.2013 Martin thanks to Wirbel+Bleifuss2
 *      23.01.14 Fix repeatrate by Woggle: http://www.vdr-portal.de/board16-video-disk-recorder/board4-vdr-installation/p1183244-gel%C3%B6st-samsung-fernbedienung-mit-usb-ir-receiver-einrichten/#post1183244
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>    /* this is libusb, see http://libusb.sourceforge.net/ */
#include <getopt.h>

#include "ir_control.h"

int ignore_dev = 0;
int del_code = 0;
int poweron_enable = -1;
int readPowerOn = 0;
int printVersion = 0;

unsigned char repeat = 0;

usb_dev_handle *handle = NULL;

int main(int argc, char **argv) {
	if(argc < 2)
		usage(0);

	int opt = 0;
	while ((opt = getopt(argc, argv, "hVdp:r:si")) != -1) {
		switch (opt) {
		case 'h':
			usage(0);
			break;
		case 'V':
			printVersion = 1;
			break;
			//		case 'v':
			//			printf("%s: Verbose option is set `%c'\n", PACKAGE, optopt);
			//			break;
		case 'd':
			del_code = 1;
			break;
		case 'p':
			if(strcmp(optarg,"0") == 0)
				poweron_enable = 0;
			else
				poweron_enable = 1;
			break;
		case 'r':
                        repeat = (unsigned char) atoi(optarg);
                        break;
		case 's':
			readPowerOn = 1;
			break;
		case 't':
			break;
		case 'i':
			ignore_dev = 1;
			break;
		case ':':
			fprintf(stderr, "%s: Error - Option `%c' needs a value\n\n",
					PACKAGE, optopt);
			usage(1);
			break;
		case '?':
			fprintf(stderr, "%s: Error - No such option: `%c'\n\n", PACKAGE,
					optopt);
			usage(1);
		}
	}

	//	usb_set_debug(1);
	usb_init();

	if ((handle = findDevice()) == NULL) {
		fprintf(stderr,	"Could not find USB device \"USB IR Remote Receiver\" with vid=0x%x pid=0x%x\n", USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
		return 1;
	}

	usb_detach_kernel_driver_np(handle, 0);
	usb_claim_interface(handle, 0);

	if(printVersion) {
		char buffer[12];
		readIrmpVersion(buffer);
		printf("%s %s\nIRMP: %s\n", PACKAGE, VERSION, buffer);
		return 0;
	}

	if(readPowerOn) {
		printf("%d\n", readPowerOnEnabled());
		return 0;
	}

	if(del_code) {
		deleteTrainedIRCode();
	}

	if(poweron_enable != -1) {
		writePowerOnEnabled(poweron_enable);
	}

	if(repeat) {
                writeMinRepeats(repeat);
	}

	usb_release_interface(handle, 0);
	usb_close(handle);
	return 0;
}

void usage(int exval) {
	printf("%s, %s control the \"USB IR Remote Receiver\"\n (www.mikrocontroller.net/articles/USB_IR_Remote_Receiver).\n\n",
			PACKAGE, VERSION);
	printf("%s [-h] | [-V] | [[[-d] | [-p[1|0]] | [-rCOUNT] | [-s] | [-t]] [-i]]\n\n",
			PACKAGE);

	printf("  -h              print this help and exit\n");
	printf("  -V              print version and exit\n\n");

	//printf("  -v              set verbose flag\n");
	printf("  -d              delete the PowerOn IR code.\n");
	printf("                  The next received code becomes the new PowerOn code\n");
	//printf("  -n              set new PowerOn IR code\n");
	printf("  -p [1|0]        set PowerOn (ON=1/OFF=0)\n");
	printf("  -r COUNT        set repeat count (default=5)\n");
	printf("  -s              status of: PowerOn (ON=1/OFF=0)\n");
	//printf("  -t              print trained IR code (PowerOn)\n");
	//printf("  -u ven:dev      use a different VENDOR:DEVICE id\n");
	printf("  -i              ignore device name. (just checks the vendor:device ID)\n");
	printf("                  needs one of the other options.\n\n");

	exit(exval);
}

// bRequest:
//			0x01	USBRQ_HID_GET_REPORT
// 				wValue:
//						ReportType (highbyte),
//							3
//						ReportID (lowbyte):
//							2		ReadPowerOnEnabled
//							3		ReadTrainendIRCode
//							7		ReadIrmpVersion
//							11		ReadLogData

//			0x09	USBRQ_HID_SET_REPORT
// 				wValue:
//						ReportType (highbyte),
//							3
//						ReportID (lowbyte):
//							4		SetPowerOnEnabled
//							5		SetTrainedIRCode
//							8		SetMinRepeats

int readPowerOnEnabled() {
	int count;
	char buffer[2];

	count = usb_control_msg(handle, /* dev_handle */
			0xa1,// LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT,  /* bmRequestType */
			0x01, /* bRequest */
			0x0302, /* wValue */
			0x0000, /* wIndex */
			buffer, /* data */
			0x02, /* wLength */
			3000); /* timeout */
	if(count != 2)
		return -1;

	return buffer[1];
}

void readTrainedIRCode(IR_DATA *code) {

}

int readIrmpVersion(char* buffer) {

	return usb_control_msg(handle, /* dev_handle */
			0xa1,// LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT,  /* bmRequestType */
			0x01, /* bRequest */
			0x0307, /* wValue */
			0x0000, /* wIndex */
			buffer, /* data */
			0x12, /* wLength */
			3000); /* timeout */
}

void readLogData() {

}

int writePowerOnEnabled(int status) {
	int count;
	char buffer[2];
	buffer[0] = 0x04;
	if(status == 0)
		buffer[1] = 0x00;
	else
		buffer[1] = 0x01;

	count = usb_control_msg(handle, /* dev_handle */
			0x21,// LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT,  /* bmRequestType */
			0x09, /* bRequest */
			0x0304, /* wValue */
			0x0000, /* wIndex */
			buffer, /* data */
			0x02, /* wLength */
			3000); /* timeout */
	return count != 2 ? -1 : 0;
}

int deleteTrainedIRCode() {
	int err;

	IR_DATA emptyCode;

	emptyCode.dummy = 0;
	emptyCode.protocol = 0;
	emptyCode.address = 0;
	emptyCode.command = 0;
	emptyCode.flags = 0;

	err = usb_control_msg(handle, /* dev_handle */
			0x21,// LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT,  /* bmRequestType */
			0x09, /* bRequest */
			0x0305, /* wValue */
			0x0000, /* wIndex */
			(char*) &emptyCode, /* data */
			sizeof(emptyCode), /* wLength */
			3000); /* timeout */
	return err == 8 ? 0 : err;
}

int writeMinRepeats(unsigned char count) {
        int retcount;
        char buffer[2];
        buffer[0] = 0;
        buffer[1] = count;
        retcount = usb_control_msg(handle, /* dev_handle */
                        0x21,// LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT, /* bmRequestType */
                        0x09, /* bRequest */
                        0x0308, /* wValue */
                        0x0000, /* wIndex */
                        buffer, /* data */
                        0x01, /* wLength */
                        3000); /* timeout */
        return retcount != 2 ? -1 : 0;
}



static int usbGetStringAscii(usb_dev_handle *dev, int index, int langid,
		char *buf, int buflen) {
	char buffer[256];
	int rval, i;

	if ((rval = usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR,
			(USB_DT_STRING << 8) + index, langid, buffer, sizeof(buffer), 1000))
			< 0)
		return rval;
	if (buffer[1] != USB_DT_STRING)
		return 0;
	if ((unsigned char) buffer[0] < rval)
		rval = (unsigned char) buffer[0];
	rval /= 2;
	/* lossy conversion to ISO Latin1 */
	for (i = 1; i < rval; i++) {
		if (i > buflen) /* destination buffer overflow */
			break;
		buf[i - 1] = buffer[2 * i];
		if (buffer[2 * i + 1] != 0) /* outside of ISO Latin1 range */
			buf[i - 1] = '?';
	}
	buf[i - 1] = 0;
	return i - 1;
}

static usb_dev_handle *findDevice(void) {
	struct usb_bus *bus;
	struct usb_device *dev;
	usb_dev_handle *handle = 0;

	usb_find_busses();
	usb_find_devices();
	for (bus = usb_busses; bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor == USBDEV_SHARED_VENDOR
					&& dev->descriptor.idProduct == USBDEV_SHARED_PRODUCT) {
				char string[256];
				int len;
				handle = usb_open(dev); /* we need to open the device in order to query strings */
				if (!handle) {
					fprintf(stderr, "Warning: cannot open USB device: %s\n",
							usb_strerror());
					continue;
				}

				if(ignore_dev)
					break;

				/* now find out whether the device actually is obdev's Remote Sensor: */
				len = usbGetStringAscii(handle, dev->descriptor.iManufacturer,
						0x0409, string, sizeof(string));
				if (len < 0) {
					fprintf(
							stderr,
							"warning: cannot query manufacturer for device: %s\n",
							usb_strerror());
					goto skipDevice;
				}

				//fprintf(stderr, "seen device from vendor ->%s<-\n", string);
				if (strcmp(string, "www.mikrocontroller.net/articles/USB_IR_Remote_Receiver") != 0 )
					goto skipDevice;
				len = usbGetStringAscii(handle, dev->descriptor.iProduct,
						0x0409, string, sizeof(string));
				if (len < 0) {
					fprintf(stderr,
							"warning: cannot query product for device: %s\n",
							usb_strerror());
					goto skipDevice;
				}

				//fprintf(stderr, "seen product ->%s<-\n", string);
				if (strcmp(string, "USB IR Remote Receiver") == 0)
					break;
				skipDevice: usb_close(handle);
				handle = NULL;
			}
		}
		if (handle)
			break;
	}
	if (!handle)
		fprintf(stderr, "Could not find USB device www.anyma.ch/uDMX\n");
	return handle;
}
