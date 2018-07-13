//
// PROJECT:         Aspia
// FILE:            ASBase/service_impl.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

namespace aspia {

class ServiceEventHandler;

class ServiceImpl
{
public:
    ServiceImpl(const std::string& name, const std::string& display_name, const std::string& description);
    virtual ~ServiceImpl() = default;

    static ServiceImpl* instance() { return instance_; }

    std::string serviceName() const { return name_; }
    std::string serviceDisplayName() const { return display_name_; }
    std::string serviceDescription() const { return description_; }

    int exec(int argc, char* argv[]);

protected:
    friend class ServiceEventHandler;

    virtual void start() = 0;
    virtual void stop() = 0;

#if defined(_WIN32)
    virtual void sessionChange(uint32_t event, uint32_t session_id) = 0;
#endif // defined(_WIN32)

    virtual void createApplication(int argc, char* argv[]) = 0;
    virtual int executeApplication() = 0;

private:
    static ServiceImpl* instance_;

    std::string name_;
    std::string display_name_;
    std::string description_;

    DISABLE_COPY(ServiceImpl)
};

} // namespace aspia
