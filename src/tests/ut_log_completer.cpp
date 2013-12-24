#ifndef BOOST_STATIC_LINK
#   define BOOST_TEST_DYN_LINK
#endif // BOOST_STATIC_LINK

#define BOOST_TEST_MODULE CtppHtml
#define BOOST_AUTO_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include <sstream>

#include "Log.hpp"

struct LogSequence {
    struct Head {
        struct Next {
            std::stringstream *_stream;

            Next(const Next &next) 
                : _stream(next._stream)
            {}
            
            template<class Type>
            explicit Next(std::stringstream *stream, const Type &value) 
                : _stream(stream)
            {
                (*_stream) << value;
            }

            template<class Type>
            Next operator << (const Type &value) {
                return Next(_stream, value);
            }
        };

        std::shared_ptr<std::stringstream> _stream;
        std::string _module;

        Head(const Head &head)
            : _stream(head._stream)
            , _module(head._module)
        {}
      
        template<class Type>
        explicit Head(const std::string& module, const Type &value) 
            : _stream(new std::stringstream)
            , _module(module)
        {
            (*_stream) << value;
        }
        
        ~Head() {
            base::Singleton<base::Log>::getInstance()->print(DEBUG, _module, _stream->str().c_str());
        }

        template<class Type>
        Next operator << (const Type &value) {
            return Next(_stream.get(), value);
        }
    };
    
    std::string _module;

    LogSequence(const std::string &module) 
        : _module(module)
    {}

    template<class Type>
    Head operator << (const Type &value) {
        return Head(_module, value);
    }
};


//struct TempLog {
//    std::stringstream _stream;
//    bool _is_head;
//    bool _is_complete;
//    std::string _method;
//    TempLog *_parent;
//
//    TempLog(const std::string &method) 
//        : _is_head(true) 
//        , _is_complete(false)
//        , _method(method)
//        , _parent(this)
//    {}
//
//    TempLog(const TempLog &log) 
//        : _is_head(false)
//        , _is_complete(log._is_head)
//        , _parent(log._parent)
//    {}
//    
//    ~TempLog() {
//        if (_is_complete) {
//            base::Singleton<base::Log>::getInstance()->print(DEBUG, _parent->_method, _parent->_stream.str().c_str());
//        }
//    }
//};
//
//
//template<class Type>
//TempLog operator << (const TempLog &log, Type type) {
//    TempLog &l = const_cast<TempLog&>(log);
//    l._parent->_stream << type;
//    TempLog _log(log);
//    return _log;
//}
    
    
BOOST_AUTO_TEST_CASE(TestLogCompleter) {
    LOG_TO_STDOUT;
    std::cout << "Begin \n";
    LogSequence t("TEST"); 
    t << 1 << "," << 2 << "," << 3;
    {
        LogSequence t("test"); 
        t << "a" << "," << "b";
    }
    std::cout << "End \n";
}
