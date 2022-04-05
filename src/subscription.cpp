#ifndef SUBSCRIPTION_HEADER_D9DA62
#define SUBSCRIPTION_HEADER_D9DA62

#include "common.hpp"
#include "observable.cpp"



/**
 * @brief Representation of a subscribed procedure.
 * 
 * @tparam T Type of the data supposed to be passed to its Subject
 */
template<class D, typename T>
class Subscription
{
    private:
        int mID;
        CRTPI_Observable<D,T>* mParent;

        Procedure<T> mAction;

    public:

        Subscription(int, CRTPI_Observable<D,T>*, Procedure<T>);
        ~Subscription();

        void call(T);
        int unsubscribe();
        int id();
};

template<typename D, typename T>
Subscription<D, T>::Subscription(int ID, CRTPI_Observable<D,T>* parent, Procedure<T> action)
{
    mID = ID;
    mParent = parent;
    mAction = action;
}

template<typename D, typename T>
Subscription<D, T>::~Subscription()
{
}

/**
 * @brief Delete this subscription from its subject
 * 
 * @tparam T Type of its Subject
 * @return 1 if it worked - 0 if the subscription list coulnd't be modified - -1 if the subscription isn't listed
 */
template<typename D, typename T>
int Subscription<D, T>::unsubscribe()
{
    return mParent->unsubscribe(mID);
}


/**
 * @brief Calls the procedure associated to the Subscription
 * 
 * @tparam T Type of its Subject
 * @param val Parameter to give to the procedure
 */
template<typename D, typename T>
void Subscription<D, T>::call(T val)
{
    mAction(val);
}

/**
 * @brief Sends ID of the subscription
 * 
 * @tparam T Type of its Subject
 * @return int ID 
 */
template<typename D, typename T>
int Subscription<D, T>::id()
{
    return mID;
}


#endif