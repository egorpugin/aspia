//
// PROJECT:         Aspia
// FILE:            system_info/serializer/dmi_impl.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_BASE__HARDWARE__DMI_H
#define _ASPIA_BASE__HARDWARE__DMI_H

#include "base/common.h"

#include "base/common.h"

#include <memory>

namespace aspia {

class DmiTable;

class DmiTableEnumerator
{
public:
    DmiTableEnumerator();

    static const size_t kMaxDataSize = 0xFA00; // 64K

    struct SmBiosData
    {
        uint8_t used_20_calling_method;
        uint8_t smbios_major_version;
        uint8_t smbios_minor_version;
        uint8_t dmi_revision;
        uint32_t length;
        uint8_t smbios_table_data[kMaxDataSize];
    };

    bool isAtEnd() const { return !current_; }
    void advance();
    const DmiTable* table() const;

    uint8_t majorVersion() const { return data_.smbios_major_version; }
    uint8_t minorVersion() const { return data_.smbios_minor_version; }

private:
    SmBiosData data_;

    const uint8_t* current_ = nullptr;
    const uint8_t* next_ = nullptr;

    mutable std::unique_ptr<DmiTable> current_table_;

    DISABLE_COPY(DmiTableEnumerator)
};

class DmiTable
{
public:
    enum Type : uint8_t
    {
        TYPE_BIOS             = 0x00,
        TYPE_SYSTEM           = 0x01,
        TYPE_BASEBOARD        = 0x02,
        TYPE_CHASSIS          = 0x03,
        TYPE_PROCESSOR        = 0x04,
        TYPE_CACHE            = 0x07,
        TYPE_PORT_CONNECTOR   = 0x08,
        TYPE_SYSTEM_SLOT      = 0x09,
        TYPE_ONBOARD_DEVICE   = 0x0A,
        TYPE_MEMORY_DEVICE    = 0x11,
        TYPE_POINTING_DEVICE  = 0x15,
        TYPE_PORTABLE_BATTERY = 0x16
    };

    Type type() const { return static_cast<Type>(table_[0]); }
    uint8_t length() const { return table_[1]; }

protected:
    explicit DmiTable(const uint8_t* table);

    template<typename T>
    T number(uint8_t offset) const
    {
        assert(offset >= length());
        return *reinterpret_cast<const T*>(table_[offset]);
    }

    std::string string(uint8_t offset) const;

private:
    const uint8_t* table_;
};

class DmiBiosTable : public DmiTable
{
public:
    std::string manufacturer() const;
    std::string version() const;
    std::string date() const;
    uint64_t biosSize() const;
    std::string biosRevision() const;
    std::string firmwareRevision() const;
    std::string address() const;
    uint64_t runtimeSize() const;

    struct Characteristics
    {
        bool isa;
        bool mca;
        bool eisa;
        bool pci;
        bool pc_card;
        bool pnp;
        bool apm;
        bool vlb;
        bool escd;
        bool boot_from_cd;
        bool selectable_boot;
        bool socketed_boot_rom;
        bool boot_from_pc_card;
        bool edd;
        bool japanese_floppy_for_nec9800;
        bool japanese_floppy_for_toshiba;
        bool floppy_525_360kb;
        bool floppy_525_12mb;
        bool floppy_35_720kb;
        bool floppy_35_288mb;
        bool print_screen;
        bool keyboard_8042;
        bool serial;
        bool printer;
        bool cga_video;
        bool nec_pc98;
        bool acpi;
        bool usb_legacy;
        bool agp;
        bool i2o_boot;
        bool ls120_boot;
        bool atapi_zip_drive_boot;
        bool ieee1394_boot;
        bool smart_battery;
        bool bios_boot_specification;
        bool key_init_network_boot;
        bool targeted_content_distrib;
        bool uefi;
        bool virtual_machine;
        bool bios_upgradeable;
        bool bios_shadowing;
    };

    void characteristics(Characteristics* result) const;

private:
    friend class DmiTableEnumerator;
    DmiBiosTable(const uint8_t* table);
};

} // namespace aspia

#endif // _ASPIA_BASE__HARDWARE__DMI_H
