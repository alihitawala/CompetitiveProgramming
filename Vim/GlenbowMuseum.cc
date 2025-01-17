#include <iostream>
#include <cstdio>

using namespace std;

typedef long long ll;

ll nC4(ll n) {
    if (n < 4)
        return 0;

    ll total = 1;
    ll fourFact = 1;

    for (ll i = 0; i < 4; ++i) {
        total *= (n-i);
        fourFact *= 4-i;
    }

    return total / fourFact;
}

int main() {
    for (ll caseNum = 1; true; ++caseNum) {
        ll L;
        cin >> L;

        if (L == 0)
            break;

        ll ways;
        if (L % 2 == 1)
            ways = 0;
        else
            ways = nC4(4 + (L-4)/2) + nC4(4 + (L-6)/2);

        printf("Case %lld: %lld\n", caseNum, ways);
    }

    return 0;
}
