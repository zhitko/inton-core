#ifndef ERROR_H
#define ERROR_H

#include "IntonCore_global.h"

#include <exception>
#include <string>

namespace IntonCore {

enum ErrorType {
    not_implemented
};

class Error: virtual public std::exception {

protected:
    ErrorType error_type;
    std::string error_where;
    std::string error_message;

public:

    explicit Error(const std::string& msg, ErrorType err_type, const std::string& err_where = "");

    virtual ~Error() noexcept;

    virtual const char* what() const noexcept;

    virtual ErrorType getErrorType() const noexcept;
};

}

#endif // ERROR_H
