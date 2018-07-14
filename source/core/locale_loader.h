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

#include "base/common.h"

#include <qhash.h>
#include <qstring.h>

class QTranslator;

namespace aspia {

class ASPIA_CORE_API LocaleLoader
{
public:
    LocaleLoader();
    ~LocaleLoader();

    QStringList localeList() const;
    QStringList sortedLocaleList() const;
    QStringList fileList(const std::string& locale_name) const;
    bool contains(const std::string& locale_name) const;
    void installTranslators(const std::string& locale_name);

    static QString translationsDir();

private:
    void removeTranslators();

    QHash<QString, QStringList> locale_list_;
    QList<QTranslator*> translator_list_;

    Q_DISABLE_COPY(LocaleLoader)
};

} // namespace aspia
