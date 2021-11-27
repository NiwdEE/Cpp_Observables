#ifndef OBSERVABLES_HEADER_D9DA62
#define OBSERVABLES_HEADER_D9DA62

#include <functional>

#define EXP(x, exp) [](auto x){return exp;}


template<typename T>
using Sub = std::function<void(T)>;

template<typename T, typename R>
using Operator = std::function<T(R)>;

template<typename T>
class Subscription;

template<typename T>
class Subject
{
    protected:
        Subscription<T>** mSubs;
        int mNextSubID;
        
        int mSubsAmt;

    public:
        Subject();
        ~Subject();

        Subscription<T>* subscribe(Sub<T>);
        int unsubscribe(int);
        void next(T);

        /* W.I.P. */

        Subject* map(Operator<T, T>);
        Subject* filter(Operator<T, bool>);

        /* ----- */
};


template<typename T>
class BehaviorSubject : public Subject<T>
{
    private:
        T mValue;

    public:
        BehaviorSubject(T);
        // ~BehaviorSubject();

        Subscription<T>* subscribe(Sub<T>);
        T getValue(void);
        void next(T);
        
};

template<typename T>
class Subscription
{
    private:
        int mID;
        Subject<T>* mParent;

        Sub<T> mAction;

    public:
        Subscription(int, Subject<T>*, Sub<T>);
        ~Subscription();

        void call(T);
        int unsubscribe();
        int id();
};


template<typename T>
Subject<T>::Subject()
{
    mSubs = NULL;
    mNextSubID = 1;
    mSubsAmt = 0;
}

template<typename T>
Subject<T>::~Subject()
{
    for(int i = 0; i < mSubsAmt; i++){
        delete mSubs[i];
    }

    free(mSubs);
}

/**
 * @brief Function that subscribe a procedure to a Subject, which will be called on every change
 * 
 * @tparam T Type of the Subject
 * @param func Procedure to subscribe
 * @return int 0 if an error occured - the ID of the subscription otherwise
 */
template<typename T>
Subscription<T>* Subject<T>::subscribe(Sub<T> func)
{
    int SubID = mNextSubID++;
    
    Subscription<T>** newSubs = (Subscription<T>**)malloc(sizeof(Subscription<T>*) * (mSubsAmt+1));

    if(!newSubs){
        return 0;
    }

    for(int i = 0; i < mSubsAmt; i++){
        newSubs[i] = mSubs[i];
    }

    free(mSubs);

    auto Sub = new Subscription<T>(SubID, this, func);
    
    newSubs[mSubsAmt] = Sub;

    mSubs = newSubs;
    mSubsAmt++;

    

    return Sub;
}

template<typename T>
int Subject<T>::unsubscribe(int id)
{
    int i;

    int index = -1;
    for(i = 0; i < mSubsAmt; i++){
        if(mSubs[i]->id() == id){
            index = i;
            break;
        }
    }

    if(index == -1) return 0;


    Subscription<T>** newSubs = (Subscription<T>**)malloc(sizeof(Subscription<T>*) * (mSubsAmt-1));

    if(!newSubs) return 0;

    for(i = 0; i < index; i++){
        newSubs[i] = mSubs[i];
    }

    delete mSubs[index];

    for(i = index+1; i < mSubsAmt; i++){
        newSubs[i-1] = mSubs[i];
    }

    free(mSubs);

    mSubs = newSubs;
    mSubsAmt--;

    return 1;
}

template<typename T>
void Subject<T>::next(T val)
{
    for(int i = 0; i < mSubsAmt; i++){
        mSubs[i]->call(val);
    }
}





template<typename T>
BehaviorSubject<T>::BehaviorSubject(T val)
{
    mValue = val;
}

/**
 * @brief Function that call and subscribe a procedure to a BehaviorSubject, which will be called on every change
 * 
 * @tparam T Type of the BehaviorSubject
 * @param func Procedure to call then subscribe
 * @return int 0 if an error occured - the ID of the subscription otherwise
 */
template<typename T>
Subscription<T>* BehaviorSubject<T>::subscribe(Sub<T> func)
{
    func(mValue);
    return Subject<T>::subscribe(func);
}

/**
 * @brief Getter wich sends the actual value of the BehaviorSubject
 * 
 * @tparam T Type of the BehaviorSubject
 * @return T Value of the BehaviorSubject
 */
template<typename T>
T BehaviorSubject<T>::getValue(void)
{
    return mValue;
}

template<typename T>
void BehaviorSubject<T>::next(T val)
{
    mValue = val;
    Subject<T>::next(val);
}



template<typename T>
Subscription<T>::Subscription(int ID, Subject<T>* parent, Sub<T> action)
{
    mID = ID;
    mParent = parent;
    mAction = action;
}

template<typename T>
Subscription<T>::~Subscription()
{
}


template<typename T>
int Subscription<T>::unsubscribe()
{
    return mParent->unsubscribe(mID);
}

template<typename T>
void Subscription<T>::call(T val)
{
    mAction(val);
}

template<typename T>
int Subscription<T>::id()
{
    return mID;
}


#endif