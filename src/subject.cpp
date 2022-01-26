#ifndef SUBJECT_HEADER_D9DA62
#define SUBJECT_HEADER_D9DA62

#include "common.hpp"
#include "observable.cpp"

/**
 * @brief Collection that will emit its value to subscribed procedures instead of storing it.
 * 
 * @tparam T Type of the data supposed to be passed to the Subject
 */
template<typename T>
class Subject : public Observable<T>
{

    public:
        Subject();
        Subject(const Subject<T>&);
        ~Subject();

        void next(T);
        void error(T);   // **
        void complete(); // **

        // Subject<T>* pipe(Operator<T*, T*>);
        // Subject<T>* pipe(Procedure<T*>);

};

template<typename T>
Subject<T>::Subject()
{
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
Subject<T>::Subject(const Subject<T>& newOne)
{
    this->mSubs = newOne.mSubs;
    this->mNextSubID = newOne.mNextSubID;
    this->mSubsAmt = newOne.mSubsAmt;

    this->mClones = newOne.mClones;
    this->mClonesAmt = newOne.mClonesAmt;

    this->mPiped = newOne.mPiped;
    this->mPipe = newOne.mPipe;
}


template<typename T>
Subject<T>::~Subject()
{
    int i;
    for(i = 0; i < this->mSubsAmt; i++){
        delete this->mSubs[i];
    }

    free(this->mSubs);

    for(i = 0; i < this->mClonesAmt; i++){
        delete this->mClones[i];
    }

    free(this->mClones);
}


/**
 * @brief Emits a new value to the Subject
 * 
 * @tparam T Type of the Subject
 * @param val Value to be emitted
 */
template<typename T>
void Subject<T>::next(T val)
{   
    int i;

    if(this->mPiped){
        T* pipedPtr = (T*)malloc(sizeof(T));
        *pipedPtr = val;

        T* pipedVal = this->mPipe(pipedPtr);
        if(pipedVal){
            for(i = 0; i < this->mSubsAmt; i++){
                this->mSubs[i]->call(*pipedVal);
            }
        }

        free(pipedPtr);
    }else{
        for(i = 0; i < this->mSubsAmt; i++){
            this->mSubs[i]->call(val);
        }
    }

    for(i = 0; i < this->mClonesAmt; i++){
        this->mClones[i]->next(val);
    }
}



/**
 * @brief Shorter and prettier way to pass a value to a Subject
 * 
 * @tparam T Type of the Subject
 * @param sub Subject (Doesn't work with pointers)
 * @param val Value to pass
 */
template<typename T>
Subject<T>& operator<<(Subject<T>& sub, const T& val)
{
    sub.next(val);
    return sub;
}


// /**
//  * @brief Transform data using a procedure that modifies value of a given pointer 
//  * 
//  * @tparam T 
//  * @param func The function (better use map() than lambdas)
//  * @return Subject<T>* Address of the piped Subject
//  */
// template<typename T>
// Subject<T>* Subject<T>::pipe(Operator<T*, T*> func)
// {    
//     auto clone = this->clone();

//     clone->mPipe = [=](T* val){
//         return func(this->mPipe(val));
//     };

//     clone->mPiped = true;

//     return clone;
// }

// /**
//  * @brief Transform data using a procedure that modifies value of a given pointer 
//  * 
//  * @tparam T 
//  * @param func The function (better use map() than lambdas)
//  * @return Subject<T>* Address of the piped Subject
//  */
// template<typename T>
// Subject<T>* Subject<T>::pipe(Procedure<T*> func)
// {
//     auto clone = this->clone();

//     clone->mPipe = [=](T* val){
//         auto a = this->mPipe(val); 
//         func(val);
//         return a;
//     };

//     clone->mPiped = true;

//     return clone;
// }

#endif