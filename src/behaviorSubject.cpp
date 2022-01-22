#ifndef BEHAVIORSUBJECT_HEADER_D9DA62
#define BEHAVIORSUBJECT_HEADER_D9DA62

#include "subject.cpp"
#include "subscription.cpp"

/**
 * @brief Subject wich stores its current value. Needs a default value.
 * 
 * @tparam T Type of the data supposed to be passed to the BehaviorSubject
 */
template<typename T>
class BehaviorSubject : public Subject<T>
{
    private:
        T mValue;

    public:
        BehaviorSubject(T);

        Subscription<T>* subscribe(Procedure<T>);
        T getValue(void);
        void next(T);
        
        template<typename R>
        BehaviorSubject<T>* pipe(R);
};


template<typename T>
BehaviorSubject<T>::BehaviorSubject(T val)
{
    mValue = val;
}

/**
 * @brief Compatibility for Subject::pipe to BehaviorSubjects
 */
template<typename T>
template<typename R>
BehaviorSubject<T>* BehaviorSubject<T>::pipe(R func)
{
    auto clone = static_cast<BehaviorSubject*>(Subject<T>::pipe(func));
    clone->mValue = this->mValue;

    return clone;
}

/**
 * @brief Calls then subscribes a procedure to a BehaviorSubject    
 * 
 * @tparam T Type of the BehaviorSubject
 * @param func Procedure to call then subscribe
 * @return the address of the subscription (NULL if an error occured)
 */
template<typename T>
Subscription<T>* BehaviorSubject<T>::subscribe(Procedure<T> func)
{
    func(mValue);
    return Subject<T>::subscribe(func);
}

/**
 * @brief Sends the current value of the BehaviorSubject
 * 
 * @tparam T Type of the BehaviorSubject
 * @return Value of the BehaviorSubject
 */
template<typename T>
T BehaviorSubject<T>::getValue(void)
{
    return mValue;
}


/**
 * @brief Save then emit a new value to the BehaviorSubject
 * 
 * @tparam T Type of the BehaviorSubject
 * @param val Value to be emitted
 */
template<typename T>
void BehaviorSubject<T>::next(T val)
{
    mValue = val;
    Subject<T>::next(val);
}

#endif