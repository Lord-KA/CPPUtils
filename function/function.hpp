#pragma once

#include <memory>

namespace g {

template<typename T>
class function;

class mInvoker;

template<typename Ret, typename ...Args>
class function<Ret(Args...)> {
public:
    function() : holder() {}

    template<typename FunctionT>
    function(FunctionT func) : holder(new func_holder<FunctionT>(func)) {}

    Ret operator()(Args... args) {
        return holder->call(args...);
    }

private:
    class func_holder_base {
    public:
        func_holder_base() {}
        virtual ~func_holder_base() {}
        virtual Ret call(Args... args) = 0;
    };

    template<typename FunctionT>
    class func_holder : public func_holder_base {
    public:
        func_holder(FunctionT func) : func_holder_base(), callee(func) {}
        Ret call(Args... args)
        {
            return callee(args...);
        }
    private:
        FunctionT callee;
    };

    std::unique_ptr<func_holder_base> holder;
};

} /* namespace g */
