//
// PROJECT:         Aspia
// FILE:            ASClient/file_remove_task.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "client/file_remove_task.h"

namespace aspia {

FileRemoveTask::FileRemoveTask(const QString& path, bool is_directory)
    : path_(path),
      is_directory_(is_directory)
{
    // Nothing
}

FileRemoveTask::FileRemoveTask(const FileRemoveTask& other) = default;
FileRemoveTask& FileRemoveTask::operator=(const FileRemoveTask& other) = default;

FileRemoveTask::FileRemoveTask(FileRemoveTask&& other) noexcept
    : path_(std::move(other.path_)),
      is_directory_(other.is_directory_)
{
    // Nothing
}

FileRemoveTask& FileRemoveTask::operator=(FileRemoveTask&& other) noexcept
{
    path_ = std::move(other.path_);
    is_directory_ = other.is_directory_;
    return *this;
}

} // namespace aspia
