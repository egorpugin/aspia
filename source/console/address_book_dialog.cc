/*
 * Aspia: Remote desktop and file transfer tool.
 * Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "console/address_book_dialog.h"

#include <QAbstractButton>
#include <QMessageBox>

#include "base/log.h"
#include "crypto/data_encryptor.h"
#include "crypto/random.h"

namespace aspia {

namespace {

constexpr int kMaxNameLength = 64;
constexpr int kMinNameLength = 1;
constexpr int kMinPasswordLength = 8;
constexpr int kMaxCommentLength = 2048;

} // namespace

AddressBookDialog::AddressBookDialog(QWidget* parent, proto::address_book::File* file,
                                     proto::address_book::Data* data, std::string* key)
    : QDialog(parent), file_(file), data_(data), key_(key)
{
    ui.setupUi(this);

    connect(ui.button_box, &QDialogButtonBox::clicked, this, &AddressBookDialog::buttonBoxClicked);

    connect(ui.combo_encryption, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &AddressBookDialog::encryptionTypedChanged);

    ui.combo_encryption->addItem(tr("Without Encryption"),
                                 QVariant(proto::address_book::ENCRYPTION_TYPE_NONE));
    ui.combo_encryption->addItem(tr("XChaCha20 + Poly1305 (256-bit key)"),
                                 QVariant(proto::address_book::ENCRYPTION_TYPE_XCHACHA20_POLY1305));

    ui.edit_name->setText(QString::fromStdString(data_->root_group().name()));
    ui.edit_comment->setPlainText(QString::fromStdString(data_->root_group().comment()));

    int current = ui.combo_encryption->findData(QVariant(file->encryption_type()));
    if (current != -1)
        ui.combo_encryption->setCurrentIndex(current);

    if (file->encryption_type() == proto::address_book::ENCRYPTION_TYPE_XCHACHA20_POLY1305)
    {
        if (!key_->empty())
        {
            QString text = tr("Double-click to change");

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

            ui.spinbox_password_salt->setValue(file_->hashing_salt().size());
            ui.spinbox_hashing_rounds->setValue(file_->hashing_rounds());

            ui.spinbox_salt_before->setValue(data_->salt1().size());
            ui.spinbox_salt_after->setValue(data_->salt2().size());

            password_changed_ = false;
        }
    }
    else
    {
        ui.edit_password->setEnabled(false);

        // Disable Advanced tab.
        ui.tab_widget->setTabEnabled(1, false);
    }

    connect(ui.spinbox_hashing_rounds, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &AddressBookDialog::hashingRoundsChanged);

    connect(ui.spinbox_password_salt, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &AddressBookDialog::hashingSaltChanged);

    ui.edit_name->setFocus();
}

bool AddressBookDialog::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonDblClick &&
        (object == ui.edit_password || object == ui.edit_password_repeat))
    {
        proto::address_book::EncryptionType encryption_type =
            static_cast<proto::address_book::EncryptionType>(
                ui.combo_encryption->currentData().toInt());

        if (encryption_type == proto::address_book::ENCRYPTION_TYPE_NONE)
            return false;

        setPasswordChanged();
    }

    return false;
}

void AddressBookDialog::buttonBoxClicked(QAbstractButton* button)
{
    if (ui.button_box->standardButton(button) != QDialogButtonBox::Ok)
    {
        reject();
        close();
        return;
    }

    auto name = ui.edit_name->text().toStdString();
    if (name.length() > kMaxNameLength)
    {
        showError(tr("Too long name. The maximum length of the name is 64 characters."));
        return;
    }
    else if (name.length() < kMinNameLength)
    {
        showError(tr("Name can not be empty."));
        return;
    }

    auto comment = ui.edit_comment->toPlainText().toStdString();
    if (comment.length() > kMaxCommentLength)
    {
        showError(tr("Too long comment. The maximum length of the comment is 2048 characters."));
        return;
    }

    proto::address_book::EncryptionType encryption_type =
        static_cast<proto::address_book::EncryptionType>(
            ui.combo_encryption->currentData().toInt());

    switch (encryption_type)
    {
        case proto::address_book::ENCRYPTION_TYPE_NONE:
        {
            file_->mutable_hashing_salt()->clear();
            file_->set_hashing_rounds(0);

            data_->mutable_salt1()->clear();
            data_->mutable_salt2()->clear();
        }
        break;

        case proto::address_book::ENCRYPTION_TYPE_XCHACHA20_POLY1305:
        {
            if (password_changed_)
            {
                auto password = ui.edit_password->text().toStdString();
                auto password_repeat = ui.edit_password_repeat->text().toStdString();

                if (password != password_repeat)
                {
                    showError(tr("The passwords you entered do not match."));
                    return;
                }

                if (password.length() < kMinPasswordLength)
                {
                    showError(tr("Password can not be shorter than 8 characters."));
                    return;
                }

                // Generate salt, which is added after each iteration of the hashing.
                // New salt is generated each time the password is changed.
                auto hashing_salt =
                    Random::generateBuffer(ui.spinbox_password_salt->value());

                // Save the salt and the number of hashing iterations.
                file_->set_hashing_rounds(ui.spinbox_hashing_rounds->value());
                *file_->mutable_hashing_salt() = hashing_salt;

                // Now generate a key for encryption/decryption.
                *key_ = DataEncryptor::createKey(password, hashing_salt,
                                                 file_->hashing_rounds());
            }

            int salt_before_size = ui.spinbox_salt_before->value();
            int salt_after_size = ui.spinbox_salt_after->value();

            if (salt_before_size != data_->salt1().size())
                *data_->mutable_salt1() = Random::generateBuffer(salt_before_size);

            if (salt_after_size != data_->salt2().size())
                *data_->mutable_salt2() = Random::generateBuffer(salt_after_size);
        }
        break;

        default:
            LOG_FATAL(logger, "Unexpected encryption type: " << encryption_type);
            return;
    }

    data_->mutable_root_group()->set_name(name);
    data_->mutable_root_group()->set_comment(comment);

    file_->set_encryption_type(encryption_type);

    accept();
    close();
}

void AddressBookDialog::encryptionTypedChanged(int item_index)
{
    proto::address_book::EncryptionType encryption_type =
        static_cast<proto::address_book::EncryptionType>(
            ui.combo_encryption->itemData(item_index).toInt());

    switch (encryption_type)
    {
        case proto::address_book::ENCRYPTION_TYPE_NONE:
        {
            ui.edit_password->setEnabled(false);
            ui.edit_password_repeat->setEnabled(false);

            ui.edit_password->clear();
            ui.edit_password_repeat->clear();

            // Disable Advanced tab.
            ui.tab_widget->setTabEnabled(1, false);
        }
        break;

        case proto::address_book::ENCRYPTION_TYPE_XCHACHA20_POLY1305:
        {
            ui.edit_password->setEnabled(true);
            ui.edit_password_repeat->setEnabled(true);

            // Enable Advanced tab.
            ui.tab_widget->setTabEnabled(1, true);
        }
        break;

        default:
            LOG_FATAL(logger, "Unexpected encryption type: " << encryption_type);
            break;
    }
}

void AddressBookDialog::hashingRoundsChanged(int /* value */)
{
    if (password_changed_ || value_reverting_)
        return;

    if (QMessageBox::question(
        this,
        tr("Confirmation"),
        tr("At change the number of hashing iterations, you will need to re-enter the password. Continue?"),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        setPasswordChanged();
    }
    else
    {
        // Revert value.
        value_reverting_ = true;
        ui.spinbox_hashing_rounds->setValue(file_->hashing_rounds());
        value_reverting_ = false;
    }
}

void AddressBookDialog::hashingSaltChanged(int /* value */)
{
    if (password_changed_ || value_reverting_)
        return;

    if (QMessageBox::question(
        this,
        tr("Confirmation"),
        tr("At change the size of hashing salt, you will need to re-enter the password. Continue?"),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        setPasswordChanged();
    }
    else
    {
        // Revert value.
        value_reverting_ = true;
        ui.spinbox_password_salt->setValue(file_->hashing_salt().size());
        value_reverting_ = false;
    }
}

void AddressBookDialog::setPasswordChanged()
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

void AddressBookDialog::showError(const QString& message)
{
    QMessageBox(QMessageBox::Warning, tr("Warning"), message, QMessageBox::Ok, this).exec();
}

} // namespace aspia
