/*
 * L1UDEV001A-Bootloader.c
 *
 *  Created on: 10.12.2016
 *      Author: Denry
 */


//initial
//#define APP_INFO_ADDR_START 0x00001000
//#define APP_SP_ADDR 0x00001100
//#define APP_ADDR_START 0x00001104

//increased, for debug
#define APP_INFO_ADDR_START 0x00004000
#define APP_SP_ADDR 0x00004100
#define APP_ADDR_START 0x00004104
#define MCU_FLASH_SIZE 0x00010000

//required to put quotation marks around defines
#define DEF2STR(x) #x
#define STR(x) DEF2STR(x)

#define SYSCLKINKHZ 12000 //for write in flash

void bootloader_debugmessage(char text[]){
	//l11uxx_uart_Send(text);


	//remove last char, ONLY if it's \n or \r
	char cutBuffer[strlen(text)];
	strcpy(cutBuffer, text);
	char *p = cutBuffer;
	if(((p[strlen(p)-1]) == '\n') || ((p[strlen(p)-1]) == '\r')) p[strlen(p)-1] = 0;
	printf(cutBuffer);
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

int md5leftrotate(int x, int c){

	return x;
}

int calculateIntFlashChecksum(char *checkSumPointer){
	//http://www.zorc.breitbandkatze.de/crc.html
	//for testing
	//doing CRC-32


	int i;
	unsigned long long divisor = 0x0000000004C11DB7; //33 bit;
	divisor = divisor << 32;
	int length = 0;//in 256 byte pages
	//length = length * 64; //this many 512bit chunks
	length = 1; //1 chunk, for testing
	//char blockOf512bit[16];
	unsigned long long blockUnderTest = 0; //64 bit, 32bit being data, 32bit being checksum
	unsigned int checksum; //

	//this is where loop starts
	//input of block
	blockUnderTest |= ('H' << 24);
	blockUnderTest |= ('i' << 16);
	//blockUnderTest |= (0 << 8);
	//blockUnderTest |= (0 << 0);

	//pad the block with zeroes to fit CRC length (32bit)
	blockUnderTest = blockUnderTest << 32;
	i=0;
	while((((blockUnderTest>>32)&0xFFFFFFFF)) != 0){
		blockUnderTest = blockUnderTest ^ ((divisor>>i)&0xFFFFFFFF);
		while(!((blockUnderTest>>(64-i)) & 0x01))i++;
		bootloader_debugmessage("B:");
		bootloader_debugmessage_valueHex(((blockUnderTest>>32)&0xFFFFFFFF),8);
		bootloader_debugmessage("\n\r");
		bootloader_debugmessage("D:");
		bootloader_debugmessage_valueHex(((divisor>>32)&0xFFFFFFFF),8);
		bootloader_debugmessage("\n\r");
		bootloader_debugmessage("------------\n\r");
	}
	checksum = blockUnderTest & 0xFFFFFFFF;
	//this is where loop ends

	//put checksum to correct place, cause I hate pointers so much
	checkSumPointer[3] = ((checksum >> 24)&0xFF);
	checkSumPointer[2] = ((checksum >> 16)&0xFF);
	checkSumPointer[1] = ((checksum >> 8)&0xFF);
	checkSumPointer[0] = ((checksum >> 0)&0xFF);
	return 1; //is OK
	return 0; //very broken
}

int updateIntFlashChecksum(){

	//

	return 1; //is OK
	return 0; //very broken
}

int clearFirstbootFlag(){
	int status=1; //by default is "fail"

	//!!!!!!!THIS IS COMMENTED OUT TO REDUCE FLASH WEAR DURING TEST
	//status = l11uxx_internal_flash_modifyOneByte(APP_INFO_ADDR_START+8+2, 0x00, SYSCLKINKHZ);

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

	int temporaryDebugValue=0;

	//if firstboot flag
	if((l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+2)) != 0x00){
		bootloader_debugmessage("Firstboot flag set: ");
		bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+2),2);
		bootloader_debugmessage("\n\r");

		//update image length
		temporaryDebugValue=((MCU_FLASH_SIZE-APP_SP_ADDR)/256);
		if(!(l11uxx_internal_flash_modifyOneByte(APP_INFO_ADDR_START+8+3, ((MCU_FLASH_SIZE-APP_SP_ADDR)/256), SYSCLKINKHZ))){
		bootloader_debugmessage("Image length updated: ");
		bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+3),2);
		bootloader_debugmessage("\n\r");
		} else {
			bootloader_debugmessage("Image length update FAILED: ");
			bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+8+3),2);
			bootloader_debugmessage("\n\r");
		}

		//calc and update checksum

		//remove firstboot flag
		//verify firstboot flag. If still there, something hellawrong, not allow boot.
		if(!(clearFirstbootFlag())) while(1);

		//continue with (re)check of internal checksum
	} else {
		bootloader_debugmessage("Firstboot flag not set. Is 0x00.\n\r");
	}


	if(getMCUID() == 35){
		//check for LPC11U35
	} else return 1; //Others not supported yet, so, yeah, sure why not.

	return 1; //is OK
	return 0; //very broken

}



int main(){


	bootloader_debugmessage("Expecting 4 byte checksum for internal image: ");
	bootloader_debugmessage_valueHex(l11uxx_internal_flash_read(APP_INFO_ADDR_START+256-4),4);
	bootloader_debugmessage("\n\r");

	bootloader_debugmessage("Test checksum for 'Hi'!\n\r");
	char checksum[32]; //only 4 bytes used for CRC-32
	calculateIntFlashChecksum(checksum);
	bootloader_debugmessage(checksum);

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

	//indicate major error?

	while(1){ //lock MCU

	}
	return 1; //o no!?
}
