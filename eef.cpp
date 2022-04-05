#include <iostream>

class A
{
    public:

        /*Common function*/
        virtual void foo(){
            std::cout << "call from A" << std::endl;
        }

        virtual A* clone(){
            /*
            ...
            ...   Code I don't want to write again for B
            ...
            */
            return new A();
        }

};

class B: public A
{

        
    public:

        /*Common function*/
        virtual void foo(){
            std::cout << "call from B" << std::endl;
        }

        /*Not derived from A*/
        void moo(){ //Doesn't work even with virtual keyword
            std::cout << "only exist in B" << std::endl;
        }
};


int main(int argc, char const *argv[])
{
    auto tB = new B();

    tB->foo();
    tB->moo();
    tB->clone()->foo();
    tB->clone()->moo();


    return 0;
}