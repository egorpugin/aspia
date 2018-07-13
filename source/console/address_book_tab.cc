//
// PROJECT:         Aspia
// FILE:            console/address_book_tab.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "console/address_book_tab.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

#include "base/message_serialization.h"
#include "client/computer_factory.h"
#include "codec/video_util.h"
#include "console/address_book_dialog.h"
#include "console/computer_dialog.h"
#include "console/computer_group_dialog.h"
#include "console/computer_item.h"
#include "console/console_settings.h"
#include "console/open_address_book_dialog.h"
#include "crypto/data_encryptor.h"
#include "crypto/secure_memory.h"

namespace aspia {

namespace {

void cleanupComputer(proto::address_book::Computer* computer)
{
    if (!computer)
        return;

    secureMemZero(computer->mutable_name());
    secureMemZero(computer->mutable_address());
    secureMemZero(computer->mutable_username());
    secureMemZero(computer->mutable_password());
    secureMemZero(computer->mutable_comment());
}

void cleanupComputerGroup(proto::address_book::ComputerGroup* computer_group)
{
    if (!computer_group)
        return;

    for (int i = 0; i < computer_group->computer_size(); ++i)
        cleanupComputer(computer_group->mutable_computer(i));

    for (int i = 0; i < computer_group->computer_group_size(); ++i)
    {
        proto::address_book::ComputerGroup* child_group =
            computer_group->mutable_computer_group(i);

        secureMemZero(child_group->mutable_name());
        secureMemZero(child_group->mutable_comment());

        cleanupComputerGroup(child_group);
    }
}

void cleanupData(proto::address_book::Data* data)
{
    if (!data)
        return;

    cleanupComputerGroup(data->mutable_root_group());

    secureMemZero(data->mutable_salt1());
    secureMemZero(data->mutable_salt2());
}

void cleanupFile(proto::address_book::File* file)
{
    if (!file)
        return;

    secureMemZero(file->mutable_hashing_salt());
    secureMemZero(file->mutable_data());
}

} // namespace

AddressBookTab::AddressBookTab(const QString& file_path,
                               proto::address_book::File&& file,
                               proto::address_book::Data&& data,
                               std::string&& key,
                               QWidget* parent)
    : ConsoleTab(ConsoleTab::AddressBook, parent),
      file_path_(file_path),
      file_(std::move(file)),
      data_(std::move(data)),
      key_(std::move(key))
{
    ui.setupUi(this);

    QList<int> sizes;
    sizes.push_back(200);
    sizes.push_back(width() - 200);

    ui.splitter->setSizes(sizes);

    ComputerGroupItem* group_item = new ComputerGroupItem(data_.mutable_root_group(), nullptr);

    ui.tree_group->addTopLevelItem(group_item);
    ui.tree_group->setCurrentItem(group_item);

    updateComputerList(group_item);

    group_item->setExpanded(group_item->IsExpanded());

    std::function<void(ComputerGroupItem*)> restore_child = [&](ComputerGroupItem* item)
    {
        int count = item->childCount();
        for (int i = 0; i < count; ++i)
        {
            ComputerGroupItem* child_item = dynamic_cast<ComputerGroupItem*>(item->child(i));
            if (child_item)
            {
                if (child_item->IsExpanded())
                    child_item->setExpanded(true);

                restore_child(child_item);
            }
        }
    };

    restore_child(group_item);

    connect(ui.tree_group, &ComputerGroupTree::itemClicked,
            this, &AddressBookTab::onGroupItemClicked);

    connect(ui.tree_group, &ComputerGroupTree::customContextMenuRequested,
            this, &AddressBookTab::onGroupContextMenu);

    connect(ui.tree_group, &ComputerGroupTree::itemCollapsed,
            this, &AddressBookTab::onGroupItemCollapsed);

    connect(ui.tree_group, &ComputerGroupTree::itemExpanded,
            this, &AddressBookTab::onGroupItemExpanded);

    connect(ui.tree_group, &ComputerGroupTree::itemDropped,
            this, &AddressBookTab::onGroupItemDropped);

    connect(ui.tree_computer, &ComputerTree::itemClicked,
            this, &AddressBookTab::onComputerItemClicked);

    connect(ui.tree_computer, &ComputerTree::customContextMenuRequested,
            this, &AddressBookTab::onComputerContextMenu);

    connect(ui.tree_computer, &ComputerTree::itemDoubleClicked,
            this, &AddressBookTab::onComputerItemDoubleClicked);
}

AddressBookTab::~AddressBookTab()
{
    cleanupData(&data_);
    cleanupFile(&file_);

    secureMemZero(&file_path_.toStdString());
    secureMemZero(&key_);
}

// static
AddressBookTab* AddressBookTab::createNew(QWidget* parent)
{
    proto::address_book::File file;
    proto::address_book::Data data;
    std::string key;

    AddressBookDialog dialog(parent, &file, &data, &key);
    if (dialog.exec() != QDialog::Accepted)
        return nullptr;

    AddressBookTab* tab = new AddressBookTab(
        QString(), std::move(file), std::move(data), std::move(key), parent);

    tab->setChanged(true);
    return tab;
}

// static
AddressBookTab* AddressBookTab::openFromFile(const QString& file_path, QWidget* parent)
{
    if (file_path.isEmpty())
        return nullptr;

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        showOpenError(parent, tr("Unable to open address book file."));
        return nullptr;
    }

    auto buffer = file.readAll();
    if (buffer.isEmpty())
    {
        showOpenError(parent, tr("Unable to read address book file."));
        return nullptr;
    }

    proto::address_book::File address_book_file;

    if (!address_book_file.ParseFromArray(buffer.constData(), buffer.size()))
    {
        showOpenError(parent, tr("The address book file is corrupted or has an unknown format."));
        return nullptr;
    }

    secureMemZero(&buffer.toStdString());

    proto::address_book::Data address_book_data;
    std::string key;

    switch (address_book_file.encryption_type())
    {
        case proto::address_book::ENCRYPTION_TYPE_NONE:
        {
            if (!address_book_data.ParseFromString(address_book_file.data()))
            {
                showOpenError(parent, tr("The address book file is corrupted or has an unknown format."));
                return nullptr;
            }
        }
        break;

        case proto::address_book::ENCRYPTION_TYPE_XCHACHA20_POLY1305:
        {
            OpenAddressBookDialog dialog(parent, address_book_file.encryption_type());
            if (dialog.exec() != QDialog::Accepted)
                return nullptr;

            key = DataEncryptor::createKey(
                dialog.password().toStdString(),
                address_book_file.hashing_salt(),
                address_book_file.hashing_rounds());

            std::string decrypted_data;

            if (!DataEncryptor::decrypt(address_book_file.data().c_str(),
                                        address_book_file.data().size(),
                                        key,
                                        &decrypted_data))
            {
                showOpenError(parent, tr("Unable to decrypt the address book with the specified password."));
                return nullptr;
            }

            if (!parseMessage(decrypted_data, address_book_data))
            {
                showOpenError(parent, tr("The address book file is corrupted or has an unknown format."));
                return nullptr;
            }

            secureMemZero(&decrypted_data);
        }
        break;

        default:
        {
            showOpenError(parent, tr("The address book file is encrypted with an unsupported encryption type."));
            return nullptr;
        }
    }

    return new AddressBookTab(file_path,
                              std::move(address_book_file),
                              std::move(address_book_data),
                              std::move(key),
                              parent);
}

QString AddressBookTab::addressBookName() const
{
    return QString::fromStdString(data_.root_group().name());
}

proto::address_book::Computer* AddressBookTab::currentComputer() const
{
    ComputerItem* current_item = dynamic_cast<ComputerItem*>(ui.tree_computer->currentItem());
    if (!current_item)
        return nullptr;

    return current_item->computer();
}

proto::address_book::ComputerGroup* AddressBookTab::currentComputerGroup() const
{
    ComputerGroupItem* current_item =
        dynamic_cast<ComputerGroupItem*>(ui.tree_group->currentItem());
    if (!current_item)
        return nullptr;

    return current_item->computerGroup();
}

void AddressBookTab::save()
{
    saveToFile(file_path_);
}

void AddressBookTab::saveAs()
{
    saveToFile(QString());
}

void AddressBookTab::addComputerGroup()
{
    ComputerGroupItem* parent_item =
        dynamic_cast<ComputerGroupItem*>(ui.tree_group->currentItem());
    if (!parent_item)
        return;

    std::unique_ptr<proto::address_book::ComputerGroup> computer_group =
        std::make_unique<proto::address_book::ComputerGroup>();

    ComputerGroupDialog dialog(this,
                               ComputerGroupDialog::CreateComputerGroup,
                               computer_group.get(),
                               parent_item->computerGroup());
    if (dialog.exec() != QDialog::Accepted)
        return;

    proto::address_book::ComputerGroup* computer_group_released = computer_group.release();

    ComputerGroupItem* item = parent_item->addChildComputerGroup(computer_group_released);
    ui.tree_group->setCurrentItem(item);
    setChanged(true);
}

void AddressBookTab::addComputer()
{
    ComputerGroupItem* parent_item =
        dynamic_cast<ComputerGroupItem*>(ui.tree_group->currentItem());
    if (!parent_item)
        return;

    std::unique_ptr<proto::address_book::Computer> computer =
        std::make_unique<proto::address_book::Computer>(ComputerFactory::defaultComputer());

    ComputerDialog dialog(this,
                          ComputerDialog::CreateComputer,
                          computer.get(),
                          parent_item->computerGroup());
    if (dialog.exec() != QDialog::Accepted)
        return;

    proto::address_book::Computer* computer_released = computer.release();

    parent_item->addChildComputer(computer_released);
    if (ui.tree_group->currentItem() == parent_item)
    {
        ui.tree_computer->addTopLevelItem(new ComputerItem(computer_released, parent_item));
    }

    setChanged(true);
}

void AddressBookTab::modifyAddressBook()
{
    ComputerGroupItem* root_item =
        dynamic_cast<ComputerGroupItem*>(ui.tree_group->topLevelItem(0));
    if (!root_item)
    {
        LOG_DEBUG(logger, "Invalid root item");
        return;
    }

    AddressBookDialog dialog(this, &file_, &data_, &key_);
    if (dialog.exec() != QDialog::Accepted)
        return;

    root_item->updateItem();
    setChanged(true);
}

void AddressBookTab::modifyComputerGroup()
{
    ComputerGroupItem* current_item =
        dynamic_cast<ComputerGroupItem*>(ui.tree_group->currentItem());
    if (!current_item)
        return;

    ComputerGroupItem* parent_item = dynamic_cast<ComputerGroupItem*>(current_item->parent());
    if (!parent_item)
        return;

    ComputerGroupDialog dialog(this,
                               ComputerGroupDialog::ModifyComputerGroup,
                               current_item->computerGroup(),
                               parent_item->computerGroup());
    if (dialog.exec() != QDialog::Accepted)
        return;

    current_item->updateItem();
    setChanged(true);
}

void AddressBookTab::modifyComputer()
{
    ComputerItem* current_item = dynamic_cast<ComputerItem*>(ui.tree_computer->currentItem());
    if (!current_item)
        return;

    ComputerDialog dialog(this,
                          ComputerDialog::ModifyComputer,
                          current_item->computer(),
                          current_item->parentComputerGroupItem()->computerGroup());
    if (dialog.exec() != QDialog::Accepted)
        return;

    current_item->updateItem();
    setChanged(true);
}

void AddressBookTab::removeComputerGroup()
{
    ComputerGroupItem* current_item =
        dynamic_cast<ComputerGroupItem*>(ui.tree_group->currentItem());
    if (!current_item)
        return;

    ComputerGroupItem* parent_item = dynamic_cast<ComputerGroupItem*>(current_item->parent());
    if (!parent_item)
        return;

    QString message =
        tr("Are you sure you want to delete computer group \"%1\" and all child items?")
        .arg(QString::fromStdString(current_item->computerGroup()->name()));

    if (QMessageBox::question(this,
                              tr("Confirmation"),
                              message,
                              QMessageBox::Yes,
                              QMessageBox::No) == QMessageBox::Yes)
    {
        cleanupComputerGroup(current_item->computerGroup());

        if (parent_item->deleteChildComputerGroup(current_item))
            setChanged(true);
    }
}

void AddressBookTab::removeComputer()
{
    ComputerItem* current_item = dynamic_cast<ComputerItem*>(ui.tree_computer->currentItem());
    if (!current_item)
        return;

    QString message = tr("Are you sure you want to delete computer \"%1\"?")
        .arg(QString::fromStdString(current_item->computer()->name()));

    if (QMessageBox::question(this,
                              tr("Confirmation"),
                              message,
                              QMessageBox::Yes,
                              QMessageBox::No) == QMessageBox::Yes)
    {
        ComputerGroupItem* parent_group = current_item->parentComputerGroupItem();

        cleanupComputer(current_item->computer());

        if (parent_group->deleteChildComputer(current_item->computer()))
        {
            delete current_item;
            setChanged(true);
        }
    }
}

void AddressBookTab::onGroupItemClicked(QTreeWidgetItem* item, int /* column */)
{
    ComputerGroupItem* current_item = dynamic_cast<ComputerGroupItem*>(item);
    if (!current_item)
        return;

    bool is_root = !current_item->parent();
    emit computerGroupActivated(true, is_root);
    updateComputerList(current_item);
}

void AddressBookTab::onGroupContextMenu(const QPoint& point)
{
    ComputerGroupItem* current_item =
        dynamic_cast<ComputerGroupItem*>(ui.tree_group->itemAt(point));
    if (!current_item)
        return;

    ui.tree_group->setCurrentItem(current_item);
    onGroupItemClicked(current_item, 0);

    bool is_root = !current_item->parent();
    emit computerGroupContextMenu(ui.tree_group->mapToGlobal(point), is_root);
}

void AddressBookTab::onGroupItemCollapsed(QTreeWidgetItem* item)
{
    ComputerGroupItem* current_item = dynamic_cast<ComputerGroupItem*>(item);
    if (!current_item)
        return;

    current_item->SetExpanded(false);
    setChanged(true);
}

void AddressBookTab::onGroupItemExpanded(QTreeWidgetItem* item)
{
    ComputerGroupItem* current_item = dynamic_cast<ComputerGroupItem*>(item);
    if (!current_item)
        return;

    current_item->SetExpanded(true);
    setChanged(true);
}

void AddressBookTab::onGroupItemDropped()
{
    ComputerGroupItem* current_item =
        dynamic_cast<ComputerGroupItem*>(ui.tree_group->currentItem());
    if (!current_item)
        return;

    updateComputerList(current_item);
    setChanged(true);
}

void AddressBookTab::onComputerItemClicked(QTreeWidgetItem* item, int /* column */)
{
    ComputerItem* current_item = dynamic_cast<ComputerItem*>(item);
    if (!current_item)
        return;

    emit computerActivated(true);
}

void AddressBookTab::onComputerContextMenu(const QPoint& point)
{
    ComputerItem* current_item = dynamic_cast<ComputerItem*>(ui.tree_computer->itemAt(point));
    if (current_item)
    {
        ui.tree_computer->setCurrentItem(current_item);
        onComputerItemClicked(current_item, 0);
    }

    emit computerContextMenu(current_item, ui.tree_computer->mapToGlobal(point));
}

void AddressBookTab::onComputerItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    ComputerItem* current_item = dynamic_cast<ComputerItem*>(item);
    if (!current_item)
        return;

    emit computerDoubleClicked(current_item->computer());
}

void AddressBookTab::showEvent(QShowEvent* event)
{
    ComputerGroupItem* current_group =
        dynamic_cast<ComputerGroupItem*>(ui.tree_group->currentItem());
    if (!current_group)
    {
        emit computerGroupActivated(false, false);
    }
    else
    {
        bool is_root = !current_group->parent();
        emit computerGroupActivated(true, is_root);
    }

    ComputerItem* current_computer = dynamic_cast<ComputerItem*>(ui.tree_computer->currentItem());

    if (!current_computer)
        emit computerActivated(false);
    else
        emit computerActivated(true);

    QWidget::showEvent(event);
}

void AddressBookTab::setChanged(bool value)
{
    is_changed_ = value;
    emit addressBookChanged(value);
}

void AddressBookTab::retranslateUi()
{
    ui.retranslateUi(this);

    QTreeWidgetItem* current = ui.tree_group->currentItem();
    if (current)
        onGroupItemClicked(current, 0);
}

void AddressBookTab::updateComputerList(ComputerGroupItem* computer_group)
{
    for (int i = ui.tree_computer->topLevelItemCount() - 1; i >= 0; --i)
    {
        QTreeWidgetItem* item = ui.tree_computer->takeTopLevelItem(i);
        delete item;
    }

    ui.tree_computer->addTopLevelItems(computer_group->ComputerList());
}

bool AddressBookTab::saveToFile(const QString& file_path)
{
    auto serialized_data = serializeMessage(data_);

    switch (file_.encryption_type())
    {
        case proto::address_book::ENCRYPTION_TYPE_NONE:
            file_.set_data(serialized_data.data(), serialized_data.size());
            break;

        case proto::address_book::ENCRYPTION_TYPE_XCHACHA20_POLY1305:
        {
            auto encrypted_data = DataEncryptor::encrypt(serialized_data, key_);
            file_.set_data(encrypted_data.data(), encrypted_data.size());
            secureMemZero(&encrypted_data);
        }
        break;

        default:
            LOG_FATAL(logger, "Unknown encryption type: " << file_.encryption_type());
            return false;
    }

    secureMemZero(&serialized_data);

    auto path = file_path;
    if (path.isEmpty())
    {
        ConsoleSettings settings;

        path = QFileDialog::getSaveFileName(this,
                                            tr("Save Address Book"),
                                            settings.lastDirectory(),
                                            tr("Aspia Address Book (*.aab)"));
        if (path.isEmpty())
            return false;

        settings.setLastDirectory(QFileInfo(path).absolutePath());
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        showSaveError(this, tr("Unable to create or open address book file."));
        return false;
    }

    auto buffer = serializeMessage(file_);

    qint64 bytes_written = file.write(buffer.c_str());

    secureMemZero(&buffer);

    if (bytes_written != buffer.size())
    {
        showSaveError(this, tr("Unable to write address book file."));
        return false;
    }

    file_path_ = path;

    setChanged(false);
    return true;
}

// static
void AddressBookTab::showOpenError(QWidget* parent, const QString& message)
{
    QMessageBox dialog(parent);

    dialog.setIcon(QMessageBox::Warning);
    dialog.setWindowTitle(tr("Warning"));
    dialog.setInformativeText(message);
    dialog.setText(tr("Could not open address book"));
    dialog.setStandardButtons(QMessageBox::Ok);

    dialog.exec();
}

// static
void AddressBookTab::showSaveError(QWidget* parent, const QString& message)
{
    QMessageBox dialog(parent);

    dialog.setIcon(QMessageBox::Warning);
    dialog.setWindowTitle(tr("Warning"));
    dialog.setInformativeText(message);
    dialog.setText(tr("Failed to save address book"));
    dialog.setStandardButtons(QMessageBox::Ok);

    dialog.exec();
}

} // namespace aspia
