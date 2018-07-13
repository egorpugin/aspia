//
// PROJECT:         Aspia
// FILE:            host/ui/user_dialog.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host/ui/user_dialog.h"

#include <QMessageBox>
#include <QMouseEvent>

#include "protocol/authorization.pb.h"

namespace aspia {

UserDialog::UserDialog(QList<User>* user_list, User* user, QWidget* parent)
    : QDialog(parent),
      user_list_(user_list),
      user_(user)
{
    ui.setupUi(this);

    ui.edit_username->setText(user_->name().c_str());

    if (!user->passwordHash().empty())
    {
        auto text = tr("Double-click to change");

        ui.edit_password->setText(text);
        ui.edit_password_repeat->setText(text);

        ui.edit_password->setEnabled(false);
        ui.edit_password_repeat->setEnabled(false);

        ui.edit_password->setEchoMode(QLineEdit::Normal);
        ui.edit_password->setInputMethodHints(Qt::ImhNone);

        ui.edit_password_repeat->setEchoMode(QLineEdit::Normal);
        ui.edit_password_repeat->setInputMethodHints(Qt::ImhNone);

        ui.edit_password->installEventFilter(this);
        ui.edit_password_repeat->installEventFilter(this);

        password_changed_ = false;
    }

    ui.checkbox_disable_user->setChecked(!(user_->flags() & User::FLAG_ENABLED));

    auto add_session_type = [&](const QIcon& icon,
                                const auto& name,
                                proto::auth::SessionType session_type)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();

        item->setText(0, name);
        item->setIcon(0, icon);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setData(0, Qt::UserRole, QVariant(session_type));

        if (user->sessions() & session_type)
            item->setCheckState(0, Qt::Checked);
        else
            item->setCheckState(0, Qt::Unchecked);

        ui.tree_sessions->addTopLevelItem(item);
    };

    add_session_type(QIcon(QStringLiteral(":/icon/monitor-keyboard.png")),
                     tr("Desktop Manage"),
                     proto::auth::SESSION_TYPE_DESKTOP_MANAGE);

    add_session_type(QIcon(QStringLiteral(":/icon/monitor.png")),
                     tr("Desktop View"),
                     proto::auth::SESSION_TYPE_DESKTOP_VIEW);

    add_session_type(QIcon(QStringLiteral(":/icon/folder-stand.png")),
                     tr("File Transfer"),
                     proto::auth::SESSION_TYPE_FILE_TRANSFER);

    add_session_type(QIcon(QStringLiteral(":/icon/system-monitor.png")),
                     tr("System Information"),
                     proto::auth::SESSION_TYPE_SYSTEM_INFO);

    connect(ui.button_check_all, &QPushButton::pressed, this, &UserDialog::onCheckAllButtonPressed);
    connect(ui.button_check_none, &QPushButton::pressed, this, &UserDialog::onCheckNoneButtonPressed);
    connect(ui.button_box, &QDialogButtonBox::clicked, this, &UserDialog::onButtonBoxClicked);
}

bool UserDialog::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonDblClick &&
        (object == ui.edit_password || object == ui.edit_password_repeat))
    {
        password_changed_ = true;

        ui.edit_password->setEnabled(true);
        ui.edit_password_repeat->setEnabled(true);

        ui.edit_password->clear();
        ui.edit_password_repeat->clear();

        Qt::InputMethodHints hints = Qt::ImhHiddenText | Qt::ImhSensitiveData |
            Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText;

        ui.edit_password->setEchoMode(QLineEdit::Password);
        ui.edit_password->setInputMethodHints(hints);

        ui.edit_password_repeat->setEchoMode(QLineEdit::Password);
        ui.edit_password_repeat->setInputMethodHints(hints);

        ui.edit_password->setFocus();
    }

    return false;
}

void UserDialog::onCheckAllButtonPressed()
{
    for (int i = 0; i < ui.tree_sessions->topLevelItemCount(); ++i)
        ui.tree_sessions->topLevelItem(i)->setCheckState(0, Qt::Checked);
}

void UserDialog::onCheckNoneButtonPressed()
{
    for (int i = 0; i < ui.tree_sessions->topLevelItemCount(); ++i)
        ui.tree_sessions->topLevelItem(i)->setCheckState(0, Qt::Unchecked);
}

void UserDialog::onButtonBoxClicked(QAbstractButton* button)
{
    if (ui.button_box->standardButton(button) == QDialogButtonBox::Ok)
    {
        auto name = ui.edit_username->text();
        if (!User::isValidName(name.toStdString()))
        {
            QMessageBox::warning(this,
                                 tr("Warning"),
                                 tr("The user name can not be empty and can contain only alphabet"
                                    " characters, numbers and ""_"", ""-"", ""."" characters."),
                                 QMessageBox::Ok);
            return;
        }

        if (name.compare(user_->name().c_str(), Qt::CaseInsensitive) != 0)
        {
            for (const auto& user : *user_list_)
            {
                if (name.compare(user.name().c_str(), Qt::CaseInsensitive) == 0)
                {
                    QMessageBox::warning(this,
                                         tr("Warning"),
                                         tr("The username you entered already exists."),
                                         QMessageBox::Ok);
                    return;
                }
            }
        }

        if (password_changed_)
        {
            auto password = ui.edit_password->text();
            auto password_repeat = ui.edit_password_repeat->text();

            if (password != password_repeat)
            {
                QMessageBox::warning(this,
                                     tr("Warning"),
                                     tr("The passwords you entered do not match."),
                                     QMessageBox::Ok);
                return;
            }


            if (!User::isValidPassword(password.toStdString()))
            {
                QMessageBox::warning(this,
                                     tr("Warning"),
                                     tr("Password can not be shorter than %n characters.",
                                        "", User::kMinPasswordLength),
                                     QMessageBox::Ok);
                return;
            }

            user_->setPassword(password.toStdString());
        }

        uint32_t flags = 0;
        if (!ui.checkbox_disable_user->isChecked())
            flags |= User::FLAG_ENABLED;

        uint32_t sessions = 0;
        for (int i = 0; i < ui.tree_sessions->topLevelItemCount(); ++i)
        {
            QTreeWidgetItem* item = ui.tree_sessions->topLevelItem(i);
            if (item->checkState(0) == Qt::Checked)
                sessions |= item->data(0, Qt::UserRole).toInt();
        }

        user_->setName(name.toStdString());
        user_->setFlags(flags);
        user_->setSessions(sessions);

        accept();
    }
    else
    {
        reject();
    }

    close();
}

} // namespace aspia
