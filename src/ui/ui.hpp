#include <iostream>

class UI {
public:
    virtual ~UI() = default;
    virtual void display() const = 0;
};

class NumberedUI : public UI {
public:
    void display() const override;
};

class CommandUI : public UI {
public:
    void display() const override;
};
