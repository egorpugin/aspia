//
// PROJECT:         Aspia
// FILE:            ASBase/service_controller.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

#include "base/win/scoped_object.h"

namespace aspia {

class ASPIA_BASE_API ServiceController
{
public:
    ServiceController();

    ServiceController(ServiceController&& other) noexcept;
    ServiceController& operator=(ServiceController&& other) noexcept;

    virtual ~ServiceController();

    static ServiceController open(const std::string& name);
    static ServiceController install(const std::string& name,
                                     const std::string& display_name,
                                     const std::string& file_path);
    static bool isInstalled(const std::string& name);

    bool setDescription(const std::string& description);
    std::string description() const;

    std::string filePath() const;

    bool isValid() const;
    bool isRunning() const;

    bool start();
    bool stop();
    bool remove();

protected:
    ServiceController(SC_HANDLE sc_manager, SC_HANDLE service);

private:
    ScopedScHandle sc_manager_;
    mutable ScopedScHandle service_;

    DISABLE_COPY(ServiceController)
};

} // namespace aspia
