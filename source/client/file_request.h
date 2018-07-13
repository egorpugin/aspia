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

#include "protocol/file_transfer_session.pb.h"

namespace aspia {

class FileRequest : public QObject
{
    Q_OBJECT

public:
    const proto::file_transfer::Request& request() const { return request_; }
    bool sendReply(const proto::file_transfer::Reply& reply);

    static FileRequest* driveListRequest(QObject* sender, const char* reply_slot);

    static FileRequest* fileListRequest(QObject* sender,
                                        const std::string& path,
                                        const char* reply_slot);

    static FileRequest* createDirectoryRequest(QObject* sender,
                                               const std::string& path,
                                               const char* reply_slot);

    static FileRequest* renameRequest(QObject* sender,
                                      const std::string& old_name,
                                      const std::string& new_name,
                                      const char* reply_slot);

    static FileRequest* removeRequest(QObject* sender,
                                      const std::string& path,
                                      const char* reply_slot);

    static FileRequest* downloadRequest(QObject* sender,
                                        const std::string& file_path,
                                        const char* reply_slot);

    static FileRequest* uploadRequest(QObject* sender,
                                      const std::string& file_path,
                                      bool overwrite,
                                      const char* reply_slot);

    static FileRequest* packetRequest(QObject* sender, const char* reply_slot);

    static FileRequest* packet(QObject* sender,
                               const proto::file_transfer::Packet& packet,
                               const char* reply_slot);

private slots:
    void senderDestroyed();

private:
    FileRequest(QObject* sender,
                proto::file_transfer::Request&& request,
                const char* reply_slot);

    QObject* sender_;
    proto::file_transfer::Request request_;
    const char* reply_slot_;

    DISABLE_COPY(FileRequest)
};

} // namespace aspia

#endif // _ASPIA_HOST__FILE_REQUEST_H
