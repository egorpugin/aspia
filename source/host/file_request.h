//
// PROJECT:         Aspia
// FILE:            host/file_request.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__FILE_REQUEST_H
#define _ASPIA_HOST__FILE_REQUEST_H

#include "base/common.h"

#include <QObject>
#include <QPointer>

#include "protocol/file_transfer_session.pb.h"

namespace aspia {

class FileRequest : public QObject
{
    Q_OBJECT

public:
    const proto::file_transfer::Request& request() const { return request_; }
    void sendReply(const proto::file_transfer::Reply& reply);

    static FileRequest* driveListRequest();
    static FileRequest* fileListRequest(const std::string& path);
    static FileRequest* createDirectoryRequest(const std::string& path);
    static FileRequest* renameRequest(const std::string& old_name, const std::string& new_name);
    static FileRequest* removeRequest(const std::string& path);
    static FileRequest* downloadRequest(const std::string& file_path);
    static FileRequest* uploadRequest(const std::string& file_path, bool overwrite);
    static FileRequest* packetRequest();
    static FileRequest* packet(const proto::file_transfer::Packet& packet);

signals:
    void replyReady(const proto::file_transfer::Request& request,
                    const proto::file_transfer::Reply& reply);

private:
    FileRequest(proto::file_transfer::Request&& request);

    proto::file_transfer::Request request_;

    DISABLE_COPY(FileRequest)
};

} // namespace aspia

#endif // _ASPIA_HOST__FILE_REQUEST_H
