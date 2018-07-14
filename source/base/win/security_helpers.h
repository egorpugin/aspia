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

#pragma once

namespace aspia {

// Initializes COM security of the process applying the passed security
// descriptor.  The function configures the following settings:
//  - Server authenticates that all data received is from the expected client.
//  - Server can impersonate clients to check their identity but cannot act on
//    their behalf.
//  - Caller's identity is verified on every call (Dynamic cloaking).
//  - Unless |activate_as_activator| is true, activations where the server
//    would run under this process's identity are prohibited.
ASPIA_BASE_API
bool initializeComSecurity(const wchar_t* security_descriptor,
                           const wchar_t* mandatory_label,
                           bool activate_as_activator);

} // namespace aspia
