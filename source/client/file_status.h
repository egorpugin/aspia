//
// PROJECT:         Aspia
// FILE:            client/file_status.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <QString>

#include "protocol/file_transfer_session.pb.h"

namespace aspia {

QString fileStatusToString(proto::file_transfer::Status status);

} // namespace aspia
