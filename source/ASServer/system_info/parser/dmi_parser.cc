//
// PROJECT:         Aspia
// FILE:            system_info/parser/dmi_parser.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "system_info/parser/dmi_parser.h"

namespace aspia {

DmiParser::DmiParser(QObject* parent, const std::string& uuid)
    : Parser(parent, uuid)
{
    // Nothing
}

// static
Parser* DmiParser::create(QObject* parent, const std::string& uuid)
{
    return new DmiParser(parent, uuid);
}

QJsonObject DmiParser::toJson()
{
    // TODO
    return QJsonObject();
}

QDomElement DmiParser::toXml()
{
    // TODO
    return QDomElement();
}

QDomElement DmiParser::toHtml()
{
    // TODO
    return QDomElement();
}

std::string DmiParser::toText()
{
    // TODO
    return std::string();
}

void DmiParser::updateData() const
{
    // TODO
}

void DmiParser::readReply(const std::string& uuid, const QByteArray& data)
{
    // TODO
}

} // namespace aspia
