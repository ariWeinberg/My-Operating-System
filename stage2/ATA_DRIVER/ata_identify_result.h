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
