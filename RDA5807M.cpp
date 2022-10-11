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
  RDS_FIFO_CLR(false),SOFTMUTE_EN(false),
  AFCD(false),I2S_ENABLE(false),
  GPIO3(0),GPIO2(0),GPIO1(0),


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

void RDA5807M::Clear_RDS_FIFO(bool On) {
  RDS_FIFO_CLR = On;
  Set();
}

void RDA5807M::SoftMute(bool On) {
  SOFTMUTE_EN = On;
  Set();
}

void RDA5807M::AFC(bool On) {
  AFCD = not On;
  Set();
}

void RDA5807M::I2S(bool On) {
  I2S_ENABLE = On;
  Set();
}

void RDA5807M::SetGPIO(int GPIO, int Choice) {
  if ((Choice < 0) or (Choice > 3))
     return;
  if ((GPIO < 1) or (GPIO > 3))
     return;
  if      (GPIO == 1) GPIO1 = Choice;
  else if (GPIO == 2) GPIO2 = Choice;
  else                GPIO3 = Choice;
}




/*******************************************************************************
 * General members following.
 ******************************************************************************/

void RDA5807M::Set(bool force) {
  uint16_t u1=0,u2,u3=0,u4=0,u5=0,u6=0,u7=0;

  //--
  if (DHIZ)                    u1 |= (1 << 15);
  if (DMUTE)                   u1 |= (1 << 14);
  if (MONO)                    u1 |= (1 << 13);
  if (BASS)                    u1 |= (1 << 12);
  if (RCLK_NON_CALIBRATE_MODE) u1 |= (1 << 11);
  if (RCLK_DIRECT_INPUT_MODE)  u1 |= (1 << 10);
  if (SEEKUP)                  u1 |= (1 << 9);
  if (SEEK)                    u1 |= (1 << 8);
  if (SKMODE)                  u1 |= (1 << 7);
                               u1 |= (CLK_MODE << 4);
  if (RDS_EN)                  u1 |= (1 << 3);
  if (NEW_METHOD)              u1 |= (1 << 2);
  if (SOFT_RESET)              u1 |= (1 << 1);
  if (ENABLE)                  u1 |= 1;
  //--
                               u2  = (CHAN << 6);
  if (DIRECT_MODE)             u2 |= (1 << 5);
  if (TUNE)                    u2 |= (1 << 4);
                               u2 |= (BAND << 2);
                               u2 |= (SPACE);
  //--
  if (STCIEN)                  u3 |= (1 << 14);
  if (RBDS)                    u3 |= (1 << 13);
  if (RDS_FIFO_EN)             u3 |= (1 << 12);
  if (DE)                      u3 |= (1 << 11);
  if (RDS_FIFO_CLR)            u3 |= (1 << 10);
  if (SOFTMUTE_EN)             u3 |= (1 << 9);
  if (AFCD)                    u3 |= (1 << 8);
  if (I2S_ENABLE)              u3 |= (1 << 6);
                               u3 |= (GPIO3 << 4);
                               u3 |= (GPIO2 << 2);
                               u3 |= (GPIO1);
  //--








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
