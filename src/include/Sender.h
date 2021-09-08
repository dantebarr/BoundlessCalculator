#pragma once

#include "Transceiver.h"
#include "pico/time.h"

class Sender : public Transceiver {
    u8 seed; //0 is an invalid value, sets the LED pattern. Available if the remote wants to let the user know the pattern

public:
    Sender(bool SPI, u8 SCK, u8 MISO, u8 MOSI, u8 NSS, u8 DIO0) : Transceiver(SPI, SCK, MISO, MOSI, NSS, DIO0){
        seed = 0;
    }

    //This sets up the IRQ for when the transmission completes. When that occurs, put it back to sleep
    void sendMessage(u8 duration, u64 features){
        if(seed == 0){//Modulo might not be needed, but I am worried about the precision of the lower few bits
            seed = to_us_since_boot(get_absolute_time()) % 255 + 1;
        }
        genCode(seed, duration, features);

        enterStandby();

        //Set the interrupt
        writeReg(REG_DIO_MAPPING1, TX_DONE);

        //Reset the pointer
        writeReg(REG_FIFO_ADDR, 0);

        //Allocate one extra byte for the register address
        u8 payload[PACKET_LENGTH + 1] = {0, seed, duration};
        *((u64*) &payload[3]) = features;

        writeRegs(REG_FIFO, payload, sizeof(payload));

        //Length is fixed
        writeReg(REG_PAYLOAD_LENGTH, sizeof(payload)-1);

        enterTx();
    }

    bool handleIRQ(){
        clearIRQ();
        enterSleep();
        return true;
    }
};

