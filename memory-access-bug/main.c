#include <stdio.h>
#include <msp430.h>
#include <sancus/sm_support.h>
#include <sancus_support/sm_io.h>
#include "reader.h"
#include "attacker.h"
#define  N_DATA 10

int main()
{
    uint16_t  i;
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
    
    
    // Starting memory accesses
    pr_info("trying to illegally access mem from main.c");
    for(i=0; i<N_DATA; i++) 
    {
    	mp = reader.public_start + i; 
    	printf("[main.c] Address 0x%.4x\n",mp);
  		stolen_data = *((uint16_t *)mp);
		printf("[main.c] Data nr.%d at addr. 0x%.4x \t 0x%.4x \n",i, mp, stolen_data);
    }
        
    EXIT();
}

