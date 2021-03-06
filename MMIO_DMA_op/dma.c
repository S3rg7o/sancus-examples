#include "dma.h"
#include "dma_driver_opcodes.h"
//==============================================
// C functions for higher level control
//==============================================
void mmio_config(uint16_t num_words, uint16_t read_address, uint16_t write_address)
{
	// The function just calls in the right way the MMIO asm config function
	uint16_t op_code;
	op_code = MMIO_READ;	
	asm_config_mmio_op(num_words, read_address, write_address, op_code);
}

void dma_read(uint16_t start_addr, uint16_t num_of_words, uint16_t * save_data)
{
	uint16_t config_register;
	uint16_t counter = 0;
	
	asm("mov %0 , &CONFIG_REG "
	   : 
	   : "i"(RESET_REGS)); // reset register before starting
	   
	// Read from start_addr
	config_register = READ_OP_ACK;
	asm_config_op( num_of_words, start_addr, READ_OP_ACK);
	while ((config_register != END_READ_ACK) && !(config_register & DMA_ERROR))
		//wait until the end of operation and save the data
		config_register = asm_dev_get_data(config_register, (uint16_t *)(save_data+counter), READ_OP_ACK, &counter);	
	asm("mov %0 , &CONFIG_REG "
	   : 
	   : "i"(RESET_REGS)); // reset register before leaving
}


void dma_write(uint16_t start_addr, uint16_t num_of_words, uint16_t * data_to_send)
{
	uint16_t config_register;
	uint16_t counter = 0;	
	//printf("[attacker-write] Starting address is 0x%.4x \n", start_addr);
	
	asm("mov %0 , &CONFIG_REG "
	   : 
	   : "i"(RESET_REGS)); // reset register before starting
	
	// Write from start_addr
	asm_config_op(num_of_words, start_addr, WRITE_OP);
	while (counter < num_of_words) 
		config_register = asm_dev_write_data(config_register, *(data_to_send+counter), &counter); //wait until the end of operation and send the data
	
	asm("mov %0 , &CONFIG_REG "
	   : 
	   : "i"(RESET_REGS)); // reset register before leaving
}


void get_struct_val(struct SancusModule* module_address, uint16_t* ts, uint16_t* te, uint16_t* ds, uint16_t* de, uint16_t* sm_id, uint16_t* vendor_id, char* name)
{
	uint16_t  config_register;
	uint16_t  sm_struct_val[7];
	uint16_t  counter = 0;

	config_register = READ_OP_ACK;
	asm_config_op(7, module_address, READ_OP_ACK);
	while (config_register != END_READ_ACK) 
		//wait until the end of operation and save the data
		config_register = asm_dev_get_data(config_register, &sm_struct_val[counter], READ_OP_ACK, &counter);
		
	// Assign output values
	*sm_id     = sm_struct_val[0];
	*vendor_id = sm_struct_val[1];
	*name      = sm_struct_val[2];
	*ts        = sm_struct_val[3];  
	*te        = sm_struct_val[4];
	*ds        = sm_struct_val[5];
	*de        = sm_struct_val[6];  
}


//==============================================
// ASM instructions for low level control
//==============================================
void asm_config_mmio_op( uint16_t num_of_words, uint16_t address, uint16_t mmio_address, uint16_t op_code)
{
	
	asm(" ; Define memory addresses  \n\t"
		".equ START_ADDR_REG , 0x0100 \n\t"
		".equ N_WORDS_REG    , 0x0102 \n\t"
		".equ CONFIG_REG     , 0x0104 \n\t"	
		".equ DATA_REG       , 0x0106 \n\t"
		".equ OUT_REG        , 0x0108 \n\t"
		".equ MMIO_ADDR_REG  , 0x010A \n\t"
		" ; Start operation           \n\t"
		" mov %3             , &MMIO_ADDR_REG  \n\t"
		" mov %0             , &START_ADDR_REG \n\t"
		" mov %1             , &N_WORDS_REG    \n\t" 
		" mov %2             , &CONFIG_REG     \n\t"
		:  //no outputs
		: "m"(address), //inputs
		  "m"(num_of_words),
		  "m"(op_code),
		  "m"(mmio_address));
}

void asm_config_op( uint16_t num_of_words, uint16_t address, uint16_t op_code)
{
	
	asm(" ; Define memory addresses  \n\t"
		".equ START_ADDR_REG , 0x0100 \n\t"
		".equ N_WORDS_REG    , 0x0102 \n\t"
		".equ CONFIG_REG     , 0x0104 \n\t"	
		".equ DATA_REG       , 0x0106 \n\t"
		".equ OUT_REG        , 0x0108 \n\t"
		" ; Start operation           \n\t"
		" mov %0             , &START_ADDR_REG \n\t"
		" mov %1             , &N_WORDS_REG    \n\t" 
		" mov %2             , &CONFIG_REG     \n\t"
		:  //no outputs
		: "m"(address), //inputs
		  "m"(num_of_words),
		  "m"(op_code));
}

uint16_t asm_dev_get_data ( uint16_t config_register, uint16_t* out, uint16_t op_code, uint16_t* counter)
{
	asm(" mov &CONFIG_REG , %0"    // Get config_reg_value
        : "=m"(config_register)); 
	if (config_register == WAIT_READ_ACK)
	{	
		asm(" mov &DATA_REG   , %0 \n\t"          // Get data
			" mov %1          , &CONFIG_REG \n\t" // Configure reading
			: "=m"(*out) 
		    : "m"(op_code));
		*counter = *counter+1;
	}
	else if (config_register == END_READ_ACK)
	{
		asm(" mov &DATA_REG   , %0 \n\t"          // Get last data
			: "=m"(*out) );
		*counter = *counter+1;
	}
	else if (config_register & DMA_ERROR) //XXX change with a better handling of the ERROR
		*counter = *counter+1;
	return config_register;  
}	



uint16_t asm_dev_write_data (uint16_t config_register, uint16_t in, uint16_t *counter)
{	
	asm(" mov &CONFIG_REG , %0"    // Get config_reg_value
        : "=m"(config_register));
    if (config_register == WRITE_OK) //write only if dma_controller is ready
    {
    	asm(" mov %0          , &OUT_REG"
		: //no outputs. Device auto-generates the ack. 
		: "m"(in) );		
		*counter = *counter+1;	
    }   	
    else if (config_register & DMA_ERROR) //XXX not elegant NOR good for availability. But it's quick and it works
        *counter = *counter+1;	// a more elegant solution is to make the error signal arrive to the software, and have the software
	return config_register;     // handling the error. Or change the "while (counter < n_words)" in the dma_write with something like
} 	                            // "while ((counter < n_words) && (~error))"

      
