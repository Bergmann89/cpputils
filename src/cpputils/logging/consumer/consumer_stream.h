#pragma once

#include <mutex>

#include <cpputils/logging/consumer/consumer.h>

namespace utl {
namespace logging {

    struct consumer_stream : public consumer
    {
    private:
        mutable std::mutex  _mutex;
        std::ostream*       _stream;
        bool                _ownsStream;

    public:
        void log(data_ptr_s data) override;

        consumer_stream(const std::string& name, std::ostream& stream, bool ownsStream, bool autoRegister);
        virtual ~consumer_stream();
    };

}
}