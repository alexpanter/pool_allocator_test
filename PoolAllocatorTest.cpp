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
    lenient_test_alloc_too_much();
    lenient_test_alloc_free_alloc();

    lenient_test_alloc_n<1>();
    lenient_test_alloc_n<2>();
    lenient_test_alloc_n<3>();
    lenient_test_alloc_n<4>();
    lenient_test_alloc_n<8>();
    lenient_test_alloc_n<32>();
    lenient_test_alloc_n<4096>();

    lenient_test_alloc_free_alloc_n<1>();
    lenient_test_alloc_free_alloc_n<2>();
    lenient_test_alloc_free_alloc_n<3>();
    lenient_test_alloc_free_alloc_n<4>();
    lenient_test_alloc_free_alloc_n<128>();
    lenient_test_alloc_free_alloc_n<4096>();

    lenient_test_no_double_free<1>();
    lenient_test_no_double_free<2>();
    lenient_test_no_double_free<3>();
    lenient_test_no_double_free<4>();

    lenient_test_random_order_free<16>();
    lenient_test_random_order_free<64>();
    lenient_test_random_order_free<256>();
}

int main(int argc, char* argv[])
{
    //test_normal();
    test_lenient();

    return 0;
}
