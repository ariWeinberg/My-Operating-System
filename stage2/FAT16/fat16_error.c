#include "fat16.h"

static fat16_error_type fat16_last_error = FAT16_OK;

fat16_error_type get_fat16_last_error()
{
    return fat16_last_error;
}

void set_fat16_last_error(fat16_error_type e)
{
    fat16_last_error = e;
}

int fat16_ok(void)
{
    set_fat16_last_error(FAT16_OK);
    return 0;
}

int fat16_fail(fat16_error_type e)
{
    set_fat16_last_error(e);
    return -1;
}
