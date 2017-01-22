/*
 * L1UDEV001A-Bootloader.c
 *
 *  Created on: 10.12.2016
 *      Author: Denry
 */


#ifdef __USE_CMSIS
#include "LPC11Uxx.h"
#endif
//this is mostly necessary for UART

//initial
//#define APP_INFO_ADDR_START 0x00001000
//#define APP_SP_ADDR 0x00001100
//#define APP_ADDR_START 0x00001104

//increased, for debug
#define APP_INFO_ADDR_START 0x00004000
#define APP_SP_ADDR 0x00004100
#define APP_ADDR_START 0x00004104
#define MCU_FLASH_SIZE 0x00010000

#define APP_INFO_POINTER_LENMSB APP_INFO_ADDR_START+8+5
#define APP_INFO_POINTER_LENLSB APP_INFO_ADDR_START+8+4

//required to put quotation marks around defines
#define DEF2STR(x) #x
#define STR(x) DEF2STR(x)

#define SYSCLKINKHZ 12000 //for write in flash


void bootloader_debugUART_init(){
	//assumes MCU is running on 12MHz IRC still

	//setting up Tx
	LPC_IOCON->PIO0_19 |= 0x1; //p0_19
	//NOT setting up Rx cause I am not listening to anything
	LPC_IOCON->PIO0_18 |= 0x1; //p0_18

	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
	LPC_SYSCON->UARTCLKDIV = 0x01;        //UART clock

	uint32_t  DL;
	//DL=6; //115200 w/o xtal, 12MHz, works well
	DL=76; //9600 w/o xtal, 12MHz


	LPC_USART->FDR = (12 << 4) | 1; //DivAddVal = 1, MulVal = 12

	LPC_USART->LCR = 0b10000011;  //  8-bit character length, DLAB enable ( DLAB = 1)
	LPC_USART->DLM = DL / 256;    //  Determines the baud rate of the UART (MSB Register)     (Access DLAB = 1)
	LPC_USART->DLL = DL % 256;    //  Determines the baud rate of the UART (LSB Register)     (Access DLAB = 1)
	LPC_USART->LCR = 0b00000011;  //  8-bit character length , DLAB disable ( DLAB = 0)
	LPC_USART->FCR = 0b00000111;  //  FIFOEN:Active high enable for both UART Rx and TX FIFOs and U0FCR[7:1] access

	return 1; //is OK
	return 0; //very broken
}

void bootloader_debugmessage(char text[]){
	//todo: it currently copies entire array. Pointer would be better

	//!!!uart version
	//l11uxx_uart_Send(text);
	int i=0;
	while(text[i] != 0){
		while (!(LPC_USART->LSR & (1<<5)));  //THRE: Transmitter Holding Register Empty.
				                                        // 0: THR contains valid data, 1: THR is empty
		LPC_USART->THR = text[i];
		i++;
	}



	//!!!printf version
	//remove last char, ONLY if it's \n or \r
	//this is done for "printf", cause it seems to consider them equal, adding unnecessary empty lines
	/*char cutBuffer[strlen(text)];
	strcpy(cutBuffer, text);
	char *p = cutBuffer;
	if(((p[strlen(p)-1]) == '\n') || ((p[strlen(p)-1]) == '\r')) p[strlen(p)-1] = 0;
	printf(cutBuffer);*/
	return;
}

void bootloader_debugmessage_valueDec(int value){
	char tempstring2[20];
	itoa (value,tempstring2,10);
	bootloader_debugmessage(tempstring2);
	return;
}

void bootloader_debugmessage_valueHex(int value, int spaces){
	char tempstring[20];
	char tempstring2[20];
	int i;
	itoa (value,tempstring2,16);
	strcpy(tempstring, "0x");
	i = strlen(tempstring2);
	for(i = spaces-i; i>0; i--) strcat(tempstring, "0");
	strcat(tempstring, tempstring2);
	bootloader_debugmessage(tempstring);
	return;
}

void jumpToApplication(){
	bootloader_debugmessage("Jumping to application!\n\r");

	//app stack pointer initial value to stack pointer
	asm volatile("ldr r0, =" STR(APP_SP_ADDR)); //NB, this is increased during debugging
	asm volatile("ldr r0, [r0]");  //not sure why, but this line is very necessary
	asm volatile("mov sp, r0");

	//app reset vector address to program counter
	asm volatile("ldr r0, =" STR(APP_ADDR_START)); //NB, this is increased during debugging
	asm volatile("ldr r0, [r0]"); //not sure why, but this line is very necessary
	asm volatile("mov pc, r0");

	return; //o no, we shouldn't get here
}

int isExtFlashInstalled(){

	return 0; //no external flash, too bad.
	return 1; //totally there
}

int getExtFlashVersion(){
	int version=0;

	return version;
}

int getIntFlashVersion(){
	int version=0;

	return version;
}

int isExtFlashChecksumOK(){
	//verify flags first, that is faster
	//verify checksum


	return 0; //very broken
	return 1; //is OK
}

int getMCUID(){
	int ID;
	// see "20.13.11" in user manual
	ID = l11uxx_internal_flash_partID();

	bootloader_debugmessage("Part ID: ");
	bootloader_debugmessage_valueHex(ID,8);
	bootloader_debugmessage("\n\r");
	switch (ID){
	case 0x0001BC40:
		ID=35; //(LPC11U)35, 64kB flash
		break;
	case 0x0000BC40:
		ID=35; //(LPC11U)35, 64kB flash
		break;
	default:
		ID=0;
		break;
	}
	return ID;
}










char crc8ccitt(const char *data, int len, char init) { //length in 8bit chunks
	// shoutout to ES for providing me a working and simple example of CRC
	// 8bit CRC, poly 0x07 (CRC8 CCITT)
	// https://en.wikipedia.org/wiki/Linear-feedback_shift_register
	char crc = init;

	bootloader_debugmessage("Starting at addr: ");
	bootloader_debugmessage_valueHex(data, 8);
	bootloader_debugmessage("\n\r");
    while (len) {
    	crc ^= *(data);

    	//0x07 = 0b00000111;
        crc ^= (crc >> 6) ^ (crc >> 7);
        crc ^= (crc << 1) ^ (crc << 2);

        data++;
        len--;
    }
    bootloader_debugmessage("Finished before checking at addr: ");
    	bootloader_debugmessage_valueHex(data, 8);
    	bootloader_debugmessage(" with checksum of ");
    	bootloader_debugmessage_valueHex(crc, 2);
    		bootloader_debugmessage("\n\r");
    return crc;
}

char calculateIntFlashChecksum(){
	//http://www.zorc.breitbandkatze.de/crc.html
	//for testing
	//doing CRC-8

	unsigned long long length = l11uxx_internal_flash_read(APP_INFO_POINTER_LENMSB);//in 256 byte pages
	length = length << 8;
	length |= (0xFF & l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+4));
	length &= 0xFFFF;

	length = length - 1; //cause one page is reserved for appinfo

	bootloader_debugmessage("Calculating for image length of ");
		bootloader_debugmessage_valueHex(length, 4);
		bootloader_debugmessage(" pages\n\r");

	//unsigned long long lastFlashAddr = APP_SP_ADDR + length;
	length = length * 256; //how many 8bit chunks
	//length = 2; //2 chars, for testing

//	int blockUnderTest = 0;
//
//	blockUnderTest |= ('H' << 24);
//	blockUnderTest |= ('i' << 16);
//	blockUnderTest |= ('L' << 8);
//	blockUnderTest |= ('o' << 0);

	bootloader_debugmessage("Commencing internal checksum calc, for ");
	bootloader_debugmessage_valueHex(length, 4);
	bootloader_debugmessage(" bytes\n\r");
	return crc8ccitt(APP_SP_ADDR, length, 0xFF);
	//checkSumPointer[0] = crc8ccitt(&blockUnderTest, length, 0xFF);

	//return 1; //is OK
	//return 0; //very broken
}











int updateIntFlashChecksum(){
	bootloader_debugmessage("Updating internal flash checksum... \n\r");
	if(!(l11uxx_internal_flash_modifyOneByte(APP_INFO_ADDR_START+256-1, calculateIntFlashChecksum(), SYSCLKINKHZ))){
		bootloader_debugmessage("Internal flash checksum changed and verified: ");
		bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+256-1),2);
		bootloader_debugmessage("\n\r");
		return 1; //is OK
	} else {
		bootloader_debugmessage("Internal flash checksum update FAILED: ");
		bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+256-1),2);
		bootloader_debugmessage("\n\r");
		return 0; //very broken
	}



}

int clearFirstbootFlag(){
	int status=1; //by default is "fail"

	//!!!!!!!IF THIS IS COMMENTED OUT, IT IS TO REDUCE FLASH WEAR DURING TEST
	status = l11uxx_internal_flash_modifyOneByte(APP_INFO_ADDR_START+8+2, 0x00, SYSCLKINKHZ);

	//verification
	if(!(status)){
		bootloader_debugmessage("Firstboot flag changed and verified: ");
		bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+2),2);
		bootloader_debugmessage("\n\r");
		return 1; //is OK
	} else {
		bootloader_debugmessage("Firstboot flag change failed: ");
		bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+2),2);
		bootloader_debugmessage("\n\r");
		return 0; //very broken
	}


}

int isIntFlashChecksumOK(){

	int checksum = 0;
	//if firstboot flag
	if((l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+2)) != 0x00){
		bootloader_debugmessage("Firstboot flag set: ");
		bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+2),2);
		bootloader_debugmessage("\n\r");

		//update image length
		//temporaryDebugValue=((MCU_FLASH_SIZE-APP_SP_ADDR)/256);

		int imageLength = ((MCU_FLASH_SIZE-APP_INFO_ADDR_START)/256); //NB, this involves appinfo also
		char imageLengthMSB = ((imageLength>>8)&0xFF);

		bootloader_debugmessage("Image length to be: ");
		bootloader_debugmessage_valueHex(imageLength,4);
		//bootloader_debugmessage(", where MSB is: ");
		//bootloader_debugmessage_valueHex(imageLengthMSB,2);
		//bootloader_debugmessage("\n\r");
		bootloader_debugmessage("Currently written image length: ");
		bootloader_debugmessage_valueHex(	( (l11uxx_internal_flash_read(APP_INFO_POINTER_LENMSB)) <<8)	+ (( (l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+4)) &0xFF)),4);
		bootloader_debugmessage("\n\r");

		if(!(l11uxx_internal_flash_modifyOneByte(APP_INFO_POINTER_LENMSB, imageLengthMSB, SYSCLKINKHZ))){
			if(!(l11uxx_internal_flash_modifyOneByte(APP_INFO_ADDR_START+8+4, (imageLength & 0x00FF), SYSCLKINKHZ))){

			bootloader_debugmessage("Image length updated: ");
			bootloader_debugmessage_valueHex(	((l11uxx_internal_flash_read(APP_INFO_POINTER_LENMSB))<<8)	+ (((l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+4))&0xFF)),4);
			bootloader_debugmessage("\n\r");
			}
			else {
						bootloader_debugmessage("Image length update step 2 FAILED: ");
						bootloader_debugmessage_valueHex(	((l11uxx_internal_flash_read(APP_INFO_POINTER_LENMSB))<<8)	+ ((l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+4))),4);
						bootloader_debugmessage("\n\r");
						return 0; //very broken
					}
		} else {
			bootloader_debugmessage("Image length update step 1 FAILED: ");
			bootloader_debugmessage_valueHex(	((l11uxx_internal_flash_read(APP_INFO_POINTER_LENMSB))<<8)	+ ((l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+4))),4);
			bootloader_debugmessage("\n\r");
			return 0; //very broken
		}

		//calc and update checksum

		if (updateIntFlashChecksum());
		else return 0; //very broken


		//remove firstboot flag
		//verify firstboot flag. If still there, something hellawrong, not allow boot.
		if(!(clearFirstbootFlag())) while(1);

		//continue with (re)check of internal checksum
	} else {
		bootloader_debugmessage("Firstboot flag not set. Is 0x00.\n\r");
	}


	if(getMCUID() == 35){
		//check for LPC11U35
		bootloader_debugmessage("Expecting 1 byte checksum for internal image: ");
		bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+256-1),2);
		bootloader_debugmessage("\n\r");
		checksum = calculateIntFlashChecksum();
		bootloader_debugmessage("Received checksum: ");
		bootloader_debugmessage_valueHex(checksum,2);
		bootloader_debugmessage("\n\r");

		if(checksum == l11uxx_internal_flash_read(APP_INFO_ADDR_START+256-1)){
			bootloader_debugmessage("Internal flash checksum OK.\n\r");
			return 1;
		}
		else{
			bootloader_debugmessage("Internal flash checksum NOT VALID.\n\r");
			return 0;
		}
	} else return 1; //Others not supported yet, so, yeah, sure why not.

	return 1; //is OK
	return 0; //very broken

}



int main(){



	bootloader_debugUART_init();


	//bootloader_debugmessage("Test checksum for 'Hi'!\n\r");
	//char checksum[32]; //only 1 byte used for CRC-8
	//calculateIntFlashChecksum(checksum);
	//bootloader_debugmessage_valueHex(checksum, 2);
	//bootloader_debugmessage("\n\r");

	//print a nice JDP logo
	/*bootloader_debugmessage("\n\r");
	bootloader_debugmessage("====================\n\r");
	bootloader_debugmessage("=                  =\n\r");
	bootloader_debugmessage("=  MMMM MMM  MMM   =\n\r");
	bootloader_debugmessage("=     M M  M M  M  =\n\r");
	bootloader_debugmessage("=     M M  M MMM   =\n\r");
	bootloader_debugmessage("=  M  M M  M M     =\n\r");
	bootloader_debugmessage("=   MM  MMM  M     =\n\r");
	bootloader_debugmessage("=                  =\n\r");
	bootloader_debugmessage("====================\n\r");*/

	//print a nicer JDP logo
		bootloader_debugmessage("\n\r");
		bootloader_debugmessage("╔═════════════════════╗\n\r");
		bootloader_debugmessage("║                     ║\n\r");
		bootloader_debugmessage("║  █████ ████  ████   ║\n\r");
		bootloader_debugmessage("║      █ █   █ █   █  ║\n\r");
		bootloader_debugmessage("║      █ █   █ ████   ║\n\r");
		bootloader_debugmessage("║  █   █ █   █ █      ║\n\r");
		bootloader_debugmessage("║   ███  ████  █      ║\n\r");
		bootloader_debugmessage("║                     ║\n\r");
		bootloader_debugmessage("╚═════════════════════╝\n\r");



	bootloader_debugmessage("L1UDEV001A bootloader is go!\n\r");
	if(isExtFlashInstalled()){ //aww yeah we have flash, time to do things
		if(getExtFlashVersion!=getIntFlashVersion){ //time to update internal flash
			if(isExtFlashChecksumOK()){ //image seems OK
				//update internal flash! Make sure to do it according to model
			}
		}
	}

	if(isIntFlashChecksumOK()) jumpToApplication();

	//attempt flashing stock image
	if(isIntFlashChecksumOK()) jumpToApplication();

	//indicate major error
	bootloader_debugmessage("Major error.\n\r");
	while(1){ //lock MCU

	}

	return 1; //o no!?
}
