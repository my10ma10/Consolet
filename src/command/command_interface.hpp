#pragma once
#include <vector>
#include <memory>
#include <string>
#include <variant>

class ICommand;

class CommandFactory;

class CommandInterface {
public:
    CommandInterface();
    virtual ~CommandInterface();

    virtual void display(const std::vector<std::unique_ptr<ICommand>>& commands) const = 0;

    virtual void processChoice(
        const std::vector<std::unique_ptr<ICommand>>& commands, 
        std::variant<std::size_t, std::string> choice) = 0;

    void endWindow();

    virtual std::string help() = 0;

protected:
    std::pair<std::string, std::vector<std::string>> 
            splitCmdString(const std::string& command_str);

};

class NumberedCI : public CommandInterface {
public:
    void display(const std::vector<std::unique_ptr<ICommand>>& commands) const override;
    void processChoice(
        const std::vector<std::unique_ptr<ICommand>>& commands, 
        std::variant<std::size_t, std::string> choice
    ) override;

    std::string help() override;

    ICommand* selectCommand(const std::vector<std::unique_ptr<ICommand>>& commands) const;
};

class SlashedCI : public CommandInterface {
public:
    void display(const std::vector<std::unique_ptr<ICommand>>& commands) const override;
    void processChoice(
        const std::vector<std::unique_ptr<ICommand>>& commands, 
        std::variant<std::size_t, std::string> str
    ) override;

    std::string help() override;

};
