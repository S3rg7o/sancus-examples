#include <stdio.h>
#include <msp430.h>
#include <sancus/sm_support.h>
#include <sancus_support/sm_io.h>
#include "reader.h"
#include "attacker.h"
#define  N_DATA 70

int main()
{
    sm_id     id;
    vendor_id vendor_id;
    char      name[10];
  
    uint16_t  ts;
	uint16_t  te;
	uint16_t  ds;
	uint16_t  de;
	uint16_t  addr;
	uint16_t  val;
    uint16_t  test_val; 
	uint16_t  *addr_pointer;
	uint16_t  *data_saved;
    uint16_t  text_section_dim;
    uint16_t  data_section_dim;
    uint16_t  i;
    uint16_t  data_to_send[N_DATA];
    // variable for trying to perform illegal access to mem from main.c
    uint16_t  stolen_data = -2; // initialize with unequivocal content
    uint16_t  mp;
    
 
	//---------------------------------------------------
    msp430_io_init();
    
   	// Allocate memory for saving read content    
    text_section_dim = N_DATA;
    data_saved = (uint16_t *) malloc(text_section_dim*sizeof(uint16_t));
    	
    // Initialize data to be written
	for (i=0; i< text_section_dim; i++)
		data_to_send[i] = i;		
	
	/*
	// Little test, to see if the access are corretly handled.
	// Test passed, that's why it's commented
	test_val = 7;
	attacker_write(0x0202, 1, &test_val);
	printf("[main.c] start write op. to test IRQ\n");
	addr_pointer = (uint16_t *)0x0202;
	val = *addr_pointer;
    printf("Value: %d \n", val);*/
    
    
	addr = 0x020a;
	addr_pointer = (uint16_t *)0x024e;
	attacker_write(addr, N_DATA, data_to_send);
	// se il dato a 0x024e dato è 0, è la prova che il dma controller non è ancora arrivato a scriverlo, ma che il controllo del sofware è già avanzato.
    val = *addr_pointer;
    printf("Value at 0x20a right after attack_write: %d \n", val);
	 
	 // dummy for
    for (i=0; i<10; i++)
    { 
      ; // NOP
     }
    val = *addr_pointer;
    printf("Value at 0x20a after a while: %d \n", val);
	  
	printf("[main.c] start reading written data with DMA\n");
    attacker_read(addr, N_DATA, data_saved);
   
    for (i = 0; i< text_section_dim; i++)
  	{
  		printf("[main->attacker] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, addr, *(data_saved+i) );  
    	addr = addr + 2;  
    }	
    
    
    EXIT();
}

