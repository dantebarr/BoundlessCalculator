#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define TIMER_ADDRESS 0x68
#define EEPROM_ADDRESS 0x57
#define MAGIC_NUM 0xCC //Protects us from invalid reads for fresh devices
#define VARIABLE_OFFSET 32 //If the upper byte != 0, the code will need to be modified, pages of 32 bytes, starting at 2nd
#define NUM_VARIABLES 11
#define EEPROM_SLEEP 15 //TODO: acknowledge polling?, value is in ms

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

class RTC {
    i2c_inst_t* const i2c;

    u8 sda;
    u8 scl;

public:
    u64 features;
    u8 seed;
    u8 duration;

    RTC(bool I2C, u8 SDA, u8 SCL) : i2c(I2C ? i2c1 : i2c0), sda(SDA), scl(SCL){
        gpio_set_function(sda, GPIO_FUNC_I2C);
        gpio_set_function(scl, GPIO_FUNC_I2C);

        i2c_init(i2c, 350*1000); //The unit goes up to 400kHz

        gpio_pull_up(sda);//Active low
        gpio_pull_up(scl);
    }

    //Neither function checks for transmission errors, call after readVariables()
    void storeVariables(std::map<std::string, double> &variableMap){
        u8 packet[2 + 4*sizeof(double)] = {0, VARIABLE_OFFSET};

        //Can only write one page of 32 bytes at a time
        *((double*) &packet[2]) = variableMap["ans"];
        *((double*) &packet[10]) = variableMap["A"];
        *((double*) &packet[18]) = variableMap["B"];
        *((double*) &packet[26]) = variableMap["C"];
        i2c_write_blocking(i2c, EEPROM_ADDRESS, packet, sizeof(packet), false);
        sleep_ms(EEPROM_SLEEP);

        packet[1] = VARIABLE_OFFSET + 32;
        *((double*) &packet[2]) = variableMap["D"];
        *((double*) &packet[10]) = variableMap["E"];
        *((double*) &packet[18]) = variableMap["F"];
        *((double*) &packet[26]) = variableMap["G"];
        i2c_write_blocking(i2c, EEPROM_ADDRESS, packet, sizeof(packet), false);
        sleep_ms(EEPROM_SLEEP);

        packet[1] = VARIABLE_OFFSET + 2*32;
        *((double*) &packet[2]) = variableMap["H"];
        *((double*) &packet[10]) = variableMap["I"];
        *((double*) &packet[18]) = variableMap["J"];
        i2c_write_blocking(i2c, EEPROM_ADDRESS, packet, sizeof(packet)-sizeof(double), false);
        sleep_ms(EEPROM_SLEEP);
    }

    void readVariables(std::map<std::string, double> &variableMap) {
        double packet[NUM_VARIABLES];
        *((u8*) &packet[0]) = 0;
        *(((u8*) &packet[0])+1) = (u8) VARIABLE_OFFSET;

        i2c_write_blocking(i2c, EEPROM_ADDRESS, (u8*) packet, 2, false);
        i2c_read_blocking(i2c, EEPROM_ADDRESS, (u8*) packet, sizeof(packet), false);

        variableMap["ans"] = packet[0];
        variableMap["A"] = packet[1];
        variableMap["B"] = packet[2];
        variableMap["C"] = packet[3];
        variableMap["D"] = packet[4];
        variableMap["E"] = packet[5];
        variableMap["F"] = packet[6];
        variableMap["G"] = packet[7];
        variableMap["H"] = packet[8];
        variableMap["I"] = packet[9];
        variableMap["J"] = packet[10];
    }

    //TODO: Wear leveling
    void storeTest(u8 seed, u8 duration, u64 features){
        u32 expiry = getCurrentTime() + duration;

        u8 packet[3+sizeof(seed)+sizeof(features)+sizeof(expiry)] = {0, 0, MAGIC_NUM, seed};//First 2 bytes are the address in the EEPROM, third is magic
        *((u32*) &packet[4]) = expiry;
        *((u64*) &packet[8]) = features;

        i2c_write_blocking(i2c, EEPROM_ADDRESS, packet, sizeof(packet), false);
        //sleep_ms(EEPROM_SLEEP);
    }

    //Returns 1 when in test mode should be applied, 0 when it shouldn't, and -1 for integrity error
    //In that case, the relevant members (features, seed, duration) are valid
    int readTest(){
        u32 expiry;

        u8 packet[1+sizeof(seed)+sizeof(features)+sizeof(expiry)] = {0};

        if(i2c_write_blocking(i2c, EEPROM_ADDRESS, packet, 2, false) == PICO_ERROR_GENERIC){
            std::cout << "i2c write blocking" << std::endl;
            return -1;
        } //Set the internal address
        if(i2c_read_blocking(i2c, EEPROM_ADDRESS, packet, sizeof(packet), false) == PICO_ERROR_GENERIC)
            return -1;

        if(packet[0] == MAGIC_NUM){
            expiry = *((u32*) &packet[2]);

            u32 currentTime = getCurrentTime();
            if(currentTime == 0)//Happens when battery died OR communication error
                return -1;
            if(currentTime < expiry){
                seed = packet[1];
                features = *((u64*) &packet[6]);
                duration = expiry - currentTime;
                return true;
            }
        }
        return false;
    }

private:
    //TODO: Fix leap year
    u32 getCurrentTime(){
        u8 rawData[7] = {};
        unsigned short months[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

        i2c_write_blocking(i2c, TIMER_ADDRESS, (u8*) months, 1, false);
        i2c_read_blocking(i2c, TIMER_ADDRESS, rawData, 7, false);
        
        //calculate minutes
        u32 totalMins = (rawData[1] & 0x0F) + (rawData[1] >> 4) * 10;

        //calculate hours
        totalMins += 60*((rawData[2] & 0x0F) + 10*((rawData[2] & 0x30) >> 4));

        //calculate days
        totalMins += 60*24*((rawData[4] & 0x0F) + 10*((rawData[4] & 0xF0) >> 4)-1);

        //calculate months
        totalMins += 60*24*months[(rawData[5] & 0x0F) + 10*((rawData[4] & 0x10) >> 4) - 1];

        //calculate years
        totalMins += 60*24*365*((rawData[6] & 0x0F) + 10*((rawData[6] & 0xF0) >> 4));


        return totalMins;
    }
};


