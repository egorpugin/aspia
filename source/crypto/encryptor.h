//
// PROJECT:         Aspia
// FILE:            crypto/encryptor.h
// LICENSE:         Mozilla Public License Version 2.0
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CRYPTO__ENCRYPTOR_H
#define _ASPIA_CRYPTO__ENCRYPTOR_H

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

#endif // _ASPIA_CRYPTO__ENCRYPTOR_H
