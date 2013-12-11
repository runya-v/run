#ifndef BOOST_STATIC_LINK
#   define BOOST_TEST_DYN_LINK
#endif // BOOST_STATIC_LINK

#define BOOST_TEST_MODULE CtppHtml
#define BOOST_AUTO_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>


struct Temp {
    int i;

    Temp() : i(0) {
        std::cout << this << " Start\n";
    }

    Temp(const Temp& tmp) : i(tmp.i+1) {
        std::cout << this << ":" << i << "\n";
    }
    
    ~Temp() {
        std::cout << this << ":" << i << "~\n";
        i--;
        
        if (not i) {
            std::cout << this << ":" << "Complete\n";
        }
    }
};


//template<class Type>
//Temp operator << (LogAggregator, Type) {
//    
//}


template<class Type>
Temp operator << (const Temp &tmp, Type) {
    Temp _tmp(tmp);
    return _tmp;
}
    
BOOST_AUTO_TEST_CASE(TestLogCompleter)
{
    std::cout << "Begin \n";
    Temp t; t << 1 << 2 << 3;
    {
        Temp t; t << 1 << 2;
    }
    std::cout << "End \n";
}


