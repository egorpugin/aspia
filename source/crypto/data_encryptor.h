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

namespace aspia {

class ASPIA_CRYPTO_API DataEncryptor
{
public:
    // Creates a key from the password. |password| must be in UTF-8 encoding.
    static std::string createKey(const std::string& password,
                                const std::string& salt,
                                int rounds);

    static std::string encrypt(const std::string& source_data, const std::string& key);

    static bool decrypt(const std::string& source_data,
                        const std::string& key,
                        std::string* decrypted_data);

    static bool decrypt(const char* source_data,
                        int source_size,
                        const std::string& key,
                        std::string* decrypted_data);

private:
    DISABLE_COPY(DataEncryptor)
};

ASPIA_CRYPTO_API
std::string sha512(const std::string &);

} // namespace aspia
