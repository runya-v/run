/*!
 * \brief  Вазовый класс исключений.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   3.11.2012
 */


#pragma once

#include <cstdint>
#include <string>

#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <boost/format.hpp>


/*!
* \brief Базовый класс для всех исключений библиотеки.
*
* Для того чтобы объявить собственный класс исключения, его необходимо
* унаследовать от данного класса. В случае необходимости возможно
* переопределение методов message и where в дочернем классе. В таком случае
* логичным действием будет вызов в этих методах родительских What и Where
* соответственно с последующей переработкой информации.
*
* Пример использования:
* BOOST_THROW_EXCEPTION(ErrorLibraryLoad()
*       << Exception::ErrorInfo("Error Information.")
*       << Exception::MethodInfo("Method Information.")
*       << boost::errinfo_errno(errno));
*/
class Exception
    : virtual public std::exception
    , virtual public boost::exception
{
protected:
    mutable std::string _what;  //!< Что произошло.
    mutable std::string _where; //!< Где произошло.

    //! \brief Получение информации о конкретной ошибке.
    template< class T >
    typename T::value_type const* get() const {
        return boost::get_error_info< T >(*this);
    }

public:
    //! \brief Тип информации об ошибке.
    typedef boost::error_info< struct ErrorInfoTag, std::string > ErrorInfo;

    //! \brief Тип расположения ошибки в коде
    typedef boost::error_info< struct MethodInfoTag, std::string > MethodInfo;

    virtual ~Exception() throw ()
    {}

    //! \brief Диагностическая информация об ошибке.
    virtual char const* what() const throw() {
        return boost::diagnostic_information_what(*this);
    }

    //! \brief Что произошло.
    virtual std::string const& message() const {
        if (const std::string* info = get< ErrorInfo >()) {
            _what = *info;
        }
        return _what;
    }

    //! \brief Где произошло.
    virtual std::string const& where() const {
        std::ostringstream where_info;

        if (char const* const filename = boost::get_error_info< boost::throw_file >(*this)) {
            where_info << *filename;

            if (int const* line = boost::get_error_info< boost::throw_line >(*this)) {
                where_info << " `" << *line << "`";
            }
        }

        if (char const* const function_name = boost::get_error_info< boost::throw_function >(*this)) {
            where_info << " in " << *function_name;
        }
        _where.assign(where_info.str());
        return _where;
    }
};
