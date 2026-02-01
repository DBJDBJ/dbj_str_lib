#include "dbj_common.h"

/* program starts here */
int main(const int argc, char *argv[static argc + 1])
{
    UNUSED(argv);
    str_256 prompt;

    // argv[0] can be very long string on some systems
    // so we use DBJ_STR_FMT to format it safely
    //it will exit if the buffer is not large enough
    DBJ_STR_FMT(&prompt, "Program \"%s\" : %s", argv[0], "Using the str_256 type!");
    DBJ_LOG("%s", prompt.data);
    return 0;
}
