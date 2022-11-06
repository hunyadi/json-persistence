#pragma once
#include <sstream>

namespace test
{
    /** Captures the output written to the the standard output stream as a string. */
    class OutputCapture
    {
    public:
        OutputCapture()
        {
            sbuf = std::cout.rdbuf();
            std::cout.rdbuf(buffer.rdbuf());
        }

        ~OutputCapture()
        {
            if (sbuf != nullptr) {
                std::cout.rdbuf(sbuf);
            }
        }

        std::string str()
        {
            if (sbuf != nullptr) {
                std::cout.rdbuf(sbuf);
                sbuf = nullptr;
            }
            return buffer.str();
        }

    private:
        std::stringstream buffer;
        std::streambuf* sbuf = nullptr;
    };
}
