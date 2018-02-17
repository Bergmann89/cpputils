#include <cpputils/logging/logger_impl.h>
#include <cpputils/logging/consumer/consumer.h>

using namespace ::utl::logging;

consumer::consumer(const std::string& n, bool autoRegister) :
    _name(n)
{
    if (autoRegister)
        register_consumer(*this);
}

consumer::~consumer()
    { unregister_consumer(*this); }