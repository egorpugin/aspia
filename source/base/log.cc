/*
 * Aspia: Remote desktop and file transfer tool.
 * Copyright (C) 2018 Egor Pugin <egor.pugin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "log.h"

#include "common.h"

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
