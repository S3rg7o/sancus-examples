#include <stdio.h>
#include <string.h>
#include <sancus_support/sm_io.h>


//==============================================
// C functions for higher level control
//==============================================
void attacker_read (uint16_t start_addr, uint16_t num_of_words, uint16_t * save_data);
void attacker_write(uint16_t start_addr, uint16_t num_of_words, uint16_t * data_to_send);
void get_struct_val(struct SancusModule* module_address, uint16_t* ts, uint16_t* te, uint16_t* ds, uint16_t* de, uint16_t* sm_id, uint16_t* vendor_id, char* name);

//==============================================
// ASM instructions for controlling the device
//==============================================
void     asm_config_op      ( uint16_t num_of_words, uint16_t address, uint16_t op_code);
uint16_t asm_dev_get_data   ( uint16_t config_register, uint16_t* out, uint16_t op_code, uint16_t *counter);
void     asm_dev_write_data ( uint16_t in, uint16_t op_code, uint16_t *counter);




