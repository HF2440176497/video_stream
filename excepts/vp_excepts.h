#pragma once

#include <stdexcept>

namespace vp_excepts {
    // Invalid argument error
    class vp_invalid_argument_error: public std::runtime_error {
    public:
        vp_invalid_argument_error(const std::string& what_arg);
        ~vp_invalid_argument_error();
    };
    
    inline vp_invalid_argument_error::vp_invalid_argument_error(const std::string& what_arg): std::runtime_error(what_arg) {
    }
    
    inline vp_invalid_argument_error::~vp_invalid_argument_error() {
    }

    // Invalid calling error
    class vp_invalid_calling_error: public std::runtime_error {
    public:
        vp_invalid_calling_error(const std::string& what_arg);
        ~vp_invalid_calling_error();
    };
    
    inline vp_invalid_calling_error::vp_invalid_calling_error(const std::string& what_arg): std::runtime_error(what_arg) {
    }
    
    inline vp_invalid_calling_error::~vp_invalid_calling_error() {
    }

    // Invalid pipeline error
    class vp_invalid_pipeline_error: public std::runtime_error {
    public:
        vp_invalid_pipeline_error(const std::string& what_arg);
        ~vp_invalid_pipeline_error();
    };
    
    inline vp_invalid_pipeline_error::vp_invalid_pipeline_error(const std::string& what_arg): std::runtime_error(what_arg) {
    }
    
    inline vp_invalid_pipeline_error::~vp_invalid_pipeline_error() {
    }

    // Not implemented error
    class vp_not_implemented_error: public std::runtime_error {
    public:
        vp_not_implemented_error(const std::string& what_arg);
        ~vp_not_implemented_error();
    };
    
    inline vp_not_implemented_error::vp_not_implemented_error(const std::string& what_arg): std::runtime_error(what_arg) {
    }
    
    inline vp_not_implemented_error::~vp_not_implemented_error() {
    }
}
