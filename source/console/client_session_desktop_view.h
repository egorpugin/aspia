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

#include <QPointer>
#include <QThread>

#include "client_session.h"
#include "connect_data.h"
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

    static uint32_t supportedVideoEncodings();
    static uint32_t supportedFeatures();

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
