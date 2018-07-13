//
// PROJECT:         Aspia
// FILE:            system_info/ui/dmi_parser.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_SYSTEM_INFO__UI__DMI_PARSER_H
#define _ASPIA_SYSTEM_INFO__UI__DMI_PARSER_H

#include "base/common.h"

#include "system_info/parser/parser.h"
#include "system_info/protocol/dmi.pb.h"

namespace aspia {

class DmiParser : public Parser
{
    Q_OBJECT

public:
    static Parser* create(QObject* parent, const std::string& uuid);

    // Parser implementation.
    QJsonObject toJson() override;
    QDomElement toXml() override;
    QDomElement toHtml() override;
    std::string toText() override;

public slots:
    // Parser implementation.
    void updateData() const override;
    void readReply(const std::string& uuid, const QByteArray& data) override;

protected:
    DmiParser(QObject* parent, const std::string& uuid);

private:
    std::unique_ptr<system_info::dmi::Dmi> dmi_;

    DISABLE_COPY(DmiParser)
};

} // namespace aspia

#endif // _ASPIA_SYSTEM_INFO__UI__DMI_PARSER_H
