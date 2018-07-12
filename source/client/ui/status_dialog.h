//
// PROJECT:         Aspia
// FILE:            client/ui/status_dialog.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CLIENT__UI__STATUS_DIALOG_H
#define _ASPIA_CLIENT__UI__STATUS_DIALOG_H

#include "base/common.h"

#include "ui_status_dialog.h"

namespace aspia {

class StatusDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatusDialog(QWidget* parent = nullptr);
    ~StatusDialog() = default;

public slots:
    // Adds a message to the status dialog. If the dialog is hidden, it also shows it.
    void addStatus(const QString& status);

private:
    Ui::StatusDialog ui;

    DISABLE_COPY(StatusDialog)
};

} // namespace aspia

#endif // _ASPIA_CLIENT__UI__STATUS_DIALOG_H
