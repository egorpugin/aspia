//
// PROJECT:         Aspia
// FILE:            console/console_window.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CONSOLE__CONSOLE_WINDOW_H
#define _ASPIA_CONSOLE__CONSOLE_WINDOW_H

#include "host/locale_loader.h"
#include "protocol/address_book.pb.h"
#include "ui_console_window.h"

namespace aspia {

class AddressBookTab;
class ComputerItem;
class Client;

class ConsoleWindow : public QMainWindow
{
    Q_OBJECT

public:
    ConsoleWindow(const QString& file_path, QWidget* parent = nullptr);
    ~ConsoleWindow() = default;

public slots:
    void onNewAddressBook();
    void onOpenAddressBook();
    void onSaveAddressBook();
    void onSaveAsAddressBook();
    void onCloseAddressBook();
    void onAddressBookProperties();
    void onAddComputer();
    void onModifyComputer();
    void onDeleteComputer();
    void onAddComputerGroup();
    void onModifyComputerGroup();
    void onDeleteComputerGroup();
    void onOnlineHelp();
    void onAbout();
    void onFastConnect();
    void onDesktopManageConnect();
    void onDesktopViewConnect();
    void onFileTransferConnect();
    void onSystemInfoConnect();

    void onCurrentTabChanged(int index);
    void onCloseTab(int index);

    void onAddressBookChanged(bool changed);
    void onComputerGroupActivated(bool activated, bool is_root);
    void onComputerActivated(bool activated);
    void onComputerGroupContextMenu(const QPoint& point, bool is_root);
    void onComputerContextMenu(ComputerItem* computer_item, const QPoint& point);
    void onComputerDoubleClicked(proto::address_book::Computer* computer);
    void onLanguageChanged(QAction* action);

protected:
    // QMainWindow implementation.
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onClientTerminated(Client* client);

private:
    void createLanguageMenu(const QString& current_locale);
    void addAddressBookTab(AddressBookTab* tab);
    AddressBookTab* currentAddressBookTab();
    void connectToComputer(const proto::address_book::Computer& computer);

    Ui::ConsoleWindow ui;
    LocaleLoader locale_loader_;
    QList<Client*> client_list_;

    Q_DISABLE_COPY(ConsoleWindow)
};

} // namespace aspia

#endif // _ASPIA_CONSOLE__CONSOLE_WINDOW_H
