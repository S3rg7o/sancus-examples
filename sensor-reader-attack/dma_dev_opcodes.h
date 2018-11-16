// Opcodes for simple_dma_device.v
// \============================\
// \============================\
// \   Configuration register   \
// \============================\
// \============================\

// -----------------------------------------------------------------------------
// | END_OP	| 0 | ~DEV_ACK | ---  |  ACK_SET  | NON_ATOMIC | RD_WR | 0 | START |
// -----------------------------------------------------------------------------
// |  15   | 14	|    13    | ---  |     4     |     3      |   2   | 1  |  0   |
// -----------------------------------------------------------------------------

// Atomic Operations
#define READ_OP        0x0005  
#define END_READ       0x8004  

// One-by-one Operations
#define WRITE_OP       0x0001
#define READ_OP_ACK    0x001D  
#define WAIT_READ_ACK  0x200D  
#define END_READ_ACK   0xA00C  


