#include "command_factory.hpp"

std::vector<std::unique_ptr<ICommand>> 
    CommandFactory::createForNumberedCI(ClientSession& session, NumberedCI& ci)
{
    std::vector<std::unique_ptr<ICommand>> res;
    
    res.emplace_back(std::make_unique<NumberedSendMsgCommand>(session, ci));

    return res;
}

std::vector<std::unique_ptr<ICommand>> 
    CommandFactory::createForSlashedCI(ClientSession& session, SlashedCI& ci)
{
    std::vector<std::unique_ptr<ICommand>> res;
    
    res.emplace_back(std::make_unique<SlashedSendMsgCommand>(session, ci));

    return res;
}
