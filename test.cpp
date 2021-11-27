#include <iostream>
#include "Observables.hpp"

void foo(Subject<int>* Obs){
    Obs->next(456);
}

int main(int argc, char const *argv[])
{

    /* * /
    // auto a = [](int x){return x+1;};
    auto a = EXP(x, x+1);

    printf("%d", a(1));

    return 0;
    /* */

    auto myBS = new BehaviorSubject<int>(0);

    auto sub1 = myBS->subscribe([](int a){
        std::cout << "New val: " << a << std::endl;
    });

    // std::cout << "New sub: " << sub << std::endl;

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
    return 0;
}
