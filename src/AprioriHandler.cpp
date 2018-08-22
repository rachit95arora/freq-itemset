#include "AprioriHandler.h"
#include <iostream>
#include "FPHandler.h"
using namespace std;
int main()
{
    // AprioriHandler* apriori_h = new AprioriHandler("/Users/rachit/Documents/retail.dat", "/Users/rachit/Documents/retail.out", 0.03);
    // apriori_h->generateFrequentItemSet();
    // delete apriori_h;

    FPHandler* fp_h = new FPHandler("/Users/rachit/Documents/retail.dat", "/Users/rachit/Documents/retail_fp.out", 0.03);
    delete fp_h;
    return 0;
}
