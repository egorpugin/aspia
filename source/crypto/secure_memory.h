//
// PROJECT:         Aspia
// FILE:            crypto/secure_memory.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CRYPTO__SECURE_MEMORY_H
#define _ASPIA_CRYPTO__SECURE_MEMORY_H

#include "base/common.h"

namespace aspia {

ASPIA_CRYPTO_API
void secureMemZero(void* data, size_t data_size);

ASPIA_CRYPTO_API
void secureMemZero(std::string* str);

} // namespace aspia

#endif // _ASPIA_CRYPTO__SECURE_MEMORY_H
