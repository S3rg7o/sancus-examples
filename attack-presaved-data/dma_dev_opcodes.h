// MMIO Locations of device registers
#define START_ADDR_REG 0x0100 
#define N_WORDS_REG    0x0102 
#define CONFIG_REG     0x0104 	
#define DATA_REG       0x0106 
#define OUT_REG        0x0108 

// Opcodes for simple_dma_device.v
// \============================\
// \============================\
// \   Configuration register   \
// \============================\
// \============================\

// -------------------------------------------------------------------
// | END_OP	| - | ~DEV_ACK |  -  | WRITE_OK |  -  | ERROR_FLAG |  -  |
// -------------------------------------------------------------------
// |  15   | 14	|    13    | 12  |    11    | 10  |      9     |  8  |  
// -------------------------------------------------------------------
//
// -------------------------------------------------------------------
// | - | - | RESET_REGS |  ACK_SET  | NON_ATOMIC | RD_WR | - | START |
// -------------------------------------------------------------------
// | 7 | 6 |     5      |     4     |     3      |   2   | 1 |   0   |
// -------------------------------------------------------------------

// Atomic Operations
#define READ_OP        0x0005  
#define END_READ       0x8004  

// Non-Atomic Operations, need main.c acknowledge. One data by one
#define WRITE_OP       0x0009
#define WRITE_OK       0x0809

#define READ_OP_ACK    0x001D  
#define WAIT_READ_ACK  0x200D  
#define END_READ_ACK   0xA00C  
#define RESET_REGS     0x0020	

// Error bit is nr. 9, so if you bitwise AND the config_register and
// DMA_ERROR you have a check of whereas the error flag is set.
#define DMA_ERROR 0x0200	


