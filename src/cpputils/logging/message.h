#pragma once

namespace utl {
namespace logging {

    struct message
    {
    private:
        std::ostringstream _msg;

    public:
        inline std::string str() const
            { return _msg.str(); }

        template <typename T>
        inline message& operator <<(const T& value)
        {
            _msg << value;
            return *this;
        }

        inline message& operator <<(std::ostream& (*callback)(std::ostream&))
        {
            callback(_msg);
            return *this;
        }

        message() :
            _msg()
            { }

        message(const std::string& msg) :
            _msg(msg)
            { }

    private:
        message(message&&) = delete;
        message(const message&) = delete;
    };

}
}
