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

#include <QGuiApplication>
#include <QScopedPointer>
#include <QPointer>

#include "base/win/scoped_com_initializer.h"
#include "core/locale_loader.h"
#include "service.h"

namespace aspia {

class HostServer;

class HostService : public Service<QGuiApplication>
{
public:
    HostService();

protected:
    // Service implementation.
    void start() override;
    void stop() override;
    void sessionChange(uint32_t event, uint32_t session_id) override;

private:
    QScopedPointer<ScopedCOMInitializer> com_initializer_;
    QPointer<HostServer> server_;

    DISABLE_COPY(HostService)
};

} // namespace aspia
