#pragma once
#include<exception>

class NotImplemented : public std::logic_error
{
private:

    std::string _text;

    NotImplemented(const char* message,const char* function)
        :
        std::logic_error("Not Implemented")
    {
        _text = message;
        _text += " : ";
        _text += function;
    };

public:

    NotImplemented()
        :
        NotImplemented("Not Implememented",__FUNCTION__)
    {
    }

    NotImplemented(const char* message)
        :
        NotImplemented(message,__FUNCTION__)
    {
    }

    virtual const char* what() const throw()
    {
        return _text.c_str();
    }
};