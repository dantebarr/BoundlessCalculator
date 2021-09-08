#pragma once

#include <stdio.h>
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "TransceiverConstants.h"

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

class Transceiver {
    spi_inst_t* const spi;

    //Check the pinout, these can only be on certain pins
    const u8 sck;
    const u8 miso;//Rx, master in slave out
    const u8 mosi;//Tx, master out slave in
    const u8 nss;//Csn, chip select

    const u8 dio0;//For interrupts

public:
    char code[CODE_LENGTH];

protected:

    //Determine which SPI is being used, by default our end is the master (which is what we want)
    Transceiver(bool SPI, u8 SCK, u8 MISO, u8 MOSI, u8 NSS, u8 DIO0) : spi(SPI ? spi1 : spi0), sck(SCK), miso(MISO), mosi(MOSI), nss(NSS), dio0(DIO0){

        //Chip select
        gpio_init(nss);
        gpio_set_dir(nss, GPIO_OUT);
        endTransaction();

        spi_init(spi, 9000*1000);//Up to 10MHZ, but there might me setup/hold violations if this is increased
        spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

        //SPI interface
        gpio_set_function(sck, GPIO_FUNC_SPI);
        gpio_set_function(mosi, GPIO_FUNC_SPI);
        gpio_set_function(miso, GPIO_FUNC_SPI);

        //LORA can only change in sleep
        enterSleep();

        //Use the whole FIFO - this might need to change for two-way
        writeReg(REG_FIFO_TX_BASEADDR, 0);
        writeReg(REG_FIFO_RX_BASEADDR, 0);

        //Begin shared config
        //Register 0x1D: Use default bandwidth of 125kHz, coding rate of 4/5, and explicit headers
        //Register 0x1E: Use default spreading factor 128 chips/symbol, and RX timeout MSB of 0, but turn CRC on
        writeReg(REG_MODEM_CONFIG_2, 0x74);
        //Register 0x26: Not using low data rate because symbol length is ~1ms, something to watch for as spreading factor and bandwidth change, using whatever gain
        //Register 0x20 & 0x21: Use default preamble size of 8
        //Registers 0x06 & 0x07 & 0x08: Using default frequency of 434 MHz
        writeReg(REG_PA_DAC, 0x87); //Turn on PA_BOOST
        writeReg(REG_PA_CONFIG, 0xFF); //Using maximum power
    }

    virtual bool handleIRQ() = 0;

    inline void enterStandby(){
        writeReg(REG_OP_MODE, STANDBY_MODE | LORA_MODE | LOW_FREQ_MODE);
    }

    inline void enterTx(){
        writeReg(REG_OP_MODE, TX_MODE | LORA_MODE | LOW_FREQ_MODE);
    }

    inline void enterRx(){
        writeReg(REG_OP_MODE, RX_MODE | LORA_MODE | LOW_FREQ_MODE);
    }

public:
    inline void enterSleep(){
        writeReg(REG_OP_MODE, SLEEP_MODE | LORA_MODE | LOW_FREQ_MODE);
    }

    //Of all of the parameters, the seed has the most randomness so we bias towards it
    void genCode(u8 seed, u8 duration, u64 features){
        u32 val = ((u32) features) ^ (u32)(features >> 32);
        val ^= ~seed << 24 | seed*duration*40503;
        snprintf(code, CODE_LENGTH, "%0*lx", CODE_LENGTH-1, val);
    }

protected:
    void writeReg(u8 reg, u8 data){
        u8 firstBit = reg | 0x80;
        u8 payload[2] = {firstBit, data};
        beginTransaction();
        spi_write_blocking(spi, payload, 2);
        endTransaction();
    }

    inline void readReg(u8 reg, u8* data){
        readRegs(reg, data, 1);
    }

    void readRegs(u8 reg, u8* data, u8 length){
        beginTransaction();
        spi_write_blocking(spi, &reg, 1);
        spi_read_blocking(spi, 0, data, length);
        endTransaction();
    }

    //NOTE: THE FIRST BYTE IN THE DATA GETS OVERWRITTEN
    void writeRegs(u8 reg, u8* data, u8 length){
        data[0] = reg | 0x80;
        beginTransaction();
        spi_write_blocking(spi, data, length);
        endTransaction();
        sleep_ms(10);
    }

    inline void clearIRQ(){
        writeReg(REG_IRQ, 0xFF);
    }

    inline void beginTransaction(){
        gpio_put(nss, 0); //Active low
    }

    inline void endTransaction(){
        gpio_put(nss, 1); //Active low
    }

};
