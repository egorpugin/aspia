//
// PROJECT:         Aspia
// FILE:            client/client_session_file_transfer.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CLIENT__CLIENT_SESSION_FILE_TRANSFER_H
#define _ASPIA_CLIENT__CLIENT_SESSION_FILE_TRANSFER_H

#include <QQueue>
#include <QPointer>

#include "client/client_session.h"
#include "client/connect_data.h"
#include "client/file_request.h"
#include "protocol/file_transfer_session.pb.h"

namespace aspia {

Q_DECLARE_METATYPE(proto::file_transfer::Request);
Q_DECLARE_METATYPE(proto::file_transfer::Reply);

class FileManagerWindow;
class FileWorker;

class ClientSessionFileTransfer : public ClientSession
{
    Q_OBJECT

public:
    ClientSessionFileTransfer(ConnectData* connect_data, QObject* parent);
    ~ClientSessionFileTransfer();

public slots:
    // ClientSession implementation.
    void messageReceived(const QByteArray& buffer) override;
    void messageWritten(int message_id) override;
    void startSession() override;
    void closeSession() override;

private slots:
    void remoteRequest(FileRequest* request);

private:
    ConnectData* connect_data_;
    QPointer<FileManagerWindow> file_manager_;

    QPointer<FileWorker> worker_;
    QPointer<QThread> worker_thread_;

    QQueue<QPointer<FileRequest>> requests_;

    Q_DISABLE_COPY(ClientSessionFileTransfer)
};

} // namespace aspia

#endif // _ASPIA_CLIENT__CLIENT_SESSION_FILE_TRANSFER_H
