#ifndef DS1302_H
#define DS1302_H

#include "stm32f103xb.h"

void ds1302_init();//ya esta
void ds1302_write(uint8_t address, uint8_t data);//ya esta
uint8_t ds1302_read(uint8_t address);//ya esta
void ds1302_set_time(uint8_t hora, uint8_t min, uint8_t seg);//medio completar
uint8_t ds1302_get_hours();
uint8_t ds1302_get_minutes();
uint8_t ds1302_get_seconds();//

void io_output();//ya esta
void io_input();//ya esta

void sclk_high();//ya esta
void sclk_low();//ya esta

void ce_high();//ya esta
void ce_low();//ya esta


#endif