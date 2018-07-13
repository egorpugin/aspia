//
// PROJECT:         Aspia
// FILE:            crypto/random.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_CRYPTO__RANDOM_H
#define _ASPIA_CRYPTO__RANDOM_H

#include "base/common.h"

namespace aspia {

class ASPIA_CRYPTO_API Random
{
public:
    static std::string generateBuffer(int size);
    static uint32_t generateNumber();

private:
    DISABLE_COPY(Random)
};

} // namespace aspia

#endif // _ASPIA_CRYPTO__RANDOM_H
