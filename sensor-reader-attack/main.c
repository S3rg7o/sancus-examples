#include <stdio.h>
#include <msp430.h>
#include <sancus/sm_support.h>
#include <sancus_support/sm_io.h>
#include "reader.h"
#include "attacker.h"


int main()
{
	unsigned sm_id;
	unsigned vendor_id;
	char     name;
	uint16_t ts;
	uint16_t te;
	uint16_t ds;
	uint16_t de;
	uint16_t *text_section_pointer;
    uint16_t *data_section_pointer;
    uint16_t text_section_dim;
    uint16_t data_section_dim;
    uint16_t i;
    // try to perform illegal access from main
    uint16_t stolen_data;
    uint16_t * mp;
    
    //FILE *fp;
	char sm_name_str[3]; //for priting the log file
	char file_name[16];

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
    pr_info("trying to perform illegal access");
    mp = ts + 8; 
   	stolen_data = *mp;
    printf("[main.c] stolen data: %d",stolen_data);
    
    
    pr_info("starting dma illegal access...");
    // Getting SM's identity
    get_struct_val(&reader, &ts, &te, &ds, &de, &sm_id, &vendor_id, &name);
    printf("%s \n SM ID: %d \n TS: 0x%.4x - TE: 0x%.4x \n DS: 0x%.4x - DE 0x%.4x \n",name,sm_id, ts, te, ds, de);
   
    // Allocate dinamic memory for saving text section content    
    text_section_dim = te - ts +1;
    data_section_dim = de - ds +1;
   	
   	// Read Text section 
    text_section_pointer = (uint16_t *) malloc(text_section_dim*sizeof(uint16_t));
    if (text_section_pointer == NULL)
    	printf("Impossible to allocate enough memory for text section!\n");
	else {
		printf("start reading into SM%d's text section...\n",sm_id);
    	attacker_read(ts, te, text_section_pointer);
  		}
  		
	/*// Write log file for Text Section
	sprintf(sm_name_str, "SM%d", sm_id);  		
	strcpy(file_name, strcat(sm_name_str,"_text_section")); 	
  	fp = fopen(file_name, "w");
	if(fp == NULL)
		return(-1);
		
	fprintf(fp,"Text section content of SM%d :\n",sm_id);
  	for (i = 0; i<text_section_dim; i++)
  		fprintf(fp, "Data nr. %d: 0x%.4x \n",i, *(text_section_pointer+i) );
  	fclose(fp);  	  	  	
  	  	
  	// Read Data section  	  	  		
    data_section_pointer = (uint16_t *) malloc(data_section_dim*sizeof(uint16_t));    
    if (text_section_pointer == NULL)
    	printf("Impossible to allocate enough memory for data section!");*/
    	
    	for (i = 0; i<2; i++)
  			printf("Data nr. %d: 0x%.4x \n",i, *(text_section_pointer+i) );
    	
    EXIT();
}

