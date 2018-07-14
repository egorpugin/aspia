//
// PROJECT:         Aspia
// FILE:            client/client_session_desktop_view.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <QPointer>
#include <QThread>

#include "client/client_session.h"
#include "client/connect_data.h"
#include "codec/video_decoder.h"
#include "protocol/address_book.pb.h"

namespace aspia {

class DesktopWindow;

class ClientSessionDesktopView : public ClientSession
{
    Q_OBJECT

public:
    ClientSessionDesktopView(ConnectData* connect_data, QObject* parent);
    virtual ~ClientSessionDesktopView();

    static quint32 supportedVideoEncodings();
    static quint32 supportedFeatures();

public slots:
    // ClientSession implementation.
    void messageReceived(const std::string& buffer) override;
    void messageWritten(int message_id) override;
    void startSession() override;
    void closeSession() override;

    virtual void onSendConfig(const proto::desktop::Config& config);

protected:
    void readVideoPacket(const proto::desktop::VideoPacket& packet);

    ConnectData* connect_data_;
    QPointer<DesktopWindow> desktop_window_;

private:
    void readConfigRequest(const proto::desktop::ConfigRequest& config_request);

    proto::desktop::VideoEncoding video_encoding_ = proto::desktop::VIDEO_ENCODING_UNKNOWN;
    std::unique_ptr<VideoDecoder> video_decoder_;

    Q_DISABLE_COPY(ClientSessionDesktopView)
};

} // namespace aspia
