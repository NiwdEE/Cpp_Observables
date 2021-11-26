#include <iostream>
#include "Observables.hpp"

void foo(Subject<int>* Obs){
    Obs->next(456);
}

int main(int argc, char const *argv[])
{

    // auto a = [](int x){return x+1;};
    auto a = EXP(x, x+1);

    printf("%d", a(1));

    return 0;

    auto myBS = new BehaviorSubject<int>(0);

    int sub1 = myBS->subscribe([](int a){
        std::cout << "New val: " << a << std::endl;
    });

    // std::cout << "New sub: " << sub << std::endl;

    myBS->next(1);


    myBS->next(2);

    int sub2 = myBS->subscribe([](int a){
        std::cout << "New val: " << a << std::endl;
    });

    myBS->next(3);

    myBS->unsubscribe(sub1);

    myBS->next(4);

    myBS->unsubscribe(sub2);

    myBS->next(5);
    

    // delete myObs;
    delete myBS;
    return 0;
}
