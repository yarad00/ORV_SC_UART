
#ifndef AXI_DEFINES
#define AXI_DEFINES

 // Burst length specifies the number of data transfers that occur within each burst.
const int AXI_BURST_LENGTH_1 = 0x0;
const int AXI_BURST_LENGTH_2 = 0x1;
const int AXI_BURST_LENGTH_3 = 0x2;
const int AXI_BURST_LENGTH_4 = 0x3;
const int AXI_BURST_LENGTH_5 = 0x4;
const int AXI_BURST_LENGTH_6 = 0x5;
const int AXI_BURST_LENGTH_7 = 0x6;
const int AXI_BURST_LENGTH_8 = 0x7;
const int AXI_BURST_LENGTH_9 = 0x8;
const int AXI_BURST_LENGTH_10 = 0x9;
const int AXI_BURST_LENGTH_11 = 0xA;
const int AXI_BURST_LENGTH_12 = 0xB;
const int AXI_BURST_LENGTH_13 = 0xC;
const int AXI_BURST_LENGTH_14 = 0xD;
const int AXI_BURST_LENGTH_15 = 0xe;
const int AXI_BURST_LENGTH_16 = 0xF;

//Burst Size specifies the maximum number of data bytes to transfer in each 
// beat, or data transfer, within a burst
const int AXI_BURST_SIZE_BYTE = 0x0;
const int AXI_BURST_SIZE_HALF = 0x1;
const int AXI_BURST_SIZE_WORD = 0x2;
const int AXI_BURST_SIZE_LONG_WORD = 0x3;
const int AXI_BURST_SIZE_16BYTES = 0x4;
const int AXI_BURST_SIZE_32BYTES = 0x5;
const int AXI_BURST_SIZE_64BYTES = 0x6;
const int AXI_BURST_SIZE_128BYTES = 0x7;

// Burst Type 
const int AXI_BURST_TYPE_FIXED = 0x0;
const int AXI_BURST_TYPE_INCR = 0x1;
const int AXI_BURST_TYPE_WRAP = 0x2;
const int AXI_BURST_TYPE_RSRV = 0x3;

// Lock Type
const int AXI_LOCK_NORMAL = 0x0;
const int AXI_LOCK_EXCLUSIVE = 0x1;
const int AXI_LOCK_LOCKED = 0x2;
const int AXI_LOCK__RESERVED = 0x3;

// Protection Type
const int AXI_PROTECTION_NORMAL = 0x0;
const int AXI_PROTECTION_PRIVILEGED = 0x1;
const int AXI_PROTECTION_SECURE = 0x0;
const int AXI_PROTECTION_NONSECURE = 0x2;
const int AXI_PROTECTION_INSTRUCTION = 0x4;
const int AXI_PROTECTION_DATA = 0x0;

// Response Type
const int AXI_RESPONSE_OKAY = 0x0;
const int AXI_RESPONSE_EXOKAY = 0x1;
const int AXI_RESPONSE_SLAVE_ERROR = 0x2;
const int AXI_RESPONSE_DECODE_ERROR = 0x2;

#endif
