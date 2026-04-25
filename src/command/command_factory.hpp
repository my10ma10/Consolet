#pragma once
#include <vector>
#include <memory>

#include "command.hpp"
#include "adapters.hpp"
#include "command_interface.hpp"

class CommandFactory {
public:
    static std::vector<std::unique_ptr<ICommand>> 
        createForNumberedCI(ClientSession& session, NumberedCI& ci);

    static std::vector<std::unique_ptr<ICommand>> 
        createForSlashedCI(ClientSession& session, SlashedCI& ci);
};