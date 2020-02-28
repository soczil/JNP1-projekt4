#ifndef __FIBIN_H__
#define __FIBIN_H__

#include <iostream>
#include <type_traits>
#include <typeinfo>

using namespace std;

const int max_id_len = 6;
const int hash_const = 37;

struct True {};

struct False {};

template<int N>
struct Fib {};

template<typename T>
struct Lit {};

template<typename ... Args>
struct Sum {};

template<typename T>
struct Inc1 {};

template<typename T>
struct Inc10 {};

template<typename T1, typename T2>
struct Eq {};

template<typename Condition, typename Then, typename Else>
struct If {};

template<long long Id, typename Value, typename Expression>
struct Let {};

template<long long Id>
struct Ref {};

template<long long Id, typename Body>
struct Lambda {};

template<typename Fun, typename Param>
struct Invoke {};

// zwraca -1, jeśli napis zawiera złe znaki lub jest za długi
constexpr static long long Var(const char *str) {
    int i = 0, x = 0;
    long long result = 0, pow = 1;

    if ((str == NULL) || (str[i] == '\0')) {
        return -1;
    }

    while ((i < max_id_len) && (str[i] != '\0')) {
        x = (int) str[i];

        if (x >= 'A' && x <= 'Z') {
            x -= 'A';
            x += 1;
        } else if (x >= 'a' && x <= 'z') {
            x -= 'a';
            x += 1;
        } else if (x >= '0' && x <= '9') {
            x -= '0';
            x += ('z' - 'a') + 1;
        } else {
            return -1;
        }

        result += x * pow;
        pow *= hash_const;
        i++;
    }

    if (str[i] != '\0') {
        return -1;
    }

    return result;
}

template<typename ValueType>
struct Fibin {
private:
    template<typename Expression, typename Env>
    struct Expr {};

    // oblicza dokładną wartość wyrażenia
    template<typename Expression, typename Env>
    struct Count {};

    template<typename ... Args>
    struct AssistantSum {};

    // konwertuje wartość logiczną na fibinowego boola
    template<bool Value>
    struct Fibool {};

    struct EmptyEnv {};

    // łączy identyfikator zmiennej z jej wartością
    template<long long Id, typename Value, typename Env>
    struct Connect {};

    // szuka wartości zmiennej o danym identyfikatorze w środowisku
    template<long long Id, typename Env>
    struct FindInEnv {};

    // aplikuje wyrażenie do wartości
    template<typename Fun, typename Value>
    struct Apply {};

    // zamyka w sobie lambde
    template<typename Lambda, typename Env>
    struct Closure {};

    template<long long Id>
    struct FindInEnv<Id, EmptyEnv> {};

    template<long long Id, typename Value, typename Env>
    struct FindInEnv<Id, Connect<Id, Value, Env>> {
        using val = Value;
    };

    template<long long Id, long long Id2, typename Value2, typename Env>
    struct FindInEnv<Id, Connect<Id2, Value2, Env>> {
        using val = typename FindInEnv<Id, Env>::val;
    };

    template<typename Env>
    struct Count<Lit<Fib<0>>, Env> {
        constexpr static ValueType val = 0;
    };

    template<typename Env>
    struct Count<Lit<Fib<1>>, Env> {
        constexpr static ValueType val = 1;
    };

    template<int N, typename Env>
    struct Count<Lit<Fib<N>>, Env> {
        constexpr static ValueType val = Count<Lit<Fib<N - 1>>, Env>::val
                                         + Count<Lit<Fib<N - 2>>, Env>::val;
    };

    template<int N, typename Env>
    struct Expr<Lit<Fib<N>>, Env> {
        static_assert(N >= 0);
        using val = Count<Lit<Fib<N>>, Env>;
    };

    template<typename Env>
    struct Expr<Lit<True>, Env> {
        using val = Lit<True>;
    };

    template<typename Env>
    struct Expr<Lit<False>, Env> {
        using val = Lit<False>;
    };

    template<typename Env>
    struct Expr<Fibool<true>, Env> {
        using val = Lit<True>;
    };

    template<typename Env>
    struct Expr<Fibool<false>, Env> {
        using val = Lit<False>;
    };

    template<typename T1, typename T2, typename Env>
    struct Expr<Eq<T1, T2>, Env> {
        using val = typename Expr<Fibool<Expr<T1, Env>::val::val
                                         ==
                                         Expr<T2, Env>::val::val>, Env>::val;
    };

    template<typename Then, typename Else, typename Env>
    struct Expr<If<Lit<True>, Then, Else>, Env> {
        using val = typename Expr<Then, Env>::val;
    };

    template<typename Then, typename Else, typename Env>
    struct Expr<If<Lit<False>, Then, Else>, Env> {
        using val = typename Expr<Else, Env>::val;
    };

    template<typename Condition, typename Then, typename Else, typename Env>
    struct Expr<If<Condition, Then, Else>, Env> {
        using val = typename Expr<If<typename Expr<Condition, Env>::val, Then, Else>, Env>::val;
    };

    template<typename T, typename Env>
    struct Count<Inc1<T>, Env> {
        constexpr static ValueType val = Expr<T, Env>::val::val
                                         + Count<Lit<Fib<1>>, Env>::val;
    };

    template<typename T, typename Env>
    struct Expr<Inc1<T>, Env> {
        using val = Count<Inc1<T>, Env>;
    };

    template<typename T, typename Env>
    struct Count<Inc10<T>, Env> {
        constexpr static ValueType val = Expr<T, Env>::val::val
                                         + Count<Lit<Fib<10>>, Env>::val;
    };

    template<typename T, typename Env>
    struct Expr<Inc10<T>, Env> {
        using val = Count<Inc10<T>, Env>;
    };

    template<typename First, typename Env>
    struct Count<AssistantSum<First>, Env> {
        constexpr static ValueType val = Expr<First, Env>::val::val;
    };

    template<typename First, typename ... NextArgs, typename Env>
    struct Count<AssistantSum<First, NextArgs ...>, Env> {
        constexpr static ValueType val = Expr<First, Env>::val::val
                                         +
                                         Count<AssistantSum<NextArgs ...>, Env>::val;
    };

    template<typename ... Args, typename Env>
    struct Expr<Sum<Args ...>, Env> {
        // muszą być co najmniej dwa argumenty
        static_assert(sizeof...(Args) > 1);
        using val = Count<AssistantSum<Args ...>, Env>;
    };

    template<long long Id, typename Value, typename Expression, typename Env>
    struct Expr<Let<Id, Value, Expression>, Env> {
        static_assert(Id != -1); // czy poprawny identyfikator
        using val = typename Expr<Expression, Connect<Id, Expr<Value, Env>, Env>>::val;
    };

    template<long long Id, typename Env>
    struct Expr<Ref<Id>, Env> {
        static_assert(Id != -1); // czy poprawny identyfikator
        using val = typename FindInEnv<Id, Env>::val::val;
    };

    template<long long Id, typename Body, typename Env>
    struct Expr<Lambda<Id, Body>, Env> {
        using val = Closure<Lambda<Id, Body>, Env>;
    };

    template<long long Id, typename Body, typename Env, typename Value>
    struct Apply<Closure<Lambda<Id, Body>, Env>, Value> {
        using val = Expr<Body, Connect<Id, Value, Env>>;
    };

    template<typename Fun, typename Param, typename Env>
    struct Expr<Invoke<Fun, Param>, Env> {
        using val = typename Apply<typename Expr<Fun, Env>::val, Expr<Param, Env>>::val::val;
    };

public:
    template<typename T, typename U = ValueType,
            typename enable_if<!is_integral<U>::value, int>::type = 0>
    constexpr static void eval() {
        cout << "Fibin doesn't support: " << typeid(ValueType).name() << endl;
    }

    template<typename T, typename U = ValueType,
            typename enable_if<is_integral<U>::value, int>::type = 0>
    constexpr static ValueType eval() {
        return Expr<T, EmptyEnv>::val::val;
    }
};

#endif /* __FIBIN_H__ */
