//
// PROJECT:         Aspia
// FILE:            host/win/host_process.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__WIN__HOST_PROCESS_H
#define _ASPIA_HOST__WIN__HOST_PROCESS_H

#include "base/common.h"

#include <QObject>
#include <QScopedPointer>

namespace aspia {

class HostProcessImpl;

class HostProcess : public QObject
{
    Q_OBJECT

public:
    enum ProcessState
    {
        NotRunning,
        Starting,
        Running
    };
    Q_ENUM(ProcessState)

    enum Account
    {
        System,
        User
    };
    Q_ENUM(Account)

    enum ErrorCode
    {
        NoError,
        NoLoggedOnUser,
        OtherError
    };
    Q_ENUM(ErrorCode)

    HostProcess(QObject* parent = nullptr);
    virtual ~HostProcess();

    void start(uint32_t session_id,
               Account account,
               const std::string& program,
               const QStringList& arguments);

    uint32_t sessionId() const;
    void setSessionId(uint32_t session_id);

    Account account() const;
    void setAccount(Account account);

    std::string program() const;
    void setProgram(const std::string& program);

    QStringList arguments() const;
    void setArguments(const QStringList& arguments);

    ProcessState state() const;

public slots:
    void start();
    void kill();

signals:
    void started();
    void finished();
    void errorOccurred(HostProcess::ErrorCode error_code);

private:
    friend class HostProcessImpl;

    QScopedPointer<HostProcessImpl> impl_;

    DISABLE_COPY(HostProcess)
};

} // namespace aspia

#endif // _ASPIA_HOST__WIN__HOST_PROCESS_H
