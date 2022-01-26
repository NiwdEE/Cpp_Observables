#include <iostream>

template<typename T>
class A
{
    public:
        A<T>* self(){
            return this;
        }

        virtual void foo(){
            std::cout << "A" << std::endl;
        }

    protected:
        int zz = 69;
};

template<typename T>
class B: public A<T>
{
    public:
        void foo(){
            std::cout << "B" << std::endl;
        }
};

template<typename T>
class C: public B<T>
{
    public:
        void foo(){
            std::cout << "C" << std::endl;
        }

        void moo(){
            std::cout << this->zz << std::endl;
        }
};

int main(int argc, char const *argv[])
{
    auto tB = new B<int>();
    auto tC = new C<int>();

    tB->foo();
    tB->self()->foo();

    tC->foo();
    tC->self()->foo();

    tC->moo();

    return 0;
}


