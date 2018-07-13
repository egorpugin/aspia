//
// PROJECT:         Aspia
// FILE:            ASClient/ui/file_transfer_dialog.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CLIENT__UI__FILE_TRANSFER_DIALOG_H
#define _ASPIA_CLIENT__UI__FILE_TRANSFER_DIALOG_H

#include "client/file_transfer.h"
#include "ui_file_transfer_dialog.h"

namespace aspia {

class FileTransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileTransferDialog(QWidget* parent = nullptr);
    ~FileTransferDialog() = default;

public slots:
    void setCurrentItem(const QString& source_path, const QString& target_path);
    void setProgress(int total, int current);
    void showError(FileTransfer* transfer, FileTransfer::Error error_type, const QString& message);

private:
    Ui::FileTransferDialog ui;

    bool task_queue_building_ = true;

    Q_DISABLE_COPY(FileTransferDialog)
};

} // namespace aspia

#endif // _ASPIA_CLIENT__UI__FILE_TRANSFER_DIALOG_H
