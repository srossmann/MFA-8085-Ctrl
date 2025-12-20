#include "avr/io.h"
#include "8085Bus.h"
#include <Arduino.h>

void setMEMR(int b)
{

  bitWrite(DDRG, PG0, 1);
  bitWrite(PORTG, PG0, b);
  NOP;
  bitWrite(DDRG, PG0, 0);
  NOP;
}

void setMEMW(int b)
{

  bitWrite(DDRG, PG2, 1);
  bitWrite(PORTG, PG2, b);
  NOP;
  bitWrite(DDRG, PG2, 0);
  NOP;
}

bool getDMA()
{
  bool b = false;
  // HOLD = 1
  bitWrite(DDRB, PB5, 1);
  bitWrite(PORTB, PB5, 0);

  DDRB &= ~(1 << PB4);

  uint8_t val = 1;
  while (val == 0)
  {
    val = (PINB & (1 << PB4)) != 0;
  }
  return b;
}

bool resetDMA()
{
  bool b = false;
  // HOLD = 1
  bitWrite(DDRB, PB5, 1);
  bitWrite(PORTB, PB5, 1);
  return b;
}

void SetReset()
{
  drive_RESET();
}

void ResetReset()
{
  drive_RESET();
  set_RESET(0);
  delay(1000);
  release_RESET();
}

void XdriveBusControl()
{
  drive_RST();
  delay(10);
  release_RST();
  drive_BUSREQ();
  getDMA();
  // while (!is_BUSACK());
  // drive_MREQ();
  drive_IORQ();
  drive_WR();
  drive_RD();
  drive_ROM_WR();
}

void driveBusControl()
{

  delay(10);

  getDMA();
}

void releaseBusControl()
{
  drive_RST();
  release_WR();
  release_RD();
  //  release_MREQ();
  release_IORQ();
  release_BUSREQ();
  release_ROM_WR();
  delay(10);
  release_RST();
  resetDMA();
}

void writeMem(uint16_t addr, uint8_t dt)
{
  drive_ADDR_BUS();
  drive_DATA_BUS();

  put_ADDR_BUS(addr);
  put_DATA_BUS(dt);

  setMEMW(0);

  NOP;
  NOP;

  setMEMW(1);

  NOP;
  release_DATA_BUS();
  release_ADDR_BUS();
}

uint8_t readMem(uint16_t addr)
{
  uint8_t dt;
  put_ADDR_BUS(addr);
  setMEMR(0);
  NOP;
  NOP; // until 120ns required
  dt = get_DATA_BUS();
  NOP;
  setMEMR(1);
  NOP;
  NOP;
  return dt;
}

void writeIO(uint16_t addr, uint8_t dt)
{
  driveBusControl();

  drive_ADDR_BUS();
  put_ADDR_BUS(addr);
  drive_DATA_BUS();
  put_DATA_BUS(dt);
  set_IORQ(0);

  set_MEMR(0);

  delay(1000);
  NOP;
  NOP;
  set_MEMR(1);

  set_IORQ(1);
  // set_IORQ(1);
  NOP;
  release_DATA_BUS();
  release_ADDR_BUS();
  releaseBusControl();
}

uint8_t readIO(uint16_t addr)
{

  uint8_t dt;
  drive_ADDR_BUS();
  put_ADDR_BUS(addr);
  set_IORQ(0);
  set_RD(0);
  NOP;
  NOP; // until 120ns required
  dt = get_DATA_BUS();
  set_RD(1);
  set_IORQ(1);
  NOP;
  release_ADDR_BUS();
  return dt;
}
