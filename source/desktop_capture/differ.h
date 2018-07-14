//
// PROJECT:         Aspia
// FILE:            desktop_capture/differ.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

#include <qregion.h>

#include <memory>

namespace aspia {

// Class to search for changed regions of the screen.
class Differ
{
public:
    explicit Differ(const QSize& size);
    ~Differ() = default;

    void calcDirtyRegion(const uint8_t* prev_image,
                         const uint8_t* curr_image,
                         QRegion* changed_region);

private:
    void markDirtyBlocks(const uint8_t* prev_image, const uint8_t* curr_image);
    void mergeBlocks(QRegion* dirty_region);

    const QRect screen_rect_;

    const int bytes_per_row_;

    const int full_blocks_x_;
    const int full_blocks_y_;

    int partial_column_width_;
    int partial_row_height_;

    int block_stride_y_;

    const int diff_width_;
    const int diff_height_;

    std::unique_ptr<uint8_t[]> diff_info_;

    typedef uint8_t(*DiffFullBlockFunc)(const uint8_t*, const uint8_t*, int);
    DiffFullBlockFunc diff_full_block_func_;

    DISABLE_COPY(Differ)
};

} // namespace aspia
