#include <iostream>
#include "Observables.hpp"

void foo(Subject<int>* Obs){
    Obs->next(456);
}


/* Testing BehaviorSubject call on subscribe and unsubcription 1 by 1

    Should print: 
New val: 0
New val: 1
New val: 2
New val: 2
New val: 3
New val: 3
New val: 4
*/
void test1(){
    auto myBS = new BehaviorSubject<int>(0);

    auto sub1 = myBS->subscribe([](int a){
        std::cout << "New val: " << a << std::endl;
    });

    myBS->next(1);

    myBS->next(2);

    auto sub2 = myBS->subscribe([](int a){
        std::cout << "New val: " << a << std::endl;
    });

    myBS->next(3);

    sub1->unsubscribe();

    myBS->next(4);

    sub2->unsubscribe();

    myBS->next(5);

    delete myBS;
}


void notNested(int a){
    std::cout << "Classic fuction call works for value: " << a << std::endl;
}
/* Testing multiple types of function as subscriptions

    Sould print:
Direct lambda fuction call works for value: 420
Indirect lambda fuction call works for value: 420
Classic fuction call works for value: 420
*/
void test2(){
    auto mySub = Subject<int>();

    auto myLambda = [](int a){
        std::cout << "Indirect lambda fuction call works for value: " << a << std::endl;
    };

    mySub.subscribe([](int a){
        std::cout << "Direct lambda fuction call works for value: " << a << std::endl;
    });

    mySub.subscribe(myLambda);

    mySub.subscribe(notNested);

    mySub.next(420);
}

/* Testing external values update

    Should print:
i isn't equal to 580080
i is equal to 42
Wait, i is equal to: 42
i isn't equal to 580080
i is equal to 42
Wait, i is equal to: 580080
*/
void test3(){
    auto myReaderSub = new Subject<int>();
    auto myWriterSub = new Subject<int>();

    int i = 42;

    /* Will get an error    v   * /
    myReaderSub->subscribe([ ](int a){
        std::cout << "I think i is equal to " << i << std::endl;
    });
    /*                          */

    //Value capture ([=]), the value won't be updated if "i" changes 
    myReaderSub->subscribe([=](int a){
        if(i != a){
            std::cout << "i isn't equal to " << a <<std::endl;
        }
        std::cout << "i is equal to " << i << std::endl;
    });

    //Reference capture ([&]), the value will be updated 
    myReaderSub->subscribe([&](int a){
        std::cout << "Wait, i is equal to: " << i << std::endl;
    });

    //Needs a reference capture to update the value
    //Can also use a mutable lambda => [](int a)mutable{...};
    myWriterSub->subscribe([&](int a){
        i = a;
    });

    myReaderSub->next(58008);

    myWriterSub->next(58008);

    myReaderSub->next(58008);

}

/* Testing to subscribe a lot of procedure and unsubscribe all (NOT OPTIMIZED DON'T CODE LIKE THIS PLEASE)

    Should print:

***************
***************
***************
***************
***************
***************
***************
***************
*/
void test4(){
    auto mySub = Subject<char>();

    for (int i = 0; i < 128; i++)
    {
        //MUST HAVE a value capture ([=]) to keep the value of "i" at the moment of the subscribe
        mySub.subscribe([=](char a){
            std::cout << (i%16==0 ? '\n' : a);
        });
    }

    mySub.next('*');

    mySub.unsubscribeAll();

    mySub.next('o');
}

int main(int argc, char const *argv[])
{

    /** /

    Operator<int, int> moo = [=](int x){
        return x;
    };

    Operator<int, int> boo = [=](int x){
        return x+1;
    };

    Operator<int, int> poo = [=](int x){
        return boo(moo(x));
    };

    moo = poo;

    std::cout << moo(0) << std::endl;

    /**/

    /* */
    Operator<int, int> a = EXP(x, x+1);
    Operator<int, int> b = EXP(y, y+5);

    auto mySub = new Subject<int>();

    auto clone = mySub->map(a);
    clone->map(b)->subscribe([](int a){
        std::cout << "nv: " << a << std::endl;
    });

    mySub->next(0);

    return 0;
    /* */

    /** /

    auto mySub = new Subject<int>();

    mySub->subscribe([](int a){
        std::cout << "nv: " << a << std::endl;
    });

    *mySub << 25 << 65;

    /**/

    // test4();


    return 0;
}
