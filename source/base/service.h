//
// PROJECT:         Aspia
// FILE:            base/service.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/service_impl.h"

namespace aspia {

template <class Application>
class Service : public ServiceImpl
{
public:
    explicit Service(const std::string& name, const std::string& display_name, const std::string& description)
        : ServiceImpl(name, display_name, description)
    {
        // Nothing
    }

    virtual ~Service() = default;

protected:
    Application* application() const { return application_.get(); }

    // ServiceImpl implementation.
    void createApplication(int argc, char* argv[]) override
    {
        application_ = std::make_unique<Application>(argc, argv);
    }

    // ServiceImpl implementation.
    int executeApplication() override
    {
        return application_->exec();
    }

private:
    std::unique_ptr<Application> application_;
};

} // namespace aspia
