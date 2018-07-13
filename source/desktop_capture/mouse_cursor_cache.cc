//
// PROJECT:         Aspia
// FILE:            desktop_capture/mouse_cursor_cache.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "desktop_capture/mouse_cursor_cache.h"

#include "base/log.h"

namespace aspia {

namespace {

constexpr size_t kMinCacheSize = 2;
constexpr size_t kMaxCacheSize = 31;

} // namespace

MouseCursorCache::MouseCursorCache(size_t cache_size) :
    cache_size_(cache_size)
{
    // Nothing
}

size_t MouseCursorCache::find(const MouseCursor* mouse_cursor)
{
    assert(mouse_cursor);

    size_t size = cache_.size();

    for (size_t index = 0; index < size; ++index)
    {
        // If the cursor is found in the cache.
        if (cache_.at(index)->isEqual(*mouse_cursor))
        {
            // Return its index.
            return index;
        }
    }

    return kInvalidIndex;
}

size_t MouseCursorCache::add(std::unique_ptr<MouseCursor> mouse_cursor)
{
    assert(mouse_cursor);

    // Add the cursor to the end of the list.
    cache_.emplace_back(std::move(mouse_cursor));

    // If the current cache size exceeds the maximum cache size.
    if (cache_.size() > cache_size_)
    {
        // Delete the first element in the cache (the oldest one).
        cache_.pop_front();
    }

    return cache_.size() - 1;
}

std::shared_ptr<MouseCursor> MouseCursorCache::Get(size_t index)
{
    if (index > kMaxCacheSize)
    {
        LOG_DEBUG(logger, "") << "Invalid cache index: " << index;
        return nullptr;
    }

    return cache_.at(index);
}

bool MouseCursorCache::isEmpty() const
{
    return cache_.empty();
}

void MouseCursorCache::clear()
{
    cache_.clear();
}

// static
bool MouseCursorCache::isValidCacheSize(size_t size)
{
    if (size < kMinCacheSize || size > kMaxCacheSize)
        return false;

    return true;
}

} // namespace aspia
