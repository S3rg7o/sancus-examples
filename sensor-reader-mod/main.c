#include <msp430.h>
#include <stdio.h>
#include <sancus/sm_support.h>
#include <sancus_support/sm_io.h>
#include "reader.h"

int main()
{
    msp430_io_init();

    pr_info("enabling sensor/reader SMs..");
    sancus_enable(&sensor);
    pr_sm_info(&sensor);
    sancus_enable(&reader);
    pr_sm_info(&reader);
    
    sensor_data_t sto_data;
    pr_info("calling 'read_sensor_data()' from main.c");	
	dump_buf((uint8_t*)&sto_data, sizeof(sensor_data_t), "  Data stolen, before sensor reading:");
	sto_data = read_sensor_data();
    dump_buf((uint8_t*)&sto_data, sizeof(sensor_data_t), "  Data stolen: after sensor reading:");


    pr_info("requesting sensor readings..");
    nonce_t no = 0xabcd;
    ReaderOutput out;
    get_readings(no, &out);

    pr_info("dumping sealed output from reader SM..");
    dump_buf((uint8_t*)&no, sizeof(no), "  Nonce");
    dump_buf((uint8_t*)&out.cipher, sizeof(out.cipher), "  Cipher");
    dump_buf((uint8_t*)&out.tag, sizeof(out.tag), "  Tag");

    pr_info("all done!");
    EXIT();
}
