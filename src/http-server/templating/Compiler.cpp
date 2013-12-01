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


Compiler::Compiler(const base::bfs::path &&fswr) {
    CTPP::VMOpcodeCollector vm_opcode_collector;
    CTPP::StaticText sys_calls;
    CTPP::StaticData static_data;
    CTPP::StaticText static_text;
    CTPP::HashTable hash_table;
    CTPP::CTPP2Compiler compiler(vm_opcode_collector, sys_calls, static_data, static_text, hash_table);

    try {
        CTPP::CTPP2FileSourceLoader source_loader;
        source_loader.LoadTemplate(fswr.string().c_str());
        CTPP::CTPP2Parser parser(&source_loader, &compiler, fswr.string());
        parser.Compile();
    }
    catch(CTPP::CTPPLogicError &e) {
        LOG(ERROR) << "CTPPLogicError: " << e.what();
    }
    catch(CTPP::CTPPUnixException &e) {
        LOG(ERROR) << "CTPPUnixException: " << e.what() << strerror(e.ErrNo());
    }
    catch(CTPP::CTPPParserSyntaxError &e) {
        LOG(ERROR) 
            <<  "CTPPParserSyntaxError: At line " << e.GetLine() 
            << ", pos. " << e.GetLinePos()
            << ": " << e.what();
    }
    catch (CTPP::CTPPParserOperatorsMismatch &e) {
        LOG(ERROR) 
            << "CTPPParserOperatorsMismatch: At line " << e.GetLine()
            << ", pos. " << e.GetLinePos()
            << ": expected " << e.Expected()
            << ", but found </" << e.Found() << ">";
    }
    catch(...) {
        LOG(ERROR) << "undefined: Bad thing happened.";
    }
    
    uint32_t code_size = 0;
    const CTPP::VMInstruction *vm_instruction = vm_opcode_collector.GetCode(code_size);
    CTPP::VMDumper dumper(code_size, vm_instruction, sys_calls, static_data, static_text, hash_table);
    uint32_t size = 0;
    const CTPP::VMExecutable *program_core = dumper.GetExecutable(size);
    const char *write_ptr = reinterpret_cast<const char*>(program_core);
    std::string file_ct2 = (fswr.parent_path() / fswr.stem()).string() + ".ct2";
    _result.reset(new FileSaver(write_ptr, size, file_ct2, true));
}


Compiler::operator const std::string&() const {
    return (*_result);
}


Compiler::operator const std::string&() {
    return (*_result);
}
