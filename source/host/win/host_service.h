//
// PROJECT:         Aspia
// FILE:            host/host_service.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__HOST_SERVICE_H
#define _ASPIA_HOST__HOST_SERVICE_H

#include <QGuiApplication>
#include <QScopedPointer>
#include <QPointer>

#include "base/win/scoped_com_initializer.h"
#include "host/locale_loader.h"
#include "host/service.h"

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
    QScopedPointer<LocaleLoader> locale_loader_;
    QPointer<HostServer> server_;

    DISABLE_COPY(HostService)
};

} // namespace aspia

#endif // _ASPIA_HOST__HOST_SERVICE_H
