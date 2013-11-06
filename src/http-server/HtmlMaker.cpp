#include "HtmlMaker.hpp"

using namespace http_server;


HtmlMaker::HtmlMaker(const std::string &need_html_file) {
    //TestSaveFile file_tmpl(NEW_LIST_TMPL, sizeof(NEW_LIST_TMPL) - 1, "new_list.tmpl");
    //CompileTemplate compile(file_tmpl);
    //TestSaveFile file_ct2(&((DataBuf)compile)[0], ((DataBuf)compile).size(), "new_list.ct2", true);
    //TestSaveFile file_json(NEW_LIST_JSON, sizeof(NEW_LIST_JSON) - 1, "new_list.json");
    //Generate gen(file_ct2, file_json, "new_list.html");
}


HtmlMaker::operator const std::string&() {
    return _html_path.string();
}
