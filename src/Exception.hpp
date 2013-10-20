#pragma once

#include <cstdint>
#include <string>

#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/format.hpp>


namespace base {
    // Для того чтобы объявить собственный класс исключения, его необходимо
    // унаследовать от данного класса. В случае необходимости возможно
    // переопределение методов message и where в дочернем классе. В таком случае
    // логичным действием будет вызов в этих методах родительских What и Where
    // соответственно с последующей переработкой информации.
    //
    // Пример использования:
    // BOOST_THROW_EXCEPTION(ErrorLibraryLoad()
    //       << Exception::ErrorInfo("Error Information.")
    //       << Exception::MethodInfo("Method Information.")
    //       << boost::errinfo_errno(errno));
    class Exception
        : virtual public std::exception
        , virtual public boost::exception
    {
    public:
        // Тип информации об ошибке.
        typedef boost::error_info<struct ErrorInfoTag, std::string> ErrorInfo;

        // Тип расположения ошибки в коде
        typedef boost::error_info<struct MethodInfoTag, std::string> MethodInfo;

        virtual ~Exception() throw();

        // Диагностическая информация об ошибке.
        virtual char const* what() const throw();

        // Что произошло.
        virtual const std::string& message() const;

        // Где произошло.
        virtual const std::string& where() const;

    protected:
        mutable std::string _what;  // Что произошло.
        mutable std::string _where; // Где произошло.

        // Получение информации о конкретной ошибке.
        template<class T>
        const typename T::value_type* get() const {
            return boost::get_error_info<T>(*this);
        }
    };
} // namespace base
