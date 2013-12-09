#ifndef BOOST_STATIC_LINK
#   define BOOST_TEST_DYN_LINK
#endif // BOOST_STATIC_LINK

#define BOOST_TEST_MODULE CtppHtml
#define BOOST_AUTO_TEST_MAIN

#include <string> 
#include <fstream>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/format.hpp>
#include <boost/utility.hpp> 

#include <ctpp2/CTPP2Parser.hpp>
#include <ctpp2/CTPP2FileSourceLoader.hpp>
#include <ctpp2/CTPP2FileOutputCollector.hpp>
#include <ctpp2/CTPP2ParserException.hpp>
#include <ctpp2/CTPP2HashTable.hpp>
#include <ctpp2/CTPP2VMDumper.hpp>
#include <ctpp2/CTPP2VMOpcodes.h>
#include <ctpp2/CTPP2SyscallFactory.hpp>
#include <ctpp2/CTPP2VMFileLoader.hpp>
#include <ctpp2/CTPP2JSONFileParser.hpp>
#include <ctpp2/CTPP2VM.hpp>
#include <ctpp2/CTPP2VMSTDLib.hpp>
#include <ctpp2/CTPP2VMException.hpp>
#include <ctpp2/CTPP2VMStackException.hpp>

#include "Log.hpp"
#include "HtmlMaker.hpp"


const char TEST_TMPL[] = 
    "<html>" 
        "<head>" 
            "<title>News List</title>" 
            "<style type=\"text/css\">" 
                "a {" 
                    "font-family: Verdana, Arial, Helvetica, sans-serif;" 
                    "color: black; font-size: 8pt; font-weight: bold;" 
                "}" 
                "span {" 
                    "font-family: Verdana, Arial, Helvetica, sans-serif;" 
                    "color: black; font-size: 10pt; font-weight: normal;" 
                "}" 
                "span.date {" 
                    "font-family: Verdana, Arial, Helvetica, sans-serif;" 
                    "color: #D00000; font-size: 10pt; font-weight: normal;" 
                "}" 
            "</style>" 
        "</head>" 
        "<body>" 
            "<TMPL_if newslist>" 
            "<div align=\"center\" style=\"width=100%\">" 
            "<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"96%\">" 
            "<TMPL_loop newslist>" 
                "<tr><td>" 
                    "<span class=\"date\"><b><i><TMPL_var DATE_FORMAT(date, \"%d %b %Y %H:%M\")>" 
                    "</i></b></span>" 
                    "<span><b><TMPL_var title></b><br><TMPL_var body><br>" 
                    "<TMPL_if publication>" 
                        "<a href=\"<TMPL_var publication>\">Main URL</a><br>" 
                    "</TMPL_if>" 
                    "<TMPL_if reporter>" 
                        "<a href=\"<TMPL_var reporter_url>\"><TMPL_var reporter></a><br>" 
                    "</TMPL_if>" 
                    "<br>" 
                    "</span>" 
                "</td></tr>" 
            "</TMPL_loop>" 
            "</table>" 
            "</div>" 
            "</TMPL_if>" 
        "</body>" 
    "</html>";


const char TEST_JSON[] = 
    "{\n"
    "   \"newslist\" : [\n"
    "       {\n"   
    "           \"date\"  : 1169090455,\n" 
    "           \"title\" : \"FreeBSD 6.3-RELEASE is now available\",\n" 
    "           \"body\"  : \"TEST Text 1\"\n"
    "       },\n" 
    "       {\n"
    "           \"date\"  : 1169019722,\n" 
    "           \"title\" : \"New RSS Feeds\",\n" 
    "           \"body\"  : \"TEST Text 2\"\n"
    "       },\n" 
    "       {\n"
    "           \"date\"         : 1169010455,\n" 
    "           \"title\"        : \"$199 Linux PC Now Available at Sears.com\",\n" 
    "           \"body\"         : \"TEST Text 3\",\n" 
    "           \"publication\"  : \"http://test_href_1.html\",\n" 
    "           \"reporter\"     : \"SOURCE Linspire, Inc.\"\n"
    "       },\n" 
    "       {\n"
    "           \"date\"         : 1168840430,\n" 
    "           \"title\"        : \"Samba's Tridge clusters code and crowds\",\n" 
    "           \"body\"         : \"TEST Text 4\",\n" 
    "           \"publication\"  : \"http://test_href_2.html\",\n" 
    "           \"reporter\"     : \"Dahna McConnachie\",\n" 
    "           \"reporter_url\" : \"http://test_href_3.html\"\n"
    "       }\n"
    "   ]\n"
    "}\n";


const char RESULT[] = 
    "<html>"
        "<head>"
            "<title>News List</title>"
            "<style type=\"text/css\">"
                "a {font-family: Verdana, Arial, Helvetica, sans-serif;color: black; font-size: 8pt; font-weight: bold;}span {font-family: Verdana, Arial, Helvetica, sans-serif;color: black; font-size: 10pt; font-weight: normal;}span.date {font-family: Verdana, Arial, Helvetica, sans-serif;color: #D00000; font-size: 10pt; font-weight: normal;}"
            "</style>"
        "</head>"
        "<body>"
            "<div align=\"center\" style=\"width=100%\">"
                "<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"96%\">"
                    "<tr><td>"
                        "<span class=\"date\"><b><i>18 Jan 2007 06:20</i></b></span>"
                        "<span><b>FreeBSD 6.3-RELEASE is now available</b><br>TEST Text 1<br><br></span>"
                    "</td></tr>"
                    "<tr><td>"
                        "<span class=\"date\"><b><i>17 Jan 2007 10:42</i></b></span>"
                        "<span><b>New RSS Feeds</b><br>TEST Text 2<br><br></span>"
                    "</td></tr>"
                    "<tr><td>"
                        "<span class=\"date\"><b><i>17 Jan 2007 08:07</i></b></span>"
                        "<span>"
                            "<b>$199 Linux PC Now Available at Sears.com</b>"
                            "<br>TEST Text 3<br>"
                            "<a href=\"http://test_href_1.html\">Main URL</a><br>"
                            "<a href=\"\">SOURCE Linspire, Inc.</a><br><br>"
                        "</span>"
                    "</td></tr>"
                    "<tr><td>"
                        "<span class=\"date\"><b><i>15 Jan 2007 08:53</i></b></span>"
                        "<span>"
                            "<b>Samba's Tridge clusters code and crowds</b>"
                            "<br>TEST Text 4<br>"
                            "<a href=\"http://test_href_2.html\">Main URL</a><br>"
                            "<a href=\"http://test_href_3.html\">Dahna McConnachie</a><br><br>"
                        "</span>"
                    "</td></tr>"
                "</table>"
            "</div>"
        "</body>"
    "</html>";
 


class FileContent {
    std::string _content;
    
public:
    FileContent(const std::string &file) {
        if (base::bfs::exists(base::bfs::path(file))) {
            std::ifstream ifs(file.c_str());
            _content = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        }
    }
    
    operator const std::string& () {
        return _content;
    }
};


struct TestGenerate {
    TestGenerate(const base::bfs::path &cur_path) {
        tmplt::FileSaver file_tmpl(TEST_TMPL, sizeof(TEST_TMPL) - 1, (cur_path / "test.tmpl").string());
        tmplt::FileSaver file_json(TEST_JSON, sizeof(TEST_JSON) - 1, (cur_path / "test.json").string());
        http_server::HtmlMaker make((cur_path / "test.html").string());
        std::string file_make = make;
        FileContent fcont(file_make);
        BOOST_CHECK_EQUAL(std::string(fcont), std::string(RESULT));
    }
};


BOOST_AUTO_TEST_CASE(TestCtppHttpGenerate) {
    LOG_TO_STDOUT;
    base::bfs::path cur_path = base::bfs::absolute(base::bfs::current_path());
   
    if (base::bfs::exists(cur_path / "test.html")) {
        LOG(DEBUG) << "Remove `" << (cur_path / "test.html") << "`";
        base::bfs::remove(cur_path / "test.html");
    }
    TestGenerate tgen(cur_path);
}

BOOST_AUTO_TEST_CASE(TestCtppHttpUpdate) {
    LOG_TO_STDOUT;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    base::bfs::path cur_path = base::bfs::absolute(base::bfs::current_path());

    if (not base::bfs::exists(cur_path / "test.html")) {
        BOOST_ERROR( "Old version file is not exists." );        
    }
    TestGenerate tgen(cur_path);
}
