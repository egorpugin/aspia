//
// PROJECT:         Aspia
// FILE:            client/ui/status_dialog.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

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

    Q_DISABLE_COPY(StatusDialog)
};

} // namespace aspia
