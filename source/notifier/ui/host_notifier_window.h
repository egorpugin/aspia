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

#include "core/locale_loader.h"
#include "notifier/host_notifier.h"
#include "protocol/notifier.pb.h"
#include "ui_host_notifier_window.h"

namespace aspia {

class HostNotifierWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HostNotifierWindow(QWidget* parent = nullptr);
    ~HostNotifierWindow() = default;

    void setChannelId(const std::string& channel_id);

public slots:
    void sessionOpen(const proto::notifier::Session& session);
    void sessionClose(const proto::notifier::SessionClose& session_close);

signals:
    void killSession(const std::string& uuid);

protected:
    // QWidget implementation.
    bool eventFilter(QObject* object, QEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:
    void quit();
    void onShowHidePressed();
    void onDisconnectAllPressed();
    void onContextMenu(const QPoint& point);
    void updateWindowPosition();

private:
    void showNotifier();
    void hideNotifier();

    Ui::HostNotifierWindow ui;

    LocaleLoader locale_loader_;
    QPoint start_pos_;
    QRect window_rect_;

    QPointer<HostNotifier> notifier_;
    std::string channel_id_;

    DISABLE_COPY(HostNotifierWindow)
};

} // namespace aspia
