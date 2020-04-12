#include "error.h"

using namespace IntonCore;

Error::Error(const std::string& msg, ErrorType err_type, const std::string& err_where):
    error_type(err_type),
    error_where(err_where),
    error_message(msg)
{}

Error::~Error() noexcept = default;

const char *Error::what() const noexcept {
    return (error_message + error_where).c_str();
}

ErrorType Error::getErrorType() const noexcept
{
    return error_type;
}
