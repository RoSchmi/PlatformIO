// Version for SDM530MT Modbus HeatingCurrentSurvey Sensor Version 1.0
// Mainboard Feather M0 Rfm69 433 MHz

// Copyright RoSchmi, 2020. License: GNU General Public License

// Debugging hints:
// https://community.platformio.org/t/problems-starting-debug-session-with-jlink-on-feather-m0/12291
//

// For debugging The bootloader must be overwritten (Fuse set to 0 Bytes- see link)
// For debugging with PlatformIO The file: adafruit_feather_m0_no_bootloader.json must be
// in folder 'boards'
// platformio.ini must contain the line: board = adafruit_feather_m0_no_bootloader

// For 'Release' Version
// platformio.ini must contain the line: board = adafruit_feather_m0


// This Application is an adaption from Felix Rusu's RFM69 library

/* RFM69 library and code by Felix Rusu - felix@lowpowerlab.com
// Get libraries at: https://github.com/LowPowerLab/
// Make sure you adjust the settings in the configuration section below !!!
// **********************************************************************************
// Copyright Felix Rusu, LowPowerLab.com
// Library and code by Felix Rusu - felix@lowpowerlab.com
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License as published by the Free Software
// Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General
// Public License along with this program.
// If not, see <http://www.gnu.org/licenses></http:>.
//
// Licence can be viewed at
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************/

//#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include "wiring_private.h" // pinPeripheral() function
#include "variant.h"
#include <ModbusRtu.h>
#include "Reform_uint16_2_float32.h"
#include "Read_2_InputRegisters.h"
#include "DataContainer.h"

//#define DebugPrint

//************************************************************
#define REG_FIFO          0x00
#define REG_OPMODE        0x01
#define REG_DATAMODUL     0x02
#define REG_BITRATEMSB    0x03
#define REG_BITRATELSB    0x04
#define REG_FDEVMSB       0x05
#define REG_FDEVLSB       0x06
#define REG_FRFMSB        0x07
#define REG_FRFMID        0x08
#define REG_FRFLSB        0x09
#define REG_OSC1          0x0A
#define REG_AFCCTRL       0x0B
#define REG_LOWBAT        0x0C
#define REG_LISTEN1       0x0D
#define REG_LISTEN2       0x0E
#define REG_LISTEN3       0x0F
#define REG_VERSION       0x10
#define REG_PALEVEL       0x11
#define REG_PARAMP        0x12
#define REG_OCP           0x13
#define REG_AGCREF        0x14  // not present on RFM69/SX1231
#define REG_AGCTHRESH1    0x15  // not present on RFM69/SX1231
#define REG_AGCTHRESH2    0x16  // not present on RFM69/SX1231
#define REG_AGCTHRESH3    0x17  // not present on RFM69/SX1231
#define REG_LNA           0x18
#define REG_RXBW          0x19
#define REG_AFCBW         0x1A
#define REG_OOKPEAK       0x1B
#define REG_OOKAVG        0x1C
#define REG_OOKFIX        0x1D
#define REG_AFCFEI        0x1E
#define REG_AFCMSB        0x1F
#define REG_AFCLSB        0x20
#define REG_FEIMSB        0x21
#define REG_FEILSB        0x22
#define REG_RSSICONFIG    0x23
#define REG_RSSIVALUE     0x24
#define REG_DIOMAPPING1   0x25
#define REG_DIOMAPPING2   0x26
#define REG_IRQFLAGS1     0x27
#define REG_IRQFLAGS2     0x28
#define REG_RSSITHRESH    0x29
#define REG_RXTIMEOUT1    0x2A
#define REG_RXTIMEOUT2    0x2B
#define REG_PREAMBLEMSB   0x2C
#define REG_PREAMBLELSB   0x2D
#define REG_SYNCCONFIG    0x2E
#define REG_SYNCVALUE1    0x2F
#define REG_SYNCVALUE2    0x30
#define REG_SYNCVALUE3    0x31
#define REG_SYNCVALUE4    0x32
#define REG_SYNCVALUE5    0x33
#define REG_SYNCVALUE6    0x34
#define REG_SYNCVALUE7    0x35
#define REG_SYNCVALUE8    0x36
#define REG_PACKETCONFIG1 0x37
#define REG_PAYLOADLENGTH 0x38
#define REG_NODEADRS      0x39
#define REG_BROADCASTADRS 0x3A
#define REG_AUTOMODES     0x3B
#define REG_FIFOTHRESH    0x3C
#define REG_PACKETCONFIG2 0x3D
#define REG_AESKEY1       0x3E
#define REG_AESKEY2       0x3F
#define REG_AESKEY3       0x40
#define REG_AESKEY4       0x41
#define REG_AESKEY5       0x42
#define REG_AESKEY6       0x43
#define REG_AESKEY7       0x44
#define REG_AESKEY8       0x45
#define REG_AESKEY9       0x46
#define REG_AESKEY10      0x47
#define REG_AESKEY11      0x48
#define REG_AESKEY12      0x49
#define REG_AESKEY13      0x4A
#define REG_AESKEY14      0x4B
#define REG_AESKEY15      0x4C
#define REG_AESKEY16      0x4D
#define REG_TEMP1         0x4E
#define REG_TEMP2         0x4F
#define REG_TESTLNA       0x58
#define REG_TESTPA1       0x5A  // only present on RFM69HW/SX1231H
#define REG_TESTPA2       0x5C  // only present on RFM69HW/SX1231H
#define REG_TESTDAGC      0x6F

//******************************************************
// RF69/SX1231 bit control definition
//******************************************************

// RegOpMode
#define RF_OPMODE_SEQUENCER_OFF       0x80
#define RF_OPMODE_SEQUENCER_ON        0x00  // Default

#define RF_OPMODE_LISTEN_ON           0x40
#define RF_OPMODE_LISTEN_OFF          0x00  // Default

#define RF_OPMODE_LISTENABORT         0x20

#define RF_OPMODE_SLEEP               0x00
#define RF_OPMODE_STANDBY             0x04  // Default
#define RF_OPMODE_SYNTHESIZER         0x08
#define RF_OPMODE_TRANSMITTER         0x0C
#define RF_OPMODE_RECEIVER            0x10


// RegDataModul
#define RF_DATAMODUL_DATAMODE_PACKET            0x00  // Default
#define RF_DATAMODUL_DATAMODE_CONTINUOUS        0x40
#define RF_DATAMODUL_DATAMODE_CONTINUOUSNOBSYNC 0x60

#define RF_DATAMODUL_MODULATIONTYPE_FSK         0x00  // Default
#define RF_DATAMODUL_MODULATIONTYPE_OOK         0x08

#define RF_DATAMODUL_MODULATIONSHAPING_00       0x00  // Default
#define RF_DATAMODUL_MODULATIONSHAPING_01       0x01
#define RF_DATAMODUL_MODULATIONSHAPING_10       0x02
#define RF_DATAMODUL_MODULATIONSHAPING_11       0x03


// RegBitRate (bits/sec) example bit rates
#define RF_BITRATEMSB_1200            0x68
#define RF_BITRATELSB_1200            0x2B
#define RF_BITRATEMSB_2400            0x34
#define RF_BITRATELSB_2400            0x15
#define RF_BITRATEMSB_4800            0x1A  // Default
#define RF_BITRATELSB_4800            0x0B  // Default
#define RF_BITRATEMSB_9600            0x0D
#define RF_BITRATELSB_9600            0x05
#define RF_BITRATEMSB_19200           0x06
#define RF_BITRATELSB_19200           0x83
#define RF_BITRATEMSB_38400           0x03
#define RF_BITRATELSB_38400           0x41

#define RF_BITRATEMSB_38323           0x03
#define RF_BITRATELSB_38323           0x43

#define RF_BITRATEMSB_34482           0x03
#define RF_BITRATELSB_34482           0xA0

#define RF_BITRATEMSB_76800           0x01
#define RF_BITRATELSB_76800           0xA1
#define RF_BITRATEMSB_153600          0x00
#define RF_BITRATELSB_153600          0xD0
#define RF_BITRATEMSB_57600           0x02
#define RF_BITRATELSB_57600           0x2C
#define RF_BITRATEMSB_115200          0x01
#define RF_BITRATELSB_115200          0x16
#define RF_BITRATEMSB_12500           0x0A
#define RF_BITRATELSB_12500           0x00
#define RF_BITRATEMSB_25000           0x05
#define RF_BITRATELSB_25000           0x00
#define RF_BITRATEMSB_50000           0x02
#define RF_BITRATELSB_50000           0x80
#define RF_BITRATEMSB_100000          0x01
#define RF_BITRATELSB_100000          0x40
#define RF_BITRATEMSB_150000          0x00
#define RF_BITRATELSB_150000          0xD5
#define RF_BITRATEMSB_200000          0x00
#define RF_BITRATELSB_200000          0xA0
#define RF_BITRATEMSB_250000          0x00
#define RF_BITRATELSB_250000          0x80
#define RF_BITRATEMSB_300000          0x00
#define RF_BITRATELSB_300000          0x6B
#define RF_BITRATEMSB_32768           0x03
#define RF_BITRATELSB_32768           0xD1
// custom bit rates
#define RF_BITRATEMSB_55555           0x02
#define RF_BITRATELSB_55555           0x40
#define RF_BITRATEMSB_200KBPS         0x00
#define RF_BITRATELSB_200KBPS         0xa0

// RegFdev - frequency deviation (Hz)
#define RF_FDEVMSB_2000             0x00
#define RF_FDEVLSB_2000             0x21
#define RF_FDEVMSB_5000             0x00  // Default
#define RF_FDEVLSB_5000             0x52  // Default
#define RF_FDEVMSB_7500             0x00
#define RF_FDEVLSB_7500             0x7B
#define RF_FDEVMSB_10000            0x00
#define RF_FDEVLSB_10000            0xA4
#define RF_FDEVMSB_15000            0x00
#define RF_FDEVLSB_15000            0xF6
#define RF_FDEVMSB_20000            0x01
#define RF_FDEVLSB_20000            0x48
#define RF_FDEVMSB_25000            0x01
#define RF_FDEVLSB_25000            0x9A
#define RF_FDEVMSB_30000            0x01
#define RF_FDEVLSB_30000            0xEC
#define RF_FDEVMSB_35000            0x02
#define RF_FDEVLSB_35000            0x3D
#define RF_FDEVMSB_40000            0x02
#define RF_FDEVLSB_40000            0x8F
#define RF_FDEVMSB_45000            0x02
#define RF_FDEVLSB_45000            0xE1
#define RF_FDEVMSB_50000            0x03
#define RF_FDEVLSB_50000            0x33
#define RF_FDEVMSB_55000            0x03
#define RF_FDEVLSB_55000            0x85
#define RF_FDEVMSB_60000            0x03
#define RF_FDEVLSB_60000            0xD7
#define RF_FDEVMSB_65000            0x04
#define RF_FDEVLSB_65000            0x29
#define RF_FDEVMSB_70000            0x04
#define RF_FDEVLSB_70000            0x7B
#define RF_FDEVMSB_75000            0x04
#define RF_FDEVLSB_75000            0xCD
#define RF_FDEVMSB_80000            0x05
#define RF_FDEVLSB_80000            0x1F
#define RF_FDEVMSB_85000            0x05
#define RF_FDEVLSB_85000            0x71
#define RF_FDEVMSB_90000            0x05
#define RF_FDEVLSB_90000            0xC3
#define RF_FDEVMSB_95000            0x06
#define RF_FDEVLSB_95000            0x14
#define RF_FDEVMSB_100000           0x06
#define RF_FDEVLSB_100000           0x66
#define RF_FDEVMSB_110000           0x07
#define RF_FDEVLSB_110000           0x0A
#define RF_FDEVMSB_120000           0x07
#define RF_FDEVLSB_120000           0xAE
#define RF_FDEVMSB_130000           0x08
#define RF_FDEVLSB_130000           0x52
#define RF_FDEVMSB_140000           0x08
#define RF_FDEVLSB_140000           0xF6
#define RF_FDEVMSB_150000           0x09
#define RF_FDEVLSB_150000           0x9A
#define RF_FDEVMSB_160000           0x0A
#define RF_FDEVLSB_160000           0x3D
#define RF_FDEVMSB_170000           0x0A
#define RF_FDEVLSB_170000           0xE1
#define RF_FDEVMSB_180000           0x0B
#define RF_FDEVLSB_180000           0x85
#define RF_FDEVMSB_190000           0x0C
#define RF_FDEVLSB_190000           0x29
#define RF_FDEVMSB_200000           0x0C
#define RF_FDEVLSB_200000           0xCD
#define RF_FDEVMSB_210000           0x0D
#define RF_FDEVLSB_210000           0x71
#define RF_FDEVMSB_220000           0x0E
#define RF_FDEVLSB_220000           0x14
#define RF_FDEVMSB_230000           0x0E
#define RF_FDEVLSB_230000           0xB8
#define RF_FDEVMSB_240000           0x0F
#define RF_FDEVLSB_240000           0x5C
#define RF_FDEVMSB_250000           0x10
#define RF_FDEVLSB_250000           0x00
#define RF_FDEVMSB_260000           0x10
#define RF_FDEVLSB_260000           0xA4
#define RF_FDEVMSB_270000           0x11
#define RF_FDEVLSB_270000           0x48
#define RF_FDEVMSB_280000           0x11
#define RF_FDEVLSB_280000           0xEC
#define RF_FDEVMSB_290000           0x12
#define RF_FDEVLSB_290000           0x8F
#define RF_FDEVMSB_300000           0x13
#define RF_FDEVLSB_300000           0x33


// RegFrf (MHz) - carrier frequency
// 315Mhz band
#define RF_FRFMSB_314             0x4E
#define RF_FRFMID_314             0x80
#define RF_FRFLSB_314             0x00
#define RF_FRFMSB_315             0x4E
#define RF_FRFMID_315             0xC0
#define RF_FRFLSB_315             0x00
#define RF_FRFMSB_316             0x4F
#define RF_FRFMID_316             0x00
#define RF_FRFLSB_316             0x00
// 433mhz band
#define RF_FRFMSB_433             0x6C
#define RF_FRFMID_433             0x40
#define RF_FRFLSB_433             0x00
#define RF_FRFMSB_434             0x6C
#define RF_FRFMID_434             0x80
#define RF_FRFLSB_434             0x00
#define RF_FRFMSB_435             0x6C
#define RF_FRFMID_435             0xC0
#define RF_FRFLSB_435             0x00
// 868Mhz band
#define RF_FRFMSB_863             0xD7
#define RF_FRFMID_863             0xC0
#define RF_FRFLSB_863             0x00
#define RF_FRFMSB_864             0xD8
#define RF_FRFMID_864             0x00
#define RF_FRFLSB_864             0x00
#define RF_FRFMSB_865             0xD8
#define RF_FRFMID_865             0x40
#define RF_FRFLSB_865             0x00
#define RF_FRFMSB_866             0xD8
#define RF_FRFMID_866             0x80
#define RF_FRFLSB_866             0x00
#define RF_FRFMSB_867             0xD8
#define RF_FRFMID_867             0xC0
#define RF_FRFLSB_867             0x00
#define RF_FRFMSB_868             0xD9
#define RF_FRFMID_868             0x00
#define RF_FRFLSB_868             0x00
#define RF_FRFMSB_869             0xD9
#define RF_FRFMID_869             0x40
#define RF_FRFLSB_869             0x00
#define RF_FRFMSB_870             0xD9
#define RF_FRFMID_870             0x80
#define RF_FRFLSB_870             0x00
// 915Mhz band
#define RF_FRFMSB_902             0xE1
#define RF_FRFMID_902             0x80
#define RF_FRFLSB_902             0x00
#define RF_FRFMSB_903             0xE1
#define RF_FRFMID_903             0xC0
#define RF_FRFLSB_903             0x00
#define RF_FRFMSB_904             0xE2
#define RF_FRFMID_904             0x00
#define RF_FRFLSB_904             0x00
#define RF_FRFMSB_905             0xE2
#define RF_FRFMID_905             0x40
#define RF_FRFLSB_905             0x00
#define RF_FRFMSB_906             0xE2
#define RF_FRFMID_906             0x80
#define RF_FRFLSB_906             0x00
#define RF_FRFMSB_907             0xE2
#define RF_FRFMID_907             0xC0
#define RF_FRFLSB_907             0x00
#define RF_FRFMSB_908             0xE3
#define RF_FRFMID_908             0x00
#define RF_FRFLSB_908             0x00
#define RF_FRFMSB_909             0xE3
#define RF_FRFMID_909             0x40
#define RF_FRFLSB_909             0x00
#define RF_FRFMSB_910             0xE3
#define RF_FRFMID_910             0x80
#define RF_FRFLSB_910             0x00
#define RF_FRFMSB_911             0xE3
#define RF_FRFMID_911             0xC0
#define RF_FRFLSB_911             0x00
#define RF_FRFMSB_912             0xE4
#define RF_FRFMID_912             0x00
#define RF_FRFLSB_912             0x00
#define RF_FRFMSB_913             0xE4
#define RF_FRFMID_913             0x40
#define RF_FRFLSB_913             0x00
#define RF_FRFMSB_914             0xE4
#define RF_FRFMID_914             0x80
#define RF_FRFLSB_914             0x00
#define RF_FRFMSB_915             0xE4  // Default
#define RF_FRFMID_915             0xC0  // Default
#define RF_FRFLSB_915             0x00  // Default
#define RF_FRFMSB_916             0xE5
#define RF_FRFMID_916             0x00
#define RF_FRFLSB_916             0x00
#define RF_FRFMSB_917             0xE5
#define RF_FRFMID_917             0x40
#define RF_FRFLSB_917             0x00
#define RF_FRFMSB_918             0xE5
#define RF_FRFMID_918             0x80
#define RF_FRFLSB_918             0x00
#define RF_FRFMSB_919             0xE5
#define RF_FRFMID_919             0xC0
#define RF_FRFLSB_919             0x00
#define RF_FRFMSB_920             0xE6
#define RF_FRFMID_920             0x00
#define RF_FRFLSB_920             0x00
#define RF_FRFMSB_921             0xE6
#define RF_FRFMID_921             0x40
#define RF_FRFLSB_921             0x00
#define RF_FRFMSB_922             0xE6
#define RF_FRFMID_922             0x80
#define RF_FRFLSB_922             0x00
#define RF_FRFMSB_923             0xE6
#define RF_FRFMID_923             0xC0
#define RF_FRFLSB_923             0x00
#define RF_FRFMSB_924             0xE7
#define RF_FRFMID_924             0x00
#define RF_FRFLSB_924             0x00
#define RF_FRFMSB_925             0xE7
#define RF_FRFMID_925             0x40
#define RF_FRFLSB_925             0x00
#define RF_FRFMSB_926             0xE7
#define RF_FRFMID_926             0x80
#define RF_FRFLSB_926             0x00
#define RF_FRFMSB_927             0xE7
#define RF_FRFMID_927             0xC0
#define RF_FRFLSB_927             0x00
#define RF_FRFMSB_928             0xE8
#define RF_FRFMID_928             0x00
#define RF_FRFLSB_928             0x00

// RegOsc1
#define RF_OSC1_RCCAL_START       0x80
#define RF_OSC1_RCCAL_DONE        0x40

// RegAfcCtrl
#define RF_AFCCTRL_LOWBETA_OFF    0x00  // Default
#define RF_AFCCTRL_LOWBETA_ON     0x20

// RegLowBat
#define RF_LOWBAT_MONITOR         0x10
#define RF_LOWBAT_ON              0x08
#define RF_LOWBAT_OFF             0x00  // Default

#define RF_LOWBAT_TRIM_1695       0x00
#define RF_LOWBAT_TRIM_1764       0x01
#define RF_LOWBAT_TRIM_1835       0x02  // Default
#define RF_LOWBAT_TRIM_1905       0x03
#define RF_LOWBAT_TRIM_1976       0x04
#define RF_LOWBAT_TRIM_2045       0x05
#define RF_LOWBAT_TRIM_2116       0x06
#define RF_LOWBAT_TRIM_2185       0x07

// RegListen1
#define RF_LISTEN1_RESOL_64       0x50
#define RF_LISTEN1_RESOL_4100     0xA0  // Default
#define RF_LISTEN1_RESOL_262000   0xF0

#define RF_LISTEN1_RESOL_IDLE_64     0x40
#define RF_LISTEN1_RESOL_IDLE_4100   0x80  // Default
#define RF_LISTEN1_RESOL_IDLE_262000 0xC0

#define RF_LISTEN1_RESOL_RX_64       0x10
#define RF_LISTEN1_RESOL_RX_4100     0x20  // Default
#define RF_LISTEN1_RESOL_RX_262000   0x30

#define RF_LISTEN1_CRITERIA_RSSI          0x00  // Default
#define RF_LISTEN1_CRITERIA_RSSIANDSYNC   0x08

#define RF_LISTEN1_END_00                 0x00
#define RF_LISTEN1_END_01                 0x02  // Default
#define RF_LISTEN1_END_10                 0x04


// RegListen2
#define RF_LISTEN2_COEFIDLE_VALUE         0xF5 // Default


// RegListen3
#define RF_LISTEN3_COEFRX_VALUE           0x20 // Default


// RegVersion
#define RF_VERSION_VER        0x24  // Default


// RegPaLevel
#define RF_PALEVEL_PA0_ON     0x80  // Default
#define RF_PALEVEL_PA0_OFF    0x00
#define RF_PALEVEL_PA1_ON     0x40
#define RF_PALEVEL_PA1_OFF    0x00  // Default
#define RF_PALEVEL_PA2_ON     0x20
#define RF_PALEVEL_PA2_OFF    0x00  // Default

#define RF_PALEVEL_OUTPUTPOWER_00000      0x00
#define RF_PALEVEL_OUTPUTPOWER_00001      0x01
#define RF_PALEVEL_OUTPUTPOWER_00010      0x02
#define RF_PALEVEL_OUTPUTPOWER_00011      0x03
#define RF_PALEVEL_OUTPUTPOWER_00100      0x04
#define RF_PALEVEL_OUTPUTPOWER_00101      0x05
#define RF_PALEVEL_OUTPUTPOWER_00110      0x06
#define RF_PALEVEL_OUTPUTPOWER_00111      0x07
#define RF_PALEVEL_OUTPUTPOWER_01000      0x08
#define RF_PALEVEL_OUTPUTPOWER_01001      0x09
#define RF_PALEVEL_OUTPUTPOWER_01010      0x0A
#define RF_PALEVEL_OUTPUTPOWER_01011      0x0B
#define RF_PALEVEL_OUTPUTPOWER_01100      0x0C
#define RF_PALEVEL_OUTPUTPOWER_01101      0x0D
#define RF_PALEVEL_OUTPUTPOWER_01110      0x0E
#define RF_PALEVEL_OUTPUTPOWER_01111      0x0F
#define RF_PALEVEL_OUTPUTPOWER_10000      0x10
#define RF_PALEVEL_OUTPUTPOWER_10001      0x11
#define RF_PALEVEL_OUTPUTPOWER_10010      0x12
#define RF_PALEVEL_OUTPUTPOWER_10011      0x13
#define RF_PALEVEL_OUTPUTPOWER_10100      0x14
#define RF_PALEVEL_OUTPUTPOWER_10101      0x15
#define RF_PALEVEL_OUTPUTPOWER_10110      0x16
#define RF_PALEVEL_OUTPUTPOWER_10111      0x17
#define RF_PALEVEL_OUTPUTPOWER_11000      0x18
#define RF_PALEVEL_OUTPUTPOWER_11001      0x19
#define RF_PALEVEL_OUTPUTPOWER_11010      0x1A
#define RF_PALEVEL_OUTPUTPOWER_11011      0x1B
#define RF_PALEVEL_OUTPUTPOWER_11100      0x1C
#define RF_PALEVEL_OUTPUTPOWER_11101      0x1D
#define RF_PALEVEL_OUTPUTPOWER_11110      0x1E
#define RF_PALEVEL_OUTPUTPOWER_11111      0x1F  // Default


// RegPaRamp
#define RF_PARAMP_3400            0x00
#define RF_PARAMP_2000            0x01
#define RF_PARAMP_1000            0x02
#define RF_PARAMP_500             0x03
#define RF_PARAMP_250             0x04
#define RF_PARAMP_125             0x05
#define RF_PARAMP_100             0x06
#define RF_PARAMP_62              0x07
#define RF_PARAMP_50              0x08
#define RF_PARAMP_40              0x09  // Default
#define RF_PARAMP_31              0x0A
#define RF_PARAMP_25              0x0B
#define RF_PARAMP_20              0x0C
#define RF_PARAMP_15              0x0D
#define RF_PARAMP_12              0x0E
#define RF_PARAMP_10              0x0F


// RegOcp
#define RF_OCP_OFF                0x0F
#define RF_OCP_ON                 0x1A  // Default

#define RF_OCP_TRIM_45            0x00
#define RF_OCP_TRIM_50            0x01
#define RF_OCP_TRIM_55            0x02
#define RF_OCP_TRIM_60            0x03
#define RF_OCP_TRIM_65            0x04
#define RF_OCP_TRIM_70            0x05
#define RF_OCP_TRIM_75            0x06
#define RF_OCP_TRIM_80            0x07
#define RF_OCP_TRIM_85            0x08
#define RF_OCP_TRIM_90            0x09
#define RF_OCP_TRIM_95            0x0A  // Default
#define RF_OCP_TRIM_100           0x0B
#define RF_OCP_TRIM_105           0x0C
#define RF_OCP_TRIM_110           0x0D
#define RF_OCP_TRIM_115           0x0E
#define RF_OCP_TRIM_120           0x0F


// RegAgcRef - not present on RFM69/SX1231
#define RF_AGCREF_AUTO_ON         0x40  // Default
#define RF_AGCREF_AUTO_OFF        0x00

#define RF_AGCREF_LEVEL_MINUS80   0x00  // Default
#define RF_AGCREF_LEVEL_MINUS81   0x01
#define RF_AGCREF_LEVEL_MINUS82   0x02
#define RF_AGCREF_LEVEL_MINUS83   0x03
#define RF_AGCREF_LEVEL_MINUS84   0x04
#define RF_AGCREF_LEVEL_MINUS85   0x05
#define RF_AGCREF_LEVEL_MINUS86   0x06
#define RF_AGCREF_LEVEL_MINUS87   0x07
#define RF_AGCREF_LEVEL_MINUS88   0x08
#define RF_AGCREF_LEVEL_MINUS89   0x09
#define RF_AGCREF_LEVEL_MINUS90   0x0A
#define RF_AGCREF_LEVEL_MINUS91   0x0B
#define RF_AGCREF_LEVEL_MINUS92   0x0C
#define RF_AGCREF_LEVEL_MINUS93   0x0D
#define RF_AGCREF_LEVEL_MINUS94   0x0E
#define RF_AGCREF_LEVEL_MINUS95   0x0F
#define RF_AGCREF_LEVEL_MINUS96   0x10
#define RF_AGCREF_LEVEL_MINUS97   0x11
#define RF_AGCREF_LEVEL_MINUS98   0x12
#define RF_AGCREF_LEVEL_MINUS99   0x13
#define RF_AGCREF_LEVEL_MINUS100  0x14
#define RF_AGCREF_LEVEL_MINUS101  0x15
#define RF_AGCREF_LEVEL_MINUS102  0x16
#define RF_AGCREF_LEVEL_MINUS103  0x17
#define RF_AGCREF_LEVEL_MINUS104  0x18
#define RF_AGCREF_LEVEL_MINUS105  0x19
#define RF_AGCREF_LEVEL_MINUS106  0x1A
#define RF_AGCREF_LEVEL_MINUS107  0x1B
#define RF_AGCREF_LEVEL_MINUS108  0x1C
#define RF_AGCREF_LEVEL_MINUS109  0x1D
#define RF_AGCREF_LEVEL_MINUS110  0x1E
#define RF_AGCREF_LEVEL_MINUS111  0x1F
#define RF_AGCREF_LEVEL_MINUS112  0x20
#define RF_AGCREF_LEVEL_MINUS113  0x21
#define RF_AGCREF_LEVEL_MINUS114  0x22
#define RF_AGCREF_LEVEL_MINUS115  0x23
#define RF_AGCREF_LEVEL_MINUS116  0x24
#define RF_AGCREF_LEVEL_MINUS117  0x25
#define RF_AGCREF_LEVEL_MINUS118  0x26
#define RF_AGCREF_LEVEL_MINUS119  0x27
#define RF_AGCREF_LEVEL_MINUS120  0x28
#define RF_AGCREF_LEVEL_MINUS121  0x29
#define RF_AGCREF_LEVEL_MINUS122  0x2A
#define RF_AGCREF_LEVEL_MINUS123  0x2B
#define RF_AGCREF_LEVEL_MINUS124  0x2C
#define RF_AGCREF_LEVEL_MINUS125  0x2D
#define RF_AGCREF_LEVEL_MINUS126  0x2E
#define RF_AGCREF_LEVEL_MINUS127  0x2F
#define RF_AGCREF_LEVEL_MINUS128  0x30
#define RF_AGCREF_LEVEL_MINUS129  0x31
#define RF_AGCREF_LEVEL_MINUS130  0x32
#define RF_AGCREF_LEVEL_MINUS131  0x33
#define RF_AGCREF_LEVEL_MINUS132  0x34
#define RF_AGCREF_LEVEL_MINUS133  0x35
#define RF_AGCREF_LEVEL_MINUS134  0x36
#define RF_AGCREF_LEVEL_MINUS135  0x37
#define RF_AGCREF_LEVEL_MINUS136  0x38
#define RF_AGCREF_LEVEL_MINUS137  0x39
#define RF_AGCREF_LEVEL_MINUS138  0x3A
#define RF_AGCREF_LEVEL_MINUS139  0x3B
#define RF_AGCREF_LEVEL_MINUS140  0x3C
#define RF_AGCREF_LEVEL_MINUS141  0x3D
#define RF_AGCREF_LEVEL_MINUS142  0x3E
#define RF_AGCREF_LEVEL_MINUS143  0x3F


// RegAgcThresh1 - not present on RFM69/SX1231
#define RF_AGCTHRESH1_SNRMARGIN_000   0x00
#define RF_AGCTHRESH1_SNRMARGIN_001   0x20
#define RF_AGCTHRESH1_SNRMARGIN_010   0x40
#define RF_AGCTHRESH1_SNRMARGIN_011   0x60
#define RF_AGCTHRESH1_SNRMARGIN_100   0x80
#define RF_AGCTHRESH1_SNRMARGIN_101   0xA0  // Default
#define RF_AGCTHRESH1_SNRMARGIN_110   0xC0
#define RF_AGCTHRESH1_SNRMARGIN_111   0xE0

#define RF_AGCTHRESH1_STEP1_0         0x00
#define RF_AGCTHRESH1_STEP1_1         0x01
#define RF_AGCTHRESH1_STEP1_2         0x02
#define RF_AGCTHRESH1_STEP1_3         0x03
#define RF_AGCTHRESH1_STEP1_4         0x04
#define RF_AGCTHRESH1_STEP1_5         0x05
#define RF_AGCTHRESH1_STEP1_6         0x06
#define RF_AGCTHRESH1_STEP1_7         0x07
#define RF_AGCTHRESH1_STEP1_8         0x08
#define RF_AGCTHRESH1_STEP1_9         0x09
#define RF_AGCTHRESH1_STEP1_10        0x0A
#define RF_AGCTHRESH1_STEP1_11        0x0B
#define RF_AGCTHRESH1_STEP1_12        0x0C
#define RF_AGCTHRESH1_STEP1_13        0x0D
#define RF_AGCTHRESH1_STEP1_14        0x0E
#define RF_AGCTHRESH1_STEP1_15        0x0F
#define RF_AGCTHRESH1_STEP1_16        0x10  // Default
#define RF_AGCTHRESH1_STEP1_17        0x11
#define RF_AGCTHRESH1_STEP1_18        0x12
#define RF_AGCTHRESH1_STEP1_19        0x13
#define RF_AGCTHRESH1_STEP1_20        0x14
#define RF_AGCTHRESH1_STEP1_21        0x15
#define RF_AGCTHRESH1_STEP1_22        0x16
#define RF_AGCTHRESH1_STEP1_23        0x17
#define RF_AGCTHRESH1_STEP1_24        0x18
#define RF_AGCTHRESH1_STEP1_25        0x19
#define RF_AGCTHRESH1_STEP1_26        0x1A
#define RF_AGCTHRESH1_STEP1_27        0x1B
#define RF_AGCTHRESH1_STEP1_28        0x1C
#define RF_AGCTHRESH1_STEP1_29        0x1D
#define RF_AGCTHRESH1_STEP1_30        0x1E
#define RF_AGCTHRESH1_STEP1_31        0x1F


// RegAgcThresh2 - not present on RFM69/SX1231
#define RF_AGCTHRESH2_STEP2_0         0x00
#define RF_AGCTHRESH2_STEP2_1         0x10
#define RF_AGCTHRESH2_STEP2_2         0x20
#define RF_AGCTHRESH2_STEP2_3         0x30  // XXX wrong -- Default
#define RF_AGCTHRESH2_STEP2_4         0x40
#define RF_AGCTHRESH2_STEP2_5         0x50
#define RF_AGCTHRESH2_STEP2_6         0x60
#define RF_AGCTHRESH2_STEP2_7         0x70  // default
#define RF_AGCTHRESH2_STEP2_8         0x80
#define RF_AGCTHRESH2_STEP2_9         0x90
#define RF_AGCTHRESH2_STEP2_10        0xA0
#define RF_AGCTHRESH2_STEP2_11        0xB0
#define RF_AGCTHRESH2_STEP2_12        0xC0
#define RF_AGCTHRESH2_STEP2_13        0xD0
#define RF_AGCTHRESH2_STEP2_14        0xE0
#define RF_AGCTHRESH2_STEP2_15        0xF0

#define RF_AGCTHRESH2_STEP3_0         0x00
#define RF_AGCTHRESH2_STEP3_1         0x01
#define RF_AGCTHRESH2_STEP3_2         0x02
#define RF_AGCTHRESH2_STEP3_3         0x03
#define RF_AGCTHRESH2_STEP3_4         0x04
#define RF_AGCTHRESH2_STEP3_5         0x05
#define RF_AGCTHRESH2_STEP3_6         0x06
#define RF_AGCTHRESH2_STEP3_7         0x07
#define RF_AGCTHRESH2_STEP3_8         0x08
#define RF_AGCTHRESH2_STEP3_9         0x09
#define RF_AGCTHRESH2_STEP3_10        0x0A
#define RF_AGCTHRESH2_STEP3_11        0x0B  // Default
#define RF_AGCTHRESH2_STEP3_12        0x0C
#define RF_AGCTHRESH2_STEP3_13        0x0D
#define RF_AGCTHRESH2_STEP3_14        0x0E
#define RF_AGCTHRESH2_STEP3_15        0x0F


// RegAgcThresh3 - not present on RFM69/SX1231
#define RF_AGCTHRESH3_STEP4_0         0x00
#define RF_AGCTHRESH3_STEP4_1         0x10
#define RF_AGCTHRESH3_STEP4_2         0x20
#define RF_AGCTHRESH3_STEP4_3         0x30
#define RF_AGCTHRESH3_STEP4_4         0x40
#define RF_AGCTHRESH3_STEP4_5         0x50
#define RF_AGCTHRESH3_STEP4_6         0x60
#define RF_AGCTHRESH3_STEP4_7         0x70
#define RF_AGCTHRESH3_STEP4_8         0x80
#define RF_AGCTHRESH3_STEP4_9         0x90  // Default
#define RF_AGCTHRESH3_STEP4_10        0xA0
#define RF_AGCTHRESH3_STEP4_11        0xB0
#define RF_AGCTHRESH3_STEP4_12        0xC0
#define RF_AGCTHRESH3_STEP4_13        0xD0
#define RF_AGCTHRESH3_STEP4_14        0xE0
#define RF_AGCTHRESH3_STEP4_15        0xF0

#define RF_AGCTHRESH3_STEP5_0         0x00
#define RF_AGCTHRESH3_STEP5_1         0x01
#define RF_AGCTHRESH3_STEP5_2         0x02
#define RF_AGCTHRESH3_STEP5_3         0x03
#define RF_AGCTHRESH3_STEP5_4         0x04
#define RF_AGCTHRESH3_STEP5_5         0x05
#define RF_AGCTHRESH3_STEP5_6         0x06
#define RF_AGCTHRESH3_STEP5_7         0x07
#define RF_AGCTHRES33_STEP5_8         0x08
#define RF_AGCTHRESH3_STEP5_9         0x09
#define RF_AGCTHRESH3_STEP5_10        0x0A
#define RF_AGCTHRESH3_STEP5_11        0x0B  // Default
#define RF_AGCTHRESH3_STEP5_12        0x0C
#define RF_AGCTHRESH3_STEP5_13        0x0D
#define RF_AGCTHRESH3_STEP5_14        0x0E
#define RF_AGCTHRESH3_STEP5_15        0x0F


// RegLna
#define RF_LNA_ZIN_50                 0x00  // Reset value
#define RF_LNA_ZIN_200                0x80  // Recommended default

#define RF_LNA_LOWPOWER_OFF           0x00  // Default
#define RF_LNA_LOWPOWER_ON            0x40

#define RF_LNA_CURRENTGAIN            0x08

#define RF_LNA_GAINSELECT_AUTO        0x00  // Default
#define RF_LNA_GAINSELECT_MAX         0x01
#define RF_LNA_GAINSELECT_MAXMINUS6   0x02
#define RF_LNA_GAINSELECT_MAXMINUS12  0x03
#define RF_LNA_GAINSELECT_MAXMINUS24  0x04
#define RF_LNA_GAINSELECT_MAXMINUS36  0x05
#define RF_LNA_GAINSELECT_MAXMINUS48  0x06


// RegRxBw
#define RF_RXBW_DCCFREQ_000           0x00
#define RF_RXBW_DCCFREQ_001           0x20
#define RF_RXBW_DCCFREQ_010           0x40  // Recommended default
#define RF_RXBW_DCCFREQ_011           0x60
#define RF_RXBW_DCCFREQ_100           0x80  // Reset value
#define RF_RXBW_DCCFREQ_101           0xA0
#define RF_RXBW_DCCFREQ_110           0xC0
#define RF_RXBW_DCCFREQ_111           0xE0

#define RF_RXBW_MANT_16               0x00  // Reset value
#define RF_RXBW_MANT_20               0x08
#define RF_RXBW_MANT_24               0x10  // Recommended default

#define RF_RXBW_EXP_0                 0x00
#define RF_RXBW_EXP_1                 0x01
#define RF_RXBW_EXP_2                 0x02
#define RF_RXBW_EXP_3                 0x03
#define RF_RXBW_EXP_4                 0x04
#define RF_RXBW_EXP_5                 0x05  // Recommended default
#define RF_RXBW_EXP_6                 0x06  // Reset value
#define RF_RXBW_EXP_7                 0x07


// RegAfcBw
#define RF_AFCBW_DCCFREQAFC_000       0x00
#define RF_AFCBW_DCCFREQAFC_001       0x20
#define RF_AFCBW_DCCFREQAFC_010       0x40
#define RF_AFCBW_DCCFREQAFC_011       0x60
#define RF_AFCBW_DCCFREQAFC_100       0x80  // Default
#define RF_AFCBW_DCCFREQAFC_101       0xA0
#define RF_AFCBW_DCCFREQAFC_110       0xC0
#define RF_AFCBW_DCCFREQAFC_111       0xE0

#define RF_AFCBW_MANTAFC_16           0x00
#define RF_AFCBW_MANTAFC_20           0x08  // Default
#define RF_AFCBW_MANTAFC_24           0x10

#define RF_AFCBW_EXPAFC_0             0x00
#define RF_AFCBW_EXPAFC_1             0x01
#define RF_AFCBW_EXPAFC_2             0x02  // Reset value
#define RF_AFCBW_EXPAFC_3             0x03  // Recommended default
#define RF_AFCBW_EXPAFC_4             0x04
#define RF_AFCBW_EXPAFC_5             0x05
#define RF_AFCBW_EXPAFC_6             0x06
#define RF_AFCBW_EXPAFC_7             0x07


// RegOokPeak
#define RF_OOKPEAK_THRESHTYPE_FIXED       0x00
#define RF_OOKPEAK_THRESHTYPE_PEAK        0x40  // Default
#define RF_OOKPEAK_THRESHTYPE_AVERAGE     0x80

#define RF_OOKPEAK_PEAKTHRESHSTEP_000     0x00  // Default
#define RF_OOKPEAK_PEAKTHRESHSTEP_001     0x08
#define RF_OOKPEAK_PEAKTHRESHSTEP_010     0x10
#define RF_OOKPEAK_PEAKTHRESHSTEP_011     0x18
#define RF_OOKPEAK_PEAKTHRESHSTEP_100     0x20
#define RF_OOKPEAK_PEAKTHRESHSTEP_101     0x28
#define RF_OOKPEAK_PEAKTHRESHSTEP_110     0x30
#define RF_OOKPEAK_PEAKTHRESHSTEP_111     0x38

#define RF_OOKPEAK_PEAKTHRESHDEC_000      0x00  // Default
#define RF_OOKPEAK_PEAKTHRESHDEC_001      0x01
#define RF_OOKPEAK_PEAKTHRESHDEC_010      0x02
#define RF_OOKPEAK_PEAKTHRESHDEC_011      0x03
#define RF_OOKPEAK_PEAKTHRESHDEC_100      0x04
#define RF_OOKPEAK_PEAKTHRESHDEC_101      0x05
#define RF_OOKPEAK_PEAKTHRESHDEC_110      0x06
#define RF_OOKPEAK_PEAKTHRESHDEC_111      0x07


// RegOokAvg
#define RF_OOKAVG_AVERAGETHRESHFILT_00    0x00
#define RF_OOKAVG_AVERAGETHRESHFILT_01    0x40
#define RF_OOKAVG_AVERAGETHRESHFILT_10    0x80  // Default
#define RF_OOKAVG_AVERAGETHRESHFILT_11    0xC0


// RegOokFix
#define RF_OOKFIX_FIXEDTHRESH_VALUE       0x06  // Default


// RegAfcFei
#define RF_AFCFEI_FEI_DONE                0x40
#define RF_AFCFEI_FEI_START               0x20
#define RF_AFCFEI_AFC_DONE                0x10
#define RF_AFCFEI_AFCAUTOCLEAR_ON         0x08
#define RF_AFCFEI_AFCAUTOCLEAR_OFF        0x00  // Default

#define RF_AFCFEI_AFCAUTO_ON              0x04
#define RF_AFCFEI_AFCAUTO_OFF             0x00  // Default

#define RF_AFCFEI_AFC_CLEAR               0x02
#define RF_AFCFEI_AFC_START               0x01


// RegRssiConfig
#define RF_RSSI_FASTRX_ON                 0x08  // not present on RFM69/SX1231
#define RF_RSSI_FASTRX_OFF                0x00  // Default

#define RF_RSSI_DONE                      0x02
#define RF_RSSI_START                     0x01


// RegDioMapping1
#define RF_DIOMAPPING1_DIO0_00            0x00  // Default
#define RF_DIOMAPPING1_DIO0_01            0x40
#define RF_DIOMAPPING1_DIO0_10            0x80
#define RF_DIOMAPPING1_DIO0_11            0xC0

#define RF_DIOMAPPING1_DIO1_00            0x00  // Default
#define RF_DIOMAPPING1_DIO1_01            0x10
#define RF_DIOMAPPING1_DIO1_10            0x20
#define RF_DIOMAPPING1_DIO1_11            0x30

#define RF_DIOMAPPING1_DIO2_00            0x00  // Default
#define RF_DIOMAPPING1_DIO2_01            0x04
#define RF_DIOMAPPING1_DIO2_10            0x08
#define RF_DIOMAPPING1_DIO2_11            0x0C

#define RF_DIOMAPPING1_DIO3_00            0x00  // Default
#define RF_DIOMAPPING1_DIO3_01            0x01
#define RF_DIOMAPPING1_DIO3_10            0x02
#define RF_DIOMAPPING1_DIO3_11            0x03


// RegDioMapping2
#define RF_DIOMAPPING2_DIO4_00            0x00  // Default
#define RF_DIOMAPPING2_DIO4_01            0x40
#define RF_DIOMAPPING2_DIO4_10            0x80
#define RF_DIOMAPPING2_DIO4_11            0xC0

#define RF_DIOMAPPING2_DIO5_00            0x00  // Default
#define RF_DIOMAPPING2_DIO5_01            0x10
#define RF_DIOMAPPING2_DIO5_10            0x20
#define RF_DIOMAPPING2_DIO5_11            0x30

#define RF_DIOMAPPING2_CLKOUT_32          0x00
#define RF_DIOMAPPING2_CLKOUT_16          0x01
#define RF_DIOMAPPING2_CLKOUT_8           0x02
#define RF_DIOMAPPING2_CLKOUT_4           0x03
#define RF_DIOMAPPING2_CLKOUT_2           0x04
#define RF_DIOMAPPING2_CLKOUT_1           0x05  // Reset value
#define RF_DIOMAPPING2_CLKOUT_RC          0x06
#define RF_DIOMAPPING2_CLKOUT_OFF         0x07  // Recommended default


// RegIrqFlags1
#define RF_IRQFLAGS1_MODEREADY            0x80
#define RF_IRQFLAGS1_RXREADY              0x40
#define RF_IRQFLAGS1_TXREADY              0x20
#define RF_IRQFLAGS1_PLLLOCK              0x10
#define RF_IRQFLAGS1_RSSI                 0x08
#define RF_IRQFLAGS1_TIMEOUT              0x04
#define RF_IRQFLAGS1_AUTOMODE             0x02
#define RF_IRQFLAGS1_SYNCADDRESSMATCH     0x01


// RegIrqFlags2
#define RF_IRQFLAGS2_FIFOFULL             0x80
#define RF_IRQFLAGS2_FIFONOTEMPTY         0x40
#define RF_IRQFLAGS2_FIFOLEVEL            0x20
#define RF_IRQFLAGS2_FIFOOVERRUN          0x10
#define RF_IRQFLAGS2_PACKETSENT           0x08
#define RF_IRQFLAGS2_PAYLOADREADY         0x04
#define RF_IRQFLAGS2_CRCOK                0x02
#define RF_IRQFLAGS2_LOWBAT               0x01  // not present on RFM69/SX1231


// RegRssiThresh
#define RF_RSSITHRESH_VALUE               0xE4  // Default


// RegRxTimeout1
#define RF_RXTIMEOUT1_RXSTART_VALUE       0x00  // Default


// RegRxTimeout2
#define RF_RXTIMEOUT2_RSSITHRESH_VALUE    0x00  // Default


// RegPreamble
#define RF_PREAMBLESIZE_MSB_VALUE         0x00  // Default
#define RF_PREAMBLESIZE_LSB_VALUE         0x03  // Default


// RegSyncConfig
#define RF_SYNC_ON                0x80  // Default
#define RF_SYNC_OFF               0x00

#define RF_SYNC_FIFOFILL_AUTO     0x00  // Default -- when sync interrupt occurs
#define RF_SYNC_FIFOFILL_MANUAL   0x40

#define RF_SYNC_SIZE_1            0x00
#define RF_SYNC_SIZE_2            0x08
#define RF_SYNC_SIZE_3            0x10
#define RF_SYNC_SIZE_4            0x18  // Default
#define RF_SYNC_SIZE_5            0x20
#define RF_SYNC_SIZE_6            0x28
#define RF_SYNC_SIZE_7            0x30
#define RF_SYNC_SIZE_8            0x38

#define RF_SYNC_TOL_0             0x00  // Default
#define RF_SYNC_TOL_1             0x01
#define RF_SYNC_TOL_2             0x02
#define RF_SYNC_TOL_3             0x03
#define RF_SYNC_TOL_4             0x04
#define RF_SYNC_TOL_5             0x05
#define RF_SYNC_TOL_6             0x06
#define RF_SYNC_TOL_7             0x07


// RegSyncValue1-8
#define RF_SYNC_BYTE1_VALUE       0x00  // Default
#define RF_SYNC_BYTE2_VALUE       0x00  // Default
#define RF_SYNC_BYTE3_VALUE       0x00  // Default
#define RF_SYNC_BYTE4_VALUE       0x00  // Default
#define RF_SYNC_BYTE5_VALUE       0x00  // Default
#define RF_SYNC_BYTE6_VALUE       0x00  // Default
#define RF_SYNC_BYTE7_VALUE       0x00  // Default
#define RF_SYNC_BYTE8_VALUE       0x00  // Default


// RegPacketConfig1
#define RF_PACKET1_FORMAT_FIXED       0x00  // Default
#define RF_PACKET1_FORMAT_VARIABLE    0x80

#define RF_PACKET1_DCFREE_OFF         0x00  // Default
#define RF_PACKET1_DCFREE_MANCHESTER  0x20
#define RF_PACKET1_DCFREE_WHITENING   0x40

#define RF_PACKET1_CRC_ON             0x10  // Default
#define RF_PACKET1_CRC_OFF            0x00

#define RF_PACKET1_CRCAUTOCLEAR_ON    0x00  // Default
#define RF_PACKET1_CRCAUTOCLEAR_OFF   0x08

#define RF_PACKET1_ADRSFILTERING_OFF            0x00  // Default
#define RF_PACKET1_ADRSFILTERING_NODE           0x02
#define RF_PACKET1_ADRSFILTERING_NODEBROADCAST  0x04


// RegPayloadLength
#define RF_PAYLOADLENGTH_VALUE          0x40  // Default


// RegBroadcastAdrs
#define RF_BROADCASTADDRESS_VALUE       0x00


// RegAutoModes
#define RF_AUTOMODES_ENTER_OFF                0x00  // Default
#define RF_AUTOMODES_ENTER_FIFONOTEMPTY       0x20
#define RF_AUTOMODES_ENTER_FIFOLEVEL          0x40
#define RF_AUTOMODES_ENTER_CRCOK              0x60
#define RF_AUTOMODES_ENTER_PAYLOADREADY       0x80
#define RF_AUTOMODES_ENTER_SYNCADRSMATCH      0xA0
#define RF_AUTOMODES_ENTER_PACKETSENT         0xC0
#define RF_AUTOMODES_ENTER_FIFOEMPTY          0xE0

#define RF_AUTOMODES_EXIT_OFF                 0x00  // Default
#define RF_AUTOMODES_EXIT_FIFOEMPTY           0x04
#define RF_AUTOMODES_EXIT_FIFOLEVEL           0x08
#define RF_AUTOMODES_EXIT_CRCOK               0x0C
#define RF_AUTOMODES_EXIT_PAYLOADREADY        0x10
#define RF_AUTOMODES_EXIT_SYNCADRSMATCH       0x14
#define RF_AUTOMODES_EXIT_PACKETSENT          0x18
#define RF_AUTOMODES_EXIT_RXTIMEOUT           0x1C

#define RF_AUTOMODES_INTERMEDIATE_SLEEP       0x00  // Default
#define RF_AUTOMODES_INTERMEDIATE_STANDBY     0x01
#define RF_AUTOMODES_INTERMEDIATE_RECEIVER    0x02
#define RF_AUTOMODES_INTERMEDIATE_TRANSMITTER 0x03


// RegFifoThresh
#define RF_FIFOTHRESH_TXSTART_FIFOTHRESH      0x00  // Reset value
#define RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY    0x80  // Recommended default

#define RF_FIFOTHRESH_VALUE                   0x0F  // Default


// RegPacketConfig2
#define RF_PACKET2_RXRESTARTDELAY_1BIT        0x00  // Default
#define RF_PACKET2_RXRESTARTDELAY_2BITS       0x10
#define RF_PACKET2_RXRESTARTDELAY_4BITS       0x20
#define RF_PACKET2_RXRESTARTDELAY_8BITS       0x30
#define RF_PACKET2_RXRESTARTDELAY_16BITS      0x40
#define RF_PACKET2_RXRESTARTDELAY_32BITS      0x50
#define RF_PACKET2_RXRESTARTDELAY_64BITS      0x60
#define RF_PACKET2_RXRESTARTDELAY_128BITS     0x70
#define RF_PACKET2_RXRESTARTDELAY_256BITS     0x80
#define RF_PACKET2_RXRESTARTDELAY_512BITS     0x90
#define RF_PACKET2_RXRESTARTDELAY_1024BITS    0xA0
#define RF_PACKET2_RXRESTARTDELAY_2048BITS    0xB0
#define RF_PACKET2_RXRESTARTDELAY_NONE        0xC0
#define RF_PACKET2_RXRESTART                  0x04

#define RF_PACKET2_AUTORXRESTART_ON           0x02  // Default
#define RF_PACKET2_AUTORXRESTART_OFF          0x00

#define RF_PACKET2_AES_ON                     0x01
#define RF_PACKET2_AES_OFF                    0x00  // Default


// RegAesKey1-16
#define RF_AESKEY1_VALUE            0x00  // Default
#define RF_AESKEY2_VALUE            0x00  // Default
#define RF_AESKEY3_VALUE            0x00  // Default
#define RF_AESKEY4_VALUE            0x00  // Default
#define RF_AESKEY5_VALUE            0x00  // Default
#define RF_AESKEY6_VALUE            0x00  // Default
#define RF_AESKEY7_VALUE            0x00  // Default
#define RF_AESKEY8_VALUE            0x00  // Default
#define RF_AESKEY9_VALUE            0x00  // Default
#define RF_AESKEY10_VALUE           0x00  // Default
#define RF_AESKEY11_VALUE           0x00  // Default
#define RF_AESKEY12_VALUE           0x00  // Default
#define RF_AESKEY13_VALUE           0x00  // Default
#define RF_AESKEY14_VALUE           0x00  // Default
#define RF_AESKEY15_VALUE           0x00  // Default
#define RF_AESKEY16_VALUE           0x00  // Default


// RegTemp1
#define RF_TEMP1_MEAS_START         0x08
#define RF_TEMP1_MEAS_RUNNING       0x04
// not present on RFM69/SX1231
#define RF_TEMP1_ADCLOWPOWER_ON     0x01  // Default
#define RF_TEMP1_ADCLOWPOWER_OFF    0x00


// RegTestLna
#define RF_TESTLNA_NORMAL           0x1B
#define RF_TESTLNA_HIGH_SENSITIVITY 0x2D


// RegTestDagc
#define RF_DAGC_NORMAL              0x00  // Reset value
#define RF_DAGC_IMPROVED_LOWBETA1   0x20
#define RF_DAGC_IMPROVED_LOWBETA0   0x30  // Recommended default


//#define CSMA_LIMIT              -90 // upper RX signal sensitivity threshold in dBm for carrier sense access
// Changed by RoSchmi
#define CSMA_LIMIT              -70 // upper RX signal sensitivity threshold in dBm for carrier sense access


#define RF69_MODE_SLEEP         0 // XTAL OFF
#define RF69_MODE_STANDBY       1 // XTAL ON
#define RF69_MODE_SYNTH         2 // PLL ON
#define RF69_MODE_RX            3 // RX MODE
#define RF69_MODE_TX            4 // TX MODE

#define RF69_CSMA_LIMIT_MS 1000
#define RF69_TX_LIMIT_MS   1000      // for sendig message: time in ms to wait for free environment to send
// Changed by RoSchmi from 40 to 150
#define ACK_CSMA_LIMIT_MS   200     // for sending ACK: time in ms to wait for free environment to send

#define RF69_MAX_DATA_LEN       61 // to take advantage of the built in AES/CRC we want to limit the frame siz 

//*********************************************************************************************
// *********** IMPORTANT SETTINGS - YOU MUST CHANGE/CONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
//Changed by RoSchmi
#define NETWORKID     100  //the same on all nodes that talk to each other
#define NODEID        2    // The unique identifier of this node
#define RECEIVER      1    // The recipient of packets (Node in the heating room)
//#define RECEIVER      3    // The recipient of packets (of the test-node)

#define RF69_315MHZ            31 // non trivial values to avoid misconfiguration
#define RF69_433MHZ            43
#define RF69_868MHZ            86
#define RF69_915MHZ            91

#define RF69_MODE_STANDBY       1 // XTAL ON

//Match frequency to the hardware version of the radio on your Feather
#define FREQUENCY     RF69_433MHZ
//#define FREQUENCY     RF69_868MHZ
//#define FREQUENCY      RF69_915MHZ
// Changed by RoSchmi
//#define ENCRYPTKEY     "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define ENCRYPTKEY       "heatingsurvey234" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW    true // set to 'true' if you are using an RFM69HCW module

//*********************************************************************************************
#define SERIAL_BAUD   115200

//#define RF69_SPI_CS           SS // SS is the SPI slave select pin, for instance D10 on ATmega328
//#define RF69_IRQ_PIN          2
//#define RF69_IRQ_NUM          0

// for Feather M0 Radio
#define RFM69_CS      8
#define RFM69_IRQ     3
#define RFM69_IRQN    3  // Pin 3 is IRQ 3!
#define RFM69_RST     4

//#define RFM69_CS      10
//#define RFM69_IRQ     2
//#define RFM69_IRQN    0  // Pin 2 is IRQ 0!
//#define RFM69_RST     9

#define LED           13  // onboard blinky


//bool digital_11_State = false;   // only for tests

#define RF69_BROADCAST_ADDR 255

// TWS: define CTLbyte bits
#define RFM69_CTL_SENDACK   0x80
#define RFM69_CTL_REQACK    0x40


unsigned long seed;
int16_t packetnum;  // packet counter, we increment per xmission, 

uint8_t _slaveSelectPin;
uint8_t _interruptPin;
uint8_t _interruptNum;
uint8_t _address;
bool _promiscuousMode;
uint8_t _powerLevel;
bool _isRFM69HW;
volatile uint8_t _mode;

volatile bool interruptHappened;

uint8_t _SPCR;
uint8_t _SPSR;

static volatile uint8_t DATA[RF69_MAX_DATA_LEN];

static volatile uint8_t  DATALEN;
static volatile uint8_t  SENDERID;
static volatile uint8_t  TARGETID;

static volatile int16_t RSSI; // most accurate RSSI during reception (closest to the reception)
static volatile uint8_t PAYLOADLEN;
  
static volatile bool ACK_REQUESTED;
static volatile bool   ACK_RECEIVED;

static volatile uint8_t ACK_RSSI_REQUESTED;  // new type of flag on ACK_REQUEST

#define RFM69_CTL_RESERVE1  0x20

int16_t _ackRSSI;         // this contains the RSSI our destination Ack'd back to us (if we enabledAutoPower)
int16_t _targetRSSI;     // if non-zero then this is the desired end point RSSI for our transmission
uint8_t _transmitLevel;  // saved powerLevel in case we do auto power adjustment, this value gets dithered

static volatile bool _inISR;

uint8_t _initResult;

// Initialization: For the first transmission actState and oldState = 1 (off)
volatile int oldState = 1;
volatile int actState = 1;


int16_t val_1 = 0;   // These val_1 to val_3 are sent to the master,  the actual values are only as example (must be changed)
int16_t val_2 = 2;
int16_t val_3 = 3;
 
volatile int repeatSend = 0;
int maxRepeatSend = 7;   // if no sending success, sending is repeated maxRepeatSend times with time between of 3 sec

volatile int8_t lastMode;

int analogPin = 1;

const int radioPacketLength = 27;
union Radiopackets
{
    char radiopacketPlus1[radioPacketLength + 1];
    char radiopacket[radioPacketLength];
};

Radiopackets packets; 

volatile int16_t sendInfo = 0;

// Forward declarations:
uint8_t initialize(uint8_t freqBand, uint8_t nodeID, uint8_t networkID);
void setHighPower(bool onOff);
void setPowerLevel(uint8_t powerLevel);
void encrypt(const char* key);
bool sendMessage(int pActState, int pOldState, uint32_t pCurrent = 0, uint32_t pPower = 0, uint32_t pWork= 0);
void setMode(uint8_t newMode);
bool receiveDone();
bool sendWithRetry(uint8_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries, uint8_t retryWaitTime); 
void Blink(byte PIN, byte DELAY_MS, byte loops);
uint8_t readReg(uint8_t addr);
void writeReg(uint8_t addr, uint8_t value);
int16_t readRSSI(bool forceTrigger);
void unselect();
void select();
void interruptHandler();
void receiveBegin();
void interruptHook(uint8_t CTLbyte);
inline void maybeInterrupts();

/********* Values to be changed by User for Modbus Stuff  ********/
const uint8_t SlaveAddress = 1;
const uint32_t InitialReleaseTimespan_Current_Ms = 1000;   // First 'Current' read after this time
const uint32_t InitialReleaseTimespan_Power_Ms = 1000;     // First 'Power' read after this time
const uint32_t InitialReleaseTimespan_Work_Ms = 1000;      // First 'Work' read after this time

const uint32_t FollowReleaseTimespan_Current_Ms = 5000;   // Following 'Current' reads after this time
const uint32_t FollowReleaseTimespan_Power_Ms = 5000;     // Following 'Power' reads after this time
const uint32_t FollowReleaseTimespan_Work_Ms = 5000;      // Following 'Work' reads after this time

/********* end of values to be changed by User ****/

// Some values with their Register Addresses that can be read from Smartmeter
const uint16_t Phase_1_Current_Address = 0x0006;    //Phase 1 current
const uint16_t Phase_2_Current_Address = 0x0008;    //Phase 2 current
const uint16_t Phase_3_Current_Address = 0x000A;    //Phase 3 current
const uint16_t Sum_of_Current_Address = 0x0030;     //Sum of line currents
const uint16_t TotalPower_Address = 0x0034;         //Total System Power
const uint16_t Import_Work_Address = 0x0048;        //Import Wh since last reset
const uint16_t Export_Work_Address = 0x004A;        //Export Wh since last reset

// data array for modbus network sharing
uint16_t au16data[16];
uint8_t u8addr;

float NotValidValue = 999.9;
float ActCurrent = NotValidValue;
float ActPower = NotValidValue;

uint32_t ActImportWork = 0;
uint16_t ImpWorkHigh;
uint16_t ImpWorkLow;
SampleValues sampleValues;
bool isForcedReadInputWork = true;
//Call Constructor of DataContainer
// Parameters means: send at least every 10 min or if the deviation is more than 10% or more than 0.2 Amps
DataContainer dataContainer(10 * 60 * 1000, TriggerDeviation::PERCENT_10, TriggerDeviationAmps::AMPERE_02);
//DataContainer dataContainer(120 * 1000, TriggerDeviation::PERCENT_10, TriggerDeviationAmps::AMPERE_02);


// not used in this example
#define kPowerOn        A3

static inline void powerOn(void)
{
        pinMode(kPowerOn, OUTPUT);
        digitalWrite(kPowerOn, HIGH);
}

/**
 *     This is a struct which contains a query to a device
 */
modbus_t datagram;

int16_2_float_function_result reform_uint16_2_float32(uint16_t u1, uint16_t u2);

regsReturnStruct get_2_InputRegisters(uint8_t pSlaveAddress);

regsReturnStruct regsRetStruct;
regsReturnStruct regsRetStruct_2;

unsigned long u32wait;

bool skipModbusFunctions = false;
uint16_t skipCounter = 0;

uint16_t Pin11Switchcounter = 0;     // variable only needed for tests
bool digital_11_State = false;

// Serial2 pin and pad definitions (in Arduino files Variant.h & Variant.cpp)
/*
#define PIN_SERIAL2_RX       (34ul)               // unsigned long (32 bit )Pin description number for PIO_SERCOM on D12 - PA19
#define PIN_SERIAL2_TX       (36ul)               // unsigned long (32 bit )Pin description number for PIO_SERCOM on D10 - PA18
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)      // SERCOM pad 2
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)    // SERCOM pad 3
*/
// Instantiate the Serial2 class if needed
//Uart Serial2(&sercom1, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);

/*
 *  Modbus object declaration
 *  u8id : node id = 0 for host, = 1..247 for device
 *  u8serno : serial port (use 0 for Serial)
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus host(0, A4); // this is host and RS-232 or USB-FTDI

ModbusSerial<decltype(Serial1)> mySerial(&Serial1);
// ModbusSerial<decltype(Serial2)> mySerial(&Serial2);

// Calling constructors (the .begin function of each class must be called in Setup)
Read_2_InputRegisters readImportWork(datagram, Import_Work_Address);
Read_2_InputRegisters readSummedCurrent(datagram, Sum_of_Current_Address);
Read_2_InputRegisters readTotalPower(datagram, TotalPower_Address);

void setup() 
{
  Serial.begin(SERIAL_BAUD);

  #ifdef DebugPrint
    while (!Serial); // wait until serial console is open, remove if not tethered to computer
  #endif

  yield();
  Serial.println("Feather RFM69HCW Transmitter");
  //powerOn();

  pinMode(LED, OUTPUT);
  // Blinky to show that program has started
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
  }

  // Serial port for Modbus transmission to Smartmeter
  host.begin(&mySerial, 2400, SERIAL_8N1); // baud-rate at 2400
  host.setTimeOut( 1000 ); // if there is no answer in 1000 ms, roll over
  host.setTxEnableDelay(100);

  // Initialization of Classes to read values from the Smartmeter (is needed)
  readImportWork.begin(host, InitialReleaseTimespan_Work_Ms);
  readSummedCurrent.begin(host, InitialReleaseTimespan_Current_Ms);
  readTotalPower.begin(host, InitialReleaseTimespan_Power_Ms);
 
  u8addr = SlaveAddress;
  
  analogReadResolution(12);

  // Pseudo random
  seed = analogRead(analogPin) & 0x000000FF;
  packetnum = (int)seed;
#ifdef DebugPrint
  Serial.print("Packetnumber: "); Serial.println(packetnum);
#endif

  // Input to read the state of the pump sensor
  pinMode(12, INPUT_PULLUP);

  pinMode(11, OUTPUT);   // only needed for tests and debugging
  
  // Hard Reset the RFM module
  pinMode(RFM69_RST, OUTPUT);   // Feather M0: 4
  digitalWrite(RFM69_RST, HIGH);  
  delay(100);
  digitalWrite(RFM69_RST, LOW);
  delay(100);

  _slaveSelectPin = RFM69_CS;    // Feather M0: 8
  _interruptPin = RFM69_IRQ;     // Feather M0: 3
  _interruptNum = RFM69_IRQN;    // Feather M0: 3
  _mode = RF69_MODE_STANDBY;     
  _promiscuousMode = false;
  _powerLevel = 31;
  _isRFM69HW = IS_RFM69HCW;
  _inISR = false;

    _initResult = 0x00;

  // Initialize radio
  _initResult = initialize(FREQUENCY, NODEID, NETWORKID);
  
  if (IS_RFM69HCW) 
  {
    setHighPower(true);    // Only for RFM69HCW & HW!
  }
  
  setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)

  encrypt(ENCRYPTKEY);
  //encrypt(0);

  //Another blinky sequence to show progress of programm
  Blink(LED, 200, 10);
  
#ifdef DebugPrint
  Serial.print("\nTransmitting at ");
  Serial.print(FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(" MHz");
#endif

 //****************    Only for tests    ****************************
  
  // GPIO to toggle GPIO11 which can be connected with 12 (for tests)
  //pinMode(11, OUTPUT);

  // Initially set Pump is on
  //digitalWrite(11, LOW);
  //#ifdef DebugPrint
  //Serial.println("Simulate: initially Pump is on");  
  //#endif
  //*************************************************************************

  // actState and oldstate are set to 1 initially
  
  while (!sendMessage(actState, oldState))
  {
     lastMode = _mode;
     setMode(RF69_MODE_STANDBY);  // Prevent the delay from beeing interrupted by interrupts from the radio, 
                                //otherweise the delay never returns      
     delay(180000);  // Wait 180 seconds before the next try to connect to the Gateway via Rfm69 (normal)
     //delay(5000);  // Wait 5 seconds before the next try (for tests)
     setMode(lastMode);            // Restore previous mode
  }
  
  // if ack received wait 15 sec
  lastMode = _mode;
  setMode(RF69_MODE_STANDBY);  // Prevent the delay from beeing interrupted by interrupts from the radio, 
                                  //otherweise the delay never returns      
  delay(15000);  // Wait x milliseconds between transmits, could also 'sleep' here!
  //delay(5000);  // Wait x milliseconds between transmits, could also 'sleep' here!
  setMode(lastMode);            // Restore previous mode
}

void loop() 
{
  lastMode = _mode;
  setMode(RF69_MODE_STANDBY);  // Prevent the delay from beeing interrupted by interrupts from the radio, 
                                //otherweise the delay never returns 
  delay(500);  // Wait x seconds between transmits, could also 'sleep' here! 
  
  setMode(lastMode);            // Restore previous mode

  /*
  // This is only for tests (simulates state-changes of input about every 20 sec)
  Pin11Switchcounter++;
  if ((Pin11Switchcounter % 40) == 0)
  {
    if (digital_11_State)
    {  
      Serial.println("Set Low");  
      digitalWrite(11, LOW);
      digital_11_State = false;
    }
    else
    { 
      Serial.println("Set High");  
      digitalWrite(11, HIGH);
      digital_11_State = true;
    } 
  }
  */


  #ifdef DebugPrint
     //Serial.println("Loop");
  #endif
  
  if (digitalRead(12) == 0)   // Pump is on 
  {
    lastMode = _mode;
    setMode(RF69_MODE_STANDBY);
    delay(150);
    setMode(lastMode);
    
    if (digitalRead(12) == 0)   // Pump is still on
    {       
        if (oldState == 1)   // was off before
        {        
          actState = 0;
          //Serial.println("Pump is on");
          skipCounter = 0;
          if (sendMessage(actState, oldState))
          {                 
            oldState = 0;                       
            repeatSend = 0;
          }
          else
          {
            packetnum--;                
            if (repeatSend > maxRepeatSend)
            {            
              oldState = 0;         
              repeatSend = 0;           
            }
            else
            {           
            repeatSend++;
         #ifdef DebugPrint
            Serial.print("Repeat "); Serial.println(repeatSend);
         #endif   
            lastMode = _mode;
            setMode(RF69_MODE_STANDBY);
            delay(3000);                // Delay between repeats             
            setMode(lastMode);
            }
          }                    
        }  
    }  
  }
   
  else
   
  {   
    lastMode = _mode;
    setMode(RF69_MODE_STANDBY);
    delay(150);
    setMode(lastMode);    
    if (digitalRead(12) != 0) // Pump is still off
    {          
      if (oldState == 0)
      {          
        actState = 1;
        //Serial.println("Pump is off");
        skipCounter = 0;
        if (sendMessage(actState, oldState))
        {
          
          oldState = 1;      
          repeatSend = 0; 
        }
        else
        {
          packetnum--;          
          if (repeatSend > maxRepeatSend)
          {   
            oldState = 1;         
            repeatSend = 0;           
          }
          else
          {          
            repeatSend++;
         #ifdef DebugPrint 
            Serial.print("Repeat "); Serial.println(repeatSend);
         #endif   
            lastMode = _mode;
            setMode(RF69_MODE_STANDBY);
            delay(3000);                 // Delay between repeats        
            setMode(lastMode);
          }
        }              
      }
    }
  }
  
        if (skipCounter > 5)       
        {
            skipCounter = 0;
        if ((regsRetStruct.ErrorCode == (int16_t)ERR_LOC_BEGIN_FUNCTION_NOT_EXECUTED) ||
            (regsRetStruct_2.ErrorCode == (int16_t)ERR_LOC_BEGIN_FUNCTION_NOT_EXECUTED))
        {
          #ifdef DebugPrint
              Serial.println("ERROR: .begin function not executed in Setup");
          #endif
        }  
        // Read 'Current' from the Smartmeter if its releaseTime has expired
        if (readSummedCurrent.isReleased())
        {
          regsRetStruct = readSummedCurrent.get_2_InputRegisters(u8addr, FollowReleaseTimespan_Current_Ms, ForceState::RespectReleaseTime);
          regsRetStruct_2 = readTotalPower.get_2_InputRegisters(u8addr, FollowReleaseTimespan_Power_Ms, ForceState::IgnoreReleaseTime);
          if ((regsRetStruct.ErrorCode == (int16_t)ERR_SUCCESS) && (regsRetStruct_2.ErrorCode == (int16_t)ERR_SUCCESS))
          {
            //Calculate value as float and print out
            int16_2_float_function_result resultStruct = reform_uint16_2_float32(regsRetStruct.HighReg, regsRetStruct.LowReg);
            int16_2_float_function_result resultStruct_2 = reform_uint16_2_float32(regsRetStruct_2.HighReg, regsRetStruct_2.LowReg);
            if ((resultStruct.validity == true) && (resultStruct_2.validity == true))
            {
              ActCurrent = resultStruct.value;
              ActPower = resultStruct_2.value;
              dataContainer.SetNewValues(millis(), ActCurrent, ActPower, ActImportWork);
              #ifdef DebugPrint
                Serial.println("  " + String(resultStruct.value, 4) + "  Amps");
                Serial.println("  " + String(resultStruct_2.value, 4) + "  Watt");
              #endif            
            }
            else
            {
              ActCurrent = NotValidValue;
              ActPower = NotValidValue;
              #ifdef DebugPrint
                Serial.println("Current value not valid ");
              #endif 
            } 
          }
          else
          {
            ActCurrent = NotValidValue;
            ActPower = NotValidValue;
            #ifdef DebugPrint       
              Serial.print("Reading Power failed. Error Code: " );
              Serial.print((int16_t)regsRetStruct.ErrorCode);
              Serial.println("");
            #endif
          }
          
        }
        // Read 'Work' from the Smartmeter if its releaseTime has expired
        if (readImportWork.isReleased() || isForcedReadInputWork)
        {
          ForceState forceState = isForcedReadInputWork ? IgnoreReleaseTime : RespectReleaseTime;
          
          isForcedReadInputWork = false;
          
          regsRetStruct = readImportWork.get_2_InputRegisters(u8addr, FollowReleaseTimespan_Work_Ms, forceState);
          if (regsRetStruct.ErrorCode == (int16_t)ERR_SUCCESS)
          {
            //Calculate value as float and print out
            ImpWorkLow = regsRetStruct.LowReg;
            ImpWorkHigh = regsRetStruct.HighReg;
            ActImportWork = (((uint32_t)regsRetStruct.HighReg) << 16) | regsRetStruct.LowReg;

            int16_2_float_function_result resultStruct = reform_uint16_2_float32(regsRetStruct.HighReg, regsRetStruct.LowReg);
            if (resultStruct.validity == true)
            {
              #ifdef DebugPrint
                Serial.println("  " + String(resultStruct.value, 4) + "  KWh");
              #endif                               
            }
            else
            {
              #ifdef DebugPrint
                Serial.println("Work value not valid ");
              #endif 
            } 
          }
           else
          {
            ActImportWork = 0xffffffff;
            #ifdef DebugPrint
              Serial.print("Reading Work failed. Error Code: ");
              Serial.print((int16_t)regsRetStruct.ErrorCode);
              Serial.println("");
            #endif           
          }
        }
              
        if (dataContainer.hasToBeSent())
        {
          sampleValues = dataContainer.getSampleValuesAndReset();
          #ifdef DebugPrint
            Serial.print("Valus from DataContainer: Current: ");
            Serial.print(sampleValues.AverageCurrent);
            Serial.print(" Amps ");
            Serial.print(sampleValues.AveragePower);
            Serial.print(" Watt ");
          #endif
          int16_2_float_function_result resultStruct = reform_uint16_2_float32(ImpWorkHigh, ImpWorkLow);
          #ifdef DebugPrint
            Serial.print(" " + String(resultStruct.value, 4) + "  kWh  ");
          #endif
          int dataState = 2;  // treat as data from the smartmeter
          
          // Current and Power is multilied by 100, take only the places before the decimal point
          volatile uint32_t intCurrent = (uint32_t)round(abs(sampleValues.AverageCurrent * 100));
          volatile uint32_t intPower = (uint32_t)round(abs(sampleValues.AveragePower * 100));
          
          // Work is transmitted as two uint16_t values as higher and lower bytes in one uint32_t number
          volatile uint32_t intWork = ImpWorkLow | (ImpWorkHigh << 16);
          
          sendMessage(dataState, dataState, intCurrent, intPower, intWork);
          #ifdef DebugPrint
            Serial.print(sampleValues.EndTime_Ms - sampleValues.StartTime_Ms);         
            Serial.print(" ms");
            Serial.println("");
          #endif
        }
        else
        {
          //Serial.println("Nothing to send");
        }
        Serial1.flush();
        
        }
        skipCounter++;

  receiveDone(); //put radio in RX mode
  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
  
}


//bool sendMessage(int pActState, int pOldState, uint32_t pCurrent, u_int32_t pPower, uint16_t pWork_High, uint16_t pWork_Low)
bool sendMessage(int pActState, int pOldState, uint32_t pCurrent, u_int32_t pPower, uint32_t pWork)
{     
      // Warning: Writing behind the end is managed through union with Char[27]
      // see definition
      
    
      packetnum++;
      packetnum = packetnum & 0x00FF;
      // sendInfo is transmitted to the gateway for tests, first 3 digits are packetnum, last digit is repeatSend
      
      sendInfo = packetnum * 10;
      sendInfo = sendInfo + repeatSend;

      #ifdef DebugPrint    
      Serial.print("SendInfo: "); Serial.println(sendInfo);
      #endif
     
     // sprintf(radiopacket, "%.3d %.1d %.1d %.4d %.4d %.4d\n", packetnum, pActState, pOldState, sendInfo, val_2, val_3);
        sprintf(packets.radiopacket, "%.3d %.1d %.1d %.4d %.4d %.4d %.4d\n", packetnum, pActState, pOldState, sendInfo, 0, 0, 0);
        
        if (pActState > 1)
        {
        // Current      
        packets.radiopacket[13] = (byte)(pCurrent >> 24) & 0x000000FF;
        packets.radiopacket[14] = (byte)(pCurrent >> 16) & 0x000000FF;
        packets.radiopacket[15] = (byte)(pCurrent >> 8) & 0x000000FF;
        packets.radiopacket[16] = (byte)pCurrent & 0x000000FF;
        
        
        // Power        
        packets.radiopacket[18] = (byte)(pPower >> 24) & 0x000000FF;
        packets.radiopacket[19] = (byte)(pPower >> 16) & 0x000000FF;
        packets.radiopacket[20] = (byte)(pPower >> 8) & 0x000000FF;
        packets.radiopacket[21] = (byte)pPower & 0x000000FF;

        // Work              
        packets.radiopacket[23] = (byte)(pWork >> 24) & 0x000000FF;
        packets.radiopacket[24] = (byte)(pWork >> 16) & 0x000000FF;       
        packets.radiopacket[25] = (byte)(pWork >> 8) & 0x000000FF;
        packets.radiopacket[26] = (byte)pWork & 0x000000FF;
        }
        else
        {
          //Current
        packets.radiopacket[13] = (byte)'a';
        packets.radiopacket[14] = (byte)'a';
        packets.radiopacket[15] = (byte)'a';
        packets.radiopacket[16] = (byte)'a';
        
        
        // Power
        
        packets.radiopacket[18] = (byte)'a';
        packets.radiopacket[19] = (byte)'a';
        packets.radiopacket[20] = (byte)'a';
        packets.radiopacket[21] = (byte)'a';

        // Work              
        packets.radiopacket[23] = (byte)'a';
        packets.radiopacket[24] = (byte)'a';       
        packets.radiopacket[25] = (byte)'a';
        packets.radiopacket[26] = (byte)'a';
        }
        

        // Is for debugging
        /*
        volatile byte B_00 = packets.radiopacket[0];
        volatile byte B_01 = packets.radiopacket[1];
        volatile byte B_02 = packets.radiopacket[2];
        volatile byte B_03 = packets.radiopacket[3];
        volatile byte B_04 = packets.radiopacket[4];
        volatile byte B_05 = packets.radiopacket[5];
        volatile byte B_06 = packets.radiopacket[6];
        volatile byte B_07 = packets.radiopacket[7];
        volatile byte B_08 = packets.radiopacket[8];
        volatile byte B_09 = packets.radiopacket[9];
        volatile byte B_10 = packets.radiopacket[10];
        volatile byte B_11 = packets.radiopacket[11];
        volatile byte B_12 = packets.radiopacket[12];
        volatile byte B_13 = packets.radiopacket[13];
        volatile byte B_14 = packets.radiopacket[14];
        volatile byte B_15 = packets.radiopacket[15];
        volatile byte B_16 = packets.radiopacket[16];
        volatile byte B_17 = packets.radiopacket[17];
        volatile byte B_18 = packets.radiopacket[18];
        volatile byte B_19 = packets.radiopacket[19];
        volatile byte B_20 = packets.radiopacket[20];
        volatile byte B_21 = packets.radiopacket[21];
        volatile byte B_22 = packets.radiopacket[22];
        volatile byte B_23 = packets.radiopacket[23];
        volatile byte B_24 = packets.radiopacket[24];
        volatile byte B_25 = packets.radiopacket[25];
        volatile byte B_26 = packets.radiopacket[26];
        volatile byte B_27 = packets.radiopacket[27];
        */

        // Is for debugging
        if (pActState == 2)
        {
          volatile int dummy = pActState;
        }
        

      /*

      if (pActState == 0)
      {
        //sprintf(radiopacket, "%.3d 0 %.4d %.4d %.4d\n", packetnum, val_1, val_2, val_3);
        // repeatSend transmitted for tests to monitor the quality of the transmission
        sprintf(radiopacket, "%.3d 0 %.4d %.4d %.4d\n", packetnum, sendInfo, val_2, val_3);
      }
      else
      {
        if (paActState == 1)
        {
          //sprintf(radiopacket, "%.3d 1 %.4d %.4d %.4d\n", packetnum, val_1, val_2, val_3);
          // repeatSend transmitted for tests to monitor the quality of the transmission
          sprintf(radiopacket, "%.3d 1 %.4d %.4d %.4d\n", packetnum, sendInfo, val_2, val_3);
        }       
        else
        {
          //sprintf(radiopacket, "%.3d 3 %.4d %.4d %.4d\n", packetnum, val_1, val_2, val_3);
          // repeatSend transmitted for tests to monitor the quality of the transmission
          sprintf(radiopacket, "%.3d 1 %.4d %.4d %.4d\n", packetnum, sendInfo, val_2, val_3);
        }
      }
      */
      
#ifdef DebugPrint    
      Serial.print("Sending "); Serial.println(packets.radiopacket);
#endif
      // sendACK();
      //target node Id, message as string or byte array, message length, No. of Retries, timeout wait for ACK in ms
      // Changed by RoSchmi
      //if (sendWithRetry(RECEIVER, packets.radiopacket, strlen(packets.radiopacket), 4, 600))
      if (sendWithRetry(RECEIVER, packets.radiopacket, radioPacketLength, 8, 500))       
      {
#ifdef DebugPrint
        Serial.println("OK");
#endif
        Blink(LED, 50, 3); //blink LED 3 times, 50ms between blinks
        return true;
      }
      else
      {
#ifdef DebugPrint       
      Serial.println("No Ack returned");
#endif 
      return false;
      }  
}


void Blink(byte PIN, byte DELAY_MS, byte loops)
{
  for (byte i = 0; i < loops; i++)
  {
    digitalWrite(PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN, LOW);
    delay(DELAY_MS);
  }
}

bool canSend()
{
  if (_mode == RF69_MODE_RX && PAYLOADLEN == 0 && readRSSI(false) < CSMA_LIMIT) // if signal stronger than -100dBm is detected assume channel activity
  {
    setMode(RF69_MODE_STANDBY);
#ifdef DebugPrint
    Serial.print("Can send, RSSI = "); Serial.println(readRSSI(false));
#endif
    return true;
  }
#ifdef DebugPrint
  Serial.print("Can not send, RSSI = "); Serial.println(readRSSI(false));
#endif
  return false;
}

// internal function
void sendFrame(uint8_t toAddress, const void* buffer, uint8_t bufferSize, bool requestACK, bool sendACK)
{
  unsigned long StartTime = millis();
  unsigned long actTime = StartTime;
  
  setMode(RF69_MODE_STANDBY); // turn off receiver to prevent reception while filling fifo
  //Serial.println("I am in sendFrame routine (2)");

  while (true)     // unusual loop, this should work even if millis is corrupted 
  {
      actTime = millis();
      if (actTime < StartTime)    // for millis rollover, don't care that timeout is longer at rollover
      {
        StartTime = 0;
      }
      if ((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) != 0x00)
      {
        break;
      }
      if(actTime - StartTime > 100)
      {
#ifdef DebugPrint
        Serial.println("Could not set to RF69_MODE_STANDBY, REG_IRQFLAGS1 is: " );Serial.println(readReg(REG_IRQFLAGS1));
#endif
        break;
      }
  }
  writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_00); // DIO0 is "Packet Sent"
  if (bufferSize > RF69_MAX_DATA_LEN) bufferSize = RF69_MAX_DATA_LEN;

  // control byte
  uint8_t CTLbyte = 0x00;
  if (sendACK)
    CTLbyte = RFM69_CTL_SENDACK;
  else if (requestACK)
    CTLbyte = RFM69_CTL_REQACK;
 
  // write to FIFO
  select();
  SPI.transfer(REG_FIFO | 0x80);
  SPI.transfer(bufferSize + 3);
  SPI.transfer(toAddress);
  SPI.transfer(_address);
  SPI.transfer(CTLbyte);

  for (uint8_t i = 0; i < bufferSize; i++)
    SPI.transfer(((uint8_t*) buffer)[i]);
  unselect();

  interruptHappened = false;  //used only for debugging
  StartTime = millis();
  // no need to wait for transmit mode to be ready since its handled by the radio
  setMode(RF69_MODE_TX);
  
  while (true)     // unusual loop, this should work even if millis is corrupted by the interrupt
  {
      actTime = millis();
      if (actTime < StartTime)    // for millis rollover, don't care that timeout is longer at rollover
      {
        StartTime = 0;
      }
      if (digitalRead(_interruptPin) == 1)
      {
        break;
      }
      //Serial.print(digitalRead(_interruptPin)); Serial.print(" elapsed Time: ");  Serial.print(millis() - StartTime); Serial.print("  interruptHappened =  ");Serial.println(interruptHappened);
      if(actTime - StartTime > RF69_TX_LIMIT_MS)
      {
        break;
      }
  }

  //Serial.print("Wait loop finshed ");Serial.print(digitalRead(_interruptPin)); Serial.print(" elapsed Time: ");  Serial.print(millis() - StartTime); Serial.print("  interruptHappened =  ");Serial.println(interruptHappened);
  
  setMode(RF69_MODE_STANDBY);
  
  StartTime = millis();
  while (true)     // unusual loop, this should work even if millis is corrupted 
  {
      actTime = millis();
      if (actTime < StartTime)    // for millis rollover, don't care that timeout is longer at rollover
      {
        StartTime = 0;
      }
      if ((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) != 0x00)
      {
        break;
      }
      if(actTime - StartTime > 100)
      {
#ifdef DebugPrint
        Serial.println("Could not set to RF69_MODE_STANDBY, REG_IRQFLAGS1 is: " );Serial.println(readReg(REG_IRQFLAGS1));
#endif
        break;
      }
  }
}

// should be called immediately after reception in case sender wants ACK
//void sendACK(const void* buffer, uint8_t bufferSize)
 void sendACK()
{
   const void* buffer = 0;
   uint8_t bufferSize = 0;
  
  setMode(RF69_MODE_RX); //Switching from STANDBY to RX before TX
  int _RSSI = RSSI; //save payload received RSSI value
  bool canSendACK = false; 
  unsigned long now = millis();
  while (millis() - now < ACK_CSMA_LIMIT_MS) //wait for free network the same time as sender waits for ACK (act. 200 ms)
  {
    if (readRSSI(false) < CSMA_LIMIT) //if signal weaker than -90dBm(CSMA_LIMIT) is detected channel should be free
    {
      canSendACK = true;
      break;
    }
    else    // readRSSI does not work, so we set canSendACK true for every case
    {
      canSendACK = true;
      break;
    }
  }
  if (canSendACK) // channel is free let's send ACK
  {
    //Serial.println(" ");Serial.print("ACK sent:");Serial.print(millis()-now, DEC); Serial.print("ms;RSSI:");Serial.println(readRSSI(false), DEC); Serial.flush();
    sendFrame(SENDERID, buffer, bufferSize, false, true);
  }
  RSSI = _RSSI; //restore payload RSSI  
}
// should be called immediately after reception in case sender wants ACK
void sendACK_original(const void* buffer, uint8_t bufferSize) {
  ACK_REQUESTED = 0;   // TWS added to make sure we don't end up in a timing race and infinite loop sending Acks
  uint8_t sender = SENDERID;
  int16_t _RSSI = RSSI; // save payload received RSSI value
  writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
  uint32_t now = millis();
  while (!canSend() && millis() - now < RF69_CSMA_LIMIT_MS) 
  {
    receiveDone();
  }
  SENDERID = sender;    // TWS: Restore SenderID after it gets wiped out by receiveDone()
  sendFrame(sender, buffer, bufferSize, false, true);
  RSSI = _RSSI; // restore payload RSSI
}


void send(uint8_t toAddress, const void* buffer, uint8_t bufferSize, bool requestACK)
{
  writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
  uint32_t now = millis();
  //Serial.println("I am in send routine");
  while (!canSend() && millis() - now < RF69_CSMA_LIMIT_MS) 
  {
  receiveDone();
#ifdef DebugPrint
  Serial.println("Wait for can send");
#endif
  }
  sendFrame(toAddress, buffer, bufferSize, requestACK, false);
}

// should be polled immediately after sending a packet with ACK request
bool ACKReceived(uint8_t fromNodeID) 
{ 
  if (receiveDone())
  {
      if (ACK_RECEIVED)  
      {
          if((SENDERID == fromNodeID) || (fromNodeID == RF69_BROADCAST_ADDR))
          {
            //RoSchmi    
            //Serial.println("ACKReceived returned true");
           
            
            return true;
          }   
      }         
  }
    return false;
}

// to increase the chance of getting a packet across, call this function instead of send
// and it handles all the ACK requesting/retrying for you :)
// The only twist is that you have to manually listen to ACK requests on the other side and send back the ACKs
// The reason for the semi-automaton is that the lib is interrupt driven and
// requires user action to read the received data and decide what to do with it
// replies usually take only 5..8ms at 50kbps@915MHz
bool sendWithRetry(uint8_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries, uint8_t retryWaitTime) 
{
  volatile unsigned long sentTime;
  volatile unsigned long actTime;
  //Serial.println("I am in sendWithRetry routine");
  for (uint8_t i = 0; i <= retries; i++)
  {
    send(toAddress, buffer, bufferSize, true);
    sentTime = millis();
    //while (millis() - sentTime < retryWaitTime)
    while (true)
    {       
      if (ACKReceived(toAddress))
      {
#ifdef DebugPrint        
        Serial.print("ACK received   "); Serial.print(" ~ms: "); Serial.println(millis() - sentTime);
#endif
        return true;
      }
      
      // do some dummy commands and show activity on the LED
      digitalWrite(LED, HIGH);     
      for (int i2 = 0; i2 < 10000; i2++)
      {
        i2++;
        i2--;
      }
      actTime = millis();
      digitalWrite(LED, LOW);   
      //Serial.println(" ("); Serial.print("actTime = "); Serial.print(actTime); Serial.print("  sentTime = ");Serial.print(sentTime); Serial.print(" elapsed = ");Serial.print(actTime - sentTime);
      if(actTime - sentTime > retryWaitTime)     
      break;      
      }
#ifdef DebugPrint
    Serial.print(" RETRY#"); Serial.println(i + 1);
#endif
  }
#ifdef DebugPrint
  Serial.println(" Leaving sendWithRetry, State: false");
#endif
  return false;
}
// set *transmit/TX* output power: 0=min, 31=max
// this results in a "weaker" transmitted signal, and directly results in a lower RSSI at the receiver
// the power configurations are explained in the SX1231H datasheet (Table 10 on p21; RegPaLevel p66): http://www.semtech.com/images/datasheet/sx1231h.pdf
// valid powerLevel parameter values are 0-31 and result in a directly proportional effect on the output/transmission power
// this function implements 2 modes as follows:
//       - for RFM69W the range is from 0-31 [-18dBm to 13dBm] (PA0 only on RFIO pin)
//       - for RFM69HW the range is from 0-31 [5dBm to 20dBm]  (PA1 & PA2 on PA_BOOST pin & high Power PA settings - see section 3.3.7 in datasheet, p22)
void setPowerLevel(uint8_t powerLevel)
{
  _powerLevel = (powerLevel > 31 ? 31 : powerLevel);
  if (_isRFM69HW) _powerLevel /= 2;
  writeReg(REG_PALEVEL, (readReg(REG_PALEVEL) & 0xE0) | _powerLevel);
}

// checks if a packet was received and/or puts transceiver in receive (ie RX or listen) mode
bool receiveDone() 
{
  //ATOMIC_BLOCK(ATOMIC_FORCEON)
  //{
  noInterrupts(); // re-enabled in unselect() via setMode() or via receiveBegin()
  if (_mode == RF69_MODE_RX && PAYLOADLEN > 0)
  {
    setMode(RF69_MODE_STANDBY); // enables interrupts
    return true;
  }
  else if (_mode == RF69_MODE_RX) // already in RX no payload yet
  {
    interrupts(); // explicitly re-enable interrupts
    return false;
  }
  receiveBegin();
  return false;
//}
}

// internal function
void isr0() 
{ _inISR = true;
 interruptHandler();
 
_inISR = false; 
}

// internal function - interrupt gets called when a packet is received
void interruptHandler() 
{  
  pinMode(13,OUTPUT);
  digitalWrite(13, HIGH);  // Light LED to show that we are in the interrupt 
  if (_mode == RF69_MODE_RX && (readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY))
  {
    //Serial.println("\n I am in the if loop");
    //RSSI = readRSSI();
    setMode(RF69_MODE_STANDBY);
    select();
    SPI.transfer(REG_FIFO & 0x7F);
    PAYLOADLEN = SPI.transfer(0);
    //Serial.print("\n PayLodLength = ");Serial.println(PAYLOADLEN);
    PAYLOADLEN = PAYLOADLEN > 66 ? 66 : PAYLOADLEN; // precaution
    TARGETID = SPI.transfer(0);
    //Serial.print("\n TargetID = ");Serial.println(TARGETID);
    if(!(_promiscuousMode || TARGETID == _address || TARGETID == RF69_BROADCAST_ADDR) // match this node's address, or broadcast address or anything in promiscuous mode
       || PAYLOADLEN < 3) // address situation could receive packets that are malformed and don't fit this libraries extra fields
    {
      PAYLOADLEN = 0;
      unselect();
      receiveBegin();
      //digitalWrite(4, 0);
      return;
    }
    //Serial.println("\n I am in the if loop");
    DATALEN = PAYLOADLEN - 3;
    SENDERID = SPI.transfer(0);
    //Serial.print("\n SenderID = ");Serial.println(SENDERID);
    uint8_t CTLbyte = SPI.transfer(0);
    //Serial.print("\n CTLbyte = ");Serial.println(CTLbyte);

    ACK_RECEIVED = CTLbyte & RFM69_CTL_SENDACK; // extract ACK-received flag
    ACK_REQUESTED = CTLbyte & RFM69_CTL_REQACK; // extract ACK-requested flag
    
    interruptHook(CTLbyte);     // TWS: hook to derived class interrupt function

    for (uint8_t i = 0; i < DATALEN; i++)
    {
      DATA[i] = SPI.transfer(0);
    }
    if (DATALEN < RF69_MAX_DATA_LEN) DATA[DATALEN] = 0; // add null at end of string
    unselect();
    setMode(RF69_MODE_RX);
    //Serial.print("\n I am leaving if loop, DataLength = "); Serial.println(DATALEN);
  }
  RSSI = readRSSI(false);
  
  digitalWrite(13, LOW);
  interruptHappened = true;  
  
  //digitalWrite(4, 0);
}

//=============================================================================
// interruptHook() - gets called by the base class interrupt handler right after the header is fetched.
//=============================================================================
void interruptHook(uint8_t CTLbyte) {
  ACK_RSSI_REQUESTED = CTLbyte & RFM69_CTL_RESERVE1; // TomWS1: extract the ACK RSSI request bit (could potentially merge with ACK_REQUESTED)
  // TomWS1: now see if this was an ACK with an ACK_RSSI response
  if (ACK_RECEIVED && ACK_RSSI_REQUESTED) {
    // the next two bytes contain the ACK_RSSI (assuming the datalength is valid)
    if (DATALEN >= 1) {
      _ackRSSI= -1 * SPI.transfer(0); //rssi was sent as single byte positive value, get the real value by * -1
      DATALEN -= 1;   // and compensate data length accordingly
      // TomWS1: Now dither transmitLevel value (register update occurs later when transmitting);
      if (_targetRSSI != 0) {
        // if (_isRFM69HW) {
          // if (_ackRSSI < _targetRSSI && _transmitLevel < 51) _transmitLevel++;
          // else if (_ackRSSI > _targetRSSI && _transmitLevel > 32) _transmitLevel--;
        // } else {
        if (_ackRSSI < _targetRSSI && _transmitLevel < 31) { _transmitLevel++; /*Serial.println("\n ======= _transmitLevel ++   ======");*/ }
        else if (_ackRSSI > _targetRSSI && _transmitLevel > 0) { _transmitLevel--; /*Serial.println("\n ======= _transmitLevel --   ======");*/ }
        //}
      }
    }
  }
}


// get the received signal strength indicator (RSSI)
int16_t readRSSI(bool forceTrigger) {
  int16_t rssi = 0;
  if (forceTrigger)
  {
    if (readReg(REG_TESTDAGC) & RF_DAGC_IMPROVED_LOWBETA0 == 0x00)  //RssiStart command and RssiDone flags are not usable when DAGC is turned on
    {
      // RSSI trigger not needed if DAGC is in continuous mode
      writeReg(REG_RSSICONFIG, RF_RSSI_START);
      while ((readReg(REG_RSSICONFIG) & RF_RSSI_DONE) == 0x00); // wait for RSSI_Ready
    }
  }
  rssi = -readReg(REG_RSSIVALUE);
  rssi >>= 1;
  return rssi;
}


 // internal function
void receiveBegin() {
  DATALEN = 0;
  SENDERID = 0;
  TARGETID = 0;
  PAYLOADLEN = 0;
  ACK_REQUESTED = 0;
  ACK_RECEIVED = 0;
  RSSI = 0;
  if (readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY)
  {
    writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
  }
  writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01); // set DIO0 to "PAYLOADREADY" in receive mode
  setMode(RF69_MODE_RX);
} 

uint8_t initialize(uint8_t freqBand, uint8_t nodeID, uint8_t networkID)
{
  const uint8_t CONFIG[][2] =
  {
    /* 0x01 */ { REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY },
    /* 0x02 */ { REG_DATAMODUL, RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00 }, // no shaping
    /* 0x03 */ { REG_BITRATEMSB, RF_BITRATEMSB_55555}, // default: 4.8 KBPS
    /* 0x04 */ { REG_BITRATELSB, RF_BITRATELSB_55555},
    /* 0x05 */ { REG_FDEVMSB, RF_FDEVMSB_50000}, // default: 5KHz, (FDEV + BitRate / 2 <= 500KHz)
    /* 0x06 */ { REG_FDEVLSB, RF_FDEVLSB_50000},

    /* 0x07 */ { REG_FRFMSB, (uint8_t) (freqBand==RF69_315MHZ ? RF_FRFMSB_315 : (freqBand==RF69_433MHZ ? RF_FRFMSB_433 : (freqBand==RF69_868MHZ ? RF_FRFMSB_868 : RF_FRFMSB_915))) },
    /* 0x08 */ { REG_FRFMID, (uint8_t) (freqBand==RF69_315MHZ ? RF_FRFMID_315 : (freqBand==RF69_433MHZ ? RF_FRFMID_433 : (freqBand==RF69_868MHZ ? RF_FRFMID_868 : RF_FRFMID_915))) },
    /* 0x09 */ { REG_FRFLSB, (uint8_t) (freqBand==RF69_315MHZ ? RF_FRFLSB_315 : (freqBand==RF69_433MHZ ? RF_FRFLSB_433 : (freqBand==RF69_868MHZ ? RF_FRFLSB_868 : RF_FRFLSB_915))) },

    // looks like PA1 and PA2 are not implemented on RFM69W, hence the max output power is 13dBm
    // +17dBm and +20dBm are possible on RFM69HW
    // +13dBm formula: Pout = -18 + OutputPower (with PA0 or PA1**)
    // +17dBm formula: Pout = -14 + OutputPower (with PA1 and PA2)**
    // +20dBm formula: Pout = -11 + OutputPower (with PA1 and PA2)** and high power PA settings (section 3.3.7 in datasheet)
    ///* 0x11 */ { REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | RF_PALEVEL_OUTPUTPOWER_11111},
    ///* 0x13 */ { REG_OCP, RF_OCP_ON | RF_OCP_TRIM_95 }, // over current protection (default is 95mA)

    // RXBW defaults are { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_5} (RxBw: 10.4KHz)
    /* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_2 }, // (BitRate < 2 * RxBw)
    //for BR-19200: /* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_3 },
    /* 0x25 */ { REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01 }, // DIO0 is the only IRQ we're using
    /* 0x26 */ { REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF }, // DIO5 ClkOut disable for power saving
    /* 0x28 */ { REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN }, // writing to this bit ensures that the FIFO & status flags are reset
    /* 0x29 */ { REG_RSSITHRESH, 220 }, // must be set to dBm = (-Sensitivity / 2), default is 0xE4 = 228 so -114dBm
    ///* 0x2D */ { REG_PREAMBLELSB, RF_PREAMBLESIZE_LSB_VALUE } // default 3 preamble bytes 0xAAAAAA
    /* 0x2E */ { REG_SYNCCONFIG, RF_SYNC_ON | RF_SYNC_FIFOFILL_AUTO | RF_SYNC_SIZE_2 | RF_SYNC_TOL_0 },
    /* 0x2F */ { REG_SYNCVALUE1, 0x2D },      // attempt to make this compatible with sync1 byte of RFM12B lib
    /* 0x30 */ { REG_SYNCVALUE2, networkID }, // NETWORK ID
    /* 0x37 */ { REG_PACKETCONFIG1, RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_OFF },
    /* 0x38 */ { REG_PAYLOADLENGTH, 66 }, // in variable length mode: the max frame size, not used in TX
    ///* 0x39 */ { REG_NODEADRS, nodeID }, // turned off because we're not using address filtering
    /* 0x3C */ { REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RF_FIFOTHRESH_VALUE }, // TX on FIFO not empty
    /* 0x3D */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
    //for BR-19200: /* 0x3D */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
    /* 0x6F */ { REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0 }, // run DAGC continuously in RX mode for Fading Margin Improvement, recommended default for AfcLowBetaOn=0
    {255, 0}
  };
#ifdef DebugPrint
  Serial.print("\nListening for Roland ");
#endif
  digitalWrite(_slaveSelectPin, HIGH);
  pinMode(_slaveSelectPin, OUTPUT);
  SPI.begin();
  unsigned long start = millis();
  uint8_t timeout = 50;
  //unsigned long timeout = 10000;
  uint8_t result = 0x00;
  
  do 
  {
  writeReg(REG_SYNCVALUE1, 0xAA); 
  result = readReg(REG_SYNCVALUE1);
  } while (result != 0xaa && millis()-start < timeout);
  
  start = millis();
  do writeReg(REG_SYNCVALUE1, 0x55); while (readReg(REG_SYNCVALUE1) != 0x55 && millis()-start < timeout);
  if (result == 0xAA)
  {
#ifdef DebugPrint
    Serial.println("\n Result is AA");
#endif
  }
  else
  {
#ifdef DebugPrint
    Serial.println("\n Result is not AA\r\n");
#endif
  }
  for (uint8_t i = 0; CONFIG[i][0] != 255; i++)
  {
  writeReg(CONFIG[i][0], CONFIG[i][1]);
#ifdef DebugPrint
  Serial.print(CONFIG[i][0]);
  Serial.print("  ");
  Serial.print(CONFIG[i][1]);
  Serial.println(" ");
#endif
  }
#ifdef DebugPrint
  Serial.println("Printing REG-initialization:");
Serial.print("REG_OPMODE ("); Serial.print(" 0x01 )"); Serial.println((byte)readReg(0x01));
Serial.print("REG_DATAMODUL ("); Serial.print(" 0x02 )"); Serial.println((byte)readReg(0x02));
Serial.print("REG_BITRATEMSB ("); Serial.print(" 0x03 )"); Serial.println(readReg(0x03));
Serial.print("REG_BITRATELSB ("); Serial.print(" 0x04 )"); Serial.println(readReg(0x04));
Serial.print("REG_FDEVMSB ("); Serial.print(" 0x05 )"); Serial.println(readReg(0x05));
Serial.println(" ");
Serial.print("REG_PALEVEL ("); Serial.print(" 0x11 )"); Serial.println(readReg(0x11));
Serial.print("REG_OCP ("); Serial.print(" 0x13 )"); Serial.println(readReg(0x13));
Serial.print("REG_DIOMAPPING1 ("); Serial.print(" 0x25 )"); Serial.println(readReg(0x25));
Serial.print("REG_DIOMAPPING2 ("); Serial.print(" 0x26 )"); Serial.println(readReg(0x26));
Serial.print("REG_IRQFLAGS2 ("); Serial.print(" 0x28 )"); Serial.println(readReg(0x28));
Serial.print("REG_RSSITHRESH ("); Serial.print(" 0x29 )"); Serial.println(readReg(0x29));
Serial.print("REG_SYNCCONFIG ("); Serial.print(" 0x2E )"); Serial.println(readReg(0x2E));
Serial.print("REG_SYNCVALUE1 ("); Serial.print(" 0x2F )"); Serial.println(readReg(0x2F));
Serial.print("REG_SYNCVALUE2 ("); Serial.print(" 0x30 )"); Serial.println(readReg(0x30));
Serial.print("REG_PACKETCONFIG1 ("); Serial.print(" 0x37 )"); Serial.println(readReg(0x37));
Serial.print("REG_PAYLOADLENGTH ("); Serial.print(" 0x38 )"); Serial.println(readReg(0x38));
Serial.print("REG_PACKETCONFIG2 ("); Serial.print(" 0x3D )"); Serial.println(readReg(0x3D));
Serial.print("REG_TESTDAGC ("); Serial.print(" 0x6F )"); Serial.println(readReg(0x6F));
#endif

// Encryption is persistent between resets and can trip you up during debugging.
  // Disable it during initialization so we always start from a known state.
  encrypt(0);
 setHighPower(_isRFM69HW); // called regardless if it's a RFM69W or RFM69HW
  setMode(RF69_MODE_STANDBY);
  start = millis();
  while (((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00) && millis()-start < timeout); // wait for ModeReady
  if (millis()-start >= timeout)
    return false;
  _inISR = false;
  attachInterrupt(_interruptNum, isr0, RISING);

  //selfPointer = this;
  _address = nodeID;


  
  
  return result;
}
void writeReg(uint8_t addr, uint8_t value)
{
  select();
  SPI.transfer(addr | 0x80);
  SPI.transfer(value);
  unselect();
}
uint8_t readReg(uint8_t addr)
{
  select();
  SPI.transfer(addr & 0x7F);
  uint8_t regval = SPI.transfer(0);
  unselect();
  return regval;
}
// select the RFM69 transceiver (save SPI settings, set CS low)
void select() {
  noInterrupts();
#if defined (SPCR) && defined (SPSR)
  // save current SPI settings
  _SPCR = SPCR;
  _SPSR = SPSR;
#endif
  // set RFM69 SPI settings
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4); // decided to slow down from DIV2 after SPI stalling in some instances, especially visible on mega1284p when RFM69 and FLASH chip both present
  digitalWrite(_slaveSelectPin, LOW);
}
// unselect the RFM69 transceiver (set CS high, restore SPI settings)
void unselect() {
  digitalWrite(_slaveSelectPin, HIGH);
  // restore SPI settings to what they were before talking to RFM69
#if defined (SPCR) && defined (SPSR)
  SPCR = _SPCR;
  SPSR = _SPSR;
#endif
  maybeInterrupts();
}
inline void maybeInterrupts()
{
  // Only reenable interrupts if we're not being called from the ISR
  if (!_inISR) interrupts();
}
// To enable encryption: radio.encrypt("ABCDEFGHIJKLMNOP");
// To disable encryption: radio.encrypt(null) or radio.encrypt(0)
// KEY HAS TO BE 16 bytes !!!
void encrypt(const char* key) {
  setMode(RF69_MODE_STANDBY);
  if (key != 0)
  {
    select();
    SPI.transfer(REG_AESKEY1 | 0x80);
    for (uint8_t i = 0; i < 16; i++)
    {
      SPI.transfer(key[i]);
    }
#ifdef DebugPrint
      Serial.println("Now the AES_Keys");
      Serial.println(readReg(REG_AESKEY1));
#endif
    unselect();
  }
  writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFE) | (key ? 1 : 0));
}

// for RFM69HW only: you must call setHighPower(true) after initialize() or else transmission won't work
void setHighPower(bool onOff) {
  _isRFM69HW = onOff;
  writeReg(REG_OCP, _isRFM69HW ? RF_OCP_OFF : RF_OCP_ON);
  if (_isRFM69HW) // turning ON
    writeReg(REG_PALEVEL, (readReg(REG_PALEVEL) & 0x1F) | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON); // enable P1 & P2 amplifier stages
  else
    writeReg(REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | _powerLevel); // enable P0 only
}

// internal function
void setHighPowerRegs(bool onOff) {
  writeReg(REG_TESTPA1, onOff ? 0x5D : 0x55);
  writeReg(REG_TESTPA2, onOff ? 0x7C : 0x70);
}

void setMode(uint8_t newMode)
{
  if (newMode == _mode)
    return;
  //Serial.println("Going to set new mode");
  switch (newMode) {
    case RF69_MODE_TX:
      writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_TRANSMITTER);
      if (_isRFM69HW) setHighPowerRegs(true);
      break;
    case RF69_MODE_RX:
      writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_RECEIVER);
      if (_isRFM69HW) setHighPowerRegs(false);
      break;
    case RF69_MODE_SYNTH:
      writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SYNTHESIZER);
      break;
    case RF69_MODE_STANDBY:
      writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_STANDBY);
      //Serial.println("Set mode to Standby");
      break;
    case RF69_MODE_SLEEP:
      writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SLEEP);
      break;
    default:
      return;
  }

  // we are using packet mode, so this check is not really needed
  // but waiting for mode ready is necessary when going from sleep because the FIFO may not be immediately available from previous mode
  while (_mode == RF69_MODE_SLEEP && (readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00); // wait for ModeReady

  _mode = newMode;
}
/*
void SERCOM1_Handler()    // Interrupt handler for SERCOM1
{
  Serial2.IrqHandler();
}
*/
