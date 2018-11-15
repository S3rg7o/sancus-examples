#include <stdio.h>
#include <string.h>
#include <sancus_support/sm_io.h>


//==============================================
// C functions for higher level control
//==============================================
void attacker_read(uint16_t start_addr, uint16_t end_addr, uint16_t * save_data);
void get_struct_val(struct SancusModule* module_address, uint16_t* ts, uint16_t* te, uint16_t* ds, uint16_t* de, unsigned * sm_id, unsigned * vendor_id, char* name);

//==============================================
// ASM instructions for controlling the device
//==============================================
uint16_t asm_config_dma_dev(uint16_t config_register, uint16_t num_of_words, uint16_t address, uint16_t op_code);
uint16_t asm_dev_get_data ( uint16_t config_register, uint16_t* out, uint16_t op_code);





