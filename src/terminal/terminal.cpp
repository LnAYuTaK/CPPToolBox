#include "terminal.h"

#include <sstream>
#include "session_context.h"
#include "dir_node.h"
#include "func_node.h"

Terminal::Terminal()
    :rootNode_(new DirNode("this is root node")){
}

Terminal::~Terminal(){
}

SessionContext* Terminal::newSession() {
    auto session  = new  SessionContext(sessions_.size());
    sessions_.push_back(session);
    return session;
}

bool Terminal::deleteSession(SessionContext *st) {
    // 取出并删除指定元素
    auto it = std::find(sessions_.begin(), sessions_.end(), st);
    if (it != sessions_.end()) {
        auto removeSession = *it;
        sessions_.erase(it);
        delete removeSession;
        removeSession = nullptr;
        return true;
    } else {
          return false ;
    }
}

uint32_t Terminal::getOptions(SessionContext  *st) const
{
    auto s = sessions_.at(st->id());
    if (s == nullptr) {
        return 0;
    }

    return s->options;
}

void Terminal::setOptions(SessionContext  *st, uint32_t options)
{
    auto s = sessions_.at(st->id());
    if (s == nullptr) {
        return;
    }
    s->options = options;
}

bool Terminal::onBegin(SessionContext  *st)
{
    auto s = sessions_.at(st->id());
    if (s == nullptr)
        return false;

    if (!(s->options & kQuietMode)) {
        s->send(
            "\r\n"
            "Welcome to  Terminal.\r\n"
            "Type 'help' for more information.\r\n"
            "\r\n"
        );
    }
    printPrompt(s);

    return true;
}

bool Terminal::onExit(SessionContext  *st)
{
    auto s = sessions_.at(st->id());
    if (s == nullptr)
        return false;

    s->send("Bye!");
    return true;
}

bool Terminal::onRecvString(SessionContext  *st, const std::string &str)
{
    auto s = sessions_.at(st->id());
    if (s == nullptr)
        return false;

    s->key_event_scanner_.start();
    KeyEventScanner::Status status = KeyEventScanner::Status::kUnsure;
    for (char c : str) {
        status = s->key_event_scanner_.next(c);
        if (status == KeyEventScanner::Status::kEnsure) {
            switch (s->key_event_scanner_.result()) {
                case KeyEventScanner::Result::kPrintable:
                    onChar(s, c);
                    break;
                case KeyEventScanner::Result::kEnter:
                    //onEnterKey(s);
                    break;
                case KeyEventScanner::Result::kBackspace:
                    onBackspaceKey(s);
                    break;
                case KeyEventScanner::Result::kTab:
                    onTabKey(s);
                    break;
                case KeyEventScanner::Result::kMoveUp:
                    onMoveUpKey(s);
                    break;
                case KeyEventScanner::Result::kMoveDown:
                    onMoveDownKey(s);
                    break;
                case KeyEventScanner::Result::kMoveLeft:
                    onMoveLeftKey(s);
                    break;
                case KeyEventScanner::Result::kMoveRight:
                    onMoveRightKey(s);
                    break;
                case KeyEventScanner::Result::kHome:
                    onHomeKey(s);
                    break;
                case KeyEventScanner::Result::kEnd:
                    onEndKey(s);
                    break;
                case KeyEventScanner::Result::kDelete:
                    onDeleteKey(s);
                    break;
                default:
                    break;
            }
            s->key_event_scanner_.start();
        }
    }

    if (status == KeyEventScanner::Status::kUnsure) {
        if (s->key_event_scanner_.stop() == KeyEventScanner::Status::kEnsure) {
            switch (s->key_event_scanner_.result()) {
                case KeyEventScanner::Result::kEnter:
                    // onEnterKey(s);
                    break;

                default:
                    break;
            }
        }
    }
    return true;
}

bool Terminal::onRecvWindowSize(SessionContext  *st, uint16_t w, uint16_t h)
{
    auto s = sessions_.at(st->id());
    if (s != nullptr) {
        s->window_width = w;
        s->window_height = h;
        return true;
    }
    return false;
}

void Terminal::printPrompt(SessionContext *s)
{
    if (!(s->options & kQuietMode))
        s->send("# ");
}

void Terminal::printHelp(SessionContext *s)
{
    const char *help_str = \
        "This terminal is only for internal debugging within the company\r\n"
        "It provides a way to interact with the program in the form of shell.\r\n"
        "\r\n"
        "There are some buildin commands:\r\n"
        "\r\n"
        "- tree      List all nodes as tree\r\n"
        "- ls        List nodes under specified path\r\n"
        "- cd        Chang directory\r\n"
        "- pwd       print current path\r\n"
        "- exit|quit Exit this\r\n"
        "- help      Print help of specified node\r\n"
        "- history   List history command\r\n"
        "- !n        Run the n command of history\r\n"
        "- !-n       Run the last n command of history\r\n"
        "- !!        Run last command\r\n"
        "\r\n";
    s->send(help_str);

    if (s->options & kEnableEcho) {
        const char *extra_str = \
            "Besides, UP,DOWN,LEFT,RIGHT,HOME,END,DELETE keys are available.\r\n"
            "Try them.\r\n"
            "\r\n";
         s->send(extra_str);
    }
}
