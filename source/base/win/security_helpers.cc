/*
 * Aspia: Remote desktop and file transfer tool.
 * Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "base/win/security_helpers.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objidl.h>
#include <sddl.h>

#include <string>

#include "base/win/scoped_local.h"
#include "base/errno_logging.h"
#include "base/typed_buffer.h"

namespace aspia {

namespace {

using ScopedAcl = TypedBuffer<ACL>;
using ScopedSd = TypedBuffer<SECURITY_DESCRIPTOR>;
using ScopedSid = TypedBuffer<SID>;

bool makeScopedAbsoluteSd(const ScopedSd& relative_sd,
                          ScopedSd* absolute_sd,
                          ScopedAcl* dacl,
                          ScopedSid* group,
                          ScopedSid* owner,
                          ScopedAcl* sacl)
{
    // Get buffer sizes.
    DWORD absolute_sd_size = 0;
    DWORD dacl_size = 0;
    DWORD group_size = 0;
    DWORD owner_size = 0;
    DWORD sacl_size = 0;

    if (MakeAbsoluteSD(relative_sd.get(),
                       nullptr,
                       &absolute_sd_size,
                       nullptr,
                       &dacl_size,
                       nullptr,
                       &sacl_size,
                       nullptr,
                       &owner_size,
                       nullptr,
                       &group_size) ||
        GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        qWarningErrno("MakeAbsoluteSD failed");
        return false;
    }

    // Allocate buffers.
    ScopedSd local_absolute_sd(absolute_sd_size);
    ScopedAcl local_dacl(dacl_size);
    ScopedSid local_group(group_size);
    ScopedSid local_owner(owner_size);
    ScopedAcl local_sacl(sacl_size);

    // Do the conversion.
    if (!MakeAbsoluteSD(relative_sd.get(),
                        local_absolute_sd.get(),
                        &absolute_sd_size,
                        reinterpret_cast<PACL>(local_dacl.get()),
                        &dacl_size,
                        local_sacl.get(),
                        &sacl_size,
                        local_owner.get(),
                        &owner_size,
                        local_group.get(),
                        &group_size))
    {
        qWarningErrno("MakeAbsoluteSD failed");
        return false;
    }

    absolute_sd->swap(local_absolute_sd);
    dacl->swap(local_dacl);
    group->swap(local_group);
    owner->swap(local_owner);
    sacl->swap(local_sacl);

    return true;
}

ScopedSd convertSddlToSd(const std::wstring& sddl)
{
    ScopedLocal<PSECURITY_DESCRIPTOR> raw_sd;
    ULONG length = 0;

    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(), SDDL_REVISION_1,
                                                              raw_sd.recieve(), &length))
    {
        qWarningErrno("ConvertStringSecurityDescriptorToSecurityDescriptorW failed");
        return ScopedSd();
    }

    ScopedSd sd(length);
    memcpy(sd.get(), raw_sd, length);

    return sd;
}

} // namespace

bool initializeComSecurity(const wchar_t* security_descriptor,
                           const wchar_t* mandatory_label,
                           bool activate_as_activator)
{
    std::wstring sddl;

    sddl.append(security_descriptor);
    sddl.append(mandatory_label);

    // Convert the SDDL description into a security descriptor in absolute format.
    ScopedSd relative_sd = convertSddlToSd(sddl);
    if (!relative_sd)
    {
        LOG_WARN(logger, "Failed to create a security descriptor");
        return false;
    }

    ScopedSd absolute_sd;
    ScopedAcl dacl;
    ScopedSid group;
    ScopedSid owner;
    ScopedAcl sacl;

    if (!makeScopedAbsoluteSd(relative_sd, &absolute_sd, &dacl,
                              &group, &owner, &sacl))
    {
        LOG_WARN(logger, "MakeScopedAbsoluteSd failed");
        return false;
    }

    DWORD capabilities = EOAC_DYNAMIC_CLOAKING;
    if (!activate_as_activator)
        capabilities |= EOAC_DISABLE_AAA;

    // Apply the security descriptor and default security settings. See
    // InitializeComSecurity's declaration for details.
    HRESULT result = CoInitializeSecurity(
        absolute_sd.get(),
        -1,        // Let COM choose which authentication services to register.
        nullptr,   // See above.
        nullptr,   // Reserved, must be nullptr.
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IDENTIFY,
        nullptr,   // Default authentication information is not provided.
        capabilities,
        nullptr);  // Reserved, must be nullptr
    if (FAILED(result))
    {
        LOG_WARN(logger, "") << "CoInitializeSecurity failed: " << result;
        return false;
    }

    return true;
}

} // namespace aspia
