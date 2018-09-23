#include <cpputils/logging/logger_impl.h>

using namespace ::utl::logging;

const std::string& logger_impl::name() const
    { return _name; }

bool logger_impl::is_enabled(log_level level) const
{
    for (auto& rule : _rules)
    {
        if (rule->is_enabled(level))
            return true;
    }
    return false;
}

void logger_impl::log(data_ptr_s data) const
{
    std::lock_guard<std::mutex> lk(_mutex);
    for (auto& r : _rules)
        r->log(data);
}
