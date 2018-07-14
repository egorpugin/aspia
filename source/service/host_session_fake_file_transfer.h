//
// PROJECT:         Aspia
// FILE:            host/host_session_fake_file_transfer.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

#include "host_session_fake.h"

namespace aspia {

class HostSessionFakeFileTransfer : public HostSessionFake
{
    Q_OBJECT

public:
    explicit HostSessionFakeFileTransfer(QObject* parent);

    // HostSessionFake implementation.
    void startSession() override;

public slots:
    // HostSessionFake implementation.
    void onMessageReceived(const std::string& buffer) override;
    void onMessageWritten(int message_id) override;

private:
    DISABLE_COPY(HostSessionFakeFileTransfer)
};

} // namespace aspia
