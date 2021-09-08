#pragma once

#define PACKET_LENGTH sizeof(u8) + sizeof(u8) + sizeof(u64)

//Constants for registers, OR these
#define SLEEP_MODE 0x00
#define LORA_MODE 0x80
#define STANDBY_MODE 0x01
#define TX_MODE 0x03
#define TX_DONE 0x40
#define RX_MODE 0x05
#define RX_DONE 0x00
#define LOW_FREQ_MODE 0x08
#define CRC_MASK 0x20

//Registers
#define REG_OP_MODE 0x01
#define REG_MODEM_CONFIG_2 0x1E
#define REG_DIO_MAPPING1 0x40
#define REG_IRQ 0x12

#define REG_FIFO 0x00
#define REG_FIFO_ADDR 0x0D
#define REG_FIFO_TX_BASEADDR 0x0E
#define REG_FIFO_RX_BASEADDR 0x0F
#define REG_RX_NUM_BYTES 0x13
#define REG_PAYLOAD_LENGTH 0x22
#define REG_PA_CONFIG 0x09
#define REG_PA_DAC 0x4D
#define REG_FIFO_RX_ADDR 0x10

#define CODE_LENGTH 9 //Including null character

