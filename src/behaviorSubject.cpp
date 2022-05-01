#ifndef BEHAVIORSUBJECT_HEADER_D9DA62
#define BEHAVIORSUBJECT_HEADER_D9DA62

#include "subject.cpp"
#include "subscription.cpp"

/**
 * @brief Subject wich stores its current value. Needs a default value.
 * 
 * @tparam T Type of the data supposed to be passed to the BehaviorSubject
 */
template<class Derived, typename T>
class CRTPI_BehaviorSubject : public CRTPI_Subject<Derived, T>
{
    protected:
        T mValue;
                

        CRTPI_BehaviorSubject<Derived, T>* clone();

        

    public:

        Subscription<Derived, T>* subscribe(Procedure<T>);
        T getValue(void);
        void next(T);

};



template<typename T>
class BehaviorSubject: public CRTPI_BehaviorSubject<BehaviorSubject<T>, T>{
    public:
        BehaviorSubject();
        BehaviorSubject(T);
};


template<typename T>
BehaviorSubject<T>::BehaviorSubject(){
    this->mSubs = NULL;
    this->mNextSubID = 1;
    this->mSubsAmt = 0;

    this->mClones = NULL;
    this->mClonesAmt = 0;

    this->mPiped = false;
    this->mPipe = [](T* a){
        return a;
    };
}


template<typename T>
BehaviorSubject<T>::BehaviorSubject(T val)
{
    this->mValue = val;
}

template<class Derived, typename T>
CRTPI_BehaviorSubject<Derived, T>* CRTPI_BehaviorSubject<Derived, T>::clone()
{
    CRTPI_BehaviorSubject<Derived, T>* clone = CRTPI_Observable<CRTPI_BehaviorSubject<Derived, T>, T>::clone();
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
template<class Derived, typename T>
Subscription<Derived, T>* CRTPI_BehaviorSubject<Derived, T>::subscribe(Procedure<T> func)
{
    func(this->mValue);
    return CRTPI_Subscribable<Derived, T>::subscribe(func);
}

/**
 * @brief Sends the current value of the BehaviorSubject
 * 
 * @tparam T Type of the BehaviorSubject
 * @return Value of the BehaviorSubject
 */
template<class Derived, typename T>
T CRTPI_BehaviorSubject<Derived, T>::getValue(void)
{
    return this->mValue;
}


/**
 * @brief Save then emit a new value to the BehaviorSubject
 * 
 * @tparam T Type of the BehaviorSubject
 * @param val Value to be emitted
 */
template<class Derived, typename T>
void CRTPI_BehaviorSubject<Derived, T>::next(T val)
{
    this->mValue = val;
    CRTPI_Subject<Derived, T>::next(val);
}

#endif