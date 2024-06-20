// C kernel entry.
void k_main(){

	unsigned char* mem= (unsigned char*) 0xb8000;

// Print BIN
	mem[0] ='B';
	mem[1] = 0x07;

	mem[2] ='I';
	mem[3] = 0x07;

	mem[4] ='N';
	mem[5] = 0x07;

// Infinite loop
	while(1){}

}
