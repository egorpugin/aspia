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

#include "base/common.h"

#include <QObject>

#include "protocol/desktop_session.pb.h"

namespace aspia {

class ASPIA_CLIENT_API Clipboard : public QObject
{
    Q_OBJECT

public:
    Clipboard(QObject* parent = nullptr);
    ~Clipboard();

public slots:
    // Receiving the incoming clipboard.
    void injectClipboardEvent(const proto::desktop::ClipboardEvent& event);

signals:
    void clipboardEvent(const proto::desktop::ClipboardEvent& event);

private slots:
    void dataChanged();

private:
    std::string last_mime_type_;
    std::string last_data_;

    DISABLE_COPY(Clipboard)
};

} // namespace aspia
