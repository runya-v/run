/*!
 * \brief  Генератор данных на основе скомпилированного шаблона и ресурсных данных.
 * \author Rostislav Velichko.
 * \date   05.11.2013
 */

#include <boost/format.hpp>

#include <ctpp2/CTPP2Parser.hpp>
#include <ctpp2/CTPP2ParserException.hpp>
#include <ctpp2/CTPP2HashTable.hpp>
#include <ctpp2/CTPP2VMDumper.hpp>
#include <ctpp2/CTPP2FileSourceLoader.hpp>
#include <ctpp2/CTPP2FileOutputCollector.hpp>
#include <ctpp2/CTPP2VMOpcodes.h>
#include <ctpp2/CTPP2SyscallFactory.hpp>
#include <ctpp2/CTPP2VMFileLoader.hpp>
#include <ctpp2/CTPP2JSONFileParser.hpp>
#include <ctpp2/CTPP2VM.hpp>
#include <ctpp2/CTPP2VMSTDLib.hpp>
#include <ctpp2/CTPP2VMException.hpp>
#include <ctpp2/CTPP2VMStackException.hpp>

#include "Log.hpp"
#include "utility.hpp"
#include "Generator.hpp"

using namespace tmplt;


Generator::Generator(const Compiler &cmplr, const std::string &file_json, const std::string &file_html) {
    CTPP::FileOutputCollector output_collector((CFile(file_html)));
    CTPP::SyscallFactory syscall_factory(100);
    CTPP::STDLibInitializer::InitLibrary(syscall_factory);
    CTPP::VMFileLoader loader(((const std::string&)cmplr).c_str());
    const CTPP::VMMemoryCore *vm_memory_core = loader.GetCore();
    CTPP::CDT hash;
    CTPP::CTPP2JSONFileParser json_file_parser(hash);
    
    try {
        json_file_parser.Parse(file_json.c_str());
        CTPP::VM vm(&syscall_factory);
        vm.Init(vm_memory_core, &output_collector, nullptr);
        std::uint32_t ip = 0;
        vm.Run(vm_memory_core, &output_collector, ip, hash, nullptr);   
        LOG(METHOD, base::Log::Level::INFO, "Generate html `" + file_html + "`.");                                    
    }
    catch(CTPP::CDTTypeCastException &e) { 
        LOG(METHOD, base::Log::Level::ERROR, std::string("Type Cast ") + e.what());                                    
    }
    catch(CTPP::CDTAccessException &e) { 
        LOG(METHOD, base::Log::Level::ERROR, std::string("Array index out of bounds: %s\n") + e.what());                   
    }
    catch(CTPP::IllegalOpcode &e) { 
        boost::format f = boost::format("Illegal opcode 0x%08X at 0x%08X") % e.GetOpcode() % e.GetIP();
        LOG(METHOD, base::Log::Level::ERROR, f.str()); 
    }
    catch(CTPP::InvalidSyscall &e) {
        if (e.GetIP() not_eq 0) {
            CTPP::VMDebugInfo debug_info(e.GetDebugInfo());
            boost::format f = boost::format("%s at 0x%08X (Template file \"%s\", Line %d, Pos %d)")
                % e.what() % e.GetIP() % e.GetSourceName() % debug_info.GetLine() % debug_info.GetLinePos();
            LOG(METHOD, base::Log::Level::ERROR, f.str());
        }
        else {
            LOG(METHOD, base::Log::Level::ERROR, std::string("Unsupported syscall: ") + e.what());
        }
    }
    catch(CTPP::InvalidCall &e) {
        CTPP::VMDebugInfo debug_info(e.GetDebugInfo());
        boost::format f = boost::format("at 0x%08X: Invalid block name \"%s\" in file \"%s\", Line %d, Pos %d")
            % e.GetIP() % e.what() % e.GetSourceName() % debug_info.GetLine() % debug_info.GetLinePos();
        LOG(METHOD, base::Log::Level::ERROR, f.str());
    }
    catch(CTPP::CodeSegmentOverrun &e) { 
        boost::format f = boost::format("%s at 0x%08X") % e.what() % e.GetIP();
        LOG(METHOD, base::Log::Level::ERROR, f.str());
    }
    catch(CTPP::StackOverflow &e) { 
        boost::format f = boost::format("Stack overflow at 0x%08X") % e.GetIP();
        LOG(METHOD, base::Log::Level::ERROR, f.str());
    }
    catch(CTPP::StackUnderflow &e) { 
        boost::format f = boost::format("Stack underflow at 0x%08X") % e.GetIP();
        LOG(METHOD, base::Log::Level::ERROR, f.str());
    }
    catch(CTPP::ExecutionLimitReached &e) { 
        boost::format f = boost::format("Execution limit reached at 0x%08X") % e.GetIP();
        LOG(METHOD, base::Log::Level::ERROR, f.str());
    }
    catch(CTPP::VMException &e) { 
        boost::format f = boost::format("VM generic exception: %s at 0x%08X") % e.what() % e.GetIP();
        LOG(METHOD, base::Log::Level::ERROR, f.str());
    }
    catch(CTPP::CTPPLogicError &e) { 
        LOG(METHOD, base::Log::Level::ERROR, e.what());                                              
    }
    catch(CTPP::CTPPUnixException &e) { 
        boost::format f = boost::format("I/O in %s: %s") % e.what() % strerror(e.ErrNo());
        LOG(METHOD, base::Log::Level::ERROR, f.str());
    }
    catch(CTPP::CTPPException &e) { 
        LOG(METHOD, base::Log::Level::ERROR, std::string("CTPP Generic exception: ") + e.what());                      
    }
    catch(...) {
        LOG(METHOD, base::Log::Level::ERROR, "Undefined.");
    }
    CTPP::STDLibInitializer::DestroyLibrary(syscall_factory);
}
