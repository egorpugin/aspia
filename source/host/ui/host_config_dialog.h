//
// PROJECT:         Aspia
// FILE:            host/ui/host_config_dialog.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_HOST__UI__HOST_CONFIG_DIALOG_H
#define _ASPIA_HOST__UI__HOST_CONFIG_DIALOG_H

#include "base/locale_loader.h"
#include "host/user.h"
#include "ui_host_config_dialog.h"

namespace aspia {

class HostConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HostConfigDialog(QWidget* parent = nullptr);
    ~HostConfigDialog() = default;

private slots:
    void onUserContextMenu(const QPoint& point);
    void onCurrentUserChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void onAddUser();
    void onModifyUser();
    void onDeleteUser();
    void onButtonBoxClicked(QAbstractButton* button);

private:
    void createLanguageList(const std::string& current_locale);
    void retranslateUi(const std::string& locale);
    void setConfigChanged(bool changed);
    bool isConfigChanged() const;
    void reloadUserList();
    bool isServiceStarted();
    bool restartService();

    Ui::HostConfigDialog ui;

    LocaleLoader locale_loader_;
    QList<User> user_list_;

    DISABLE_COPY(HostConfigDialog)
};

} // namespace aspia

#endif // _ASPIA_HOST__UI__HOST_CONFIG_DIALOG_H
