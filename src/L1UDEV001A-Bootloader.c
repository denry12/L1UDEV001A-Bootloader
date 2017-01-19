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
	int i;

	int length = 0;//in 256 byte pages
	//length = length * 64; //this many 512bit chunks
	length = 1; //1 chunk, for testing
	int blockOf512bit[16]; //16*32bit words
	int F;
	int g;
	int dTemp;
	const char s[64] = { 	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
							7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
							4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
							6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 };

	const int K[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
	 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
	 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
	  0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
	 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
	  0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
	 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
	 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
	 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
	 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
	 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

	static int wordA0=0x67452301, wordB0=0xefcdab89   , wordC0=0x98badcfe   , wordD0=0x10325476   ;
	int wordA, wordB   , wordC   , wordD   ;

	//input of block with preprocessing
	blockOf512bit[0] = 'H';
	blockOf512bit[1] = 'i';
	blockOf512bit[2] = 0x80000000;
	blockOf512bit[3] = 0;
	blockOf512bit[4] = 0;
	blockOf512bit[5] = 0;
	blockOf512bit[6] = 0;
	blockOf512bit[7] = 0;
	blockOf512bit[8] = 0;
	blockOf512bit[9] = 0;
	blockOf512bit[10] = 0;
	blockOf512bit[11] = 0;
	blockOf512bit[12] = 0;
	blockOf512bit[13] = 0;
	blockOf512bit[14] = 0;
	blockOf512bit[15] = 2;


	//for each chunk, TODO:

	wordA=wordA0, wordB=wordB0   , wordC=wordC0   , wordD=wordD0   ;

	for(i=0; i<64; i++){
		if(i<16){
			F = (wordB & wordC) || ((~wordB) & wordD);
			g = i;
		}
		if((i>16)||(i<32)){
			F = (wordD & wordB) | ((~wordD) & wordC);
			g = ((5*i+1) % 16);
		}
		if((i>32)||(i<48)){
			F = wordB ^ wordC ^ wordD;
			g = ((3*i+1) % 16);
		}
		if((i>48)||(i<64)){
			F = wordC ^ (wordB | (~wordD));
			g = ((7*i) % 16);
		}




	dTemp = wordD;
	wordD = wordC;
	wordC = wordB;
	wordB = wordB + md5leftrotate(wordA + F + K[i] + blockOf512bit[g], s[i]);
	wordA = dTemp;
	}
	//Add this chunk's hash to result so far:
	wordA0 = wordA0 + wordA;
	wordB0 = wordB0 + wordB;
	wordC0 = wordC0 + wordC;
	wordD0 = wordD0 + wordD;

	//checksumPointer[0] = 'A';
	//checksumPointer[1] = 'y';
	//checksumPointer[2] = 'y';

	for(i=0;i<15; i++){
		checkSumPointer[i] = checkSumPointer[i+1];
	}
	for(i=0;i<15; i++){
			checkSumPointer[i] = checkSumPointer[i+1];
		}
	for(i=0;i<15; i++){
			checkSumPointer[i] = checkSumPointer[i+1];
		}
	for(i=0;i<15; i++){
			checkSumPointer[i] = checkSumPointer[i+1];
		}
	checkSumPointer[3] = wordA0;
	checkSumPointer[2] = wordB0;
	checkSumPointer[1] = wordC0;
	checkSumPointer[0] = wordD0;
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




	bootloader_debugmessage("Test checksum for 'Hi'!\n\r");
	char md5checksum[32];
	calculateIntFlashChecksum(md5checksum);
	bootloader_debugmessage(md5checksum);

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
