//
// PROJECT:         Aspia
// FILE:            host/host_session_file_transfer.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "host_session.h"

namespace aspia {

class FileWorker;

class HostSessionFileTransfer : public HostSession
{
    Q_OBJECT

public:
    explicit HostSessionFileTransfer(const std::string& channel_id);
    ~HostSessionFileTransfer() = default;

public slots:
    // HostSession implementation.
    void messageReceived(const std::string& buffer) override;
    void messageWritten(int message_id) override;

protected:
    void startSession() override;
    void stopSession() override;

private:
    QPointer<FileWorker> worker_;

    DISABLE_COPY(HostSessionFileTransfer)
};

} // namespace aspia
