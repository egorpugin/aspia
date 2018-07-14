//
// PROJECT:         Aspia
// FILE:            console/about_dialog.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "ui_about_dialog.h"

namespace aspia {

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog() = default;

private:
    Ui::AboutDialog ui;

    Q_DISABLE_COPY(AboutDialog)
};

} // namespace aspia
