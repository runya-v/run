#ifndef BOOST_STATIC_LINK
#   define BOOST_TEST_DYN_LINK
#endif // BOOST_STATIC_LINK

#define BOOST_TEST_MODULE ForexPrj
#define BOOST_AUTO_TEST_MAIN

#define THREAD_POOL_STATISTICS

#include <string> 

#include <boost/test/output_test_stream.hpp>
#include <boost/format.hpp>

#include <CTPP2.hpp>

#include "Log.hpp"


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


const std::string NEW_LIST_HTML = 
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


class CompileTemplate {
    std::vector<uint8_t> _result;
    
public:
    CompileTemplate(const std::string &source) {
        VMOpcodeCollector vm_opcode_collector;
        StaticText sys_calls;
        StaticData static_data;
        StaticText static_text;
        HashTable hash_table;
        CTPP2Compiler compiler(vm_opcode_collector, sys_calls, static_data, static_text, hash_table);

        try {
            CTPP2FileSourceLoader source_loader;
            source_loader.LoadTemplate(source);
            CTPP2Parser parser(&source_loader, &compiler, source);
            parser.Compile();
        }
        catch(CTPPLogicError &e) {
            LOG(METHOD, base::Log::Level::ERROR, e.what());
        }
        catch(CTPPUnixException &e) {
            LOG(METHOD, base::Log::Level::ERROR, std::string(e.what()) + strerror(e.ErrNo()));
        }
        catch(CTPPParserSyntaxError &e) {
            boost::format f = boost::format("At line %d, pos. %d: %s")
                % e.GetLine()
                % e.GetLinePos()
                % e.what()
                ;
            LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        catch (CTPPParserOperatorsMismatch &e) {
            boost::format f = boost::format("At line %d, pos. %d: expected %s, but found </%s>")
                % e.GetLine() 
                % e.GetLinePos()
                % e.Expected()
                % e.Found()
                ;
            LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        catch(...) {
            LOG(METHOD, base::Log::Level::ERROR, "Bad thing happened.");
        }

        UINT_32 code_size = 0;
        const VMInstruction *vm_instruction = vm_opcode_collector.GetCode(code_size);
        VMDumper dumper(code_size, vm_instruction, sys_calls, static_data, static_text, hash_table);
        UINT_32 size = 0;
        const VMExecutable *program_core = dumper.GetExecutable(size);
        _result.assign(program_core, program_core + size);
    }
};


class Generate {
    
    
public:
    Generate() {
        // 1. Создаем коллектор результатов. Вывод направляем в STDOUT
        FileOutputCollector oOutputCollector(stdout);
        // 2. Создаем фабрику объектов
        SyscallFactory oSyscallFactory(100);
        // .... и загружаем стандартную библиотеку
        STDLibInitializer::InitLibrary(oSyscallFactory);
        // 3. Загружаем файл с диска
        VMFileLoader oLoader(argv[1]);
        // Получаем образ программы 
        const VMMemoryCore *pVMMemoryCore = oLoader.GetCore();

        /*
        * 4. Заполняем данные, которые хотим вывести.
        *    И если в прошлый раз мы добавляли их вручную,
        *    то сейчас используем для этого JSON
        */
        CDT oHash;
        CTPP2JSONFileParser oJSONFileParser(oHash);
        oJSONFileParser.Parse(argv[2]);

        // 5. Создаем виртуальную машину
        VM oVM(oSyscallFactory);

        // 6. Инициализируем машину для запуска программы
        oVM.Init(oOutputCollector, *pVMMemoryCore);

        // 7. Запускаем программу
        UINT_32 iIP = 0;
        oVM.Run(*pVMMemoryCore, iIP, oHash);   

    }
};


BOOST_AUTO_TEST_CASE(TestPerformance) {
}
