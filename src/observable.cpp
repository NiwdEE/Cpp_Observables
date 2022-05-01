#ifndef OBSERVABLES_BASE_HEADER_D9DA62
#define OBSERVABLES_BASE_HEADER_D9DA62

#include "common.hpp"
#include "subscription.cpp"

/**
 * @brief Abstract class to generalize function subscription, can be used to create objects like EventEmitters or Listeners
 * 
 * @tparam Derived CRTP parameter
 * @tparam T type of the data supposed to be passed to subscriptions
 */
template<class Derived, typename T>
class CRTPI_Subscribable
{
    protected:
        Subscription<Derived, T>** mSubs;
        int mSubsAmt;

        int mNextSubID;

    public:

        Subscription<Derived, T>* subscribe(Procedure<T>);
        int unsubscribe(int);
        void unsubscribeAll();
};

/*
    /!\ /!\ /!\
    I won't define constructor, destructor, or pure class (not the CRPT interface)
    because this class is abstract and isn't supposed to have instances
    /!\ /!\ /!\
*/


/**
 * @brief Subscribes a procedure to the Subject.
 * 
 * @tparam T Type of the Subject
 * @param proc Procedure to subscribe
 * @return the address of the subscription (NULL if an error occured)
 */
template<class Derived, typename T>
Subscription<Derived, T>* CRTPI_Subscribable<Derived, T>::subscribe(Procedure<T> proc)
{
    int SubID = mNextSubID++;
    
    Subscription<Derived, T>** newSubs = (Subscription<Derived, T>**)malloc(sizeof(Subscription<Derived, T>*) * (mSubsAmt+1));

    if(!newSubs){
        return NULL;
    }

    for(int i = 0; i < this->mSubsAmt; i++){
        newSubs[i] = this->mSubs[i];
    }

    free(mSubs);

    auto Sub = new Subscription<Derived, T>(SubID, static_cast<Derived*>(this), proc);
    
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
template<class Derived, typename T>
int CRTPI_Subscribable<Derived, T>::unsubscribe(int id)
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


    Subscription<Derived, T>** newSubs = (Subscription<Derived, T>**)malloc(sizeof(Subscription<Derived, T>*) * (mSubsAmt-1));

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
template<class Derived, typename T>
void CRTPI_Subscribable<Derived, T>::unsubscribeAll()
{
    for(int i = 0; i < mSubsAmt; i++){
        delete mSubs[i];
    }

    free(mSubs);
    mSubsAmt = 0;
    mNextSubID = 1;
}


/**
 * @brief Collection that will emit its value to subscribed procedures instead of storing it.
 * 
 * @tparam Derived CRTP parameter
 * @tparam T Type of the data supposed to be passed to the Subject
 */
template<class Derived, typename T>
class CRTPI_Observable: public CRTPI_Subscribable<Derived, T>
{
    protected:

        Derived** mClones;
        int mClonesAmt;

        bool mPiped;
        Operator<T*, T*> mPipe;

        Derived* clone();

    public:

        CRTPI_Observable();

        Derived* pipe(Operator<T*, T*>);
        Derived* pipe(Procedure<T*>);

};

template<typename T>
class Observable: public CRTPI_Observable<Observable<T>, T>
{
    public:
        Observable();
        Observable(const Observable<T>&);
        ~Observable();
};

template<class Derived, typename T>
CRTPI_Observable<Derived, T>::CRTPI_Observable(){
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
Observable<T>::Observable(): CRTPI_Observable<Observable<T>, T>()
{

}

template<typename T>
Observable<T>::Observable(const Observable<T>& newOne)
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
Observable<T>::~Observable()
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
 * @brief Create a clone Subject, that will be called with same values than his parent
 *
 * @tparam T Data type of Subject
 * @return Subject<T>* Address of the clone
 */
template<class Derived, typename T>
Derived* CRTPI_Observable<Derived, T>::clone(void)
{
    auto clone = new Derived();

    Derived** newClones = (Derived**)malloc(sizeof(Derived*) * (mClonesAmt+1));

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
template<class Derived, typename T>
Derived* CRTPI_Observable<Derived, T>::pipe(Operator<T*, T*> func)
{    
    auto clone = this->clone();

    clone->mPipe = [=](T* val){
        return func(val);
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
template<class Derived, typename T>
Derived* CRTPI_Observable<Derived, T>::pipe(Procedure<T*> func)
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
