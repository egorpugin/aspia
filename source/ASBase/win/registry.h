//
// PROJECT:         Aspia
// FILE:            ASBase/win/registry.h
// LICENSE:         Mozilla Public License Version 2.0
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
// NOTE:            This file based on Chromium code.
//

#pragma once

#include "base/common.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>

namespace aspia {

class RegistryKey
{
public:
    RegistryKey() = default;
    explicit RegistryKey(HKEY key);
    RegistryKey(HKEY rootkey, const wchar_t* subkey, REGSAM access);

    ~RegistryKey();

    // True while the key is valid.
    bool isValid() const;

    LONG create(HKEY rootkey, const wchar_t* subkey, REGSAM access);

    LONG createWithDisposition(HKEY rootkey, const wchar_t* subkey,
                               DWORD *disposition, REGSAM access);

    // Opens an existing reg key.
    LONG open(HKEY rootkey, const wchar_t* subkey, REGSAM access);

    //
    // Returns false if this key does not have the specified value, or if an error
    // occurrs while attempting to access it.
    //
    bool hasValue(const wchar_t* name) const;

    //
    // Reads raw data into |data|. If |name| is null or empty, reads the key's
    // default value, if any.
    //
    LONG readValue(const wchar_t* name, void* data, DWORD* dsize, DWORD* dtype) const;

    //
    // Reads a REG_DWORD (uint32_t) into |out_value|. If |name| is null or empty,
    // reads the key's default value, if any.
    //
    LONG readValueDW(const wchar_t* name, DWORD* out_value) const;

    //
    // Reads a REG_BINARY (array of chars) into |out_value|. If |name| is null or empty,
    // reads the key's default value, if any.
    //
    LONG readValueBIN(const wchar_t* name, std::string* out_value) const;

    //
    // Reads a string into |out_value|. If |name| is null or empty, reads
    // the key's default value, if any.
    //
    LONG readValue(const wchar_t* name, std::wstring* out_value) const;

    // Sets raw data, including type.
    LONG writeValue(const wchar_t* name, const void* data, DWORD dsize, DWORD dtype);

    // Sets an int32_t value.
    LONG writeValue(const wchar_t* name, DWORD in_value);

    // Sets a string value.
    LONG writeValue(const wchar_t* name, const wchar_t* in_value);

    // Closes this reg key.
    void close();

private:
    HKEY key_ = nullptr;
    REGSAM wow64access_ = 0;

    DISABLE_COPY(RegistryKey)
};

// Iterates the entries found in a particular folder on the registry.
class RegistryValueIterator
{
public:
    // Constructs a Registry Value Iterator with default WOW64 access.
    RegistryValueIterator(HKEY root_key, const wchar_t* folder_key);

    // Constructs a Registry Key Iterator with specific WOW64 access, one of
    // KEY_WOW64_32KEY or KEY_WOW64_64KEY, or 0.
    // Note: |wow64access| should be the same access used to open |root_key|
    // previously, or a predefined key (e.g. HKEY_LOCAL_MACHINE).
    // See http://msdn.microsoft.com/en-us/library/windows/desktop/aa384129.aspx.
    RegistryValueIterator(HKEY root_key,
                          const wchar_t* folder_key,
                          REGSAM wow64access);

    ~RegistryValueIterator();

    DWORD valueCount() const;

    // True while the iterator is valid.
    bool valid() const;

    // Advances to the next registry entry.
    void operator++();

    const wchar_t* name() const { return name_.c_str(); }
    const wchar_t* value() const { return value_.data(); }
    // ValueSize() is in bytes.
    DWORD valueSize() const { return value_size_; }
    DWORD type() const { return type_; }

    int index() const { return index_; }

private:
    // Reads in the current values.
    bool read();

    void initialize(HKEY root_key, const wchar_t* folder_key, REGSAM wow64access);

    // The registry key being iterated.
    HKEY key_ = nullptr;

    // Current index of the iteration.
    int index_;

    // Current values.
    std::wstring name_;
    std::vector<wchar_t> value_;
    DWORD value_size_;
    DWORD type_;

    DISABLE_COPY(RegistryValueIterator)
};

class RegistryKeyIterator
{
public:
    // Constructs a Registry Key Iterator with default WOW64 access.
    RegistryKeyIterator(HKEY root_key, const wchar_t* folder_key);

    // Constructs a Registry Value Iterator with specific WOW64 access, one of
    // KEY_WOW64_32KEY or KEY_WOW64_64KEY, or 0.
    // Note: |wow64access| should be the same access used to open |root_key|
    // previously, or a predefined key (e.g. HKEY_LOCAL_MACHINE).
    // See http://msdn.microsoft.com/en-us/library/windows/desktop/aa384129.aspx.
    RegistryKeyIterator(HKEY root_key, const wchar_t* folder_key, REGSAM wow64access);

    ~RegistryKeyIterator();

    DWORD subkeyCount() const;

    // True while the iterator is valid.
    bool valid() const;

    // Advances to the next entry in the folder.
    void operator++();

    const wchar_t* name() const { return name_; }

    int index() const { return index_; }

private:
    // Reads in the current values.
    bool read();

    void initialize(HKEY root_key, const wchar_t* folder_key, REGSAM wow64access);

    // The registry key being iterated.
    HKEY key_ = nullptr;

    // Current index of the iteration.
    int index_;

    wchar_t name_[MAX_PATH];

    DISABLE_COPY(RegistryKeyIterator)
};

} // namespace aspia
