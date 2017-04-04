/**
Author: Michael Monaghan
Facilitates comunication using the RFM95 series of chips.
*/
#include <fcntl.h>
#include <string>
#include <iostream>
#include "Radio.hpp"
#include "spidev.h"
#include <sys/ioctl.h>
using namespace std;

Radio::Radio() {
  fd = open("/dev/spi0.0", O_RDWR);
  if (fd < 0) {
    cerr << "open() error: " << fd << endl;
    error = true;
  } else {
    error = false;
  }
}

void Radio::setReg(unsigned char reg, unsigned char value) {
  reg |= 0x80;
  struct spi_ioc_transfer mesg[2] = {{
    .tx_buf = (__u64) &reg,
    .rx_buf = (__u64) NULL,
    .len = 1,
    .speed_hz = 0,
    .delay_usecs = 0,
    .bits_per_word = 0,
    .cs_change = 0,
    .tx_nbits = 0,
    .rx_nbits = 0,
    .pad = 0
  },{
    .tx_buf = (__u64) &value,
    .rx_buf = (__u64) NULL,
    .len = 1,
    .speed_hz = 0,
    .delay_usecs = 0,
    .bits_per_word = 0,
    .cs_change = 1,
    .tx_nbits = 0,
    .rx_nbits = 0,
    .pad = 0
  }};
  int status = ioctl(fd, SPI_IOC_MESSAGE(2), mesg);
}
unsigned char Radio::readReg(unsigned char reg) {
  unsigned char value;
  struct spi_ioc_transfer mesg[2] = {
    {
      .tx_buf = (__u64) &reg,
      .rx_buf = (__u64) NULL,
      .len = 1,
      .speed_hz = 0,
      .delay_usecs = 0,
      .bits_per_word = 0,
      .cs_change = 0,
      .tx_nbits = 0,
      .rx_nbits = 0,
      .pad = 0
    },{
      .tx_buf = (__u64) NULL,
      .rx_buf = (__u64) &value,
      .len = 1,
      .speed_hz = 0,
      .delay_usecs = 0,
      .bits_per_word = 0,
      .cs_change = 1,
      .tx_nbits = 0,
      .rx_nbits = 0,
      .pad = 0
    }
  };
  int status = ioctl(fd, SPI_IOC_MESSAGE(2), mesg);
  return value;
}
void Radio::readFIFO(void *data, unsigned char length) {
  unsigned char reg = 0x00;
  struct spi_ioc_transfer mesg[2] = {
    {
      .tx_buf = (__u64) &reg,
      .rx_buf = (__u64) NULL,
      .len = 1,
      .speed_hz = 0,
      .delay_usecs = 0,
      .bits_per_word = 0,
      .cs_change = 0,
      .tx_nbits = 0,
      .rx_nbits = 0,
      .pad = 0
    },{
      .tx_buf = (__u64) NULL,
      .rx_buf = (__u64) data,
      .len = length,
      .speed_hz = 0,
      .delay_usecs = 0,
      .bits_per_word = 0,
      .cs_change = 1,
      .tx_nbits = 0,
      .rx_nbits = 0,
      .pad = 0
    }
  };
  int status = ioctl(fd, SPI_IOC_MESSAGE(2), mesg);
}
void Radio::writeFIFO(const void *data, unsigned char length) {
  unsigned char reg = 0x80;
  struct spi_ioc_transfer mesg[2] = {
    {
      .tx_buf = (__u64) &reg,
      .rx_buf = (__u64) NULL,
      .len = 1,
      .speed_hz = 0,
      .delay_usecs = 0,
      .bits_per_word = 0,
      .cs_change = 0,
      .tx_nbits = 0,
      .rx_nbits = 0,
      .pad = 0
    },{
      .tx_buf = (__u64) data,
      .rx_buf = (__u64) NULL,
      .len = length,
      .speed_hz = 0,
      .delay_usecs = 0,
      .bits_per_word = 0,
      .cs_change = 1,
      .tx_nbits = 0,
      .rx_nbits = 0,
      .pad = 0
    }
  };
  int status = ioctl(fd, SPI_IOC_MESSAGE(2), mesg);
}

void Radio::init() {
  // LoRa and standby mode
  setReg(0x01, 0x81);

  // 915MHz center frequency
  // Target * 2^19 / 32,000,000
  setReg(0x06, 0xE4);
  setReg(0x07, 0xC0);
  setReg(0x08, 0x00);

  // 500KHz bandwidth, 4/5 coding rate, Implicit header
  setReg(0x1D, 0x93);

  // SF=12, CRC on
  setReg(0x1E, 0xC4);

  // Payload Length = 1 //TODO
  setReg(0x22, PACKET_LENGTH);

  // Auto LNA Gain
  setReg(0x24, 0x04);

  // enable interrupts
  setReg(0x11, 0xE8);
}

void Radio::startReceiving() {
  // Continous receive mode
  setReg(0x01, 0x85);
  // clear all interrupts
  setReg(0x12, 0xFF);
  // mask all the others
  setReg(0x11, ~0x60);
}

void Radio::stopReceiving() {
  setReg(0x01, 0x81);
}

void Radio::receive(void *data) {
  // Wait for receipt of packet
  //TODO use GPIO interrupt
  while ((getReg(0x12) & 0x60)!=0x40) {
  }
  // clear interrupts
  setReg(0x12, 0xFF);
  // set FIFO pointer
  setReg(0x0D, getReg(0x10));
  readFIFO(data, PACKET_LENGTH);
}

void Radio::transmit(const void *data) {
  // clear all interrupts
  setReg(0x12, 0xFF);
  // mask all the others
  setReg(0x11, ~0x80);
  // Wait for receipt of packet
  //TODO use GPIO interrupt
  while ((getReg(0x12) & 0x80)!=0x80) {
  }
  // clear interrupts
  setReg(0x12, 0xFF);
}
