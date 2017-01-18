/*
 * L1UDEV001A-Bootloader.c
 *
 *  Created on: 10.12.2016
 *      Author: Denry
 */


//these are here for informative purposes only atm, should be written into jumpToApplication function
#define APP_INFO_ADDR_START 0x00001000
#define APP_SP_ADDR 0x00001100
#define APP_ADDR_START 0x00001104

void jumpToApplication(){
	//app stack pointer initial value to stack pointer
	asm volatile("ldr r0, =0x1100");
	asm volatile("ldr r0, [r0]");
	asm volatile("mov sp, r0");

	//app reset vector address to program counter
	asm volatile("ldr r0, =0x1104");
	asm volatile("ldr r0, [r0]");
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
	ID=35; //(LPC11U)35, 64kB flash
	return ID;
}

int updateIntFlashChecksum(){



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
