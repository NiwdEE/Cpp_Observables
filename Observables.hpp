#ifndef OBSERVABLES_HEADER_D9DA62
#define OBSERVABLES_HEADER_D9DA62

#include <functional>

#define EXP(x, exp) [](auto x){return exp;}


template<typename T>
using Sub = std::function<void(T)>;

template<typename T, typename R>
using Operator = std::function<T(R)>;


template<typename T>
class Subject
{
    protected:
        Sub<T>* mSubs;
        int* mIDsMap;
        
        int mSubsAmt;

    public:
        Subject();
        ~Subject();

        int subscribe(Sub<T>);
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

        int subscribe(Sub<T>);
        T getValue(void);
        void next(T);
        
};



template<typename T>
Subject<T>::Subject()
{
    mSubs = NULL;
    mIDsMap = NULL;
    mSubsAmt = 0;
}

template<typename T>
Subject<T>::~Subject()
{
    free(mSubs);
    free(mIDsMap);
}

template<typename T>
int Subject<T>::subscribe(Sub<T> func)
{
    int SubID = (mSubsAmt > 0) ? mIDsMap[mSubsAmt-1] + 1 : 1;
    
    Sub<T>* newSubs = (Sub<T>*)malloc(sizeof(Sub<T>) * (mSubsAmt+1));
    int* newMap = (int*)malloc(sizeof(int) * (mSubsAmt+1));

    if(!newMap || !newSubs){
        return 0;
    }

    for(int i = 0; i < mSubsAmt; i++){
        newSubs[i] = mSubs[i];
        newMap[i] = mIDsMap[i];
    }

    free(mSubs);
    free(mIDsMap);

    newSubs[mSubsAmt] = func;
    newMap[mSubsAmt] = SubID;

    mSubs = newSubs;
    mIDsMap = newMap;
    mSubsAmt++;

    return SubID;
}

template<typename T>
int Subject<T>::unsubscribe(int id)
{
    int i;

    int index = -1;
    for(i = 0; i < mSubsAmt; i++){
        if(mIDsMap[i] == id){
            index = i;
            break;
        }
    }

    if(index == -1) return 0;


    int* newMap = (int*)malloc(sizeof(int) * (mSubsAmt-1));
    Sub<T>* newSubs = (Sub<T>*)malloc(sizeof(Sub<T>) * (mSubsAmt-1));

    if(!newMap || !newSubs) return 0;

    for(i = 0; i < index; i++){
        newSubs[i] = mSubs[i];
        newMap[i] = mIDsMap[i];
    }

    for(i = index+1; i < mSubsAmt; i++){
        newSubs[i-1] = mSubs[i];
        newMap[i-1] = mIDsMap[i];
    }

    free(mSubs);
    free(mIDsMap);

    mSubs = newSubs;
    mIDsMap = newMap;
    mSubsAmt--;

    return 1;
}

template<typename T>
void Subject<T>::next(T val){
    for(int i = 0; i < mSubsAmt; i++){
        mSubs[i](val);
    }
}

template<typename T>
BehaviorSubject<T>::BehaviorSubject(T val)
{
    mValue = val;
}

template<typename T>
int BehaviorSubject<T>::subscribe(Sub<T> func){
    func(mValue);
    return Subject<T>::subscribe(func);
}


template<typename T>
T BehaviorSubject<T>::getValue(void){
    return mValue;
}

template<typename T>
void BehaviorSubject<T>::next(T val){
    mValue = val;
    Subject<T>::next(val);
}

#endif