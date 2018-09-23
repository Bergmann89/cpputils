#include <cstring>
#include <cpputils/misc/stream.h>
#include <cpputils/logging/consumer/consumer_stream.h>

using namespace ::utl;
using namespace ::utl::logging;

void consumer_stream::log(data_ptr_s data)
{
    std::lock_guard<std::mutex> lk(_mutex);

    using namespace std;
    if (!data)
        return;
    auto& d = *data;
    auto t = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(d.time.time_since_epoch()).count();
    auto f = d.file;
    if (f)
    {
        auto tmp = strrchr(f, '/');
        if (tmp)
            f = tmp + 1;
    }
    else
        f = "unknown";

    stream_format_saver format_saver(*_stream);
    if (t >= 0)                         *_stream << "["   << fixed << setfill(' ') << setw(17) <<  setprecision(6) << t << "] ";
    switch(d.level)
    {
        case log_level::debug:              *_stream << "DEBUG "; break;
        case log_level::info:               *_stream << "INFO  "; break;
        case log_level::warn:               *_stream << "WARN  "; break;
        case log_level::error:              *_stream << "ERROR "; break;
    }

    if (d.sender)                       *_stream << "0x"  << hex << setw(2 * sizeof(void*)) << setfill('0') << d.sender;
    else                                *_stream << "                  ";
    if (d.thread != std::thread::id())  *_stream << "@"   << hex << setw(2 * sizeof(void*)) << setfill('0') << d.thread;
    else                                *_stream << "                 ";
    if (!d.name.empty())                *_stream << " '" << d.name << "'";
    if (d.line)                         *_stream << " - " << setw(25) << setfill(' ') << f << ":" << setw(5) << setfill(' ') << dec << d.line;
    if (!d.message.empty())
    {
        *_stream << ": "  << d.message;
        if (d.message.back() != '\n')
            *_stream << std::endl;
    }
    else
        *_stream << std::endl;
}

consumer_stream::consumer_stream(const std::string& name, std::ostream& stream, bool ownsStream, bool autoRegister) :
    consumer    (name, autoRegister),
    _stream     (&stream),
    _ownsStream (ownsStream)
    { }

consumer_stream::~consumer_stream()
{
    if (_ownsStream && _stream)
    {
        delete _stream;
        _stream = nullptr;
    }
}