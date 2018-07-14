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

#include <QObject>

#include "protocol/authorization.pb.h"

namespace aspia {

class HostSessionFake : public QObject
{
    Q_OBJECT

public:
    virtual ~HostSessionFake() = default;

    static HostSessionFake* create(proto::auth::SessionType session_type, QObject* parent);

    virtual void startSession() = 0;

signals:
    void writeMessage(int message_id, const std::string& buffer);
    void readMessage();
    void errorOccurred();

public slots:
    virtual void onMessageReceived(const std::string& buffer) = 0;
    virtual void onMessageWritten(int message_id) = 0;

protected:
    explicit HostSessionFake(QObject* parent);
};

} // namespace aspia
