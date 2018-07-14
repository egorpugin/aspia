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

#include "client_user_authorizer.h"

#include "base/message_serialization.h"
#include "ui/authorization_dialog.h"
#include "crypto/data_encryptor.h"
#include "crypto/secure_memory.h"

namespace aspia {

namespace {

const quint32 kKeyHashingRounds = 100000;
const quint32 kPasswordHashingRounds = 100000;

enum MessageId { LogonRequest, ClientChallenge };

std::string createPasswordHash(const std::string& password)
{
    auto data = password;
    for (int i = 0; i < kPasswordHashingRounds; ++i)
        data = sha512(data);
    return data;
}

std::string createSessionKey(const std::string& password_hash, const std::string& nonce)
{
    auto data = password_hash;
    for (quint32 i = 0; i < kKeyHashingRounds; ++i)
        data = sha512(data + nonce);
    return data;
}

} // namespace

ClientUserAuthorizer::ClientUserAuthorizer(QWidget* parent)
    : QObject(parent)
{
    // Nothing
}

ClientUserAuthorizer::~ClientUserAuthorizer()
{
    secureMemZero(&username_);
    secureMemZero(&password_);

    cancel();
}

void ClientUserAuthorizer::setSessionType(proto::auth::SessionType session_type)
{
    session_type_ = session_type;
}

void ClientUserAuthorizer::setUserName(const QString& username)
{
    username_ = username.toStdString();
}

void ClientUserAuthorizer::setPassword(const QString& password)
{
    password_ = password.toStdString();
}

void ClientUserAuthorizer::start()
{
    if (state_ != NotStarted)
    {
        LOG_WARN(logger, "Authorizer already started");
        return;
    }

    proto::auth::ClientToHost message;

    // We do not support other authorization methods yet.
    message.mutable_logon_request()->set_method(proto::auth::METHOD_BASIC);

    state_ = Started;
    emit writeMessage(LogonRequest, serializeMessage(message));
}

void ClientUserAuthorizer::cancel()
{
    if (state_ == Finished)
        return;

    state_ = Finished;
    emit finished(proto::auth::STATUS_CANCELED);
}

void ClientUserAuthorizer::messageWritten(int message_id)
{
    if (state_ == Finished)
        return;

    emit readMessage();
}

void ClientUserAuthorizer::messageReceived(const std::string& buffer)
{
    if (state_ == Finished)
        return;

    proto::auth::HostToClient message;

    if (parseMessage(buffer, message))
    {
        if (message.has_server_challenge())
        {
            readServerChallenge(message.server_challenge());
            return;
        }
        else if (message.has_logon_result())
        {
            readLogonResult(message.logon_result());
            return;
        }
    }

    emit errorOccurred(tr("Protocol error: Unknown message from host."));
    cancel();
}

void ClientUserAuthorizer::readServerChallenge(
    const proto::auth::ServerChallenge& server_challenge)
{
    if (server_challenge.nonce().empty())
    {
        emit errorOccurred(tr("Authorization error: Empty nonce is not allowed."));
        cancel();
        return;
    }

    if (username_.empty() || password_.empty())
    {
        AuthorizationDialog dialog(dynamic_cast<QWidget*>(parent()));

        dialog.setUserName(username_.c_str());
        dialog.setPassword(password_.c_str());

        if (dialog.exec() == AuthorizationDialog::Rejected)
        {
            emit errorOccurred(tr("Authorization is canceled by the user."));
            cancel();
            return;
        }

        username_ = dialog.userName().toStdString();
        password_ = dialog.password().toStdString();
    }

    auto session_key = createSessionKey(createPasswordHash(password_), server_challenge.nonce());

    proto::auth::ClientToHost message;

    proto::auth::ClientChallenge* client_challenge = message.mutable_client_challenge();
    client_challenge->set_session_type(session_type_);
    client_challenge->set_username(username_);
    client_challenge->set_session_key(session_key);

    secureMemZero(&session_key);

    auto serialized_message = serializeMessage(message);

    secureMemZero(client_challenge->mutable_username());
    secureMemZero(client_challenge->mutable_session_key());

    emit writeMessage(ClientChallenge, serialized_message);

    secureMemZero(&serialized_message);
}

void ClientUserAuthorizer::readLogonResult(const proto::auth::LogonResult& logon_result)
{
    state_ = Finished;
    emit finished(logon_result.status());
}

} // namespace aspia
