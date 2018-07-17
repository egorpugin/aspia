#pragma once

#include "host_process.h"

#include "crypto/encryptor.h"
#include "protocol/notifier.pb.h"

#define _WIN32_WINNT 0x0601
#include <boost/asio.hpp>

#include <list>
#include <string>
#include <vector>

#include <windows.h>

namespace aspia
{

using callback = std::function<void(const std::string &)>;
using tcp = boost::asio::ip::tcp;
using MessageSizeType = uint32_t;

enum ChannelState
{
    Connected,
    Encrypted
};

enum ChannelType
{
    ServerChannel,
    ClientChannel
};

struct user
{
    std::string name;
    std::string password_hash;
    uint32_t flags = 0;
    uint32_t sessions = 0;
};

struct channel_data
{
    std::string buffer_;
    std::string read_buffer;
    std::string write_buffer;
    bool read_size_received_ = false;
    size_t read_size_ = 0;

    channel_data()
    {
        buffer_.resize(8 * 1024);
    }
};

struct ipc_channel : channel_data
{
    std::string name_;
    boost::asio::windows::stream_handle ipc_;
    int read_size_len_ = sizeof(MessageSizeType);
    size_t write_size_ = 0;

    ipc_channel(boost::asio::io_context &ioc);
    //ipc_channel(boost::asio::io_context &ioc, const std::string &); // client variant

    void start(callback cb);

    void read(callback cb);
    void write(callback cb);
    void write(callback cb, const std::string &msg);

    bool is_valid() const { return const_cast<ipc_channel*>(this)->ipc_.native_handle() != INVALID_HANDLE_VALUE; }
};

struct ipc_server : ipc_channel
{
    ipc_server(boost::asio::io_context &ioc);
};

struct net_channel : channel_data, std::enable_shared_from_this<net_channel>
{
    net_channel(tcp::socket &&s);

protected:
    tcp::socket socket;

    void read(callback cb);
    void write(callback cb);
    void write(callback cb, const std::string &msg);

    virtual std::string decrypt(const std::string &s) { return s; }
    virtual std::string encrypt(const std::string &s) { return s; }
};

struct child
{
    HostProcess process;
    ipc_server ipc;

    child();
    ~child();
};

struct session : net_channel
{
    std::string user_name;
    std::string uuid;
    std::string address;
    proto::auth::SessionType type{ proto::auth::SESSION_TYPE_UNKNOWN };

    session(tcp::socket &&s);
    ~session();

    void start();
    void stop();

private:
    Encryptor encryptor_;
    std::string nonce_;
    ChannelState channel_state_ = Connected;
    ChannelType channel_type_;
    std::unique_ptr<child> host;

    using net_channel::write;
    void write(const std::string &msg);

    std::string decrypt(const std::string &s) override;
    std::string encrypt(const std::string &s) override;

    void on_write(const std::string &msg);
    void on_client_hello(const std::string &msg);

    void read_host_ipc();
    void write_host_ipc(const std::string &msg);

    proto::auth::Status basic_authorization(const std::string& session_key);
    void on_new_auth_connection(const std::string &msg);
};

struct service
{
    // data
    std::string name;
    std::string display_name;
    std::string description;
    std::vector<user> users;
    std::vector<session*> sessions;
    std::unique_ptr<child> notifier;

    // service data
    boost::asio::io_context ioc;
    tcp::acceptor acceptor{ ioc };
    tcp::socket socket{ ioc };
    SERVICE_STATUS_HANDLE status_handle{ nullptr };
    SERVICE_STATUS status{ 0 };

    //void install();
    //void remove();

    void run(uint16_t port);
    void stop();

    void set_status(DWORD status);
    void session_change(DWORD event_type, DWORD session_id);

    void update_notifier(session &s);
    void delete_session(const session &s);

private:
    void accept();

    void read_notifier_ipc();
    void write_notifier_ipc(const std::string &msg);

    void start_notifier();
    void send_sessions_to_notifier(session &s);
};

service &get_service();

}
