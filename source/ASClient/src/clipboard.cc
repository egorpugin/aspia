//
// PROJECT:         Aspia
// FILE:            ASBase/clipboard.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "client/clipboard.h"

#include <QClipboard>
#include <QDebug>
#include <QGuiApplication>
#include <QMimeData>

namespace aspia {

namespace {

const char kMimeTypeTextUtf8[] = "text/plain; charset=UTF-8";

} // namespace

Clipboard::Clipboard(QObject* parent)
    : QObject(parent)
{
    connect(QGuiApplication::clipboard(), &QClipboard::dataChanged,
            this, &Clipboard::dataChanged);
}

Clipboard::~Clipboard()
{
    disconnect(QGuiApplication::clipboard(), &QClipboard::dataChanged,
               this, &Clipboard::dataChanged);
}

void Clipboard::injectClipboardEvent(const proto::desktop::ClipboardEvent& event)
{
    if (event.mime_type() != kMimeTypeTextUtf8)
        return;

    last_mime_type_ = event.mime_type();
    last_data_ = event.data();

    QString text = event.data().c_str();

#if defined(_WIN32)
    text.replace(QLatin1String("\n"), QLatin1String("\r\n"));
#endif

    QGuiApplication::clipboard()->setText(text);
}

void Clipboard::dataChanged()
{
    const QMimeData* mime_data = QGuiApplication::clipboard()->mimeData();
    if (!mime_data->hasText())
        return;

    proto::desktop::ClipboardEvent event;
    event.set_mime_type(kMimeTypeTextUtf8);
    event.set_data(mime_data->text().replace(
        QLatin1String("\r\n"), QLatin1String("\n")).toStdString());

    if (event.mime_type() == last_mime_type_ && event.data() == last_data_)
        return;

    emit clipboardEvent(event);
}

} // namespace aspia
