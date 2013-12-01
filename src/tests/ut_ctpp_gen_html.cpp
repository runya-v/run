#ifndef BOOST_STATIC_LINK
#   define BOOST_TEST_DYN_LINK
#endif // BOOST_STATIC_LINK

#define BOOST_TEST_MODULE CtppHtml
#define BOOST_AUTO_TEST_MAIN

#include <string> 
#include <fstream>
#include <stdexcept>

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


const char NEW_LIST_TMPL[] = 
    "<html>" 
    "    <head>" 
    "        <title>News List</title>" 
    "        <style type=\"text/css\">" 
    "            a {" 
    "              font-family: Verdana, Arial, Helvetica, sans-serif;" 
    "              color: black; font-size: 8pt; font-weight: bold;" 
    "            }" 
    "            span {" 
    "              font-family: Verdana, Arial, Helvetica, sans-serif;" 
    "              color: black; font-size: 10pt; font-weight: normal;" 
    "            }" 
    "            span.date {" 
    "              font-family: Verdana, Arial, Helvetica, sans-serif;" 
    "              color: #D00000; font-size: 10pt; font-weight: normal;" 
    "            }" 
    "        </style>" 
    "    </head>" 
    "    <body>" 
    "        <TMPL_if newslist>" 
    "        <div align=\"center\" style=\"width=100%\">" 
    "        <table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"96%\">" 
    "        <TMPL_loop newslist>" 
    "            <tr><td>" 
    "                <span class=\"date\"><b><i><TMPL_var DATE_FORMAT(date, \"%d %b %Y %H:%M\")>" 
    "                </i></b></span>" 
    "                <span><b><TMPL_var title></b><br><TMPL_var body><br>" 
    "                <TMPL_if publication>" 
    "                    <a href=\"<TMPL_var publication>\">Main URL</a><br>" 
    "                </TMPL_if>" 
    "                <TMPL_if reporter>" 
    "                    <a href=\"<TMPL_var reporter_url>\"><TMPL_var reporter></a><br>" 
    "                </TMPL_if>" 
    "                <br>" 
    "                </span>" 
    "            </td></tr>" 
    "        </TMPL_loop>" 
    "        </table>" 
    "        </div>" 
    "        </TMPL_if>" 
    "    </body>" 
    "</html>";


const char NEW_LIST_JSON[] = 
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

const char RESULT_JSON[] = 
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
            "<div align=\"center\" style=\"width=100%\">"        
                "<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"96%\">"   
                    "<tr>"
                        "<td>"                
                            "<span class=\"date\"><b><i>18 Jan 2007 06:20                </i></b>"
                            "</span>"                
                            "<span><b>FreeBSD 6.3-RELEASE is now available</b><br>TEST Text 1<br>"               
                            "<br>"                
                            "</span>"            
                        "</td>"
                    "</tr>"                    
                    "<tr>"
                        "<td>"                
                            "<span class=\"date\"><b><i>17 Jan 2007 10:42                </i></b>"
                            "</span>"                
                            "<span><b>New RSS Feeds</b><br>TEST Text 2<br>"                                                
                            "<br>"                
                            "</span>"            
                        "</td>"
                    "</tr>"                    
                    "<tr>"
                        "<td>"                
                            "<span class=\"date\"><b><i>17 Jan 2007 08:07                </i></b>"
                            "</span>"                
                            "<span><b>$199 Linux PC Now Available at Sears.com</b><br>TEST Text 3<br>"                                    
                            "<a href=\"http://test_href_1.html\">Main URL</a><br>"                                                    
                            "<a href="">SOURCE Linspire, Inc.</a><br>"                                
                            "<br>"                
                            "</span>"            
                        "</td>"
                    "</tr>"                    
                    "<tr>"
                        "<td>"                
                            "<span class=\"date\"><b><i>15 Jan 2007 08:53                </i></b>"
                            "</span>"                
                            "<span><b>Samba\'s Tridge clusters code and crowds</b><br>TEST Text 4<br>"                                    
                            "<a href=\"http://test_href_2.html\">Main URL</a><br>"                                                    
                            "<a href=\"http://test_href_3.html\">Dahna McConnachie</a><br>"                                
                            "<br>"                
                            "</span>"            
                        "</td>"
                    "</tr>"                
                "</table>"        
            "</div>"            
        "</body>"
    "</html>";


class TestSaveFile {
    base::bfs::path _path;
    
public:
    TestSaveFile(const char *source, const uint32_t size, const std::string &file_name, bool binary = false)
        : _path(file_name)
    {
        LOG(INFO) << "Create: `" << _path << "`.";
        std::ofstream ofs;
        
        if (binary) {
            ofs.open(_path.string(), std::ios::binary);
        }
        else {
            ofs.open(_path.string());
        }

        if (ofs.is_open()) {
            ofs.write(source, size);
        }
        else {
            throw std::runtime_error("Can`t create file: `" + file_name + "`.");
        }
    }


    ~TestSaveFile() {
        if (base::bfs::exists(_path)) {
            base::bfs::remove(_path);
            LOG(INFO) << "Delete: `" << _path << "`.";
        }
    }
    
    operator std::string () {
        return _path.string();
    }
};


typedef std::vector<char> DataBuf;


class CompileTemplate {
    DataBuf _result;
    
public:
    CompileTemplate(const std::string &file_source) {
        CTPP::VMOpcodeCollector vm_opcode_collector;
        CTPP::StaticText sys_calls;
        CTPP::StaticData static_data;
        CTPP::StaticText static_text;
        CTPP::HashTable hash_table;
        CTPP::CTPP2Compiler compiler(vm_opcode_collector, sys_calls, static_data, static_text, hash_table);

        try {
            CTPP::CTPP2FileSourceLoader source_loader;
            source_loader.LoadTemplate(file_source.c_str());
            CTPP::CTPP2Parser parser(&source_loader, &compiler, file_source);
            parser.Compile();
        }
        catch(CTPP::CTPPLogicError &e) {
            //LOG(METHOD, base::Log::Level::ERROR, std::string("CTPPLogicError: ") + e.what());
        }
        catch(CTPP::CTPPUnixException &e) {
            //LOG(METHOD, base::Log::Level::ERROR, std::string("CTPPUnixException: ") + std::string(e.what()) + strerror(e.ErrNo()));
        }
        catch(CTPP::CTPPParserSyntaxError &e) {
            boost::format f = boost::format("At line %d, pos. %d: %s")
                % e.GetLine()
                % e.GetLinePos()
                % e.what()
                ;
            //LOG(METHOD, base::Log::Level::ERROR, std::string("CTPPParserSyntaxError: ") + f.str());
        }
        catch (CTPP::CTPPParserOperatorsMismatch &e) {
            boost::format f = boost::format("At line %d, pos. %d: expected %s, but found </%s>")
                % e.GetLine() 
                % e.GetLinePos()
                % e.Expected()
                % e.Found()
                ;
            //LOG(METHOD, base::Log::Level::ERROR, std::string("CTPPParserOperatorsMismatch: ") + f.str());
        }
        catch(...) {
            //LOG(METHOD, base::Log::Level::ERROR, std::string("undefined: ") + "Bad thing happened.");
        }
        
        uint32_t code_size = 0;
        const CTPP::VMInstruction *vm_instruction = vm_opcode_collector.GetCode(code_size);
        CTPP::VMDumper dumper(code_size, vm_instruction, sys_calls, static_data, static_text, hash_table);
        uint32_t size = 0;
        const CTPP::VMExecutable *program_core = dumper.GetExecutable(size);
        const uint8_t *write_ptr = reinterpret_cast<const uint8_t*>(program_core);
        _result.assign(write_ptr, write_ptr + size);
    }
    
    operator const DataBuf&() {
        return _result;
    }
};


class CFileHeader 
    : boost::noncopyable
{
    FILE *_file;
    
public:
    CFileHeader(const std::string &file_name) 
        : _file(fopen(file_name.c_str(), "w"))
    {
        if (nullptr == _file) {
            throw std::runtime_error("Can`t open redirect file `" + file_name + "`.");
        }
    }
    
    ~CFileHeader() {
        fclose(_file);
    }
    
    operator FILE* () {
        return _file;   
    }
};


class Generate {
public:
    Generate(const std::string &file_ct2, const std::string &file_json, const std::string &file_html) {
        LOG(DEBUG) << "file_ct2: " << file_ct2 << " file_json: " << file_json << " file_html: " << file_html;
        CFileHeader cfile(file_html);
        CTPP::FileOutputCollector output_collector(cfile);
        CTPP::SyscallFactory syscall_factory(100);
        CTPP::STDLibInitializer::InitLibrary(syscall_factory);
        CTPP::VMFileLoader loader(file_ct2.c_str());
        const CTPP::VMMemoryCore *vm_memory_core = loader.GetCore();
        CTPP::CDT hash;
        CTPP::CTPP2JSONFileParser json_file_parser(hash);

        try {
            json_file_parser.Parse(file_json.c_str());
            CTPP::VM vm(&syscall_factory);
            vm.Init(vm_memory_core, &output_collector, nullptr);
            std::uint32_t ip = 0;
            vm.Run(vm_memory_core, &output_collector, ip, hash, nullptr);   
            LOG(INFO) << "Generate: `" << file_html << "`.";
        }
        catch(CTPP::CDTTypeCastException &e) { 
            //LOG(METHOD, base::Log::Level::ERROR, std::string("Type Cast ") + e.what());                                    
        }
        catch(CTPP::CDTAccessException &e) { 
            //LOG(METHOD, base::Log::Level::ERROR, std::string("Array index out of bounds: %s\n") + e.what());                   
        }
        catch(CTPP::IllegalOpcode &e) { 
            boost::format f = boost::format("Illegal opcode 0x%08X at 0x%08X") % e.GetOpcode() % e.GetIP();
            //LOG(METHOD, base::Log::Level::ERROR, f.str()); 
        }
        catch(CTPP::InvalidSyscall &e) {
            if (e.GetIP() not_eq 0) {
                CTPP::VMDebugInfo debug_info(e.GetDebugInfo());
                boost::format f = boost::format("%s at 0x%08X (Template file \"%s\", Line %d, Pos %d)")
                    % e.what() % e.GetIP() % e.GetSourceName() % debug_info.GetLine() % debug_info.GetLinePos();
                //LOG(METHOD, base::Log::Level::ERROR, f.str());
            }
            else {
                //LOG(METHOD, base::Log::Level::ERROR, std::string("Unsupported syscall: ") + e.what());
            }
        }
        catch(CTPP::InvalidCall &e) {
            CTPP::VMDebugInfo debug_info(e.GetDebugInfo());
            boost::format f = boost::format("at 0x%08X: Invalid block name \"%s\" in file \"%s\", Line %d, Pos %d")
                % e.GetIP() % e.what() % e.GetSourceName() % debug_info.GetLine() % debug_info.GetLinePos();
            //LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        catch(CTPP::CodeSegmentOverrun &e) { 
            boost::format f = boost::format("%s at 0x%08X") % e.what() % e.GetIP();
            //LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        catch(CTPP::StackOverflow &e) { 
            boost::format f = boost::format("Stack overflow at 0x%08X") % e.GetIP();
            //LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        catch(CTPP::StackUnderflow &e) { 
            boost::format f = boost::format("Stack underflow at 0x%08X") % e.GetIP();
            //LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        catch(CTPP::ExecutionLimitReached &e) { 
            boost::format f = boost::format("Execution limit reached at 0x%08X") % e.GetIP();
            //LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        catch(CTPP::VMException &e) { 
            boost::format f = boost::format("VM generic exception: %s at 0x%08X") % e.what() % e.GetIP();
            //LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        catch(CTPP::CTPPLogicError &e) { 
            //LOG(METHOD, base::Log::Level::ERROR, e.what());                                              
        }
        catch(CTPP::CTPPUnixException &e) { 
            boost::format f = boost::format("I/O in %s: %s") % e.what() % strerror(e.ErrNo());
            //LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        catch(CTPP::CTPPException &e) { 
            //LOG(METHOD, base::Log::Level::ERROR, std::string("CTPP Generic exception: ") + e.what());                      
        }
        catch(...) {
            //LOG(METHOD, base::Log::Level::ERROR, "Undefined.");
        }
        
        CTPP::STDLibInitializer::DestroyLibrary(syscall_factory);
    }
};


BOOST_AUTO_TEST_CASE(TestCtppHttpGenerate) {
    base::Singleton<base::Log>::getInstance()->init(true, false);
    base::bfs::path cur_path = base::bfs::absolute(base::bfs::current_path());
    TestSaveFile file_json(NEW_LIST_JSON, sizeof(NEW_LIST_JSON) - 1, (cur_path / "new_list.json").string());
    TestSaveFile file_tmpl(NEW_LIST_TMPL, sizeof(NEW_LIST_TMPL) - 1, (cur_path / "new_list.tmpl").string());
    CompileTemplate compile(file_tmpl);
    TestSaveFile file_ct2(&((DataBuf)compile)[0], ((DataBuf)compile).size(), (cur_path / "new_list.ct2").string(), true);
    Generate gen(file_ct2, file_json, (cur_path / "new_list.html").string());
}
