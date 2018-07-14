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

#include "base/common.h"
#include "base/errno_logging.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // defined(_WIN32)

enum QtMsgType
{
    QtDebugMsg,
    QtWarningMsg,
    QtCriticalMsg,
    QtFatalMsg,
    QtInfoMsg,
    QtSystemMsg = QtCriticalMsg
};

namespace aspia {

std::string errnoToString(SystemErrorCode error_code)
{
#if defined(_WIN32)
    const DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

    const int kErrorMessageBufferSize = 256;
    wchar_t buffer[kErrorMessageBufferSize];

    DWORD length = FormatMessageW(flags,
                                  nullptr,
                                  error_code,
                                  0,
                                  buffer,
                                  kErrorMessageBufferSize,
                                  nullptr);
    if (length)
    {
        auto s = to_string(buffer);
        s += " (" + std::to_string(error_code) + ")";
        return s;
    }

    return "Error (" + std::to_string(GetLastError()) + ") while retrieving error (" + std::to_string(error_code) + ")";
#else
#error Platform support not implemented
#endif
}

void errnoToLog(boost::log::trivial::severity_level lvl, const char* file, int line, const char* message, ...)
{
    SystemErrorCode error_code = GetLastError();

    std::string buffer(1024, 0);

    va_list ap;
    va_start(ap, message);
    if (message)
        vsprintf(buffer.data(), message, ap);
    va_end(ap);

    buffer += ": " + errnoToString(error_code);

    /*QMessageLogContext context;
    context.file = file;
    context.line = line;

    qt_message_output(type, context, buffer);*/

#define ASPIA_LOG_TRIVIAL(lvl)                                         \
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get(), \
                                 (::boost::log::keywords::severity = lvl))

    ASPIA_LOG_TRIVIAL(lvl) << buffer;

    SetLastError(error_code);
}

} // namespace aspia
