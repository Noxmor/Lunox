#define CBREW_ENABLE_ASSERTS
#define CBREW_ENABLE_CONSOLE_COLORS
#define CBREW_IMPLEMENTATION
#include "cbrew.h"

void create_lunox_core(void)
{
    CbrewProject* project = CBREW_PRJ_NEW("Lunox-Core", CBREW_PROJECT_TYPE_STATIC_LIB);
    CBREW_PRJ_FILES(project, "lunox/**.c");
    CBREW_PRJ_INCLUDE_DIR(project, "lunox/src");
    CBREW_PRJ_FLAG(project, "-Wall");
    CBREW_PRJ_FLAG(project, "-Wextra");
    CBREW_PRJ_FLAG(project, "-Wunreachable-code");


    CbrewConfig* debug = CBREW_CFG_NEW(project, "Debug", "bin/Lunox-Core-Debug", "bin-int/Lunox-Core-Debug");
    CBREW_CFG_DEFINE(debug, "LNX_DEBUG");
    CBREW_CFG_FLAG(debug, "-g");
    CBREW_CFG_FLAG(debug, "-O0");

    CbrewConfig* release = CBREW_CFG_NEW(project, "Release", "bin/Lunox-Core-Release", "bin-int/Lunox-Core-Release");
    CBREW_CFG_DEFINE(release, "LNX_RELEASE");
    CBREW_CFG_FLAG(release, "-g");
    CBREW_CFG_FLAG(release, "-O2");

    CbrewConfig* dist = CBREW_CFG_NEW(project, "Dist", "bin/Lunox-Core-Dist", "bin-int/Lunox-Core-Dist");
    CBREW_CFG_DEFINE(dist, "LNX_DIST");
    CBREW_CFG_FLAG(dist, "-O3");

}

void create_lunox_engine(void)
{
    CbrewProject* project = CBREW_PRJ_NEW("Lunox", CBREW_PROJECT_TYPE_APP);
    CBREW_PRJ_FILES(project, "engine/**.c");
    CBREW_PRJ_INCLUDE_DIR(project, "lunox/src");
    CBREW_PRJ_INCLUDE_DIR(project, "engine/src");
    CBREW_PRJ_LINK(project, "bin/Lunox-Core-Debug/Lunox-Core");
    CBREW_PRJ_FLAG(project, "-Wall");
    CBREW_PRJ_FLAG(project, "-Wextra");
    CBREW_PRJ_FLAG(project, "-Wunreachable-code");


    CbrewConfig* debug = CBREW_CFG_NEW(project, "Debug", "bin/Lunox-Debug", "bin-int/Lunox-Debug");
    CBREW_CFG_DEFINE(debug, "LNX_DEBUG");
    CBREW_CFG_FLAG(debug, "-g");
    CBREW_CFG_FLAG(debug, "-O0");

    CbrewConfig* release = CBREW_CFG_NEW(project, "Release", "bin/Lunox-Release", "bin-int/Lunox-Release");
    CBREW_CFG_DEFINE(release, "LNX_RELEASE");
    CBREW_CFG_FLAG(release, "-g");
    CBREW_CFG_FLAG(release, "-O2");

    CbrewConfig* dist = CBREW_CFG_NEW(project, "Dist", "bin/Lunox-Dist", "bin-int/Lunox-Dist");
    CBREW_CFG_DEFINE(dist, "LNX_DIST");
    CBREW_CFG_FLAG(dist, "-O3");
}

int main(int argc, char** argv)
{
    CBREW_AUTO_REBUILD(argc, argv);

    create_lunox_core();
    create_lunox_engine();

    cbrew_build();

    return EXIT_SUCCESS;
}