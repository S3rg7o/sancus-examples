#include <msp430.h>
#include <stdio.h>
#include <sancus/sm_support.h>
#include <sancus_support/sm_io.h>
#include "reader.h"
#include "attacker.h"

int main()
{
	uint16_t ts;
	uint16_t te;
	uint16_t ds;
	uint16_t de;

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
    pr_info("starting dma illegal access...");
    get_struct_val(&reader,ts,te,ds,de);
    pr_info("protected section addresses:");
    printf("[attacker] %s \t: 0x%.4x \n","text start",ts);
    printf("[attacker] %s \t: 0x%.4x \n","text end"  ,te);
    printf("[attacker] %s \t: 0x%.4x \n","data start",ds);
    printf("[attacker] %s \t: 0x%.4x \n","data end"  ,de);
    pr_info("start reading into...");
    printf("[attacker] %s \t: 0x%.4x \n","Start addr.",ds);
    printf("[attacker] %s \t: 0x%.4x \n","End addr."  ,de);
  	attacker_read(ds, de);
    
    EXIT();
}

