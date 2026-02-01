#ifndef ATA_DRIVER_H
#define ATA_DRIVER_H
#include "../STD/int.h"
#pragma pack(push, 1)

typedef struct ata_identify {

    /* 0 */
    uint16_t general_config;

    /* 1–9 */
    uint16_t obsolete1;
    uint16_t specific_config;
    uint16_t obsolete2;
    uint16_t retired1[2];
    uint16_t obsolete3;
    uint16_t reserved_cfa1[2];

    /* 10–19 */
    uint16_t serial_number[10];

    /* 20–21 */
    uint16_t retired2[2];

    /* 22 */
    uint16_t obsolete4;

    /* 23–26 */
    uint16_t firmware_revision[4];

    /* 27–46 */
    uint16_t model_number[20];

    /* 47 */
    uint16_t max_sectors_per_interrupt;

    /* 48 */
    uint16_t reserved1;

    /* 49 */
    uint16_t capabilities1;

    /* 50 */
    uint16_t capabilities2;

    /* 51–52 */
    uint16_t obsolete5[2];

    /* 53 */
    uint16_t field_validity;

    /* 54–58 */
    uint16_t obsolete6[5];

    /* 59 */
    uint16_t multi_sector_settings;

    /* 60–61 */
    uint16_t lba28_sector_count[2];

    /* 62 */
    uint16_t obsolete7;

    /* 63 */
    uint16_t dma_modes;

    /* 64 */
    uint16_t pio_modes;

    /* 65–68 */
    uint16_t min_multiword_dma_cycle;
    uint16_t rec_multiword_dma_cycle;
    uint16_t min_pio_cycle_no_flow;
    uint16_t min_pio_cycle_with_flow;

    /* 69–70 */
    uint16_t reserved2[2];

    /* 71–74 */
    uint16_t reserved3[4];

    /* 75 */
    uint16_t queue_depth;

    /* 76–79 */
    uint16_t sata_capabilities;
    uint16_t sata_additional_capabilities;
    uint16_t sata_features_supported;
    uint16_t sata_features_enabled;

    /* 80 */
    uint16_t major_version;

    /* 81 */
    uint16_t minor_version;

    /* 82–84 */
    uint16_t command_set_support[3];

    /* 85–87 */
    uint16_t command_set_enabled[3];

    /* 88 */
    uint16_t ultra_dma_modes;

    /* 89–90 */
    uint16_t security_erase_time;
    uint16_t enhanced_security_erase_time;

    /* 91 */
    uint16_t current_apm_value;

    /* 92 */
    uint16_t master_password_revision;

    /* 93 */
    uint16_t hardware_reset_result;

    /* 94 */
    uint16_t acoustic_management;

    /* 95–99 */
    uint16_t reserved4[5];

    /* 100–103 */
    uint16_t lba48_sector_count[4];

    /* 104–127 */
    uint16_t reserved5[24];

    /* 128 */
    uint16_t security_status;

    /* 129–159 */
    uint16_t vendor_specific1[31];

    /* 160–167 */
    uint16_t cfa_power_mode;
    uint16_t reserved6[7];

    /* 168 */
    uint16_t device_nominal_form_factor;

    /* 169–175 */
    uint16_t reserved7[7];

    /* 176–205 */
    uint16_t current_media_serial[30];

    /* 206 */
    uint16_t sct_command_transport;

    /* 207–208 */
    uint16_t reserved8[2];

    /* 209 */
    uint16_t alignment_of_logical_blocks;

    /* 210–211 */
    uint16_t write_read_verify_sector_count[2];

    /* 212–213 */
    uint16_t write_read_verify_sector_count_ext[2];

    /* 214 */
    uint16_t nv_cache_capabilities;

    /* 215–216 */
    uint16_t nv_cache_size[2];

    /* 217 */
    uint16_t nominal_media_rotation_rate;

    /* 218 */
    uint16_t reserved9;

    /* 219 */
    uint16_t nv_cache_options;

    /* 220 */
    uint16_t write_read_verify_modes;

    /* 221 */
    uint16_t reserved10;

    /* 222 */
    uint16_t transport_major_version;

    /* 223 */
    uint16_t transport_minor_version;

    /* 224–229 */
    uint16_t reserved11[6];

    /* 230–233 */
    uint16_t extended_num_user_addressable_sectors[4];

    /* 234 */
    uint16_t min_blocks_per_download_microcode;

    /* 235 */
    uint16_t max_blocks_per_download_microcode;

    /* 236–254 */
    uint16_t reserved12[19];

    /* 255 */
    uint16_t integrity_word;

} ata_identify_t;

#pragma pack(pop)


typedef union ata_identify_result {

    uint16_t raw[256];
    ata_identify_t fields;

} ata_identify_u;

typedef enum ata_command
{
    NOP = 0x00,
    CFA_REQUEST_EXTENDED_ERROR_CODE = 0x03,
    DATA_SET_MANAGEMENT = 0x06,
    DATA_SET_MANAGEMENT_XL = 0x07,
    DEVICE_RESET = 0x08,
    REQUEST_SENSE_DATA_EXT = 0x0B,
    RECALIBRATE = 0x10,
    RECALIBRATE1 = 0x11,
    GET_PHYSICAL_ELEMENT_STATUS = 0x12,
    RECALIBRATE2 = 0x12,
    RECALIBRATE3 = 0x13,
    RECALIBRATE4 = 0x14,
    RECALIBRATE5 = 0x15,
    RECALIBRATE6 = 0x16,
    RECALIBRATE7 = 0x17,
    RECALIBRATE8 = 0x18,
    RECALIBRATE9 = 0x19,
    RECALIBRATE10 = 0x1A,
    RECALIBRATE11 = 0x1B,
    RECALIBRATE12 = 0x1C,
    RECALIBRATE13 = 0x1D,
    RECALIBRATE14 = 0x1E,
    RECALIBRATE15 = 0x1F,
    READ_SECTOR_S = 0x20,
    READ_SECTOR_S_NO_RETRY = 0x21,
    READ_LONG = 0x22,
    READ_LONG_NO_RETRY = 0x23,
    READ_SECTOR_S_EXT = 0x24,


    

} ata_command;
typedef enum ata_mode
{
    chs = 0,
    lba
} ata_mode;
typedef enum ata_drive_type
{
    master = 0,
    slave
} ata_drive_type;
typedef struct DEVICE_EXTENDED_28
{
    uint8_t sector_count;
    uint8_t lba_low;
    uint8_t lba_mid;
    uint8_t lba_high;
    uint8_t dev_sel;
    uint8_t command;
} DEVICE_EXTENDED_28;



#define ATA_PRIMARY_BASE     0x1F0
#define ATA_PRIMARY_DEVCTL   0x3F6

/* differentiate ATA, ATAPI, SATA and SATAPI */
#define ATADEV_PATAPI 5
#define ATADEV_SATAPI 4
#define ATADEV_PATA 3
#define ATADEV_SATA 2
#define ATADEV_UNKNOWN 1


/* ATA command block register offsets */
#define REG_DATA        0   /* 0x1F0 */
#define REG_ERROR       1   /* 0x1F1 (read) */
#define REG_FEATURES    1   /* 0x1F1 (write) */
#define REG_SECCOUNT    2   /* 0x1F2 */
#define REG_LBA_LO      3   /* 0x1F3 */
#define REG_CYL_LO      4   /* 0x1F4 */
#define REG_LBA_MID      4   /* 0x1F4 */
#define REG_CYL_HI      5   /* 0x1F5 */
#define REG_LBA_HI      5   /* 0x1F5 */
#define REG_DEVSEL      6   /* 0x1F6 */
#define REG_STATUS      7   /* 0x1F7 (read) */
#define REG_COMMAND     7   /* 0x1F7 (write) */

#define NO_ATA_IDENTIFY_DEBUG_PRINT_INFO
#define NO_ATA_IDENTIFY_DEBUG_PRINT_ERR

#define ATA_TIMEOUT_BSY   100000
#define ATA_TIMEOUT_DRQ   100000

typedef enum {
    ATA_ERR_NONE = 0,
    ATA_ERR_NO_DEVICE,
    ATA_ERR_NOT_ATA,
    ATA_ERR_DEVICE_FAULT,
    ATA_ERR_TIMEOUT,
    ATA_ERR_NO_MEMORY,
    ATA_ERR_INVALID_ARG

} ata_error_t;


#include "../STD/int.h"

uint32_t ata_u32(uint16_t lo, uint16_t hi);
int ata_wait_not_bsy(uint16_t io_base);
int ata_wait_drq(uint16_t io_base);
const char *devtype_to_string(int type);

const char *devtype_to_string(int type);
typedef struct DEVICE {
    uint16_t base;     /* Command block base (e.g. 0x1F0 or 0x170) */
    uint16_t dev_ctl;  /* Control block base (e.g. 0x3F6 or 0x376) */
} DEVICE;

extern DEVICE ata_primary;

typedef enum ata_error_type
{
    ok = 0,

    /* ATA status / error register bits */
    AMNF = 1,
    TKZNF = 2,
    ABRT = 4,
    MCR = 8,
    IDNF = 16,
    MC = 32,
    UNC = 64,
    BBK = 128,

    /* generic */
    ERROR = 512,
    INVALID_ARGUMENT = 1024,

    /* polling / timing */
    DRQ_TIMEOUT,
    BSY_TIMEOUT,
    DEVICE_FAULT,
    BUS_FLOAT,
    DEVICE_DOES_NOT_EXIST,
    ERROR_WHILE_DRQ_POLL,

} ata_error_type;


ata_error_type get_ata_last_error();
void set_ata_last_error(ata_error_type e);
int ata_ok(void);
int ata_fail(ata_error_type e);


int detect_devtype (int slavebit, DEVICE *ctrl);
int ata_soft_reset(uint16_t dcr_port);
int ata_init();
int ata_identify(DEVICE *drv, ata_identify_u *identify_result);
int pio28_read(void *dest, DEVICE *drv, uint32_t sectors, uint32_t abs_lba);
void ata_wait_400ns(uint16_t io_base);
int ata_get_last_error(void);
#endif
