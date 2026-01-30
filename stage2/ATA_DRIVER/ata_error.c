#include "ata_error.h"

ata_error_type ata_last_error = ok;

ata_error_type get_ata_last_error()
{
    return ata_last_error;
}

void set_ata_last_error(ata_error_type e)
{
    ata_last_error = e;
}
