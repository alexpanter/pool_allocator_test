#include "Tests.h"
#include "LenientTests.h"

void test_normal()
{
    single_test();
    two_test();
    test1<1>();
    test1<2>();
    test1<3>();
    test1<4>();
    test1<8>();
    test1<16>();
    test1<33>();
    test1<104>();

    test_no_double_free();
    test_no_double_free_n<2>();
    test_no_double_free_n<3>();
    test_no_double_free_n<4>();

    random_test<256>();
    random_order_free<256>();
}

void test_lenient()
{
    lenient_test_alloc();
}

int main(int argc, char* argv[])
{
    test_normal();

    return 0;
}
