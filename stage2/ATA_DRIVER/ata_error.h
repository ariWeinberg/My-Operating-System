typedef enum ata_error_type
{
    ok = 0,
    AMNF = 1,
    TKZNF = 2,
    ABRT = 4,
    MCR = 8,
    IDNF = 16,
    MC = 32,
    UNC = 64,
    BBK = 128,
    ERROR = 512,
    INVALID_ARGUMENT = 1024,
    ERROR_WHILE_DRQ_POLL,
    DRQ_TIMEOUT,
    BUS_FLOAT,
    DEVICE_DOES_NOT_EXIST,
    
    

} ata_error_type;

ata_error_type get_ata_last_error();
void set_ata_last_error(ata_error_type e);
