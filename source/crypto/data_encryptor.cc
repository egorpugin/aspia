//
// PROJECT:         Aspia
// FILE:            crypto/data_encryptor.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "base/log.h"
#include "crypto/data_encryptor.h"

extern "C" {
#pragma warning(push, 3)
#include <sodium.h>
#pragma warning(pop)
} // extern "C"

namespace aspia {

namespace {

const size_t kChunkSize = 4096;

} // namespace

// static
std::string DataEncryptor::createKey(const std::string& password,
                                    const std::string& salt,
                                    int rounds)
{
    std::string data = password;

    for (int i = 0; i < rounds; ++i)
    {
        auto d = data + salt;
        data.resize(32);
        crypto_hash_sha256((uint8_t*)data.data(), (uint8_t*)d.data(), d.size());
    }

    return data;
}

// static
std::string DataEncryptor::encrypt(const std::string& source_data, const std::string& key)
{
    assert(key.size() == crypto_secretstream_xchacha20poly1305_KEYBYTES);

    std::string encrypted_data;
    encrypted_data.resize(crypto_secretstream_xchacha20poly1305_HEADERBYTES);

    crypto_secretstream_xchacha20poly1305_state state;

    crypto_secretstream_xchacha20poly1305_init_push(
        &state,
        reinterpret_cast<uint8_t*>(encrypted_data.data()),
        reinterpret_cast<const uint8_t*>(key.data()));

    const uint8_t* input_buffer = reinterpret_cast<const uint8_t*>(source_data.data());
    size_t input_pos = 0;

    bool end_of_buffer = false;

    do
    {
        size_t consumed = std::min(source_data.size() - input_pos, kChunkSize);
        uint8_t tag = 0;

        if (consumed < kChunkSize)
        {
            tag = crypto_secretstream_xchacha20poly1305_TAG_FINAL;
            end_of_buffer = true;
        }

        uint8_t output_buffer[kChunkSize + crypto_secretstream_xchacha20poly1305_ABYTES];
        uint64_t output_length;

        crypto_secretstream_xchacha20poly1305_push(&state,
                                                   output_buffer, &output_length,
                                                   input_buffer + input_pos, consumed,
                                                   nullptr, 0,
                                                   tag);

        size_t old_size = encrypted_data.size();

        encrypted_data.resize(old_size + static_cast<size_t>(output_length));
        memcpy(encrypted_data.data() + old_size, output_buffer, static_cast<size_t>(output_length));

        input_pos += consumed;

    } while (!end_of_buffer);

    return encrypted_data;
}

// static
bool DataEncryptor::decrypt(const std::string& source_data,
                            const std::string& key,
                            std::string* decrypted_data)
{
    return decrypt(source_data.data(), source_data.size(), key, decrypted_data);
}

// static
bool DataEncryptor::decrypt(const char* source_data, int source_size, const std::string& key,
                            std::string* decrypted_data)
{
    if (!source_data || source_size < crypto_secretstream_xchacha20poly1305_HEADERBYTES ||
        !decrypted_data)
    {
        LOG_WARN(logger, "Invalid parameters");
        return false;
    }

    if (key.size() != crypto_secretstream_xchacha20poly1305_KEYBYTES)
    {
        LOG_WARN(logger, "Invalid key size");
        return false;
    }

    decrypted_data->clear();

    crypto_secretstream_xchacha20poly1305_state state;

    if (crypto_secretstream_xchacha20poly1305_init_pull(
            &state, reinterpret_cast<const uint8_t*>(source_data),
            reinterpret_cast<const uint8_t*>(key.data())) != 0)
    {
        LOG_WARN(logger, "crypto_secretstream_xchacha20poly1305_init_pull failed");
        return false;
    }

    const uint8_t* input_buffer = reinterpret_cast<const uint8_t*>(source_data) +
                                 crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    size_t input_size = source_size - crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    size_t input_pos = 0;

    bool end_of_buffer = false;

    do
    {
        size_t consumed = std::min(input_size - input_pos,
                                   kChunkSize + crypto_secretstream_xchacha20poly1305_ABYTES);

        uint8_t output_buffer[kChunkSize];
        uint64_t output_length;
        uint8_t tag;

        if (crypto_secretstream_xchacha20poly1305_pull(&state, output_buffer, &output_length, &tag,
                                                       input_buffer + input_pos, consumed, nullptr,
                                                       0) != 0)
        {
            LOG_WARN(logger, "crypto_secretstream_xchacha20poly1305_pull failed");
            return false;
        }

        input_pos += consumed;

        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL)
        {
            if (input_pos != input_size)
            {
                LOG_WARN(logger, "Unexpected end of buffer");
                return false;
            }

            end_of_buffer = true;
        }

        size_t old_size = decrypted_data->size();

        decrypted_data->resize(old_size + static_cast<size_t>(output_length));
        memcpy(decrypted_data->data() + old_size, output_buffer,
               static_cast<size_t>(output_length));

    } while (!end_of_buffer);

    return true;
}

} // namespace aspia
