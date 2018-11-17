#include <stdio.h>
#include <msp430.h>
#include <sancus/sm_support.h>
#include <sancus_support/sm_io.h>
#include "reader.h"
#include "attacker.h"
#define  N_DATA 10

int main()
{
    sm_id     id;
    vendor_id vendor_id;
    char      name[10];
    uint16_t  print_add; // used in printing correct accessed mem. addresses
    uint16_t  ts;
	uint16_t  te;
	uint16_t  ds;
	uint16_t  de;
	uint16_t  *data_saved;
    uint16_t  text_section_dim;
    uint16_t  data_section_dim;
    uint16_t  i;
    uint16_t  data_to_send[N_DATA];
    // variable for trying to perform illegal access to mem from main.c
    uint16_t  stolen_data = -2; // initialize with unequivocal content
    uint16_t  mp;
    
    // Initialize with a message
	for (i=0; i<N_DATA; i++)
		data_to_send[i] = (i%2 == 0) ? 0xF00D : 0xF1D0;
			
	//---------------------------------------------------
    msp430_io_init();

    pr_info("enabling sensor/reader SMs..");
    sancus_enable(&sensor);
    pr_sm_info(&sensor);
    sancus_enable(&reader);
    pr_sm_info(&reader);

    pr_info("requesting sensor readings..");
    nonce_t no = 0xabcd;
    ReaderOutput out;
    get_readings(no, &out);

    pr_info("dumping sealed output from reader SM..");
    dump_buf((uint8_t*)&no, sizeof(no), "  Nonce");
    dump_buf((uint8_t*)&out.cipher, sizeof(out.cipher), "  Cipher");
    dump_buf((uint8_t*)&out.tag, sizeof(out.tag), "  Tag");

    pr_info("all done!");
    
    /*// Starting memory accesses
    pr_info("trying to illegally access mem from main.c");
    for(i=0; i<N_DATA; i++) 
    {
    	mp = reader.public_start + i; 
    	printf("[main.c] Address 0x%.4x\n",mp);
  		stolen_data = *((uint16_t *)mp);
		printf("[main.c] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, mp, stolen_data);
    }*/
    
    pr_info("starting dma illegal access...");
    
    // Getting SM's identity
    get_struct_val(&reader, &ts, &te, &ds, &de, &id, &vendor_id, name);
    printf("SM ID: %d \n TS: 0x%.4x - TE: 0x%.4x \n DS: 0x%.4x - DE 0x%.4x \n", id, ts, te, ds, de);
   	
   	// Allocate dinamic memory for saving read content    
    text_section_dim = N_DATA;
    data_saved = (uint16_t *) malloc(text_section_dim*sizeof(uint16_t));
    	
	// Read Text section 
    if (data_saved == NULL) {
    	printf("[main.c] impossible to allocate enough memory for text section!\n");
		EXIT();
	}
	else {
		printf("[main.c] start reading into SM%d's text section...\n",id);
    	attacker_read(ts, N_DATA, data_saved);
  		}

	print_add = ts;  		  	  		
   	for (i = 0; i<N_DATA; i++)
   	{
		printf("[attacker] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, print_add, *(data_saved+i) );
		print_add = print_add +2;
	}	
	// Write into Text Section	
	printf("[main.c] start writing into SM%d's text section...\n",id);
	for (i = 0; i<N_DATA; i++)
		printf("data to write nr.%d: 0x%.4x \n",i,data_to_send[i]);
	attacker_write(ts, N_DATA, data_to_send);
	
	printf("[main.c] start reading into SM%d's text section after having written...\n",id);
    attacker_read(ts, N_DATA, data_saved);
    
    print_add = ts; 
    for (i = 0; i<N_DATA; i++)
  	{
  		printf("[attacker] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, print_add, *(data_saved+i) );  
    	print_add = print_add + 2;  
    }		
    
    printf("-----------------------------------------------------------\n");
    printf("-----------------------------------------------------------\n");
    printf("-----------------------------------------------------------\n");
        
    
    // Allocate dinamic memory for saving read content    
    data_section_dim = N_DATA;
    data_saved = (uint16_t *) malloc(data_section_dim*sizeof(uint16_t));
    	
	// Read Data section 
    if (data_saved == NULL) {
    	printf("[main.c] impossible to allocate enough memory for data section!\n");
		EXIT();
	}
	else {
		printf("[main.c] start reading into SM%d's data section...\n",id);
    	attacker_read(ds, N_DATA, data_saved);
  		}

	print_add = ds;  		  	  		
   	for (i = 0; i<N_DATA; i++)
   	{
		printf("[attacker] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, print_add, *(data_saved+i) );
		print_add = print_add +2;
	}	
	// Write into Data Section	
	printf("[main.c] start writing into SM%d's data section...\n",id);
	for (i = 0; i<N_DATA; i++)
		printf("data to write nr.%d: 0x%.4x \n",i,data_to_send[i]);
	attacker_write(ds, N_DATA, data_to_send);
	
	printf("[main.c] start reading into SM%d's data section after having written...\n",id);
    attacker_read(ds, N_DATA, data_saved);
    
    print_add = ds; 
    for (i = 0; i<N_DATA; i++)
  	{
  		printf("[attacker] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, print_add, *(data_saved+i) );  
    	print_add = print_add + 2;  
    }	
  
    
    EXIT();
}

