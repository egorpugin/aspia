/*
 * Aspia: Remote desktop and file transfer tool.
 * Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
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

#pragma once

#include <primitives/log.h>

#include <string>

namespace aspia {

#if defined(_WIN32)
using SystemErrorCode = unsigned long;
#elif defined(Q_OS_UNIX)
using SystemErrorCode = int;
#endif

ASPIA_BASE_API
std::string errnoToString(SystemErrorCode error_code);

ASPIA_BASE_API
void errnoToLog(boost::log::trivial::severity_level lvl, const char* file, int line, const char* message, ...);

#define qDebugErrno(_msg_, ...) \
    errnoToLog(boost::log::trivial::severity_level::debug, __FILE__, __LINE__, _msg_, ##__VA_ARGS__)

#define qWarningErrno(_msg_, ...) \
    errnoToLog(boost::log::trivial::severity_level::warning, __FILE__, __LINE__, _msg_, ##__VA_ARGS__)

#define qCriticalErrno(_msg_, ...) \
    errnoToLog(boost::log::trivial::severity_level::error, __FILE__, __LINE__, _msg_, ##__VA_ARGS__)

#define qFatalErrno(_msg_, ...) \
    errnoToLog(boost::log::trivial::severity_level::fatal, __FILE__, __LINE__, _msg_, ##__VA_ARGS__)

#define qInfoErrno(_msg_, ...) \
    errnoToLog(boost::log::trivial::severity_level::info, __FILE__, __LINE__, _msg_, ##__VA_ARGS__)

} // namespace aspia
