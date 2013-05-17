#pragma once

#include <string>
#include <cinttypes>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WVBoxLayout>
#include <Wt/WFileUpload>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WProgressBar>
#include <Wt/WBreak>
#include <Wt/WLineEdit>

#include <sys/stat.h>
#include <sys/types.h>

#include "ResultView.hpp"


namespace web {

    static const std::uint32_t LINE_LENGHT = 43;
    static const std::uint32_t INPUT_LINE_LENGHT = 43;
    static const std::uint32_t UPLOADED_FILE_SIZE = 40000000;


    template<class OperationType>
    class CriptView
        : public Wt::WContainerWidget
        , public ResultView
    {
        /// \brief Переменная вертикального распределения виджетов.
        Wt::WVBoxLayout *_vlayout;

        /// \brief Переменна хранения контента.
        Wt::WStackedWidget *_content;

        /// \brief Переменна хранения пути до рабочей директории.
        std::string _abs_path;

        /// \brief Переменна хранения загруженного файла ключа.
        Wt::WFileUpload *_key_file_path;

        /// \brief Переменна хранения загруженного текстового файла.
        Wt::WFileUpload *_text_file_path;

        void onButtonOk() {
            std::stringstream res;

            try {
                if (not _key_file_path->empty() and not _text_file_path->empty()) {
                    res << std::string(OperationType(_key_file_path->spoolFileName(), _text_file_path->spoolFileName()));
                    res << "<br/>";
                }
            }
            catch (...) {
                res << "<p>" << Wt::WString::tr("CannotCropt").toUTF8() << "</p>";
            }
            std::cout << "Result: {\n" << res.str() << "\n}\n";
            ResultView::Init(this, this, res.str());
        }

        void onFileToLarge() {
            std::string str = Wt::WString::tr("FileToLarge").toUTF8();
            ResultView::Init(this, this, str);
        }

    public:
        CriptView(Wt::WStackedWidget *content, const std::string &abs_path)
            : _content(content)
            , _abs_path(abs_path)
        {
            Init();
        }

        void Init() {
            this->clear();
            this->resize(Wt::WLength(100, Wt::WLength::Percentage), Wt::WLength(100, Wt::WLength::Percentage));

            Wt::WTable *table = new Wt::WTable();
            unsigned line_num = 1;

            new Wt::WLabel(Wt::WString::tr("PathToKeyFile"), table->elementAt(line_num, 0));
            table->elementAt(line_num, 0)->resize(Wt::WLength(LINE_LENGHT, Wt::WLength::FontEx), Wt::WLength::Auto);
            _key_file_path = new Wt::WFileUpload(table->elementAt(line_num, 1));
            _key_file_path->setProgressBar(new Wt::WProgressBar());
            _key_file_path->changed().connect(_key_file_path, &Wt::WFileUpload::upload);
            _key_file_path->fileTooLarge().connect(this, &CriptView::onFileToLarge);
            ++line_num;

            new Wt::WLabel(Wt::WString::tr("PathToCriptFile"), table->elementAt(line_num, 0));
            table->elementAt(line_num, 0)->resize(Wt::WLength(LINE_LENGHT, Wt::WLength::FontEx), Wt::WLength::Auto);
            _text_file_path = new Wt::WFileUpload(table->elementAt(line_num, 1));
            _text_file_path->setProgressBar(new Wt::WProgressBar());
            _text_file_path->changed().connect(_text_file_path, &Wt::WFileUpload::upload);
            _text_file_path->fileTooLarge().connect(this, &CriptView::onFileToLarge);

            ++line_num;

            Wt::WPushButton* bt_action = new Wt::WPushButton(Wt::WString::tr("OnButton"), table->elementAt(line_num, 1));
            bt_action->clicked().connect(this, &CriptView::onButtonOk);

            Wt::WVBoxLayout* vlayout = new Wt::WVBoxLayout(this);
            vlayout->addWidget(table, 0);
        }
    };
}
