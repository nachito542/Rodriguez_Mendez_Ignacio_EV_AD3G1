#include "ds1302.h"

int CE=7;
int ent=3;
int SCLK= 5;
int dato=0;
int w_p=0;
int t_seg=0;
int t_mins=0;
int t_hora=0;


void ds1302_init(){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN|RCC_APB2ENR_AFIOEN;

    GPIOA->CRH &=~(0XF<<(CE%8)*4);
    GPIOA->CRH |=(0X4<<(CE%8)*4);
    GPIOA->BSRR |=(1<<CE+16);

    GPIOA->CRH &=~(0XF<<(SCLK%8)*4);
    GPIOA->CRH |=(0X1<<(SCLK%8)*4);
}

void io_output(){

 GPIOA->CRL&=~(0XF<<ent*4);
 GPIOA->CRL|=(0X1<<ent*4);

}

void io_input(){

 GPIOA->CRL&=~(0XF<<ent*4);
 GPIOA->CRL|=(0X4<<ent*4);
 GPIOA->BSRR|=(1<<ent+16);

}

void sclk_high(){
 GPIOA->BSRR|=(1<<SCLK);

}
void sclk_low(){
 GPIOA->BSRR|=(1<<SCLK+16);
}

void ce_high(){
 GPIOA->BSRR|=(1<<CE);
    
}

void ce_low(){
 GPIOA->BSRR|=(1<<CE+16);
}

uint8_t ds1302_read(uint8_t address){
    
    ce_high();
    sclk_low();
    io_output();
    
    for(int i=0;i<8;i++){
        dato=((address>>i)&0X01);
        if(dato)GPIOA->BSRR|=(1<<ent);
        else GPIOA->BSRR|=(1<<ent+16);
        sclk_high();
        sclk_low();
    }
    return dato;
}


void ds1302_write(uint8_t address, uint8_t data){
    ce_high();
    io_intput();
    for(int i=0;i<8;i++){
        if(((address>>i)&0X01)) GPIOA->BSRR|=(1<<ent);
        else  GPIOA->BSRR|=(1<<ent+16);
        sclk_high();
        sclk_low();
    }
}

void ds1302_set_time(uint8_t hora, uint8_t min, uint8_t seg){
    t_seg=(seg>>4)+(seg&0X0F);
    t_mins=((min>>4)+(min&0X0F));
    t_hora=(hora>>4)+(hora&0X0F);

    ds1302_write(0X80,t_seg);
    ds1302_write(0X82,t_mins);
    ds1302_write(84,t_hora);

}

uint8_t ds1302_get_seconds(){
    int  segs_read=ds1302_read(0X80);
    t_seg=(segs_read>>4)+(segs_read&0X0F);
}

uint8_t ds1302_get_minutes(){
     int  segs_read=ds1302_read(0X82);
    t_seg=(segs_read>>4)+(segs_read&0X0F);
}

uint8_t ds1302_get_minutes(){
     int  segs_read=ds1302_read(0X84);
    t_seg=(segs_read>>4)+(segs_read&0X0F);
}