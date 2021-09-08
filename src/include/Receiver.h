#pragma once

#include "Transceiver.h"

class Receiver : public Transceiver {

public:
    u8 seed;//Use to set the LED pattern
    u8 duration;
    u64 features;

    Receiver(bool SPI, u8 SCK, u8 MISO, u8 MOSI, u8 NSS, u8 DIO0) : Transceiver(SPI, SCK, MISO, MOSI, NSS, DIO0){
        duration = 0;
        features = 0;

        enterStandby();
        //Set the interrupt
        writeReg(REG_DIO_MAPPING1, RX_DONE);

        enterRx();
    }

    //Returns true if there was a valid message
    bool handleIRQ(){
        //First check the CRC
        std::cout << "handling IRQ" << std::endl;
        u8 readValue;
        readReg(REG_IRQ, &readValue);
        clearIRQ();
        if(readValue & CRC_MASK)
            return false;
        std::cout << "CRC correct" << std::endl;


        //Now check the length
        readReg(REG_RX_NUM_BYTES, &readValue);
        std::cout << (int)readValue << std::endl;

        if(readValue != PACKET_LENGTH)
            return false;
        std::cout << "Packet length correct" << std::endl;


        //Now set the pointer in the FIFO
        readReg(REG_FIFO_RX_ADDR, &readValue);
        writeReg(REG_FIFO_ADDR, readValue);

        //Actually read the message
        u8 message[PACKET_LENGTH];
        readRegs(REG_FIFO, message, PACKET_LENGTH);
        seed = message[0];
        duration = message[1];
        features = *((u64*) &message[2]);

        //If there are false positives, we can check SNR, RSSI, or add a magic number/checksum

        enterSleep();//Note we will only ever get one message because of this

        genCode(seed, duration, features);
        return true;
    }
};

