/*!
 * \brief  Кокпилятор шаблона.
 * \author Rostislav Velichko.
 * \date   05.11.2013
 */

#include <boost/format.hpp>

#include <ctpp2/CTPP2Parser.hpp>
#include <ctpp2/CTPP2FileSourceLoader.hpp>
#include <ctpp2/CTPP2FileOutputCollector.hpp>
#include <ctpp2/CTPP2ParserException.hpp>
#include <ctpp2/CTPP2VMDumper.hpp>

#include "Log.hpp"
#include "Compiler.hpp"


using namespace tmplt;


Compiler::Compiler(const std::string &file_source) {
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


Compiler::operator const DataBuf&() {
    return _result;
}
