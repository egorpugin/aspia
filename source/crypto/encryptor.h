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

#pragma once

#include "base/common.h"

#include <vector>

namespace aspia {

// Implements encryption of messages with using xsalsa20 + poly1305 algorithms.
class ASPIA_CRYPTO_API Encryptor
{
public:
    enum Mode
    {
        ServerMode,
        ClientMode
    };

    explicit Encryptor(Mode mode);
    ~Encryptor();

    bool readHelloMessage(const std::string& message_buffer);
    std::string helloMessage();

    std::string encrypt(const std::string& source_buffer);
    std::string decrypt(const std::string& source_buffer);

private:
    const Mode mode_;

    std::vector<uint8_t> local_public_key_;
    std::vector<uint8_t> local_secret_key_;

    std::vector<uint8_t> encrypt_key_;
    std::vector<uint8_t> decrypt_key_;

    std::vector<uint8_t> encrypt_nonce_;
    std::vector<uint8_t> decrypt_nonce_;

    DISABLE_COPY(Encryptor)
};

} // namespace aspia
