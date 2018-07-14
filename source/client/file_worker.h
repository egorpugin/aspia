//
// PROJECT:         Aspia
// FILE:            host/file_worker.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "client/file_depacketizer.h"
#include "client/file_packetizer.h"
#include "client/file_request.h"
#include "protocol/file_transfer_session.pb.h"

namespace aspia {

class ASPIA_CLIENT_API FileWorker : public QObject
{
    Q_OBJECT

public:
    FileWorker(QObject* parent = nullptr);
    ~FileWorker() = default;

    proto::file_transfer::Reply doRequest(const proto::file_transfer::Request& request);

public slots:
    void executeRequest(FileRequest* request);

private:
    proto::file_transfer::Reply doDriveListRequest();
    proto::file_transfer::Reply doFileListRequest(
        const proto::file_transfer::FileListRequest& request);
    proto::file_transfer::Reply doCreateDirectoryRequest(
        const proto::file_transfer::CreateDirectoryRequest& request);
    proto::file_transfer::Reply doRenameRequest(
        const proto::file_transfer::RenameRequest& request);
    proto::file_transfer::Reply doRemoveRequest(
        const proto::file_transfer::RemoveRequest& request);
    proto::file_transfer::Reply doDownloadRequest(
        const proto::file_transfer::DownloadRequest& request);
    proto::file_transfer::Reply doUploadRequest(
        const proto::file_transfer::UploadRequest& request);
    proto::file_transfer::Reply doPacketRequest();
    proto::file_transfer::Reply doPacket(const proto::file_transfer::Packet& packet);

    std::unique_ptr<FileDepacketizer> depacketizer_;
    std::unique_ptr<FilePacketizer> packetizer_;

    DISABLE_COPY(FileWorker)
};

} // namespace aspia
