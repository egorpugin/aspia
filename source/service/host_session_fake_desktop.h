//
// PROJECT:         Aspia
// FILE:            host/host_session_fake_desktop.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

#include "host_session_fake.h"
#include "protocol/desktop_session.pb.h"

namespace aspia {

class DesktopFrame;
class VideoEncoder;

class HostSessionFakeDesktop : public HostSessionFake
{
    Q_OBJECT

public:
    explicit HostSessionFakeDesktop(QObject* parent);

    // HostSessionFake implementation.
    void startSession() override;

public slots:
    // HostSessionFake implementation.
    void onMessageReceived(const std::string& buffer) override;
    void onMessageWritten(int message_id) override;

private:
    std::unique_ptr<VideoEncoder> createEncoder(const proto::desktop::Config& config);
    std::unique_ptr<DesktopFrame> createFrame();
    void sendPacket(std::unique_ptr<proto::desktop::VideoPacket> packet);

    DISABLE_COPY(HostSessionFakeDesktop)
};

} // namespace aspia
