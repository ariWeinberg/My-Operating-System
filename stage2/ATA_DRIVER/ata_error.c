#include "ATA.h"

static ata_error_type ata_last_error = ok;

ata_error_type get_ata_last_error()
{
    return ata_last_error;
}

void set_ata_last_error(ata_error_type e)
{
    ata_last_error = e;
}

inline int ata_ok(void)
{
    set_ata_last_error(ok);
    return 0;
}

inline int ata_fail(ata_error_type e)
{
    set_ata_last_error(e);
    return -1;
}
