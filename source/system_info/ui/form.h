//
// PROJECT:         Aspia
// FILE:            system_info/ui/form.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_SYSTEM_INFO__UI__FORM_H
#define _ASPIA_SYSTEM_INFO__UI__FORM_H

#include <QWidget>

namespace aspia {

class SystemInfoRequest;

class Form : public QWidget
{
    Q_OBJECT

public:
    virtual ~Form() = default;

    std::string uuid() const { return uuid_; }

public slots:
    virtual void readReply(const std::string& uuid, const QByteArray& data) = 0;

signals:
    void sendRequest(SystemInfoRequest* request);

protected:
    Form(QWidget* parent, const std::string& uuid)
        : QWidget(parent),
          uuid_(uuid)
    {
        // Nothing
    }

private:
    const std::string uuid_;
};

} // namespace aspia

#endif // _ASPIA_SYSTEM_INFO__UI__FORM_H
