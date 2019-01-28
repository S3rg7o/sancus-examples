#include <stdio.h>
#include <msp430.h>
#include <sancus/sm_support.h>
#include <sancus_support/sm_io.h>
#include "reader.h"
#include "dma.h"
#define  N_DATA 17

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
	uint16_t  addr2;
	uint16_t  val;
    uint16_t  *addr_pointer2;
	uint16_t  *data_saved;
    uint16_t  i;
    uint16_t  *data_to_send;
    // variable for trying to perform illegal access to mem from main.c
    uint16_t  stolen_data = -2; // initialize with unequivocal content
    uint16_t  mp;
    
 
	//---------------------------------------------------
    msp430_io_init();
    
   	// Allocate memory for saving read content    
    data_saved   = (uint16_t *) malloc(N_DATA*sizeof(uint16_t));
    data_to_send = (uint16_t *) malloc(N_DATA*sizeof(uint16_t));
    	
    // Initialize data to be written
	for (i=0; i< N_DATA; i++)
		*(data_to_send+i) = i;		
	   
	addr  =(uint16_t)data_to_send;
	addr2 = 0x0240;
	addr_pointer2 = (uint16_t *)0x0240;
	
	// ======================================
	// MMIO operations: just sets the regions
	// ======================================
	mmio_config(N_DATA, addr, addr2);
	
	for(i=0; i<7; i++)
	{
		printf("NOP during DMA MMIO op.\n");	
	}	
		
		
	// =================================
	// DMA operation through the Device
	// this stops the software
	// =================================
	
	printf("Ora uso le normali funzioni\n");
	
	dma_write(addr, N_DATA, data_to_send);
	// se il dato a 0x024e dato è 0, è la prova che il dma controller non è ancora arrivato a scriverlo, ma che il controllo del sofware è già avanzato.
    val = *addr_pointer2;
    printf("Value at 0x20a right after attack_write: %d \n", val);
	for (i=0; i<10; i++) // dummy for    
    // NOP  
    val = *addr_pointer2;
    printf("Value at 0x20a after a while: %d \n", val);
	  
	printf("[main.c] start reading written data with DMA\n");
    dma_read(addr, N_DATA, data_saved);
   
    for (i = 0; i< N_DATA; i++)
  	{
  		printf("[main->attacker] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, addr, *(data_saved+i) );  
    	addr = addr + 2;  
    }	
    
    
    EXIT();
}

