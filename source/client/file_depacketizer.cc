//
// PROJECT:         Aspia
// FILE:            host/file_depacketizer.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "client/file_depacketizer.h"

#include "base/log.h"

namespace aspia {

FileDepacketizer::FileDepacketizer(QPointer<QFile>& file)
{
    file_.swap(file);
}

// static
std::unique_ptr<FileDepacketizer> FileDepacketizer::create(
    const std::string& file_path, bool overwrite)
{
    QFile::OpenMode mode = QFile::WriteOnly;

    if (overwrite)
        mode |= QFile::Truncate;

    QPointer<QFile> file = new QFile(file_path.c_str());

    if (!file->open(mode))
        return nullptr;

    return std::unique_ptr<FileDepacketizer>(new FileDepacketizer(file));
}

bool FileDepacketizer::writeNextPacket(const proto::file_transfer::Packet& packet)
{
    assert(!file_.isNull() && file_->isOpen());

    // The first packet must have the full file size.
    if (packet.flags() & proto::file_transfer::Packet::FLAG_FIRST_PACKET)
    {
        file_size_ = packet.file_size();
        left_size_ = file_size_;
    }

    const size_t packet_size = packet.data().size();

    if (!file_->seek(file_size_ - left_size_))
    {
        LOG_DEBUG(logger, "seek failed");
        return false;
    }

    if (file_->write(packet.data().data(), packet_size) != packet_size)
    {
        LOG_DEBUG(logger, "Unable to write file");
        return false;
    }

    left_size_ -= packet_size;

    if (packet.flags() & proto::file_transfer::Packet::FLAG_LAST_PACKET)
    {
        file_size_ = 0;
        file_->close();
    }

    return true;
}

} // namespace aspia
