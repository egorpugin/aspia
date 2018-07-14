//
// PROJECT:         Aspia
// FILE:            host/screen_updater.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

#include <QEvent>
#include <QThread>

#include <condition_variable>
#include <memory>
#include <mutex>

#include "protocol/desktop_session.pb.h"

namespace aspia {

class ScreenUpdater : public QThread
{
    Q_OBJECT

public:
    ScreenUpdater(const proto::desktop::Config& config, QObject* parent);
    ~ScreenUpdater();

    void update();

    class UpdateEvent : public QEvent
    {
    public:
        static const int kType = QEvent::User + 1;

        UpdateEvent()
            : QEvent(static_cast<QEvent::Type>(kType))
        {
            // Nothing
        }

        std::unique_ptr<aspia::proto::desktop::VideoPacket> video_packet;
        std::unique_ptr<aspia::proto::desktop::CursorShape> cursor_shape;

    private:
        DISABLE_COPY(UpdateEvent)
    };

    class ErrorEvent : public QEvent
    {
    public:
        static const int kType = QEvent::User + 2;

        ErrorEvent()
            : QEvent(static_cast<QEvent::Type>(kType))
        {
            // Nothing
        }

    private:
        DISABLE_COPY(ErrorEvent)
    };

protected:
    // QThread implementation.
    void run() override;

private:
    std::condition_variable update_condition_;
    std::mutex update_lock_;
    bool update_required_ = false;
    bool terminate_ = false;

    proto::desktop::Config config_;

    DISABLE_COPY(ScreenUpdater)
};

} // namespace aspia
