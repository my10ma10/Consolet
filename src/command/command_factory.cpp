#include "command_factory.hpp"

std::vector<std::unique_ptr<ICommand>> 
    CommandFactory::createForNumberedCI(Client& client, NumberedCI& ci)
{
    std::vector<std::unique_ptr<ICommand>> res;
    
    res.emplace_back(std::make_unique<NumberedSendMsgCommand>(client, ci));

    return res;
}

std::vector<std::unique_ptr<ICommand>> 
    CommandFactory::createForSlashedCI(Client& client, SlashedCI& ci)
{
    std::vector<std::unique_ptr<ICommand>> res;
    
    res.emplace_back(std::make_unique<SlashedSendMsgCommand>(client, ci));

    return res;
}
