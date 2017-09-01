#include <iostream>
#include <vector>
#include <cassert>
#include <functional>

#include "trampoline.h"

using namespace std;


void test1() {

    cout << "test1: less than 6 arguments " << endl;

    {
        trampoline<int (int, int, int, int, int)>
                t([&] (int p0, int p1, int p2, int p3, int p4) {return p0 * p1 * p2 * p3 * p4;});
        auto p = t.get();
        assert (120 == p(1, 2, 3, 4, 5));
    }
    {
        trampoline<double (double, double, double, double, double)>
                t([&] (double p0, double p1, double p2, double p3, double p4) {return p1 + p2 + p3 + p4 + p0;});
        auto p = t.get();
        assert (2.8 == p(1.0, -1.2, 1, 1, 1));
    }
    {
        trampoline<float (int, double, double, float, float)>
                t([&] (int p0, double p1, double p2, float p3, float p4) {return (p1 + p2 + p3 + p4 + p0);});
        auto p = t.get();
        assert (5.5 == p(1, 1.2, 1.3, 1, 1));
    }
    {
        trampoline<int (int&)>
                t([&] (int p0) {return p0;});
        auto p = t.get();
        int k = 5;
        assert (5 == p(k));
    }
    cout << "test1 PASSED" << endl << endl;
}

void test2 () {
    cout << "test2: more than 5 arguments" << endl;
    {
        trampoline<long long (int, int, int, int, int, int, int, int)>
                t([&] (int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7)
                  {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        assert (8 == p(1, 1, 1, 1, 1, 1, 1, 1));

    }

    {
        trampoline<long long (double, double, double, double, double, double, double, double)>
                t([&] (double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7)
                  {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        assert (8 == p(1, 1, 1, 1, 1, 1, 1, 1));
    }

    {
        trampoline<long long (float, float, float, float, float, float, float, float)>
                t([&] (float p0, float p1, float p2, float p3, float p4, float p5, float p6, float p7)
                  {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        assert (8 == p(1, 1, 1, 1, 1, 1, 1, 1));
    }

    {
        trampoline<long long (double, int, float, int, int, double, double, float)>
                t([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
                  {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        assert (8 == p(1, 1, 1, 1, 1, 1, 1, 1));
    }

    {
        trampoline<long long (double&, int&, float&, int, int, double, double, float&)>
                t([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
                  {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p = t.get();
        double a = 1;
        int b = 1;
        float c = 1, d = 1;
        assert (8 == p(a, b, c, 1, 1, 1, 1, d));
    }

    {
        trampoline<float (double, int, float, int, int, double, double, float)>
                t1([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
                   {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        auto p1 = t1.get();

        trampoline<float (double, int, float, int, const int&, double&, double, float&)>
                t2([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
                   {return p1 + p2 + p3 + p4 + p0 + p5 + p6 + p7;});
        const int a = 1;
        double b = 3.7;
        float c = 4.1;
        auto p2 = t2.get();
        assert ((float)(p1(1, 1, 1, 1, 1, 1, 1, 1) + 103.8) == p2(1, 2, 100, -1, a, b, 1, c));
    }

    {
        trampoline<float (double, int, float, int, int, double, double, float)>
                t([&] (double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7)
                  {return p2;});
        auto p = t.get();
        assert(p(1, 2, 3, 4, 5, 6, 7, 8) == 3);
    }

    {
        trampoline<int(double, int, float, int, int, double, double, float)>
                t([&](double p0, int p1, float p2, int p3, int p4, double p5, double p6, float p7) { return p7; });
        auto p = t.get();
        assert(p(1, 2, 3, 4, 5, 6, 7, 8.8) == 8);
    }

    cout << "test2 PASSED" << endl << endl;
}


void test_constructors() {
    cout << "test3: constructors" << endl;
    trampoline<int(int)> tr([](int a){return a * 5;});
    trampoline<int(int)> trr(std::move(tr));
    trampoline<int(int)> tr_copy = std::move(tr);
    assert(tr.get()(3) == trr.get()(3));
    assert(tr.get()(3) == tr_copy.get()(3));
    cout << "test3 PASSED" << endl;
}

int main() {
    test1();
    test2();
    test_constructors();
    return 0;
}