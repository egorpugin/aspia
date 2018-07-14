//
// PROJECT:         Aspia
// FILE:            console/console_tab.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <QWidget>

namespace aspia {

class ConsoleTab : public QWidget
{
    Q_OBJECT

public:
    enum Type { AddressBook };

    ConsoleTab(Type type, QWidget* parent);
    virtual ~ConsoleTab() = default;

    Type type() const { return type_; }

private:
    const Type type_;

    Q_DISABLE_COPY(ConsoleTab)
};

} // namespace aspia
