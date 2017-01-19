/*
 * L1UDEV001A-Bootloader.c
 *
 *  Created on: 10.12.2016
 *      Author: Denry
 */


//these are here for informative purposes only atm, should be written into jumpToApplication function
//initial
//#define APP_INFO_ADDR_START 0x00001000
//#define APP_SP_ADDR 0x00001100
//#define APP_ADDR_START 0x00001104

//increased, for debug
#define APP_INFO_ADDR_START 0x00004000
#define APP_SP_ADDR 0x00004100
#define APP_ADDR_START 0x00004104

//required to put quotation marks around defines
#define DEF2STR(x) #x
#define STR(x) DEF2STR(x)

void jumpToApplication(){
	bootloader_debugmessage("Jumping to application!\n\r");

	/*
	 * this part works nicely, but doesn't have "defines" in it
	//app stack pointer initial value to stack pointer
	asm volatile("ldr r0, =0x4100"); //NB, this is increased during debugging
	asm volatile("ldr r0, [r0]");  //not sure why, but this line is very necessary
	asm volatile("mov sp, r0");

	//app reset vector address to program counter
	asm volatile("ldr r0, =0x4104"); //NB, this is increased during debugging
	asm volatile("ldr r0, [r0]"); //not sure why, but this line is very necessary
	asm volatile("mov pc, r0");
*/

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

void bootloader_debugmessage(char text[]){
	printf(text);
	//l11uxx_uart_Send(text);
	return;
}

void bootloader_debugmessage_valueDec(int value){
	char tempstring2[20];
	itoa (value,tempstring2,10);
	bootloader_debugmessage(tempstring2);
	return;
}

void bootloader_debugmessage_valueHex(int value){
	char tempstring[20];
	char tempstring2[20];
	itoa (value,tempstring2,16);
	strcpy(tempstring, "0x");
	strcat(tempstring, tempstring2);
	bootloader_debugmessage(tempstring);
	return;
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
	bootloader_debugmessage_valueHex(ID);
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


int updateIntFlashChecksum(){

	//

	return 1; //is OK
	return 0; //very broken
}

int isIntFlashChecksumOK(){

	//if firstboot flag
	//update image length
	//calc and update checksum
	//remove firstboot flag
	//verify firstboot flag. If still there, something hellawrong, not allow boot.
	//continue with (re)check of internal checksum

	if(getMCUID() == 35){
		//check for LPC11U35
	} else return 1; //Others not supported yet, so, yeah, sure why not.

	return 1; //is OK
	return 0; //very broken

}



int main(){
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
