/*
	Xenon ATAPI Emulation. 
*/

#include "CDROM.h"

//
// IDE status register flags.
//

#define IDE_STATUS_ERR                          ((u8)0x01)
#define IDE_STATUS_IDX                          ((u8)0x02)
#define IDE_STATUS_DRQ                          ((u8)0x08)
#define IDE_STATUS_DRDY                         ((u8)0x40)
#define IDE_STATUS_BSY                          ((u8)0x80)

//
// IDE device control register flags.
//

#define IDE_DEVICE_CONTROL_NIEN                 ((u8)0x02)
#define IDE_DEVICE_CONTROL_SRST                 ((u8)0x04)

//
// IDE command values.
//

#define IDE_COMMAND_DEVICE_RESET                ((u8)0x08)
#define IDE_COMMAND_READ_SECTORS                ((u8)0x20)
#define IDE_COMMAND_READ_DMA_EXT                ((u8)0x25)
#define IDE_COMMAND_WRITE_SECTORS               ((u8)0x30)
#define IDE_COMMAND_WRITE_DMA_EXT               ((u8)0x35)
#define IDE_COMMAND_VERIFY                      ((u8)0x40)
#define IDE_COMMAND_VERIFY_EXT                  ((u8)0x42)
#define IDE_COMMAND_SET_DEVICE_PARAMETERS       ((u8)0x91)
#define IDE_COMMAND_PACKET                      ((u8)0xA0)
#define IDE_COMMAND_IDENTIFY_PACKET_DEVICE      ((u8)0xA1)
#define IDE_COMMAND_READ_MULTIPLE               ((u8)0xC4)
#define IDE_COMMAND_WRITE_MULTIPLE              ((u8)0xC5)
#define IDE_COMMAND_SET_MULTIPLE_MODE           ((u8)0xC6)
#define IDE_COMMAND_READ_DMA                    ((u8)0xC8)
#define IDE_COMMAND_WRITE_DMA                   ((u8)0xCA)
#define IDE_COMMAND_STANDBY_IMMEDIATE           ((u8)0xE0)
#define IDE_COMMAND_FLUSH_CACHE                 ((u8)0xE7)
#define IDE_COMMAND_IDENTIFY_DEVICE             ((u8)0xEC)
#define IDE_COMMAND_SET_FEATURES                ((u8)0xEF)
#define IDE_COMMAND_SECURITY_SET_PASSWORD       ((u8)0xF1)
#define IDE_COMMAND_SECURITY_UNLOCK             ((u8)0xF2)
#define IDE_COMMAND_SECURITY_DISABLE_PASSWORD   ((u8)0xF6)

//
// IDE feature values for an ATA device.
//

#define IDE_FEATURE_SET_TRANSFER_MODE           ((u8)0x03)

//
// IDE feature flags for an ATAPI device.
//

#define IDE_FEATURE_DMA                         ((u8)0x01)
#define IDE_FEATURE_OVL                         ((u8)0x02)

//
// IDE interrupt reason flags for an ATAPI device.
//

#define IDE_INTERRUPT_REASON_CD                 ((u8)0x01)
#define IDE_INTERRUPT_REASON_IO                 ((u8)0x02)

//
// Transfer modes for the set transfer mode feature.
//

#define IDE_TRANSFER_MODE_PIO                   ((u8)0x00)
#define IDE_TRANSFER_MODE_PIO_NO_IORDY          ((u8)0x01)
#define IDE_TRANSFER_MODE_PIO_MODE_3            ((u8)0x0B)
#define IDE_TRANSFER_MODE_PIO_MODE_4            ((u8)0x0C)
#define IDE_TRANSFER_MODE_MWDMA_MODE_0          ((u8)0x20)
#define IDE_TRANSFER_MODE_MWDMA_MODE_1          ((u8)0x21)
#define IDE_TRANSFER_MODE_MWDMA_MODE_2          ((u8)0x22)
#define IDE_TRANSFER_MODE_UDMA_MODE_0           ((u8)0x40)
#define IDE_TRANSFER_MODE_UDMA_MODE_1           ((u8)0x41)
#define IDE_TRANSFER_MODE_UDMA_MODE_2           ((u8)0x42)
#define IDE_TRANSFER_MODE_UDMA_MODE_3           ((u8)0x43)
#define IDE_TRANSFER_MODE_UDMA_MODE_4           ((u8)0x44)
#define IDE_TRANSFER_MODE_UDMA_MODE_5           ((u8)0x45)

//
// Data transfer values for an ATAPI device.
//

#define ATAPI_ATAPI_CD_SECTOR_SHIFT              11
#define ATAPI_ATAPI_CD_SECTOR_SIZE               (1 << ATAPI_ATAPI_CD_SECTOR_SHIFT)
#define ATAPI_ATAPI_CD_SECTOR_MASK               (ATAPI_ATAPI_CD_SECTOR_SIZE - 1)
#define ATAPI_ATAPI_CD_MAXIMUM_TRANSFER_SECTORS  64
#define ATAPI_ATAPI_RAW_CD_SECTOR_SIZE           2352
#define ATAPI_ATAPI_MAXIMUM_TRANSFER_BYTES       (ATAPI_ATAPI_CD_MAXIMUM_TRANSFER_SECTORS * ATAPI_ATAPI_CD_SECTOR_SIZE)
#define ATAPI_ATAPI_MAXIMUM_TRANSFER_PAGES       (ATAPI_ATAPI_MAXIMUM_TRANSFER_BYTES >> PAGE_SHIFT)

//
// Control and status flags for the DBDMA interface.
//

#define ATAPI_DBDMA_CONTROL_RUN                  0x8000
#define ATAPI_DBDMA_CONTROL_PAUSE                0x4000
#define ATAPI_DBDMA_CONTROL_FLUSH                0x2000
#define ATAPI_DBDMA_CONTROL_WAKE                 0x1000
#define ATAPI_DBDMA_CONTROL_DEAD                 0x0800
#define ATAPI_DBDMA_CONTROL_ACTIVE               0x0400

//
// Command codes for a DBDMA command entry.
//

#define ATAPI_DBDMA_COMMAND_OUTPUT_MORE          0
#define ATAPI_DBDMA_COMMAND_OUTPUT_LAST          1
#define ATAPI_DBDMA_COMMAND_INPUT_MORE           2
#define ATAPI_DBDMA_COMMAND_INPUT_LAST           3
#define ATAPI_DBDMA_COMMAND_STORE_QUAD           4
#define ATAPI_DBDMA_COMMAND_LOAD_QUAD            5
#define ATAPI_DBDMA_COMMAND_NOP                  6
#define ATAPI_DBDMA_COMMAND_STOP                 7

CDROM::CDROM()
{
	// Set PCI Ven & Dev ID.
	u32 devID = 0x58021414;
	memcpy(&pciConfigSpace.data[0], &devID, 4);
}

void CDROM::Read(u64 readAddress, u64* data, u8 byteCount)
{

}

void CDROM::Write(u64 writeAddress, u64 data, u8 byteCount)
{

}

void CDROM::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void CDROM::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}
