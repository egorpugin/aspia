//
// PROJECT:         Aspia
// FILE:            base/file_logger.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_BASE__FILE_LOGGER_H
#define _ASPIA_BASE__FILE_LOGGER_H

#include "base/common.h"

#include <QFile>
#include <QScopedPointer>

namespace aspia {

class ASPIA_BASE_API FileLogger
{
public:
    FileLogger();
    ~FileLogger();

    bool startLogging(const std::string& prefix);

private:
    static void messageHandler(QtMsgType type,
                               const QMessageLogContext& context,
                               const QString& msg);

    static QScopedPointer<QFile> file_;

    DISABLE_COPY(FileLogger);
};

} // namespace aspia

#endif // _ASPIA_BASE__FILE_LOGGER_H
