#include "AprioriHandler.h"
#include <iostream>
using namespace std;
int main()
{
    AprioriHandler* apriori_h = new AprioriHandler("/Users/rachit/Documents/sample.dat", "/Users/rachit/Documents/sample.out", 0.2);
    apriori_h->generateFrequentItemSet();

    return 0;
}
