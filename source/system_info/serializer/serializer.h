//
// PROJECT:         Aspia
// FILE:            system_info/serializer/serializer.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_SYSTEM_INFO__SERIALIZER__SERIALIZER_H
#define _ASPIA_SYSTEM_INFO__SERIALIZER__SERIALIZER_H

#include <QObject>

#include <google/protobuf/message_lite.h>

namespace aspia {

class Serializer : public QObject
{
    Q_OBJECT

public:
    std::string categoryUuid() const { return category_uuid_; }

    void setRequestUuid(const std::string& uuid) { request_uuid_ = uuid; }
    std::string requestUuid() const { return request_uuid_; }

    void setParams(const QByteArray& params) { params_ = params; }
    QByteArray params() const { return params_; }

public slots:
    virtual void start() = 0;

signals:
    void replyReady(const std::string& request_uuid, const QByteArray& data);
    void finished();

protected:
    Serializer(QObject* parent, const std::string& uuid);
    void onReady(const google::protobuf::MessageLite& message);

private:
    const std::string category_uuid_;
    std::string request_uuid_;
    QByteArray params_;
};

} // namespace aspia

#endif // _ASPIA_SYSTEM_INFO__SERIALIZER__SERIALIZER_H
