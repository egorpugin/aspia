//
// PROJECT:         Aspia
// FILE:            base/typed_buffer.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_BASE__TYPED_BUFFER_H
#define _ASPIA_BASE__TYPED_BUFFER_H

namespace aspia {

//
// A scoper for a variable-length structure such as SID, SECURITY_DESCRIPTOR and
// similar. These structures typically consist of a header followed by variable-
// length data, so the size may not match sizeof(T). The class supports
// move-only semantics and typed buffer getters.
//
template <typename T>
class TypedBuffer
{
public:
    TypedBuffer()
        : TypedBuffer(0)
    {
        // Nothing
    }

    // Creates an instance of the object allocating a buffer of the given size.
    explicit TypedBuffer(size_t length)
        : length_(length)
    {
        if (length_ != 0)
            buffer_ = reinterpret_cast<T*>(new uint8_t[length_]);
    }

    TypedBuffer(TypedBuffer&& rvalue) noexcept
        : TypedBuffer()
    {
        swap(rvalue);
    }

    ~TypedBuffer()
    {
        if (buffer_)
        {
            delete[] reinterpret_cast<uint8_t*>(buffer_);
            buffer_ = nullptr;
        }
    }

    TypedBuffer& operator=(TypedBuffer&& rvalue) noexcept
    {
        swap(rvalue);
        return *this;
    }

    // Accessors to get the owned buffer.
    // operator* and operator-> will assert() if there is no current buffer.
    T& operator*() const
    {
        assert(buffer_ != nullptr);
        return *buffer_;
    }

    T* operator->() const
    {
        assert(buffer_ != nullptr);
        return buffer_;
    }

    T* get() const { return buffer_; }
    size_t length() const { return length_; }

    // Helper returning a pointer to the structure starting at a specified byte
    // offset.
    T* getAtOffset(uint32_t offset)
    {
        return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(buffer_) + offset);
    }

    // Allow TypedBuffer<T> to be used in boolean expressions.
    explicit operator bool() const { return buffer_ != nullptr; }

    // Swap two buffers.
    void swap(TypedBuffer& other)
    {
        std::swap(buffer_, other.buffer_);
        std::swap(length_, other.length_);
    }

private:
    // Points to the owned buffer.
    T* buffer_ = nullptr;

    // Length of the owned buffer in bytes.
    size_t length_;

    DISABLE_COPY(TypedBuffer)
};

} // namespace aspia

#endif // _ASPIA_BASE__TYPED_BUFFER_H
