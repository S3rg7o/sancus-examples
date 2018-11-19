#include <stdio.h>
#include <msp430.h>
#include <sancus/sm_support.h>
#include <sancus_support/sm_io.h>
#include "reader.h"
#include "attacker.h"
#define  N_DATA 4

int main()
{
    sm_id     id;
    vendor_id vendor_id;
    char      name[10];
    uint16_t  add; // used in printing correct accessed mem. addresses
    uint16_t  ts;
	uint16_t  te;
	uint16_t  ds;
	uint16_t  de;
	uint16_t  text_section_dim;
    uint16_t  i;
    uint16_t  data_saved;
    // variable for trying to perform illegal access to mem from main.c
    uint16_t  stolen_data = -2; // initialize with unequivocal content
    uint16_t  mp;
    
 
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
   	
    // Read Text Section	
    text_section_dim = N_DATA;
    printf("[main.c] start reading into SM%d's text section...\n",id);
    add = ts;  	
    for (i=0; i < text_section_dim; i++)
	{
		attacker_read(add, 1, &data_saved);
		printf("[main->attacker] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, add, data_saved);
		add = add +2;
  	}  			  	 
	
	// Write into Text Section	
	printf("[main.c] start writing into SM%d's text section...\n",id);
	add = ts;
	for (i=0; i < text_section_dim; i++)
	{
		attacker_write(add, 1, &i);
		add = add+2;
	}
	
	// Read Text Section
	printf("[main.c] start reading into SM%d's text section after having written...\n",id);
    add = ts; 
    for (i=0; i < text_section_dim; i++)
	{
		attacker_read(add, 1, &data_saved);
		printf("[main->attacker] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, add, data_saved);
		add = add +2;
  	}  	
        
    EXIT();
}

