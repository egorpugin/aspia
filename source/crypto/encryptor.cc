//
// PROJECT:         Aspia
// FILE:            crypto/encryptor.cc
// LICENSE:         Mozilla Public License Version 2.0
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "crypto/encryptor.h"

#include "base/message_serialization.h"
#include "protocol/key_exchange.pb.h"

extern "C" {
#pragma warning(push, 3)
#include <sodium.h>
#pragma warning(pop)
} // extern "C"

namespace aspia {

Encryptor::Encryptor(Mode mode)
    : mode_(mode)
{
    if (sodium_init() == -1)
    {
        LOG_WARN(logger, "sodium_init failed");
        return;
    }

    std::vector<uint8_t> public_key;
    public_key.resize(crypto_kx_PUBLICKEYBYTES);

    std::vector<uint8_t> secret_key;
    secret_key.resize(crypto_kx_SECRETKEYBYTES);

    if (crypto_kx_keypair(public_key.data(), secret_key.data()) != 0)
    {
        LOG_WARN(logger, "crypto_kx_keypair failed");
        return;
    }

    local_public_key_ = std::move(public_key);
    local_secret_key_ = std::move(secret_key);
}

Encryptor::~Encryptor()
{
    if (!encrypt_key_.empty())
    {
        sodium_memzero(encrypt_key_.data(), encrypt_key_.size());
        encrypt_key_.clear();
    }

    if (!decrypt_key_.empty())
    {
        sodium_memzero(decrypt_key_.data(), decrypt_key_.size());
        decrypt_key_.clear();
    }

    if (!encrypt_nonce_.empty())
    {
        sodium_memzero(encrypt_nonce_.data(), encrypt_nonce_.size());
        encrypt_nonce_.clear();
    }

    if (!decrypt_nonce_.empty())
    {
        sodium_memzero(decrypt_nonce_.data(), decrypt_nonce_.size());
        decrypt_nonce_.clear();
    }
}

bool Encryptor::readHelloMessage(const std::string& message_buffer)
{
    if (local_public_key_.empty() || local_secret_key_.empty())
        return false;

    proto::HelloMessage message;

    if (!parseMessage(message_buffer, message))
        return false;

    if (message.public_key().size() != crypto_kx_PUBLICKEYBYTES)
        return false;

    if (message.nonce().size() != crypto_secretbox_NONCEBYTES)
        return false;

    decrypt_nonce_.resize(crypto_secretbox_NONCEBYTES);
    memcpy(decrypt_nonce_.data(), message.nonce().data(), crypto_secretbox_NONCEBYTES);

    std::vector<uint8_t> decrypt_key;
    decrypt_key.resize(crypto_kx_SESSIONKEYBYTES);

    std::vector<uint8_t> encrypt_key;
    encrypt_key.resize(crypto_kx_SESSIONKEYBYTES);

    if (mode_ == ServerMode)
    {
        if (crypto_kx_server_session_keys(
                decrypt_key.data(),
                encrypt_key.data(),
                local_public_key_.data(),
                local_secret_key_.data(),
                reinterpret_cast<const uint8_t*>(message.public_key().data())) != 0)
        {
            LOG_WARN(logger, "crypto_kx_server_session_keys failed");
            return false;
        }
    }
    else
    {
        assert(mode_ == ClientMode);

        if (crypto_kx_client_session_keys(
               decrypt_key.data(),
               encrypt_key.data(),
               local_public_key_.data(),
               local_secret_key_.data(),
               reinterpret_cast<const uint8_t*>(message.public_key().data())) != 0)
        {
            LOG_WARN(logger, "crypto_kx_client_session_keys failed");
            return false;
        }
    }

    sodium_memzero(message.mutable_public_key()->data(), message.mutable_public_key()->size());
    sodium_memzero(message.mutable_nonce()->data(), message.mutable_nonce()->size());

    if (mode_ == ClientMode)
    {
        sodium_memzero(local_public_key_.data(), local_public_key_.size());
        sodium_memzero(local_secret_key_.data(), local_secret_key_.size());

        local_public_key_.clear();
        local_secret_key_.clear();
    }

    decrypt_key_ = std::move(decrypt_key);
    encrypt_key_ = std::move(encrypt_key);

    return true;
}

std::string Encryptor::helloMessage()
{
    if (local_public_key_.empty() || local_secret_key_.empty())
        return std::string();

    encrypt_nonce_.resize(crypto_secretbox_NONCEBYTES);

    // Generate nonce for encryption.
    randombytes_buf(encrypt_nonce_.data(), encrypt_nonce_.size());

    proto::HelloMessage message;

    message.set_public_key(local_public_key_.data(), local_public_key_.size());
    message.set_nonce(encrypt_nonce_.data(), encrypt_nonce_.size());

    std::string message_buffer = serializeMessage(message);

    sodium_memzero(message.mutable_public_key()->data(), message.mutable_public_key()->size());
    sodium_memzero(message.mutable_nonce()->data(), message.mutable_nonce()->size());

    if (mode_ == ServerMode)
    {
        sodium_memzero(local_public_key_.data(), local_public_key_.size());
        sodium_memzero(local_secret_key_.data(), local_secret_key_.size());

        local_public_key_.clear();
        local_secret_key_.clear();
    }

    return message_buffer;
}

std::string Encryptor::encrypt(const std::string& source_buffer)
{
    assert(local_public_key_.empty());
    assert(local_secret_key_.empty());
    assert(encrypt_nonce_.size() == crypto_secretbox_NONCEBYTES);
    assert(!encrypt_key_.empty());

    sodium_increment(encrypt_nonce_.data(), crypto_secretbox_NONCEBYTES);

    std::string encrypted_buffer;
    encrypted_buffer.resize(source_buffer.size() + crypto_secretbox_MACBYTES);

    // Encrypt message.
    if (crypto_secretbox_easy(reinterpret_cast<uint8_t*>(encrypted_buffer.data()),
                              reinterpret_cast<const uint8_t*>(source_buffer.data()),
                              source_buffer.size(),
                              encrypt_nonce_.data(),
                              encrypt_key_.data()) != 0)
    {
        LOG_WARN(logger, "crypto_secretbox_easy failed");
        return std::string();
    }

    return encrypted_buffer;
}

std::string Encryptor::decrypt(const std::string& source_buffer)
{
    assert(local_public_key_.empty());
    assert(local_secret_key_.empty());
    assert(decrypt_nonce_.size() == crypto_secretbox_NONCEBYTES);
    assert(!decrypt_key_.empty());

    sodium_increment(decrypt_nonce_.data(), crypto_secretbox_NONCEBYTES);

    std::string decrypted_buffer;
    decrypted_buffer.resize(source_buffer.size() - crypto_secretbox_MACBYTES);

    // Decrypt message.
    if (crypto_secretbox_open_easy(reinterpret_cast<uint8_t*>(decrypted_buffer.data()),
                                   reinterpret_cast<const uint8_t*>(source_buffer.data()),
                                   source_buffer.size(),
                                   decrypt_nonce_.data(),
                                   decrypt_key_.data()) != 0)
    {
        LOG_WARN(logger, "crypto_secretbox_open_easy failed");
        return std::string();
    }

    return decrypted_buffer;
}

} // namespace aspia
