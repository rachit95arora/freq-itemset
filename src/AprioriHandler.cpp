#include "AprioriHandler.h"
#include <iostream>
using namespace std;
int main()
{
    AprioriHandler* apriori_h = new AprioriHandler("/Users/rachit/Documents/retail.dat", "/Users/rachit/Documents/retail.out", 0.03);
    apriori_h->generateFrequentItemSet();

    return 0;
}
