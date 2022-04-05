#include <iostream>

//CRTPI = "CRTP Interface", used for inheritances between CRTPs

template<typename Derived, typename T>
class CRTPI_A
{
    protected:
        T x = 0;


    public:

        T getX(){
            return x;
        }

        Derived* clone(){
            /*
            ...
            ...   Code I don't want to write again for B and it's childs
            ...
            */
            return new Derived();
        }

};

template<typename Derived, typename T>
class CRTPI_B: public CRTPI_A<Derived, T>
{
    public:

        //Only for B and its childs
        void iwd(){
            std::cout << "Hi, i'm a B child !" << std::endl;
        }


};

template<typename Derived, typename T>
class CRTPI_C: public CRTPI_B<Derived, T>{};

template<typename T>
class A: public CRTPI_A<A<T>, T>
{
    public:

        A(){};

        A(T z){
            this->x = z;
        }

        void foo(){
            std::cout << "call from A" << std::endl;
        }
};

template<typename T>
class B: public CRTPI_B<B<T>, T>
{

        
    public:

        B(){};

        B(T z){
            this->x = z;
        }

        void foo(){
            std::cout << "call from B" << std::endl;
        }

        //Not in CRTP interface so won't be inherited by C
        void UwU(){
            std::cout << "I'm exclusive to B" << std::endl;
        }

};

template<typename T>
class C: public CRTPI_C<C<T>, T>
{
    public:

        C(){};

        C(T z){
            this->x = z;
        };

        void foo(){
            std::cout << "call from C" << std::endl;
        }
};


int main(int argc, char const *argv[])
{
    auto tA = new A<char>('A');
    auto tB = new B<int>(2);
    auto tC = new C<float>(420.69);


    tA->foo();
    tA->clone()->foo();

    printf("\n");

    tB->foo();
    tB->iwd();
    tB->clone()->foo();
    tB->clone()->iwd();

    tB->UwU();

    printf("\n");

    tC->foo();
    tC->iwd();
    // tC->UwU(); //Won't work but that's planned

    std::cout << "\n" << tA->getX() << ":" << tB->getX() <<":" << tC->getX() << std::endl;


    return 0;
}


