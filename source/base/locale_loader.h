//
// PROJECT:         Aspia
// FILE:            base/locale_loader.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#pragma once

#include "base/common.h"

#include <QHash>
#include <QStringList>

class QTranslator;

namespace aspia {

class ASPIA_BASE_API LocaleLoader
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

    DISABLE_COPY(LocaleLoader)
};

} // namespace aspia
