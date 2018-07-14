//
// PROJECT:         Aspia
// FILE:            base/message_serialization.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/log.h"

#include <google/protobuf/message_lite.h>

namespace aspia {

static std::string serializeMessage(const google::protobuf::MessageLite& message)
{
    size_t size = message.ByteSizeLong();
    if (!size)
    {
        LOG_WARN(logger, "Empty messages are not allowed");
        return {};
    }

    std::string buffer;
    buffer.resize(size);

    message.SerializeWithCachedSizesToArray(reinterpret_cast<uint8_t*>(buffer.data()));
    return buffer;
}

template <class T>
bool parseMessage(const std::string& buffer, T& message)
{
    if (!message.ParseFromArray(buffer.data(), buffer.size()))
    {
        LOG_WARN(logger, "Received message that is not a valid protocol buffer");
        return false;
    }

    return true;
}

} // namespace aspia
