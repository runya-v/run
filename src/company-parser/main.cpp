/*!
 * \brief  Web сервер: Реализует парсинг компаний и продуктов.
 * \author Rostislav Velichko.
 * \date   26.05.2011
 */


// -s -f ../companies2.xml -r ../rubrics2.xml -a

#include <string>
#include <map>
#include <exception>
#include <memory>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

#include "Plugin.hpp"
#include "Error.hpp"
#include "CompanyFileSeparator.hpp"
#include "CompanyParser.hpp"
#include "RubricParser.hpp"


namespace po = boost::program_options;
namespace bfs = boost::filesystem;


static const uint32_t default_max_count = 1000;
static const char default_result_dir_name[] = "result";


class LayoutParser
    : public Plugin
{
public:
    LayoutParser()
    {}

    bool execute() {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h",        "Help information.")
            ("separate,s",    "Separate input company file.")
            ("all,a",         "Convert all files in directory.")
            ("separate-dir,S", po::value< std::string >()->default_value("separated"), "Directory name for separated files.")
            ("file,f",         po::value< std::string >()->default_value("companies2.xml"), "Company file name.")
            ("count,c",        po::value< uint32_t >()->default_value(1000), "Company count in group for separate.")
            ("rubric,r",       po::value< std::string >()->default_value("rubrics2.xml"), "Rubric files name.")
            ("result-dir,R",   po::value< std::string >()->default_value("result"), "Directory name for complete result.")
            ;

        po::variables_map vm;
        po::store(po::parse_command_line(Plugin::_argc, Plugin::_argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::clog << desc << std::endl;
        }
        else {
            try {
                if (not vm.count("separate") and not vm.count("all")) {
                    std::clog << desc << std::endl;
                    return EXIT_SUCCESS;
                }

                if (vm.count("file")) {
                    if (vm.count("separate")) {
                        bfs::path sdir = bfs::absolute(bfs::path(vm["separate-dir"].as< std::string >()));

                        if (not bfs::exists(sdir)) {
                            bfs::create_directory(sdir);
                        }

                        NParser::CompanyFileSeparator(
                            vm["file"].as< std::string >(),
                            vm["separate-dir"].as< std::string >(),
                            vm["count"].as< std::uint32_t >()
                            );
                        std::clog << "Separate complete.\n" << std::flush;
                    }
                    else {
                        NParser::PRubricParser rubrics(
                            new NParser::RubricParser(vm["rubric"].as< std::string >())
                            );
                        NParser::CompanyParser(
                            vm["file"].as< std::string >(),
                            rubrics,
                            vm["result-dir"].as< std::string >()
                            );
                        std::clog << "Parse one file complete.\n" << std::flush;
                    }
                }

                if (vm.count("all")) {
                    bfs::path pdir =
                        bfs::absolute(bfs::path(vm["separate-dir"].as< std::string >()));

                    if (bfs::exists(pdir)) {
                        bfs::directory_iterator itdir(pdir);
                        bfs::directory_iterator eod;
                        std::vector< std::string > paths;

                        BOOST_FOREACH(bfs::path const& it, std::make_pair(itdir, eod)) {
                            paths.push_back(it.string());
                        }

                        uint32_t paths_size = paths.size();
                        uint32_t counter = paths_size;

                        std::shared_ptr< NParser::RubricParser > rubrics(
                            new NParser::RubricParser(vm["rubric"].as< std::string >())
                            );
                        std::string result_path =
                            bfs::absolute(bfs::path(vm["result-dir"].as< std::string >())).string();

                        BOOST_FOREACH(const std::string& sep_dir, paths) {
                            NParser::CompanyParser(sep_dir, rubrics, result_path);
                            std::clog
                                << "# Parse: " << ((paths_size - (--counter)) * 100 / paths_size)
                                << "% \n";
                        }
                        //NParser::CloseParsig();
                        std::clog << "All parse complete.\n" << std::flush;
                    }
                    else {
                        std::cerr << "Can`t find dir: `" << vm["separate-dir"].as< std::string >() << "`\n";
                    }
                }
            }
            catch (const NParser::Stop& e) {
                std::cerr << "Is Stopped.\n";
            }
            catch (const Error& e) {
                std::cerr << e.what() << "\n";
            }
            catch (const bfs::filesystem_error& e) {
                std::cerr << e.what() << "\n";
            }
            catch (const std::exception& e) {
                std::cerr << e.what() << "\n";
            }
        }
        return true;
    }
} company_parser;
