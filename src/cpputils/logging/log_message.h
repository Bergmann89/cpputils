#pragma once

namespace utl {
namespace logging {

    struct log_message
    {
    private:
        std::ostringstream _msg;

    public:
        inline std::string str() const
            { return _msg.str(); }

        template <typename T>
        inline log_message& operator <<(const T& value)
        {
            _msg << value;
            return *this;
        }

        inline log_message& operator <<(std::ostream& (*callback)(std::ostream&))
        {
            callback(_msg);
            return *this;
        }

        log_message() :
            _msg()
            { }

        log_message(const std::string& msg) :
            _msg(msg)
            { }

    private:
        log_message(log_message&&) = delete;
        log_message(const log_message&) = delete;
    };

}
}
