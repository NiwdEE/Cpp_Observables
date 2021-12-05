#include <iostream>
#include "Observables.hpp"

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
    /*          -----           */

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


/* Testing maps, filters and streams

    Should print:
Input value: 0
Output value: -282
(If u didn't know, 0 is even)
Input value: 1
Output value: 69
Input value: 2
Output value: 420
(If u didn't know, 702 is even)
Input value: 3
Output value: 771
Input value: 4
Output value: 1122
(If u didn't know, 1404 is even)
Input value: 5
Output value: 1473

*/
void test5(){
    auto mySub = new Subject<int>();

    auto a = EXP(x, x*351);
    auto b = EXP(x, x-282);

    auto myMappedSub = mySub->pipe(map<int>(a));

    myMappedSub->pipe(map<int>(b))->subscribe([](int a){
        std::cout << "Output value: " << a << std::endl;
    });


    //Even if we subscribe after, the original Subject's subscriptions will be called first
    mySub->subscribe([](int a){
        std::cout << "Input value: " << a << std::endl;
    });

    //Same level of line 181 but subscribed after, so will be called after
    myMappedSub->pipe(filter<int>(EXP(x, x%2==0)))->subscribe([](int a){
        std::cout << "(If u didn't know, " << a << " is even)" << std::endl;
    });

    *mySub << 0 << 1 << 2 << 3 << 4 << 5;    
}

/* Testing maps and testing on BehaviorSubject
    Should print:
0 is even
2 is even
4 is even
6 is even
8 is even
10 is even
*/
void test6(){
    auto myBS = new BehaviorSubject<int>(0);

    myBS->pipe(filter<int>(EXP(x, x%2==0)))->subscribe([](int a){
        std::cout << a << " is even" << std::endl;
    });

    for(int i = 0; i <= 10; i++){
        *myBS << i;
    }

}

int main(int argc, char const *argv[])
{
    void (* tests [])() = {test1, test2, test3, test4, test5, test6};

    int n = 0;
    std::cout << "which test ?"<< std::endl;
    scanf("%d", &n);
    if(n && n < 7){
        tests[n-1]();
    }

    return 0;
}
