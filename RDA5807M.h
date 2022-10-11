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
#include <stdint.h> // uint{8,16,32}_t

class RDA5807M {
private:
  uint16_t CHIPID;
  uint16_t b1,b2,b3,b4,b5,b6,b7;
  uint16_t Wr[7]; // 0x02..0x08
  uint16_t Rd[6]; // 0x0A..0x0F
  bool RDSR;
  bool STC;
  bool SF;
  bool RDSS;
  bool BLK_E;
  bool ST;
  uint16_t READCHAN;
  uint16_t RSSI;
  bool FM_TRUE;
  bool FM_READY;
  bool ABCD_E;
  uint8_t BLERA,BLERB;
  uint16_t RDSA,RDSB,RDSC,RDSD;

  bool DHIZ,DMUTE,MONO,BASS;
  bool RCLK_NON_CALIBRATE_MODE;
  bool RCLK_DIRECT_INPUT_MODE;
  bool SKMODE,SEEK,SEEKUP;
  uint8_t CLK_MODE;
  bool RDS_EN,NEW_METHOD,SOFT_RESET;
  bool ENABLE;
  uint16_t CHAN;
  bool DIRECT_MODE;
  bool TUNE;
  uint8_t BAND;
  uint8_t SPACE;
  bool STCIEN;
  bool RBDS;
  bool RDS_FIFO_EN;
  bool DE;
  bool RDS_FIFO_CLR;
  bool SOFTMUTE_EN;
  bool AFCD;
  bool I2S_ENABLE;
  uint8_t GPIO3,GPIO2,GPIO1;

  bool INT_MODE;
  uint8_t Seek_mode;
  uint8_t SEEKTH;
  uint8_t LNA_PORT_SEL;
  uint8_t LNA_ICSEL_BIT;
  uint8_t VOLUME;




  bool MODE_65MHz; // If 0x07h_bit<9> ( band )=1, 65-76MHz; =0, 50-76MHz  


  unsigned long lastRead;

  void Set(bool force = false);
  void Get(void);
  uint16_t Get(uint8_t Register);
public:
  RDA5807M();

  /* The Chip ID should read as 0x58xx, ie. 0x5804.
   */
  unsigned ChipId(void);

  /* false = No RDS/RBDS group ready (default)
   * true  = New RDS/RBDS group ready
   */
  bool RDS_ready(void);

  /* True, if a seek or tune operation completed.
   * false = Not complete
   * true  = Complete
   */
  bool TuneComplete(void);

  /* True, if the seek operation fails to find a channel with an RSSI
   *   level greater than SeekLevel().
   * false = Seek successful
   * true  = Seek failure
   */
  bool SeekFail(void);

  /* True, if the RDS decoder is synchronized.
   *   Available only in RDS verbose mode.
   * false = RDS decoder not synchronized(default)
   * true  = RDS decoder synchronized
   */
  bool RDS_sync(void);

  /* True, if an RDS Block E has been found (US only, RDSB).
   *   Available only if RDS enabled.
   * false = no Block E has been found
   * true  = Block E has been found
   */
  bool RDS_BlockE(void);

  /* Stereo Indicator.
   * false = Mono
   * true  = Stereo
   */
  bool StereoIndicator(void);

  /* Returns the current channel number, where Frequency is
   * set as
   *   Frequency = Channel Spacing (kHz) x Channel Number + Band Begin
   *
   *   Band Begin is
   *      Band 0: 87.0 MHz
   *      Band 1: 76.0 MHz
   *      Band 2: 76.0 MHz
   *      Band 3: 65.0 MHz (or 50MHz)
   * The Channel Number is updated after a tune or seek operation.
   */
  uint16_t ChannelNumber(void);

  /* Received Signal Strength Indicator (RSSI)
   *   returned value is logarithmic,
   *   0   = min
   *   127 = max
   */
  uint8_t SignalStrength(void);

  /* True, if the current channel is a station.
   * false = the current channel is not a station
   * true  = the current channel is a station
   */
  bool IsStation(void);

  /* No detailed explanation found for this flag.
   * false = FM not ready
   * true  = FM ready
   */
  bool FM_ready(void);

  /* Returns true, if RDS is US-style RDS (RDBS).
   * false = block id of registers 0x0C..0x0F is A,B,C,D 
   * true  = block id of registers 0x0C..0x0F is E
   */
  bool RDS_is_RDBS(void);

  /* Block Error Level of RDS BLOCK A or B (in RDS mode) or
   * BLOCK E (in RBDS mode, RDS_is_RDBS() is true).
   * 0 = no block errors
   * 1 = 1..2 errors requiring correction
   * 2 = 3..5 errors requiring correction
   * 3 = 6 or more errors or error in checkword,
   *     correction not possible
   * Available only in RDS Verbose mode
   */
  uint8_t RDS_BlockErrors_A(void);
  uint8_t RDS_BlockErrors_B(void);

  /* Returns RDS Block A..D (in RDS mode) or
   * Block E (in RBDS mode, RDS_is_RDBS() is true).
   * Error Handling and Decoding needs to be done in user space.
   */
  uint16_t RDS_BlockA(void);
  uint16_t RDS_BlockB(void);
  uint16_t RDS_BlockC(void);
  uint16_t RDS_BlockD(void);

  /* Audio Output On/Off.
   * false = High impedance;
   * true  = Normal operation (on)
   */
  void AudioEnable(bool On);

  /* Mute Output.
   * false = not muted
   * true  = muted
   */
  void Muted(bool On);

  /* Mono.
   * false = Stereo
   * true  = Force Mono
   */
  void Mono(bool On);

  /* Bass Boost.
   * false = no bass boost
   * true  = bass boost enabled
   */
  void BassBoost(bool On);

  /* RCLK always on.
   * false = RCLK clock is not always supplied when FM works
   * true  = RCLK clock is always supplied
   */
  void RCLK_Always(bool On);

  /* RCLK clock use the direct input mode
   */
  void RCLK_DirektInput(bool On);

  /* FM Station Seek Direction.
   * false = Seek down
   * true  = Seek up
   */
  void SeekDirection(bool Up);

  /* FM Station Seek.
   * false = stop seek
   * true  = enable seek, flag is reset
   *
   * NOTE: Reset to false by SF=1 or STC=1
   */
  void Seek(bool On);

  /* FM Station Seek, stop on band limits
   * false = wrap on band limit and continue
   * true  = stop on band limit
   */
  void SeekStopBandlimits(bool On);

  /* Set clock frequency.
   * 0: 32.768kHz
   * 1: 12MHz
   * 2: 13MHz
   * 3: 19.2MHz
   * 5: 24MHz
   * 6: 24MHz
   * 7: 38.4MHz
   * Any other value is invalid.
   */
  void ClockFrequency(int Choice);

  /* RDS enable (and US variant RDSB)
   */
  void RDS_enable(bool On);

  /* New Demodulate Method
   * May increase sensitity by 1dB
   */
  void NewDemod(bool On);

  /* Soft reset.
   */
  void SoftReset(bool On);

  /* Power Up Enable.
   */
  void PowerUp(bool On);

  /* Sets the current channel number, where Frequency is
   * set as
   *   Frequency = Channel Spacing (kHz) x Channel Number + Band Begin
   *
   *   Band Begin is
   *      Band 0: 87.0 MHz
   *      Band 1: 76.0 MHz
   *      Band 2: 76.0 MHz
   *      Band 3: 65.0 MHz (or 50MHz)
   */
  void ChannelNumber(uint16_t Channel);

  /* Directly Control Mode, Only used when test.
   */
  void TestMode(bool On);

  /* Start/Stop a tune operation.
   */
  void Tune(bool On);

  /* Band Select.
   * 0: 87–108MHz (US/Europe)
   * 1: 76–91MHz (Japan)
   * 2: 76–108MHz (world wide)
   * 3: 65–76MHz （East Europe）
   * 4: 50-65MHz
   */
  void Band(int Choice);

  /* Channel Spacing.
   * 0: 100kHz
   * 1: 200kHz
   * 2:  50kHz
   * 3:  25KHz
   */
  void ChannelSpacing(int Choice);

  /* Seek/Tune Complete Interrupt Enable.
   * Generate a low pulse on GPIO2, when complete.
   */
  void SeekTuneInterrupt(bool On);

  /* Enable RDBS (Radio Broadcast Data System) support.
   * Only used in US.
   */
  void RBDS_enable(bool On);

  /* RDS fifo mode enable.
   */
  void RDS_FIFO_mode(bool On);

  /* FM De-emphasis.
   * false: 75 μs (US)
   * true : 50 μs (Europe)
   */
  void Deemphasis(bool Europe);

  /* On = clear RDS fifo
   */
  void Clear_RDS_FIFO(bool On);

  /* softmute enable
   */
  void SoftMute(bool On);

  /* enable/disable AFC.
   */  
  void AFC(bool On);

  /* enable/disable digital I2S audio.
   */  
  void I2S(bool On);

  /* Set GPIO function.
   * GPIO  : 1 or 2 or 3
   * Choice:
   *   0: high impedance
   *   1: special purpose.
   *      GPIO3: Mono/Stereo indicator
   *      GPIO2: Interrupt Output
   *      GPIO1: do not use.
   *   2: high
   *   3: low
   */
  void SetGPIO(int GPIO, int Choice);

  /* Seek/Tune Interrupt mode.
   * false: 5ms
   * true : wait until Reg 0x0C was read
   */
  void InterruptMode(bool Wait);

  /* add RSSI (signal strength) seek mode.
   */
  void RSSISeekMode(bool On);

  /* Seek SNR threshold value.
   *   0..15, default:8 (-71dBm)
   */
  void SeekThreshold(int Value);

  /* Select Low Noise Amplifier input port.
   * 0: no input
   * 1: (-) input
   * 2: (+) input (default)
   * 3: dual port input
   */
  void LNA_InputPort(int Value);

  /* Select Low Noise Amplifier current.
   * 0: 1.8mA
   * 1: 2.1mA
   * 2: 2.5mA
   * 3: 3.0mA
   */
  void LNA_Current(int Value);

  /* Audio Volume.
   *   0..15, default:11
   */
  void Volume(int Value);


};
