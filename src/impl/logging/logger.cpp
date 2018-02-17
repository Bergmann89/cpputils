#include <cpputils/logging/logger.h>

using namespace ::utl::logging;

const std::string& logger::name() const
{
    static std::string name;
    return name;
}

bool logger::is_enabled(log_level level) const
    { return false; }

void logger::log(data_ptr_s data) const
    { /* no op */ }