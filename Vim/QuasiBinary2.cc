#include <iostream>
#include <vector>

#define INF 1000000000
#define MAXN 1000040
//#define MAXN 100

using namespace std;

typedef long long ll;
typedef vector<ll> vi;

int main() {
    vi best(MAXN, INF);
    vi parent(MAXN);
    
    best[0] = 0;
    parent[0] = -1;

    for (ll i = 1; i < MAXN; ++i) {
        for (ll shift = 0; shift <= (1 << 6); ++shift) {
            string num = "0000000";
            for (ll j = 0, copy = shift; copy != 0; ++j) {
                num[6-j] = '0' + copy % 2;
                copy /= 2;
            }

//            cerr << num << endl;

            ll nInt = atoi(num.c_str());
            if (i - nInt >= 0) {
                if (best[i-nInt]+1 < best[i]) {
                    parent[i] = i-nInt;
                    best[i] = best[i-nInt]+1;
                }
            }
        }
    }
    
    ll n;
    cin >> n;

    cout << best[n] << endl;
    cout << (n - parent[n]);
    
    ll cur = parent[n];
    while (cur != 0) {
        cout << " " << (cur - parent[cur]);
        cur = parent[cur];
    }

    cout << endl;

    return 0;
}
