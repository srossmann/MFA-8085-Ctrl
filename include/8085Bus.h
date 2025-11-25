/********************************************/
/*                                          */
/*                                          */
/*                                          */
/*                                          */
/*                                          */
/*                                          */
/*                                          */
/*                                          */
/*                                          */
/*                                          */
/********************************************/
#include "avr/io.h"

/********************************************/
/*                                          */
/********************************************/
#define set_WR(sv) bitWrite(PORTG, PG0, sv)
#define drive_WR() \
  set_WR(1);       \
  bitWrite(DDRG, PG0, 1)
#define release_WR() bitWrite(DDRG, PG0, 0)

/********************************************/
/*                                          */
/********************************************/
#define set_RD(sv) bitWrite(PORTG, PG1, sv)
#define drive_RD() \
  set_RD(1);       \
  bitWrite(DDRG, PG1, 1)
#define release_RD() bitWrite(DDRG, PG1, 0)

/********************************************/
/*                                          */
/********************************************/
#define set_MREQ(sv) bitWrite(PORTG, PG2, sv)
#define drive_MREQ() \
  set_MREQ(1);       \
  bitWrite(DDRG, PG2, 1)
#define release_MREQ() bitWrite(DDRG, PG2, 0)

/********************************************/
/*                                          */
/********************************************/
#define set_IORQ(sv) bitWrite(PORTD, PD7, sv)
#define drive_IORQ() \
  set_IORQ(1);       \
  bitWrite(DDRD, PD7, 1)
#define release_IORQ() bitWrite(DDRD, PD7, 0)

/********************************************/
/*                                          */
/********************************************/
#define set_ROM_WR(sv) bitWrite(PORTB, PB7, sv)
#define drive_ROM_WR() \
  set_ROM_WR(1);       \
  bitWrite(DDRB, PB7, 1)
#define release_ROM_WR()  \
  bitWrite(DDRB, PB7, 0); \
  set_ROM_WR(0)

/********************************************/
/*                                          */
/********************************************/
#define drive_RST()        \
  bitWrite(PORTB, PB6, 0); \
  bitWrite(DDRB, PB6, 1)
#define release_RST() bitWrite(DDRB, PB6, 0)

/********************************************/
/*                                          */
/********************************************/
#define drive_BUSREQ()     \
  bitWrite(PORTB, PB4, 0); \
  bitWrite(DDRB, PB4, 1)
#define release_BUSREQ() bitWrite(DDRB, PB4, 0)

/********************************************/
/*                                          */
/********************************************/
#define is_BUSACK() bit_is_clear(PINB, PB5)

/********************************************/
/*                                          */
/********************************************/
#define put_ADDR_BUS(addr) \
  PORTA = lowByte(addr);   \
  PORTC = highByte(addr)
#define drive_ADDR_BUS() DDRA = DDRC = 0xFF
#define release_ADDR_BUS() DDRA = PORTA = DDRC = PORTC = 0x00

/********************************************/
/*                                          */
/********************************************/
#define get_DATA_BUS() PINL
#define drive_DATA_BUS() DDRL = 0xFF
#define put_DATA_BUS(dt) PORTL = (dt)
#define release_DATA_BUS() DDRL = PORTL = 0x00

/********************************************/
/*                                          */
/********************************************/
#define NOP __asm__ __volatile__("nop\n\t")

/********************************************/
/*                                          */
/********************************************/
#define ROM_ADDRESS 0x0000
/********************************************/
/*                                          */
/********************************************/
#define ROM_SIZE 0x0800 // 2kB

/********************************************/
/*                                          */
/********************************************/
#define SDCARD_CS_PIN 53

void driveBusControl();