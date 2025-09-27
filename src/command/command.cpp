#include "command.hpp"

CommandParser::CommandParser(std::unique_ptr<ICommand> cmd)
    : command(std::move(cmd))
    {}


MsgCommand::MsgCommand(const std::string& name, const std::string& text)
    : _msg(name, text)
    {}


