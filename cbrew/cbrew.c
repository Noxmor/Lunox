#define CBREW_ENABLE_ASSERTS
#define CBREW_ENABLE_CONSOLE_COLORS
#define CBREW_IMPLEMENTATION
#include "cbrew.h"

void create_lunox(void)
{
    CbrewProject* lunox = CBREW_PRJ_NEW("Lunox", CBREW_PROJECT_TYPE_APP);
    CBREW_PRJ_FILES(lunox, "Lunox/**.c");
    CBREW_PRJ_FLAG(lunox, "-Wall");
    CBREW_PRJ_FLAG(lunox, "-Wextra");
    CBREW_PRJ_FLAG(lunox, "-Wunreachable-code");


    CbrewConfig* lunox_debug = CBREW_CFG_NEW(lunox, "Debug", "bin/Lunox-Debug", "bin-int/Lunox-Debug");
    CBREW_CFG_DEFINE(lunox_debug, "LNX_DEBUG");
    CBREW_CFG_FLAG(lunox_debug, "-g");
    CBREW_CFG_FLAG(lunox_debug, "-O0");

    CbrewConfig* lunox_release = CBREW_CFG_NEW(lunox, "Release", "bin/Lunox-Release", "bin-int/Lunox-Release");
    CBREW_CFG_DEFINE(lunox_release, "LNX_RELEASE");
    CBREW_CFG_FLAG(lunox_release, "-g");
    CBREW_CFG_FLAG(lunox_release, "-O2");

    CbrewConfig* lunox_dist = CBREW_CFG_NEW(lunox, "Dist", "bin/Lunox-Dist", "bin-int/Lunox-Dist");
    CBREW_CFG_DEFINE(lunox_dist, "LNX_DIST");
    CBREW_CFG_FLAG(lunox_dist, "-O3");

}

int main(int argc, char** argv)
{
    CBREW_AUTO_REBUILD(argc, argv);

    create_lunox();

    cbrew_build();

    return EXIT_SUCCESS;
}