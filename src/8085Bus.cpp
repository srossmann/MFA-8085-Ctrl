#include "avr/io.h"
#include "8085Bus.h"
#include <Arduino.h>



void driveBusControl()
{
  drive_RST();
  delay(10);
  release_RST();
  drive_BUSREQ();
  while (!is_BUSACK());
  drive_MREQ();
  drive_IORQ();
  drive_WR();
  drive_RD();
  drive_ROM_WR();
}

void releaseBusControl()
{
  drive_RST();
  release_WR();
  release_RD();
  release_MREQ();
  release_IORQ();
  release_BUSREQ();
  release_ROM_WR();
  delay(10);
  release_RST();
}

void writeMem(uint16_t addr, uint8_t dt)
{
  drive_ADDR_BUS();
  put_ADDR_BUS(addr);
  drive_DATA_BUS();
  put_DATA_BUS(dt);
  set_MREQ(0);
#if USE_ROM_WR
  set_ROM_WR(0);
#else
  set_WR(0);
#endif
  NOP;
  NOP;
#if USE_ROM_WR
  set_ROM_WR(1);
#else
  set_WR(1);
#endif
  set_MREQ(1);
  NOP;
  release_DATA_BUS();
  release_ADDR_BUS();
}

uint8_t readMem(uint16_t addr)
{
  uint8_t dt;

  drive_ADDR_BUS();
  put_ADDR_BUS(addr);
  set_MREQ(0);
  set_RD(0);
  NOP;
  NOP; // until 120ns required
  dt = get_DATA_BUS();
  set_RD(1);
  set_MREQ(1);
  NOP;
  release_ADDR_BUS();

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

  set_WR(0);

  delay(1000);
  NOP;
  NOP;
  set_WR(1);

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




