//
// PROJECT:         Aspia
// FILE:            ASBase/errno_logging.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

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
