//
// PROJECT:         Aspia
// FILE:            system_info/ui/parser.h
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#ifndef _ASPIA_SYSTEM_INFO__UI__PARSER_H
#define _ASPIA_SYSTEM_INFO__UI__PARSER_H

#include <QIcon>
#include <QObject>
#include <QJsonObject>
#include <QtXml>

namespace aspia {

class SystemInfoRequest;

class Parser : public QObject
{
public:
    virtual ~Parser() = default;

    virtual QJsonObject toJson() = 0;
    virtual QDomElement toXml() = 0;
    virtual QDomElement toHtml() = 0;
    virtual std::string toText() = 0;

    std::string uuid() const { return uuid_; }

public slots:
    virtual void updateData() const = 0;
    virtual void readReply(const std::string& uuid, const QByteArray& data) = 0;

signals:
    void sendRequest(SystemInfoRequest* request);
    void dataUpdated();

protected:
    Parser(QObject* parent, const std::string& uuid)
        : QObject(parent),
          uuid_(uuid)
    {
        // Nothing
    }

private:
    std::string uuid_;
};

} // namespace aspia

#endif // _ASPIA_SYSTEM_INFO__UI__PARSER_H
