/*******************************************************************************
 * RDA5807M single-chip I2C FM stereo radio arduino library
 * Copyright (C) 2022  Winfried Koehler
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * The latest version of this library can always be found at it's github page,
 * https://github.com/wirbel-at-vdr-portal/RDA5807M
 ******************************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include "RDA5807M.h"

/*******************************************************************************
 * constants
 ******************************************************************************/
static constexpr uint8_t Address = 0x10; // 7-bit I2C chip address (0010000b)

#define RD16(dst)  \
    dst = ((uint16_t)Wire.read() << 8) | Wire.read()


RDA5807M::RDA5807M(void) : b1(0),b2(0),b3(0),b4(0),b5(0),b6(0),b7(0),
  DHIZ(true),DMUTE(true),MONO(false),BASS(false),
  RCLK_NON_CALIBRATE_MODE(false),
  RCLK_DIRECT_INPUT_MODE(false),
  SKMODE(true),SEEK(false),SEEKUP(true),
  CLK_MODE(0),
  RDS_EN(false),NEW_METHOD(false),SOFT_RESET(false),

  ENABLE(true),CHAN(5),DIRECT_MODE(false),
  TUNE(false),BAND(0),SPACE(0),STCIEN(false),
  RBDS(false),RDS_FIFO_EN(false),DE(1),


  lastRead(0) {
  Get();
  CHIPID = Get(0x00);
}

unsigned RDA5807M::ChipId(void) {
  return CHIPID;
}

bool RDA5807M::RDS_ready(void) {
  Get();
  return RDSR;
}

bool RDA5807M::TuneComplete(void) {
  Get();
  return STC;
}

bool RDA5807M::SeekFail(void) {
  Get();
  return SF;
}

bool RDA5807M::RDS_sync(void) {
  Get();
  return RDSS;
}

bool RDA5807M::RDS_BlockE(void) {
  Get();
  return BLK_E;
}

bool RDA5807M::StereoIndicator(void) {
  Get();
  return ST;
}

uint16_t RDA5807M::ChannelNumber(void) {
  Get();
  return READCHAN;
}

uint8_t RDA5807M::SignalStrength(void) {
  Get();
  return RSSI;
}

bool RDA5807M::IsStation(void) {
  Get();
  return FM_TRUE;
}

bool RDA5807M::FM_ready(void) {
  Get();
  return FM_READY;
}

bool RDA5807M::RDS_is_RDBS(void) {
  Get();
  return ABCD_E;
}

uint8_t RDA5807M::RDS_BlockErrors_A(void) {
  Get();
  return BLERA;
}

uint8_t RDA5807M::RDS_BlockErrors_B(void) {
  Get();
  return BLERB;
}

uint16_t RDA5807M::RDS_BlockA(void) {
  Get();
  return RDSA;
}

uint16_t RDA5807M::RDS_BlockB(void) {
  Get();
  return RDSB;
}

uint16_t RDA5807M::RDS_BlockC(void) {
  Get();
  return RDSC;
}

uint16_t RDA5807M::RDS_BlockD(void) {
  Get();
  return RDSD;
}

void RDA5807M::AudioEnable(bool On) {
  DHIZ = not On;
  Set();
}

void RDA5807M::Muted(bool On) {
  DMUTE = not On;
  Set();
}

void RDA5807M::Mono(bool On) {
  MONO = On;
  Set();
}

void RDA5807M::BassBoost(bool On) {
  BASS = On;
  Set();
}

void RDA5807M::RCLK_Always(bool On) {
  RCLK_NON_CALIBRATE_MODE = not On;
  Set();
}

void RDA5807M::RCLK_DirektInput(bool On) {
  RCLK_DIRECT_INPUT_MODE = On;
  Set();
}

void RDA5807M::SeekStopBandlimits(bool On) {
  SKMODE = On;
  Set();
}

void RDA5807M::Seek(bool On) {
  SEEK = On; //  NOTE: Reset to false by SF=1 or STC=1
  Set();
}

void RDA5807M::SeekDirection(bool Up) {
  SEEKUP = Up;
  Set();
}

void RDA5807M::ClockFrequency(int Choice) {
  CLK_MODE = (Choice >= 0) && (Choice <= 7)? Choice : 0;
  Set();
}

void RDA5807M::SoftReset(bool On) {
  SOFT_RESET = On;
  Set();
  SOFT_RESET = false; // do not trigger twice.
}

void RDA5807M::NewDemod(bool On) {
  NEW_METHOD = On;
  Set();
}

void RDA5807M::RDS_enable(bool On) {
  RDS_EN = On;
  Set();
}

void RDA5807M::PowerUp(bool On) {
  ENABLE = On;
  Set();
}

void RDA5807M::ChannelNumber(uint16_t Channel) {
  CHAN = Channel & 0x3FF;
  Set();
}

void RDA5807M::TestMode(bool On) {
  DIRECT_MODE = On;
  Set();
}

void RDA5807M::Tune(bool On) {
  TUNE = On;
  Set();
}

void RDA5807M::Band(int Choice) {
  MODE_65MHz = Choice == 4 ? false : true;
  BAND = (Choice == 4) ? 3 : Choice & 3;
  Set();
}

void RDA5807M::ChannelSpacing(int Choice) {
  SPACE = Choice & 3;
  Set();
}

void RDA5807M::SeekTuneInterrupt(bool On) {
  STCIEN = On;
  Set();
}

void RDA5807M::RBDS_enable(bool On) {
  RBDS = On;
  Set();
}

void RDA5807M::RDS_FIFO_mode(bool On) {
  RDS_FIFO_EN = On;
  Set();
}

void RDA5807M::Deemphasis(bool Europe) {
  DE = Europe;
  Set();
}






/*******************************************************************************
 * General members following.
 ******************************************************************************/

void RDA5807M::Set(bool force) {
  uint16_t u1=0,u2=0,u3=0,u4=0,u5=0,u6=0,u7=0;

  if (force ||
      (u1 != b1) || (u2 != b2) || (u3 != b3) || (u4 != b4) ||
      (u5 != b5) || (u6 != b6) || (u7 != b7)) {
     Wire.beginTransmission(Address); 
     Wire.write(u1);
     Wire.write(u2);
     Wire.write(u3);
     Wire.write(u4);
     Wire.write(u5);
     Wire.write(u6);
     Wire.write(u7);
     Wire.endTransmission();
     b1 = u1;
     b2 = u2;
     b3 = u3;
     b4 = u4;
     b5 = u5;
     b6 = u6;
     b7 = u7;
     }
}

uint16_t RDA5807M::Get(uint8_t Register) {
  Wire.beginTransmission(Address + 1);
  Wire.write(Register);
  Wire.endTransmission(false);

  Wire.requestFrom(Address + 1, 1 * sizeof(uint16_t));
  uint16_t result;
  RD16(result);
  Wire.endTransmission();
  return result;
}

void RDA5807M::Get(void) {
  if (millis() < (lastRead + 500))
     return;

  lastRead = millis();
  Wire.beginTransmission(Address);
  Wire.requestFrom(Address, 9 * sizeof(uint16_t));
  if (Wire.available() >= 9 * sizeof(uint16_t)) {
     RD16(Rd[0]); // 0x0A
     RD16(Rd[1]); // 0x0B
     RD16(Rd[2]); // 0x0C
     RD16(Rd[3]); // 0x0D
     RD16(Rd[4]); // 0x0E
     RD16(Rd[5]); // 0x0F
     }
  Wire.endTransmission();

  RDSR     = (Rd[0] & 0x8000) > 0;
  STC      = (Rd[0] & 0x4000) > 0;
  SF       = (Rd[0] & 0x2000) > 0;
  RDSS     = (Rd[0] & 0x1000) > 0;
  BLK_E    = (Rd[0] & 0x800 ) > 0;
  ST       = (Rd[0] & 0x400 ) > 0;
  READCHAN =  Rd[0] & 0x3FF;

  RSSI     =  Rd[1] >> 9;
  FM_TRUE  = (Rd[1] & 0x100 ) > 0;
  FM_READY = (Rd[1] & 0x80  ) > 0;
  ABCD_E   = (Rd[1] & 0x10  ) > 0;
  BLERA    = (Rd[1] & 0xC   ) >> 2;
  BLERB    = (Rd[1] & 0x3   );

  RDSA     =  Rd[2];
  RDSB     =  Rd[3];
  RDSC     =  Rd[4];
  RDSD     =  Rd[5];
}
