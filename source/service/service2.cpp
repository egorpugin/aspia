#include "service2.h"

#include "host.h"
#include "host_user_authorizer.h"

#include "base/errno_logging.h"
#include "base/log.h"
#include "base/message_serialization.h"
#include "core/user.h"
#include "crypto/secure_memory.h"
#include "crypto/data_encryptor.h"
#include "crypto/random.h"
#include "ipc/ipc_server.h"

//#include <boost/uuid.hpp>
#include <quuid.h>
#include <boost/dll.hpp>

#define STOP_SESSION_NO_RETURN_ARG(a)   do { (a).stop(); } while (0)
#define STOP_SESSION_ARG(a)             do { STOP_SESSION_NO_RETURN_ARG(a); return; } while (0)
#define STOP_SESSION_NO_RETURN          STOP_SESSION_NO_RETURN_ARG(*this)
#define STOP_SESSION                    STOP_SESSION_ARG(*this)

namespace aspia
{

extern const char *kNotifierFileName;

const char* sessionTypeToString(proto::auth::SessionType session_type);

service &get_service()
{
    static service svc;
    return svc;
}

namespace
{

static constexpr inline uint32_t kMaxMessageSize = 16 * 1024 * 1024; // 16MB
static constexpr inline int64_t kMaxWriteSize = 1200;

static constexpr inline uint32_t kKeyHashingRounds = 100000;
static constexpr inline uint32_t kNonceSize = 16;

std::string generateNonce()
{
    return Random::generateBuffer(kNonceSize);
}

std::string createSessionKey(const std::string& password_hash, const std::string& nonce)
{
    auto data = password_hash;
    for (uint32_t i = 0; i < kKeyHashingRounds; ++i)
        data = sha512(data + nonce);
    return data;
}

std::string createWriteBuffer(const std::string& message_buffer)
{
    uint32_t message_size = message_buffer.size();

    uint8_t buffer[4];
    size_t length = 1;

    buffer[0] = message_size & 0x7F;
    if (message_size > 0x7F) // 127 bytes
    {
        buffer[0] |= 0x80;
        buffer[length++] = message_size >> 7 & 0x7F;

        if (message_size > 0x3FFF) // 16383 bytes
        {
            buffer[1] |= 0x80;
            buffer[length++] = message_size >> 14 & 0x7F;

            if (message_size > 0x1FFFF) // 2097151 bytes
            {
                buffer[2] |= 0x80;
                buffer[length++] = message_size >> 21;
            }
        }
    }

    std::string write_buffer;
    write_buffer.resize(length + message_size);

    memcpy(write_buffer.data(), buffer, length);
    memcpy(write_buffer.data() + length, message_buffer.data(), message_size);

    return write_buffer;
}

}

void service::run(uint16_t port)
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
    acceptor.open(endpoint.protocol());
    acceptor.bind(endpoint);
    acceptor.listen();
    accept();

    ioc.run();
}

void service::stop()
{
    ioc.stop();
}

void service::accept()
{
    acceptor.async_accept(socket, [this](boost::system::error_code ec)
    {
        if (ec)
            return;
        auto s = std::make_shared<session>(std::move(socket));
        s->start();
        sessions.push_back(s.get());
        accept();
    });
}

void service::delete_session(const session &s)
{
    sessions.erase(std::remove(sessions.begin(), sessions.end(), &s), sessions.end());
    if (sessions.empty())
        notifier.reset();
}

void service::set_status(DWORD s)
{
    status.dwServiceType = SERVICE_WIN32;
    status.dwControlsAccepted = 0;
    status.dwCurrentState = s;
    status.dwWin32ExitCode = NO_ERROR;
    status.dwServiceSpecificExitCode = NO_ERROR;
    status.dwWaitHint = 0;

    if (s == SERVICE_RUNNING)
        status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_SESSIONCHANGE;

    if (s != SERVICE_RUNNING && s != SERVICE_STOPPED)
        ++status.dwCheckPoint;
    else
        status.dwCheckPoint = 0;

    if (!SetServiceStatus(status_handle, &status))
        qWarningErrno("SetServiceStatus failed");
}

void service::session_change(DWORD event_type, DWORD session_id)
{
    switch (event_type)
    {
    case WTS_CONSOLE_CONNECT:
        if (!sessions.empty())
            start_notifier();
        break;
    case WTS_CONSOLE_DISCONNECT:
        /*if (restart_timer_id_ != 0)
        {
            killTimer(restart_timer_id_);
            restart_timer_id_ = 0;
        }*/
        notifier.reset();
        break;
    case WTS_SESSION_LOGON:
        if (session_id == WTSGetActiveConsoleSessionId() && !sessions.empty())
            start_notifier();
        break;
    default:
        break;
    }
}

void service::start_notifier()
{
    notifier = std::make_unique<child>();
    if (!notifier->ipc.is_valid())
        return;

    notifier->ipc.start([this](const std::string &)
    {
        read_notifier_ipc();

        // Send information about all connected sessions to the notifier.
        for (auto &s : sessions)
            send_sessions_to_notifier(*s);
    });

    notifier->process.setAccount(HostProcess::User);
    notifier->process.setSessionId(WTSGetActiveConsoleSessionId());
    notifier->process.setProgram(boost::dll::program_location().parent_path().string() + '/' + kNotifierFileName);
    notifier->process.setArguments(QStringList() << QStringLiteral("--channel_id") << notifier->ipc.name_.c_str());

    // Start the process. After the start, the process must connect to the IPC server and
    // slot |onIpcNewConnection| will be called.
    notifier->process.start();
}

void service::send_sessions_to_notifier(session &s)
{
    proto::notifier::ServiceToNotifier message;

    proto::notifier::Session* session = message.mutable_session();
    session->set_uuid(s.uuid);
    session->set_remote_address(s.address);
    session->set_username(s.user_name);
    session->set_session_type(s.type);

    notifier->ipc.write(nullptr, serializeMessage(message));
}

void service::update_notifier(session &s)
{
    // notifier is restarted when
    // 1. the process is dead
    // 2. or we lost ipc connection
    if (notifier)
    {
        if (notifier->process.state() == HostProcess::ProcessState::NotRunning)
        {
            notifier.reset();
        }
    }

    if (!notifier)
        start_notifier();
    else
    {
        for (auto &s : sessions)
            send_sessions_to_notifier(*s);
    }
}

void service::read_notifier_ipc()
{
    notifier->ipc.read([this](const std::string &msg)
    {
        proto::notifier::NotifierToService message;

        if (!parseMessage(msg, message))
        {
            LOG_WARN(logger, "Invalid message from notifier");
            return;
        }

        if (message.has_kill_session())
        {
            LOG_INFO(logger, "Command to terminate the session from the notifier is received");

            auto uuid = message.kill_session().uuid();
            for (auto &session : get_service().sessions)
            {
                if (session->uuid == uuid)
                {
                    STOP_SESSION_NO_RETURN_ARG(*session);
                    break;
                }
            }
        }
        else
        {
            LOG_WARN(logger, "Unhandled message from notifier");
        }

        // Read next message.
        read_notifier_ipc();
    });
}

void service::write_notifier_ipc(const std::string &msg)
{
    notifier->ipc.write([this](const std::string &)
    {
    }, msg);
}

child::child()
    : ipc(get_service().ioc)
{
}

child::~child()
{
    process.kill();
}

session::session(tcp::socket &&s)
    : net_channel(std::move(s))
    , encryptor_(Encryptor::ServerMode)
    , channel_type_(ServerChannel)
{
}

session::~session()
{
    get_service().delete_session(*this);
}

void session::start()
{
    read([this](const std::string &s) {on_client_hello(s); });
}

void session::stop()
{
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    socket.close();
    host.reset();
}

std::string session::decrypt(const std::string &s)
{
    if (channel_state_ == Encrypted)
        return encryptor_.decrypt(s);
    return s;
}

std::string session::encrypt(const std::string &s)
{
    if (channel_state_ == Encrypted)
        return encryptor_.encrypt(s);
    return s;
}

void session::write(const std::string &msg)
{
    if (msg.empty() || msg.size() > kMaxMessageSize)
        return;
    write([this](const std::string &s) {on_write(s); }, msg);
}

void session::read_host_ipc()
{
    auto self = shared_from_this();
    host->ipc.read([this, self](const std::string &msg)
    {
        write([this](const std::string &) { read_host_ipc(); }, msg);
    });
}

void session::write_host_ipc(const std::string &msg)
{
    // remove?
    auto self = shared_from_this();
    host->ipc.write([this, self](const std::string &)
    {
        read([this](const std::string &msg) {write_host_ipc(msg); });
    }, msg);
}

void session::on_write(const std::string &msg)
{
    switch (channel_state_)
    {
    case Encrypted:
        //if (message_id != -1)
        //emit messageWritten(message_id);
        break;

    case Connected:
        if (channel_type_ == ServerChannel)
        {
            channel_state_ = Encrypted;
            //pinger_timer_id_ = startTimer(std::chrono::seconds(30));
            if (get_service().users.empty())
            {
                LOG_WARN(logger, "Empty user list or invalid network channel");
                STOP_SESSION;
            }
            read([this](const std::string &s) {on_new_auth_connection(s); });
        }
        else
        {
            //read();
        }
        break;
    }
}

void session::on_client_hello(const std::string &msg)
{
    switch (channel_state_)
    {
    case Encrypted:
        //emit messageReceived();
        break;
    case Connected:
        if (!encryptor_.readHelloMessage(msg))
            STOP_SESSION;
        if (channel_type_ == ServerChannel)
            write(encryptor_.helloMessage());
        else
        {
            channel_state_ = Encrypted;
            //pinger_timer_id_ = startTimer(std::chrono::seconds(30));
        }
        break;
    }
}

proto::auth::Status session::basic_authorization(const std::string& session_key)
{
    if (!User::isValidName(user_name))
    {
        LOG_WARN(logger, "") << "Invalid user name: " << user_name;
        return proto::auth::STATUS_ACCESS_DENIED;
    }

    if (session_key.empty())
    {
        LOG_WARN(logger, "Empty session key");
        return proto::auth::STATUS_ACCESS_DENIED;
    }

    for (const auto& user : get_service().users)
    {
        if (stricmp(user.name.c_str(), user_name.c_str()) == 0)
        {
            if (createSessionKey(user.password_hash.c_str(), nonce_) != session_key)
            {
                LOG_WARN(logger, "") << "Wrong password for user " << user_name;
                return proto::auth::STATUS_ACCESS_DENIED;
            }

            if (!(user.flags & User::FLAG_ENABLED))
            {
                LOG_WARN(logger, "") << "User " << user_name << " is disabled";
                return proto::auth::STATUS_ACCESS_DENIED;
            }

            if (!(user.sessions & type))
            {
                LOG_WARN(logger, "") << "Session type " << type
                    << " is disabled for user " << user_name;
                return proto::auth::STATUS_ACCESS_DENIED;
            }

            return proto::auth::STATUS_SUCCESS;
        }
    }

    LOG_WARN(logger, "") << "User not found: " << user_name;
    return proto::auth::STATUS_ACCESS_DENIED;
}

void session::on_new_auth_connection(const std::string &msg)
{
    LOG_INFO(logger, "Start authorization");

    proto::auth::ClientToHost message;
    if (parseMessage(msg, message))
    {
        if (message.has_logon_request())
        {
            auto &logon_request = message.logon_request();

            // We do not support other authorization methods yet.
            if (logon_request.method() != proto::auth::METHOD_BASIC)
            {
                LOG_WARN(logger, "") << "Unsupported authorization method: " << logon_request.method();
                auto status = proto::auth::STATUS_ACCESS_DENIED;
                proto::auth::HostToClient message;
                message.mutable_logon_result()->set_status(status);
                write(serializeMessage(message));
                STOP_SESSION;
            }

            auto method_ = logon_request.method();

            nonce_ = generateNonce();
            if (nonce_.empty())
            {
                LOG_DEBUG(logger, "Empty nonce generated");
                STOP_SESSION;
            }

            proto::auth::HostToClient message;
            message.mutable_server_challenge()->set_nonce(nonce_.data(), nonce_.size());
            write(serializeMessage(message));
            read([this](const std::string &s) {on_new_auth_connection(s); });
            return;
        }
        else if (message.has_client_challenge())
        {
            if (nonce_.empty())
            {
                LOG_WARN(logger, "Unexpected client challenge. Nonce not generated yet");
                STOP_SESSION;
            }

            auto &client_challenge = message.client_challenge();

            auto session_key = client_challenge.session_key();
            user_name = client_challenge.username();
            type = client_challenge.session_type();
            address = socket.remote_endpoint().address().to_string();

            auto status_ = basic_authorization(session_key.c_str());

            secureMemZero(&session_key);

            {
                proto::auth::HostToClient message;
                message.mutable_logon_result()->set_status(status_);
                write(nullptr, serializeMessage(message));
            }

            secureMemZero(message.mutable_client_challenge()->mutable_username());
            secureMemZero(message.mutable_client_challenge()->mutable_session_key());

            if (status_ != proto::auth::STATUS_SUCCESS)
                STOP_SESSION;

            if (user_name.empty())
            {
                LOG_WARN(logger, "Invalid user name");
                STOP_SESSION;
            }

            uuid = QUuid::createUuid().toString().toStdString();

            if (uuid.empty())
            {
                LOG_WARN(logger, "Invalid session UUID");
                STOP_SESSION;
            }

            LOG_INFO(logger, "") << "Starting" << sessionTypeToString(type)
                << "session for" << user_name;

            switch (type)
            {
            case proto::auth::SESSION_TYPE_DESKTOP_MANAGE:
            case proto::auth::SESSION_TYPE_DESKTOP_VIEW:
            case proto::auth::SESSION_TYPE_FILE_TRANSFER:
                break;
            default:
                LOG_WARN(logger, "Invalid session type: " << type);
                STOP_SESSION;
            }

            auto session_id_ = WTSGetActiveConsoleSessionId();

            host = std::make_unique<child>();
            if (!host->ipc.is_valid())
                STOP_SESSION;

            auto self = shared_from_this();
            host->ipc.start([this, self](const std::string &)
            {
                read([this](const std::string &msg) {write_host_ipc(msg); });
                read_host_ipc();
            });

            {
                host->process.setSessionId(session_id_);
                host->process.setProgram(boost::dll::program_location().parent_path().string() + "/aspia_host.exe");

                QStringList arguments;

                arguments << QStringLiteral("--channel_id") << host->ipc.name_.c_str();
                arguments << QStringLiteral("--session_type");

                switch (type)
                {
                case proto::auth::SESSION_TYPE_DESKTOP_MANAGE:
                    host->process.setAccount(HostProcess::Account::System);
                    arguments << QStringLiteral("desktop_manage");
                    break;

                case proto::auth::SESSION_TYPE_DESKTOP_VIEW:
                    host->process.setAccount(HostProcess::Account::System);
                    arguments << QStringLiteral("desktop_view");
                    break;

                case proto::auth::SESSION_TYPE_FILE_TRANSFER:
                    host->process.setAccount(HostProcess::Account::User);
                    arguments << QStringLiteral("file_transfer");
                    break;

                default:
                    LOG_FATAL(logger, "Unknown session type: " << type);
                    break;
                }

                host->process.setArguments(arguments);

                /*connect(session_process_, &HostProcess::errorOccurred, [this](HostProcess::ErrorCode error_code)
                {
                if (s.type == proto::auth::SESSION_TYPE_FILE_TRANSFER &&
                error_code == HostProcess::NoLoggedOnUser)
                {
                if (!startFakeSession())
                stop();
                }
                else
                {
                stop();
                }
                });
                connect(session_process_, &HostProcess::finished, this, &Host::dettachSession);
                */

                host->process.start();
            }

            get_service().update_notifier(*this);
        }
    }
}

ipc_channel::ipc_channel(boost::asio::io_context &ioc)
    : ipc_(ioc)
{
}

void ipc_channel::start(callback cb)
{
    boost::asio::windows::overlapped_ptr overlappedPtr(ipc_.get_io_context(),
        [cb](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (ec)
            return;

        if (cb)
            cb({});
    });

    auto overlapped = overlappedPtr.get();
    BOOL ok = ConnectNamedPipe(ipc_.native_handle(), overlapped);
    DWORD lastError = GetLastError();

    if (!ok && lastError != ERROR_IO_PENDING)
    {
        // The operation completed immediately, so a completion notification needs
        // to be posted. When complete() is called, ownership of the OVERLAPPED-
        // derived object passes to the io_service.
        boost::system::error_code ec(lastError,
            boost::asio::error::get_system_category());
        overlappedPtr.complete(ec, 0);
    }
    else
    {
        // The operation was successfully initiated, so ownership of the
        // OVERLAPPED-derived object has passed to the io_service.
        overlappedPtr.release();
    }
}

void ipc_channel::read(callback cb)
{
    ipc_.async_read_some(boost::asio::buffer(buffer_),
        [this, cb](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (ec)
            return;

        if (!bytes_transferred)
            return read(cb);

        size_t ptr = 0;

    begin:
        if (!read_size_received_)
        {
            while (ptr < bytes_transferred && read_size_len_--)
                ((char*)&read_size_)[sizeof(MessageSizeType) - read_size_len_ - 1] = buffer_[ptr++];
            if (read_size_len_ == 0)
            {
                read_size_received_ = true;

                if (!read_size_ || read_size_ > kMaxMessageSize)
                {
                    LOG_WARN(logger, "") << "Wrong message size: " << read_size_;
                    return;
                }
            }
        }

        if (read_buffer.size() < read_size_)
        {
            if (ptr < bytes_transferred)
            {
                size_t len = std::min(bytes_transferred - ptr, read_size_ - read_buffer.size());
                read_buffer.append(buffer_.data() + ptr, buffer_.data() + ptr + len);
                ptr += len;
                if (read_buffer.size() < read_size_)
                    return read(cb);
            }
            else
                return read(cb);
        }

        read_size_ = 0;
        read_size_len_ = sizeof(MessageSizeType);
        read_size_received_ = false;

        if (cb)
            cb(read_buffer);
        read_buffer.clear();

        if (ptr < bytes_transferred)
            goto begin;
    });
}

void ipc_channel::write(callback cb, const std::string &msg)
{
    write_size_ = msg.size();
    write_buffer.resize(sizeof(write_size_) + msg.size());
    memcpy(write_buffer.data(), &write_size_, sizeof(write_size_));
    memcpy(write_buffer.data() + sizeof(write_size_), msg.data(), msg.size());
    boost::asio::async_write(ipc_, boost::asio::buffer(write_buffer),
        [this, cb](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (ec)
            return;
        if (cb)
            cb({});
    });
}

ipc_server::ipc_server(boost::asio::io_context &ioc)
    : ipc_channel(ioc)
{
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;      //non inheritable handle, same as default
    sa.lpSecurityDescriptor = 0;    //default security descriptor

    QScopedPointer<SECURITY_DESCRIPTOR> pSD;
    PSID worldSID = 0;
    QByteArray aclBuffer;
    QByteArray tokenUserBuffer;
    QByteArray tokenGroupBuffer;

    // create security descriptor if access options were specified
    //if ((socketOptions & QLocalServer::WorldAccessOption))
    if (1)
    {
        pSD.reset(new SECURITY_DESCRIPTOR);
        if (!InitializeSecurityDescriptor(pSD.data(), SECURITY_DESCRIPTOR_REVISION)) {
            return;
        }
        HANDLE hToken = NULL;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
            return;
        DWORD dwBufferSize = 0;
        GetTokenInformation(hToken, TokenUser, 0, 0, &dwBufferSize);
        tokenUserBuffer.fill(0, dwBufferSize);
        PTOKEN_USER pTokenUser = (PTOKEN_USER)tokenUserBuffer.data();
        if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwBufferSize, &dwBufferSize)) {
            CloseHandle(hToken);
            return;
        }

        dwBufferSize = 0;
        GetTokenInformation(hToken, TokenPrimaryGroup, 0, 0, &dwBufferSize);
        tokenGroupBuffer.fill(0, dwBufferSize);
        PTOKEN_PRIMARY_GROUP pTokenGroup = (PTOKEN_PRIMARY_GROUP)tokenGroupBuffer.data();
        if (!GetTokenInformation(hToken, TokenPrimaryGroup, pTokenGroup, dwBufferSize, &dwBufferSize)) {
            CloseHandle(hToken);
            return;
        }
        CloseHandle(hToken);

        SID_IDENTIFIER_AUTHORITY WorldAuth = { SECURITY_WORLD_SID_AUTHORITY };
        if (!AllocateAndInitializeSid(&WorldAuth, 1, SECURITY_WORLD_RID,
            0, 0, 0, 0, 0, 0, 0,
            &worldSID)) {
            return;
        }

        //calculate size of ACL buffer
        DWORD aclSize = sizeof(ACL) + ((sizeof(ACCESS_ALLOWED_ACE)) * 3);
        aclSize += GetLengthSid(pTokenUser->User.Sid) - sizeof(DWORD);
        aclSize += GetLengthSid(pTokenGroup->PrimaryGroup) - sizeof(DWORD);
        aclSize += GetLengthSid(worldSID) - sizeof(DWORD);
        aclSize = (aclSize + (sizeof(DWORD) - 1)) & 0xfffffffc;

        aclBuffer.fill(0, aclSize);
        PACL acl = (PACL)aclBuffer.data();
        InitializeAcl(acl, aclSize, ACL_REVISION_DS);

        //if (socketOptions & QLocalServer::UserAccessOption)
        if (0)
        {
            if (!AddAccessAllowedAce(acl, ACL_REVISION, FILE_ALL_ACCESS, pTokenUser->User.Sid)) {
                FreeSid(worldSID);
                return;
            }
        }
        //if (socketOptions & QLocalServer::GroupAccessOption)
        if (0)
        {
            if (!AddAccessAllowedAce(acl, ACL_REVISION, FILE_ALL_ACCESS, pTokenGroup->PrimaryGroup)) {
                FreeSid(worldSID);
                return;
            }
        }
        //if (socketOptions & QLocalServer::OtherAccessOption)
        if (1)
        {
            if (!AddAccessAllowedAce(acl, ACL_REVISION, FILE_ALL_ACCESS, worldSID)) {
                FreeSid(worldSID);
                return;
            }
        }
        SetSecurityDescriptorOwner(pSD.data(), pTokenUser->User.Sid, FALSE);
        SetSecurityDescriptorGroup(pSD.data(), pTokenGroup->PrimaryGroup, FALSE);
        if (!SetSecurityDescriptorDacl(pSD.data(), TRUE, acl, FALSE)) {
            FreeSid(worldSID);
            return;
        }

        sa.lpSecurityDescriptor = pSD.data();
    }


    auto channel_id_ = generateUniqueChannelId();
    name_ = "\\\\.\\pipe\\" + channel_id_;
    auto hPipe = CreateNamedPipeA(name_.c_str(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED | FILE_FLAG_FIRST_PIPE_INSTANCE,
        0, 1,
        65536, 65536,
        //0, 0,
        3000, &sa);
    ipc_.assign(hPipe);
}

net_channel::net_channel(tcp::socket &&s)
    : socket(std::move(s))
{
    boost::asio::ip::tcp::no_delay o(true);
    socket.set_option(o);
}

void net_channel::read(callback cb)
{
    auto self = shared_from_this();
    socket.async_read_some(boost::asio::buffer(buffer_),
        [this, self, cb](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (ec)
            return;

        size_t ptr = 0;

    begin:
        if (!read_size_received_)
        {
            if (ptr >= bytes_transferred)
                return read(cb);
            uint8_t byte = buffer_[ptr++];
            switch (ptr - 1)
            {
            case 0:
                // If the first byte is zero, then message ping is received.
                // This message is ignored.
                if (byte == 0)
                    goto begin;
                read_size_ += byte & 0x7F;
                break;
            case 1:
                read_size_ += (byte & 0x7F) << 7;
                break;
            case 2:
                read_size_ += (byte & 0x7F) << 14;
                break;
            case 3:
                read_size_ += byte << 21;
                break;
            }

            if (byte & 0x80 && ptr - 1 < 3)
                goto begin;

            read_size_received_ = true;
            if (!read_size_ || read_size_ > kMaxMessageSize)
            {
                LOG_WARN(logger, "") << "Wrong message size: " << read_size_;
                return;
            }
        }

        if (read_buffer.size() < read_size_)
        {
            if (ptr < bytes_transferred)
            {
                size_t len = std::min(bytes_transferred - ptr, read_size_ - read_buffer.size());
                read_buffer.append(buffer_.data() + ptr, buffer_.data() + ptr + len);
                ptr += len;
                if (read_buffer.size() < read_size_)
                    return read(cb);
            }
            else
                return read(cb);
        }

        read_size_ = 0;
        read_size_received_ = false;

        read_buffer = decrypt(read_buffer);
        if (cb)
            cb(read_buffer);
        read_buffer.clear();

        if (ptr < bytes_transferred)
            goto begin;
    });
}

void net_channel::write(callback cb, const std::string &msg)
{
    write_buffer = encrypt(msg);
    write_buffer = createWriteBuffer(write_buffer);
    auto self = shared_from_this();
    boost::asio::async_write(socket, boost::asio::buffer(write_buffer),
        [this, self, cb](boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (ec)
            return;
        if (cb)
            cb(write_buffer);
    });
}

}
