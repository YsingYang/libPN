#include <functional>
#include <memory>

class PNTCPConnection;

typedef std::shared_ptr<PNTCPConnection> TCPConnectionPtr;

typedef std::function<void()> TimerCallback;
typedef std::function<void(const TCPConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TCPConnectionPtr&, const char* data, ssize_t len)> MessageCallback;
typedef std::function<void(const TCPConnectionPtr&)> CloseCallback;
