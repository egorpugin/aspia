#include "log.h"

#include "../common.h"

namespace aspia {

void initLoggerForApplication(int argc, char **argv)
{
    LoggerSettings log_settings;
    log_settings.log_level = "debug";// "info";
    log_settings.log_file = fs::path(argv[0]).filename().stem().string();
    log_settings.simple_logger = true;
    //log_settings.print_trace = true;
    initLogger(log_settings);
}

}
