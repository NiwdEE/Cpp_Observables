#ifndef SUBJECT_HEADER_D9DA62
#define SUBJECT_HEADER_D9DA62

#include "common.hpp"
#include "subscription.cpp"


/**
 * @brief Collection that will emit its value to subscribed procedures instead of storing it.
 * 
 * @tparam T Type of the data supposed to be passed to the Subject
 */
template<typename T>
class Subject
{
    protected:
        Subscription<T>** mSubs;
        int mSubsAmt;

        int mNextSubID;


        Subject<T>** mClones;
        int mClonesAmt;

        bool mPiped;
        Operator<T*, T*> mPipe;

        Subject* clone();

    public:
        Subject();
        Subject(const Subject<T>&);
        ~Subject();

        Subscription<T>* subscribe(Procedure<T>);
        int unsubscribe(int);
        void unsubscribeAll();
        void next(T);

        Subject<T>* pipe(Operator<T*, T*>);
        Subject<T>* pipe(Procedure<T*>);

};

template<typename T>
Subject<T>::Subject()
{
    mSubs = NULL;
    mNextSubID = 1;
    mSubsAmt = 0;

    mClones = NULL;
    mClonesAmt = 0;

    mPiped = false;
    mPipe = [](T* a){
        return a;
    };
}

template<typename T>
Subject<T>::Subject(const Subject<T>& newOne)
{
    mSubs = newOne.mSubs;
    mNextSubID = newOne.mNextSubID;
    mSubsAmt = newOne.mSubsAmt;

    mClones = newOne.mClones;
    mClonesAmt = newOne.mClonesAmt;

    mPiped = newOne.mPiped;
    mPipe = newOne.mPipe;
}


template<typename T>
Subject<T>::~Subject()
{
    int i;
    for(i = 0; i < mSubsAmt; i++){
        delete mSubs[i];
    }

    free(mSubs);

    for(i = 0; i < mClonesAmt; i++){
        delete mClones[i];
    }

    free(mClones);
}

/**
 * @brief Subscribes a procedure to the Subject.
 * 
 * @tparam T Type of the Subject
 * @param proc Procedure to subscribe
 * @return the address of the subscription (NULL if an error occured)
 */
template<typename T>
Subscription<T>* Subject<T>::subscribe(Procedure<T> proc)
{
    int SubID = mNextSubID++;
    
    Subscription<T>** newSubs = (Subscription<T>**)malloc(sizeof(Subscription<T>*) * (mSubsAmt+1));

    if(!newSubs){
        return NULL;
    }

    for(int i = 0; i < mSubsAmt; i++){
        newSubs[i] = mSubs[i];
    }

    free(mSubs);

    auto Sub = new Subscription<T>(SubID, this, proc);
    
    newSubs[mSubsAmt] = Sub;

    mSubs = newSubs;
    mSubsAmt++;

    return Sub;
}


/**
 * @brief Delete a single subscription from the subject (You should use Subscription->unsubscribe() instead).
 * 
 * @tparam T Type of the Subject
 * @param id The id of the subscription
 * @return -1 if the id doesn't exist - 0 if the subscription list coulnd't be modified - 1 if it worked
 */
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

    if(index == -1) return -1;


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

/**
 * @brief Unsubscribes every subscriptions
 */
template<typename T>
void Subject<T>::unsubscribeAll()
{
    for(int i = 0; i < mSubsAmt; i++){
        delete mSubs[i];
    }

    free(mSubs);
    mSubsAmt = 0;
    mNextSubID = 1;
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

    if(mPiped){
        T* pipedPtr = (T*)malloc(sizeof(T));
        *pipedPtr = val;

        T* pipedVal = mPipe(pipedPtr);
        if(pipedVal){
            for(i = 0; i < mSubsAmt; i++){
                mSubs[i]->call(*pipedVal);
            }
        }

        free(pipedPtr);
    }else{
        for(i = 0; i < mSubsAmt; i++){
            mSubs[i]->call(val);
        }
    }

    for(i = 0; i < mClonesAmt; i++){
        mClones[i]->next(val);
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

/**
 * @brief Create a clone Subject, that will be called with same values than his parent
 *
 * @tparam T Data type of Subject
 * @return Subject<T>* Address of the clone
 */
template<typename T>
Subject<T>* Subject<T>::clone(void)
{
    auto clone = new Subject<T>();

    Subject<T>** newClones = (Subject<T>**)malloc(sizeof(Subject<T>*) * (mClonesAmt+1));

    if(!newClones){
        return NULL;
    }

    for(int i = 0; i < mClonesAmt; i++){
        newClones[i] = mClones[i];
    }

    free(mClones);
    newClones[mClonesAmt] = clone;
    mClones = newClones;

    mClonesAmt++;

    return clone;
}


/**
 * @brief Transform data using a procedure that modifies value of a given pointer 
 * 
 * @tparam T 
 * @param func The function (better use map() than lambdas)
 * @return Subject<T>* Address of the piped Subject
 */
template<typename T>
Subject<T>* Subject<T>::pipe(Operator<T*, T*> func)
{    
    auto clone = this->clone();

    clone->mPipe = [=](T* val){
        return func(this->mPipe(val));
    };

    clone->mPiped = true;

    return clone;
}

/**
 * @brief Transform data using a procedure that modifies value of a given pointer 
 * 
 * @tparam T 
 * @param func The function (better use map() than lambdas)
 * @return Subject<T>* Address of the piped Subject
 */
template<typename T>
Subject<T>* Subject<T>::pipe(Procedure<T*> func)
{
    auto clone = this->clone();

    clone->mPipe = [=](T* val){
        auto a = this->mPipe(val); 
        func(val);
        return a;
    };

    clone->mPiped = true;

    return clone;
}

#endif