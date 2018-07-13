//
// PROJECT:         Aspia
// FILE:            host/system_info_request.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__SYSTEM_INFO_REQUEST_H
#define _ASPIA_HOST__SYSTEM_INFO_REQUEST_H

#include "base/common.h"

#include <QObject>
#include "base/common.h"

#include "protocol/system_info_session.pb.h"

namespace aspia {

class ASPIA_CLIENT_API SystemInfoRequest : public QObject
{
    Q_OBJECT

public:
    const proto::system_info::Request& request() const { return request_; }
    const std::string& requestUuid() const { return request_.request_uuid(); }

    static SystemInfoRequest* categoryList();
    static SystemInfoRequest* category(const std::string& uuid, const QByteArray& params);

signals:
    void replyReady(const proto::system_info::Reply& reply);

private:
    SystemInfoRequest(proto::system_info::Request&& request);

    proto::system_info::Request request_;

    DISABLE_COPY(SystemInfoRequest)
};

} // namespace aspia

#endif // _ASPIA_HOST__SYSTEM_INFO_REQUEST_H
