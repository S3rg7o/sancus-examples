// \============================\
// \   Configuration register   \
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

// Opcodes for simple_dma_device.v
//--------------------------------

//MMIO Operations
#define MMIO_READ   0x0003 //solely set the rd_wr to 1 and triggers the start
#define MMIO_WRITE  0x0001 //solely set the rd_wr to 0 and triggers the start

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


