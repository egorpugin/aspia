//
// PROJECT:         Aspia
// FILE:            desktop_capture/mouse_cursor_cache.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include <deque>

#include "desktop_capture/mouse_cursor.h"

namespace aspia {

class ASPIA_DESKTOP_CAPTURE_API MouseCursorCache
{
public:
    explicit MouseCursorCache(size_t cache_size);
    ~MouseCursorCache() = default;

    static const size_t kInvalidIndex = std::numeric_limits<size_t>::max();

    // Looks for a matching cursor in the cache.
    // If the cursor is already in the cache, the cursor index in the cache is
    // returned.
    // If the cursor is not in the cache, -1 is returned.
    size_t find(const MouseCursor* mouse_cursor);

    // Adds the cursor to the cache and returns the index of the added element.
    size_t add(std::unique_ptr<MouseCursor> mouse_cursor);

    // Returns the pointer to the cached cursor by its index in the cache.
    std::shared_ptr<MouseCursor> Get(size_t index);

    // Checks an empty cache or not.
    bool isEmpty() const;

    // Clears the cache.
    void clear();

    // The current size of the cache.
    size_t size() const { return cache_size_; }

    static bool isValidCacheSize(size_t size);

private:
    std::deque<std::shared_ptr<MouseCursor>> cache_;
    const size_t cache_size_;
};

} // namespace aspia
