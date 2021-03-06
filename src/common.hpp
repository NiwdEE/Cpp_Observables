#ifndef OBSERVABLES_COMMON_HEADER_D9DA62
#define OBSERVABLES_COMMON_HEADER_D9DA62

#include <functional>

#define EXP(x, exp) [](auto x){return exp;}


template<typename T>
using Procedure = std::function<void(T)>;

template<typename T, typename R>
using Operator = std::function<T(R)>;

template<class Derived, typename T>
class CRTPI_Observable;

template<typename T>
class Observable;



template<class Derived, typename T>
class CRTPI_Subject;

template<typename T>
class Subject;



template<class Derived, typename T>
class CRTPI_BehaviorSubject;

template<typename T>
class BehaviorSubject;


template<class D, typename T>
class Subscription;

/**
 * @brief /!\ Made to be used inside of Subject->pipe() /!\
 * 
 * Transforms data using the given function
 * 
 * @tparam T Data type of input/output
 * @param func The transformation function 
 */
template<typename T, typename R>
Procedure<T*> takeUntil(R cond)
{
    return [=](T* val){
        *val = func(*val);
    };
}

/**
 * @brief /!\ Made to be used inside of Subject->pipe() /!\
 * 
 * Transforms data using the given function
 * 
 * @tparam T Data type of input/output
 * @param func The transformation function 
 */
template<typename T>
Procedure<T*> map(Operator<T, T> func)
{
    return [=](T* val){
        *val = func(*val);
    };
}

/**
 * @brief /!\ Made to be used inside of Subject->pipe() /!\
 * 
 * Filter data using the given expression
 * 
 * @tparam T Data type of input
 * @param exp The condition as expression
 */
template<typename T>
Operator<T*, T*> filter(Operator<T, T> exp)
{
    return [=](T* val){
        if(!exp(*val)) return (T*)NULL;
        return val;
    };
}


/**
 * @brief used for testing only
 */
template<typename T>
Procedure<T*> nullPipe()
{
    return [=](T* val){
        val = val;
    };
}

#endif