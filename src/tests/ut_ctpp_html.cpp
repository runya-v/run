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

#include "Log.hpp"


const std::string NEW_LIST_TMPL = 
    "<html>" \
    "    <head>" \
    "        <title>News List</title>" \
    "        <style type=\"text/css\">" \
    "            a {" \
    "              font-family: Verdana, Arial, Helvetica, sans-serif;" \
    "              color: black; font-size: 8pt; font-weight: bold;" \
    "            }" \
    "            span {" \
    "              font-family: Verdana, Arial, Helvetica, sans-serif;" \
    "              color: black; font-size: 10pt; font-weight: normal;" \
    "            }" \
    "            span.date {" \
    "              font-family: Verdana, Arial, Helvetica, sans-serif;" \
    "              color: #D00000; font-size: 10pt; font-weight: normal;" \
    "            }" \
    "        </style>" \
    "    </head>" \
    "    <body>" \
    "        <TMPL_if newslist>" \
    "        <div align=\"center\" style=\"width=100%\">" \
    "        <table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"96%\">" \
    "        <TMPL_loop newslist>" \
    "            <tr><td>" \
    "                <span class=\"date\"><b><i><TMPL_var DATE_FORMAT(date, \"%d %b %Y %H:%M\")>" \
    "                </i></b></span>" \
    "                <span><b><TMPL_var title></b><br><TMPL_var body><br>" \
    "                <TMPL_if publication>" \
    "                    <a href=\"<TMPL_var publication>\">Main URL</a><br>" \
    "                </TMPL_if>" \
    "                <TMPL_if reporter>" \
    "                    <a href=\"<TMPL_var reporter_url>\"><TMPL_var reporter></a><br>" \
    "                </TMPL_if>" \
    "                <br>" \
    "                </span>" \
    "            </td></tr>" \
    "        </TMPL_loop>" \
    "        </table>" \
    "        </div>" \
    "        </TMPL_if>" \
    "    </body>" \
    "</html>";


const std::string NEW_LIST_JSON = 
    "{\"newslist\" : [" \
    "{   \"date\"  : 1169090455," \
        "\"title\" : \"FreeBSD 6.3-RELEASE is now available\"," \
        "\"body\"  : \"Please .......\" }," \
    "{   \"date\"  : 1169019722," \
        "\"title\" : \"New RSS Feeds\"," \
        "\"body\"  : \"Six new .......\" }," \
    "{   \"date\"  : 1169010455," \
        "\"title\" : \"$199 Linux PC Now Available at Sears.com\"," \
        "\"body\"  : \"Linspire, Inc ........\"," \
        "\"publication\"  : \"http:// .......\"," \
        "\"reporter\"     : \"SOURCE Linspire, Inc.\" }," \
    "{   \"date\"  : 1168840430," \
        "\"title\" : \"Samba's Tridge clusters code and crowds\"," \
        "\"body\"  : \"Andrew .......\"," \
        "\"publication\"  : \"http:// .......\"," \
        "\"reporter\"     : \"Dahna McConnachie\"," \
        "\"reporter_url\" : \"http:// .......\" } ] }";


class TestSaveFile {
    base::bfs::path _path;
    
public:
    TestSaveFile(const std::string &source, const std::string &file_name)
            : _path(base::bfs::absolute(base::bfs::current_path()) / file_name)
    {
        LOG(METHOD, base::Log::Level::INFO, "Create: `" + _path.string() + "`");
        std::ofstream ofs(_path.string());

        if (ofs.is_open()) {
            ofs << source;
        }
        else {
            throw std::runtime_error("Can`t create file: `" + file_name + "`");
        }
    }


    ~TestSaveFile() {
        if (base::bfs::exists(_path)) {
            base::bfs::remove(_path);
            LOG(METHOD, base::Log::Level::INFO, "Delete: `" + _path.string() + "`");
        }
    }
    
    operator std::string () {
        return _path.string();
    }
};


class CompileTemplate {
    std::vector<uint8_t> _result;
    
public:
    CompileTemplate(const std::string &file_source) {
        LOG(METHOD, base::Log::Level::DEBUG, "Compile: `" + file_source + "`");
        
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
            LOG(METHOD, base::Log::Level::ERROR, std::string("CTPPLogicError: ") + e.what());
        }
        catch(CTPP::CTPPUnixException &e) {
            LOG(METHOD, base::Log::Level::ERROR, std::string("CTPPUnixException: ") + std::string(e.what()) + strerror(e.ErrNo()));
        }
        catch(CTPP::CTPPParserSyntaxError &e) {
            boost::format f = boost::format("At line %d, pos. %d: %s")
                % e.GetLine()
                % e.GetLinePos()
                % e.what()
                ;
            LOG(METHOD, base::Log::Level::ERROR, std::string("CTPPParserSyntaxError: ") + f.str());
        }
        catch (CTPP::CTPPParserOperatorsMismatch &e) {
            boost::format f = boost::format("At line %d, pos. %d: expected %s, but found </%s>")
                % e.GetLine() 
                % e.GetLinePos()
                % e.Expected()
                % e.Found()
                ;
            LOG(METHOD, base::Log::Level::ERROR, std::string("CTPPParserOperatorsMismatch: ") + f.str());
        }
        catch(...) {
            LOG(METHOD, base::Log::Level::ERROR, std::string("undefined: ") + "Bad thing happened.");
        }
        
        uint32_t code_size = 0;
        const CTPP::VMInstruction *vm_instruction = vm_opcode_collector.GetCode(code_size);
        CTPP::VMDumper dumper(code_size, vm_instruction, sys_calls, static_data, static_text, hash_table);
        uint32_t size = 0;
        const CTPP::VMExecutable *program_core = dumper.GetExecutable(size);
        const uint8_t *write_ptr = reinterpret_cast<const uint8_t*>(program_core);
        _result.assign(write_ptr, write_ptr + size);
    }
};


class Generate {
    
    
public:
    Generate() {
        //// 1. Создаем коллектор результатов. Вывод направляем в STDOUT
        //CTPP::FileOutputCollector oOutputCollector(stdout);
        //// 2. Создаем фабрику объектов
        //CTPP::SyscallFactory oSyscallFactory(100);
        //// .... и загружаем стандартную библиотеку
        //CTPP::STDLibInitializer::InitLibrary(oSyscallFactory);
        //// 3. Загружаем файл с диска
        //CTPP::VMFileLoader oLoader(argv[1]);
        //// Получаем образ программы 
        //const CTPP::VMMemoryCore *pVMMemoryCore = oLoader.GetCore();
        //
        ///*
        //* 4. Заполняем данные, которые хотим вывести.
        //*    И если в прошлый раз мы добавляли их вручную,
        //*    то сейчас используем для этого JSON
        //*/
        //CTPP::CDT oHash;
        //CTPP::CTPP2JSONFileParser oJSONFileParser(oHash);
        //oJSONFileParser.Parse(argv[2]);
        //
        //// 5. Создаем виртуальную машину
        //CTPP::VM oVM(*oSyscallFactory);
        //
        //// 6. Инициализируем машину для запуска программы
        //oVM.Init(oOutputCollector, *pVMMemoryCore);
        //
        //// 7. Запускаем программу
        //UINT_32 iIP = 0;
        //oVM.Run(*pVMMemoryCore, iIP, oHash);   

    }
};


BOOST_AUTO_TEST_CASE(TestCtppHttpGenerate) {
    CompileTemplate compile(TestSaveFile(NEW_LIST_TMPL, "new_list.tmpl"));
}
