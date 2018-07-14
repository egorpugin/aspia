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

#include "host_session_fake.h"

#include "host_session_fake_desktop.h"
#include "host_session_fake_file_transfer.h"

namespace aspia {

HostSessionFake::HostSessionFake(QObject* parent)
    : QObject(parent)
{
    // Nothing
}

// static
HostSessionFake* HostSessionFake::create(proto::auth::SessionType session_type, QObject* parent)
{
    switch (session_type)
    {
        case proto::auth::SESSION_TYPE_DESKTOP_MANAGE:
        case proto::auth::SESSION_TYPE_DESKTOP_VIEW:
            return new HostSessionFakeDesktop(parent);

        case proto::auth::SESSION_TYPE_FILE_TRANSFER:
            return new HostSessionFakeFileTransfer(parent);

        default:
            return nullptr;
    }
}

} // namespace aspia
