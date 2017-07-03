#include <functional>
#include <memory>

class PNTCPConnection;
class PNTimestamp;
class Buffer;

typedef std::shared_ptr<PNTCPConnection> TCPConnectionPtr;

typedef std::function<void()> TimerCallback;
typedef std::function<void(const TCPConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TCPConnectionPtr&, Buffer* buf, PNTimestamp)> MessageCallback;
typedef std::function<void(const TCPConnectionPtr&)> CloseCallback;
