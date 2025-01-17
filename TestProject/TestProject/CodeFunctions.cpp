//============================================================================
// Name        :
// Author      : Bryce Sandlund
// Version     :
// Copyright   :
// Description : Code reference functions/ algorithms
//============================================================================

#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <sstream>
#include <list>
#include <map>
#include <fstream>
#include <string>
#include <time.h>
#include <queue>
#include <tuple>
#include <functional>
#include <unordered_set>
#include <unordered_map>

#define INF 1000000000
#define all(c) (c).begin(),(c).end()
#define tr(c,i) for(typeof((c).begin()) i = (c).begin(); i != (c).end(); ++i)
#define EP .00001

using namespace std;

typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<bool> vb;
typedef vector<vi> vvi;
typedef vector<ii> vii;
typedef vector<vb> vvb;
typedef vector<double> vd;
typedef vector<vd> vvd;
typedef long long LL;

/*******************************************
***************data structures**************
*******************************************/
// Union-Find Disjoint Sets Library written in OOP manner, using both path compression and union by rank heuristics
class UnionFind {                                              // OOP style
private:
  vi p, rank, setSize;                       // remember: vi is vector<int>
  int numSets;
public:
  UnionFind(int N) {
    setSize.assign(N, 1); numSets = N; rank.assign(N, 0);
    p.assign(N, 0); for (int i = 0; i < N; i++) p[i] = i; }
  int findSet(int i) { return (p[i] == i) ? i : (p[i] = findSet(p[i])); }
  bool isSameSet(int i, int j) { return findSet(i) == findSet(j); }
  void unionSet(int i, int j) { 
    if (!isSameSet(i, j)) { numSets--; 
    int x = findSet(i), y = findSet(j);
    // rank is used to keep the tree short
    if (rank[x] > rank[y]) { p[y] = x; setSize[x] += setSize[y]; }
    else                   { p[x] = y; setSize[y] += setSize[x];
                             if (rank[x] == rank[y]) rank[y]++; } } }
  int numDisjointSets() { return numSets; }
  int sizeOfSet(int i) { return setSize[findSet(i)]; }
};

//union-find set: the vector/array contains the parent of each node - lightweight version only implementing path compression from Stanford ACM
int find(vector <int>& C, int x){return (C[x]==x) ? x : C[x]=find(C, C[x]);} //C++

#define MAX_N 100010                         // second approach: O(n log n)
char T[MAX_N];
int n;                                        // the length of input string
int RA[MAX_N], tempRA[MAX_N];        // rank array and temporary rank array
int SA[MAX_N], tempSA[MAX_N];    // suffix array and temporary suffix array
//int c[MAX_N];                                    // for counting/radix sort

void countingSort(int k) {                                          // O(n)
  int i, sum, maxi = max(300, n);   // up to 255 ASCII chars or length of n
  vector<int> c(500, 0);
  for (i = 0; i < n; i++)       // count the frequency of each integer rank
    c[i + k < n ? RA[i + k] : 0]++;
  for (i = sum = 0; i < maxi; i++) {
    int t = c[i]; c[i] = sum; sum += t;
  }
  for (i = 0; i < n; i++)          // shuffle the suffix array if necessary
    tempSA[c[SA[i]+k < n ? RA[SA[i]+k] : 0]++] = SA[i];
  for (i = 0; i < n; i++)                     // update the suffix array SA
    SA[i] = tempSA[i];
}

void constructSA() {         // this version can go up to 100000 characters
  int i, k, r;
  for (i = 0; i < n; i++) RA[i] = T[i];                 // initial rankings
  for (i = 0; i < n; i++) SA[i] = i;     // initial SA: {0, 1, 2, ..., n-1}
  for (k = 1; k < n; k <<= 1) {       // repeat sorting process log n times
    countingSort(k);  // actually radix sort: sort based on the second item
    countingSort(0);          // then (stable) sort based on the first item
    tempRA[SA[0]] = r = 0;             // re-ranking; start from rank r = 0
    for (i = 1; i < n; i++)                    // compare adjacent suffixes
      tempRA[SA[i]] = // if same pair => same rank r; otherwise, increase r
      (RA[SA[i]] == RA[SA[i-1]] && RA[SA[i]+k] == RA[SA[i-1]+k]) ? r : ++r;
    for (i = 0; i < n; i++)                     // update the rank array RA
      RA[i] = tempRA[i];
    if (RA[SA[n-1]] == n-1) break;               // nice optimization trick
} }

ii stringMatching(string s) {                      // string matching in O(m log n)
	int m = s.size();
	const char *P = s.c_str();
  int lo = 0, hi = n-1, mid = lo;              // valid matching = [0..n-1]
  while (lo < hi) {                                     // find lower bound
    mid = (lo + hi) / 2;                              // this is round down
    int res = strncmp(T + SA[mid], P, m);  // try to find P in suffix 'mid'
    if (res >= 0) hi = mid;        // prune upper half (notice the >= sign)
    else          lo = mid + 1;           // prune lower half including mid
  }                                      // observe `=' in "res >= 0" above
  if (strncmp(T + SA[lo], P, m) != 0) return ii(-1, -1);    // if not found
  ii ans; ans.first = lo;
  lo = 0; hi = n - 1; mid = lo;
  while (lo < hi) {            // if lower bound is found, find upper bound
    mid = (lo + hi) / 2;
    int res = strncmp(T + SA[mid], P, m);
    if (res > 0) hi = mid;                              // prune upper half
    else         lo = mid + 1;            // prune lower half including mid
  }                           // (notice the selected branch when res == 0)
  if (strncmp(T + SA[hi], P, m) != 0) hi--;                 // special case
  ans.second = hi;
  return ans;
} // return lower/upperbound as first/second item of the pair, respectively. Inclusive on both ends!!

//binary indexed tree - taken from Stanford ACM
#define LOGSZ 17

int ft[(1<<LOGSZ)+1];
int N = (1<<LOGSZ);

// add v to value at x
void adjust(int x, int v) {
  while(x <= N) {
    ft[x] += v;
    x += (x & -x);
  }
}

// get cumulative sum up to and including x
int rsq(int x) {
  int res = 0;
  while(x) {
    res += ft[x];
    x -= (x & -x);
  }
  return res;
}

// get largest value with cumulative sum less than or equal to x;
// for smallest, pass x-1 and add 1 to result
int getind(int x) {
  int idx = 0, mask = N;
  while(mask && idx < N) {
    int t = idx + mask;
    if(x >= ft[t]) {
      idx = t;
      x -= ft[t];
    }
    mask >>= 1;
  }
  return idx;
}

//trie
typedef struct trienode
{
	bool word;
	trienode *children[26];
	bool present[26];
} trienode;

trienode *root;

void init (trienode* node)
{
	for (int i = 0; i < 26; ++i)
	{
		node->present[i] = false;
	}
	node->word = false;
}

int charToInt(char c)
{
	return c - 97;
}

void add(string word)
{
	trienode *cur = root;
	for (int i = 0; i < word.size(); ++i)
	{
		int letter = charToInt(word[i]);
		if (cur->present[letter])
		{
			cur = cur->children[letter];
		}
		else
		{
			trienode* newNode = (trienode*) malloc(sizeof(trienode));
			init(newNode);
			cur->present[letter] = true;
			cur->children[letter] = newNode;
			cur = cur->children[letter];
		}
		if (i == word.size() - 1)
		{
			cur->word = true;
		}
	}
}

/*******************************************
********************graph*******************
*******************************************/

double kruskal(vector<pair<double, ii> > &EdgeList, int n)
{
		sort(EdgeList.begin(), EdgeList.end()); // sort by edge weight O(E log E)
                      // note: pair object has built-in comparison function

		double mst_cost = 0;
		vector<int> UF(n);
		for (int i = 0; i < n; ++i)
		{
			UF[i] = i;
		}

		int matched = 0;
		for (vector<pair<double, ii> >::iterator iter = EdgeList.begin(); iter != EdgeList.end() && matched < n-1; ) {                      // for each edge, O(E)
			pair<double, ii> front = *iter;
			int x = find(UF, front.second.first), y = find(UF, front.second.second);
			if (x != y) {  // check
				mst_cost += front.first;                // add the weight of e to MST
				UF[x] = UF[y];    // link them
				++matched;
			}        // note: the runtime cost of UFDS is very light
			++iter;
		}

		return matched == n-1 ? mst_cost : -1;
}

vi dijkstra(vector<vii> &AdjList, int s)
{
  int V = AdjList.size();

  // Dijkstra routine
  vi dist(V, INF); dist[s] = 0;                    // INF = 1B to avoid overflow
  priority_queue< ii, vector<ii>, greater<ii> > pq; pq.push(ii(0, s));
         // ^to sort the pairs by increasing distance from s
  while (!pq.empty()) {                                             // main loop
    ii front = pq.top(); pq.pop();     // greedy: pick shortest unvisited vertex
    int d = front.first, u = front.second;
    if (d > dist[u]) continue;   // this check is important, see the explanation
    for (int j = 0; j < (int)AdjList[u].size(); j++) {
      ii v = AdjList[u][j];                       // all outgoing edges from u
      if (dist[u] + v.second < dist[v.first]) {
        dist[v.first] = dist[u] + v.second;                 // relax operation
        pq.push(ii(dist[v.first], v.first));
  } } }  // note: this variant can cause duplicate items in the priority queue

  return dist;
}

// This code performs maximum (cardinality) bipartite matching. - does not support weighted edges, (Hungarian Algorithm)
//
// Running time: O(|E| |V|) -- often much faster in practice
//
//   INPUT: w[i][j] = edge between row node i and column node j
//   OUTPUT: mr[i] = assignment for row node i, -1 if unassigned
//           mc[j] = assignment for column node j, -1 if unassigned
//           function returns number of matches made

bool FindMatch(int i, const vvi &w, vi &mr, vi &mc, vi &seen) {
  for (int j = 0; j < w[i].size(); j++) {
    if (w[i][j] && !seen[j]) {
      seen[j] = true;
      if (mc[j] < 0 || FindMatch(mc[j], w, mr, mc, seen)) {
        mr[i] = j;
        mc[j] = i;
        return true;
      }
    }
  }
  return false;
}

int BipartiteMatching(const vvi &w, vi &mr, vi &mc) {
  mr = vi(w.size(), -1);
  mc = vi(w[0].size(), -1);
  
  int ct = 0;
  for (int i = 0; i < w.size(); i++) {
    vi seen(w[0].size());
    if (FindMatch(i, w, mr, mc, seen)) ct++;
  }
  return ct;
}

///////////////////////////////////////////////////////////////////////////
// Min cost bipartite matching via shortest augmenting paths - Hungarian Algorithm
//
// This is an O(n^3) implementation of a shortest augmenting path
// algorithm for finding min cost perfect matchings in dense
// graphs.  In practice, it solves 1000x1000 problems in around 1
// second.
//
//   cost[i][j] = cost for pairing left node i with right node j
//   Lmate[i] = index of right node that left node i pairs with
//   Rmate[j] = index of left node that right node j pairs with
//
// The values in cost[i][j] may be positive or negative.  To perform
// maximization, simply negate the cost[][] matrix.
///////////////////////////////////////////////////////////////////////////

double MinCostMatching(const vvd &cost, vi &Lmate, vi &Rmate) {
  int n = int(cost.size());

  // construct dual feasible solution
  vd u(n);
  vd v(n);
  for (int i = 0; i < n; i++) {
    u[i] = cost[i][0];
    for (int j = 1; j < n; j++) u[i] = min(u[i], cost[i][j]);
  }
  for (int j = 0; j < n; j++) {
    v[j] = cost[0][j] - u[0];
    for (int i = 1; i < n; i++) v[j] = min(v[j], cost[i][j] - u[i]);
  }
  
  // construct primal solution satisfying complementary slackness
  Lmate = vi(n, -1);
  Rmate = vi(n, -1);
  int mated = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (Rmate[j] != -1) continue;
      if (fabs(cost[i][j] - u[i] - v[j]) < 1e-10) {
	Lmate[i] = j;
	Rmate[j] = i;
	mated++;
	break;
      }
    }
  }
  
  vd dist(n);
  vi dad(n);
  vi seen(n);
  
  // repeat until primal solution is feasible
  while (mated < n) {
    
    // find an unmatched left node
    int s = 0;
    while (Lmate[s] != -1) s++;
    
    // initialize Dijkstra
    fill(dad.begin(), dad.end(), -1);
    fill(seen.begin(), seen.end(), 0);
    for (int k = 0; k < n; k++) 
      dist[k] = cost[s][k] - u[s] - v[k];
    
    int j = 0;
    while (true) {
      
      // find closest
      j = -1;
      for (int k = 0; k < n; k++) {
	if (seen[k]) continue;
	if (j == -1 || dist[k] < dist[j]) j = k;
      }
      seen[j] = 1;
      
      // termination condition
      if (Rmate[j] == -1) break;
      
      // relax neighbors
      const int i = Rmate[j];
      for (int k = 0; k < n; k++) {
	if (seen[k]) continue;
	const double new_dist = dist[j] + cost[i][k] - u[i] - v[k];
	if (dist[k] > new_dist) {
	  dist[k] = new_dist;
	  dad[k] = j;
	}
      }
    }
    
    // update dual variables
    for (int k = 0; k < n; k++) {
      if (k == j || !seen[k]) continue;
      const int i = Rmate[k];
      v[k] += dist[k] - dist[j];
      u[i] -= dist[k] - dist[j];
    }
    u[s] += dist[j];
    
    // augment along path
    while (dad[j] >= 0) {
      const int d = dad[j];
      Rmate[j] = Rmate[d];
      Lmate[Rmate[j]] = j;
      j = d;
    }
    Rmate[j] = s;
    Lmate[s] = j;
    
    mated++;
  }
  
  double value = 0;
  for (int i = 0; i < n; i++)
    value += cost[i][Lmate[i]];
  
  return value;
}

// Adjacency list implementation of FIFO push relabel maximum flow
// with the gap relabeling heuristic.  This implementation is
// significantly faster than straight Ford-Fulkerson.  It solves
// random problems with 10000 vertices and 1000000 edges in a few
// seconds, though it is possible to construct test cases that
// achieve the worst-case.
//
// Running time:
//     O(|V|^3)
//
// INPUT: 
//     - graph, constructed using AddEdge()
//     - source
//     - sink
//
// OUTPUT:
//     - maximum flow value
//     - To obtain the actual flow values, look at all edges with
//       capacity > 0 (zero capacity edges are residual edges).


struct Edge {
  int from, to, cap, flow, index;
  Edge(int from, int to, int cap, int flow, int index) :
    from(from), to(to), cap(cap), flow(flow), index(index) {}
};

struct PushRelabel {
  int N;
  vector<vector<Edge> > G;
  vector<LL> excess;
  vector<int> dist, active, count;
  queue<int> Q;

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N), count(2*N) {}

  void AddEdge(int from, int to, int cap) {
    G[from].push_back(Edge(from, to, cap, 0, G[to].size()));
    if (from == to) G[from].back().index++;
    G[to].push_back(Edge(to, from, 0, 0, G[from].size() - 1));
  }

  void Enqueue(int v) { 
    if (!active[v] && excess[v] > 0) { active[v] = true; Q.push(v); } 
  }

  void Push(Edge &e) {
    int amt = int(min(excess[e.from], LL(e.cap - e.flow)));
    if (dist[e.from] <= dist[e.to] || amt == 0) return;
    e.flow += amt;
    G[e.to][e.index].flow -= amt;
    excess[e.to] += amt;    
    excess[e.from] -= amt;
    Enqueue(e.to);
  }
  
  void Gap(int k) {
    for (int v = 0; v < N; v++) {
      if (dist[v] < k) continue;
      count[dist[v]]--;
      dist[v] = max(dist[v], N+1);
      count[dist[v]]++;
      Enqueue(v);
    }
  }

  void Relabel(int v) {
    count[dist[v]]--;
    dist[v] = 2*N;
    for (int i = 0; i < G[v].size(); i++) 
      if (G[v][i].cap - G[v][i].flow > 0)
	dist[v] = min(dist[v], dist[G[v][i].to] + 1);
    count[dist[v]]++;
    Enqueue(v);
  }

  void Discharge(int v) {
    for (int i = 0; excess[v] > 0 && i < G[v].size(); i++) Push(G[v][i]);
    if (excess[v] > 0) {
      if (count[dist[v]] == 1) 
	Gap(dist[v]); 
      else
	Relabel(v);
    }
  }

  LL GetMaxFlow(int s, int t) {
    count[0] = N-1;
    count[N] = 1;
    dist[s] = N;
    active[s] = active[t] = true;
    for (int i = 0; i < G[s].size(); i++) {
      excess[s] += G[s][i].cap;
      Push(G[s][i]);
    }
    
    while (!Q.empty()) {
      int v = Q.front();
      Q.pop();
      active[v] = false;
      Discharge(v);
    }
    
    LL totflow = 0;
    for (int i = 0; i < G[s].size(); i++) totflow += G[s][i].flow;
    return totflow;
  }
};

//expects adjList pairs in the form (v, w), where v is vertex and w is weight
pair<bool, vi> bellmanFord(vector<vii> &AdjList, int s)
{
	int V = AdjList.size();
// Bellman Ford routine
  vi dist(V, INF); dist[s] = 0;
  for (int i = 0; i < V - 1; i++)  // relax all E edges V-1 times, overall O(VE)
    for (int u = 0; u < V; u++)                        // these two loops = O(E)
      for (int j = 0; j < (int)AdjList[u].size(); j++) {
        ii v = AdjList[u][j];        // we can record SP spanning here if needed
        dist[v.first] = min(dist[v.first], dist[u] + v.second);         // relax
      }

  bool hasNegativeCycle = false;
  for (int u = 0; u < V; u++)                          // one more pass to check
    for (int j = 0; j < (int)AdjList[u].size(); j++) {
      ii v = AdjList[u][j];
      if (dist[v.first] > dist[u] + v.second)                 // should be false
        hasNegativeCycle = true;     // but if true, then negative cycle exists!
    }
	return make_pair(hasNegativeCycle, dist);

}

/*******************************************
***************polynomial stuff*************
*******************************************/

vector<long double> derive(vector<long double> poly)
{
	vector<long double> newPoly(poly.size()-1);
	for (int i = 0; i < poly.size() - 1; ++i)
	{
		newPoly[i] = poly[i+1]*(i+1);
	}
	return newPoly;
}

//multiplies poly1 of size n with poly2 of size m in O(nm) time (better possible with FFT)
vector<long double> multiply(vector<long double> poly1, vector<long double> poly2)
{
	vector<long double> result(poly1.size() + poly2.size() - 1, 0);
	for (int i = 0; i < poly1.size(); ++i)
	{
		for (int j = 0; j < poly2.size(); ++j)
		{
			result[i+j] += poly1[i] * poly2[j];
		}
	}
	return result;
}

//computes the polynomial of size n to the power p in O(pn) time
vector<long double> toThePower(vector<long double> poly, int power)
{
	vector<long double> result(poly);
	for (int i = 1; i < power; ++i)
	{
		result = multiply(result, poly);
	}
	return result;
}

//finds where x where ax^2 + bx + c = 0 or returns the empty vector. Works for a == 0. If b == 0 or discriminant = 0, returns empty vector.
vector<long double> qFormula(long double a, long double b, long double c)
{
	vector<long double> ans;
	if (a == 0)
	{
		if (b == 0)
		{
			return ans;
		}
		ans.push_back(-1 * c / b);
	}
	long double discriminant = b*b-4*a*c;
	if (discriminant < 0)
		return ans;
	ans.push_back((-b+sqrt(discriminant))/(2*a));
	ans.push_back((-b-sqrt(discriminant))/(2*a));
	return ans;
}

//evaluates the polynomial at position x
long double polyAt(vector<long double> poly, long double x)
{
	long double ans = 0;
	for (int i = 0; i < poly.size(); ++i)
	{
		ans += pow(x, i)*poly[i];
	}
	return ans;
}

/*******************************************
******vector geometry/ linear algebra*******
*******************************************/

long double dotProd(vector<long double> one, vector<long double> two)
{
	long double sum = 0;
	for (int i = 0; i < one.size(); ++i)
	{
		sum += one[i] * two[i];
	}
	return sum;
}

vector<long double> crossProd(vector<long double> first, vector<long double> second)
{
	vector<long double> result(3);
	result[0] = first[1] * second[2] - first[2] * second[1];
	result[1] = -1 * (first[0] * second[2] - first[2] * second[0]);
	result[2] = first[0] * second[1] - first[1] * second[0];
	return result;
}

vector<long double> vMinus(vector<long double> x1, vector<long double> x2)
{
	vector<long double> result(x1.size());
	for (int i = 0; i < result.size(); ++i)
	{
		result[i] = x1[i] - x2[i];
	}
	return result;
}

long double magnitude(vector<long double> x)
{
	long double sum = 0;
	for (int i = 0; i < x.size(); ++i)
	{
		sum += x[i] * x[i];
	}
	return sqrt(sum);
}

// add two matrices
vvi matrixA(vvi A, vvi B)
{
	vvi newM(A.size(), vi(A[0].size()));

	if (A.size() != B.size() || A[0].size() != B[0].size())
		cerr << "error in matrixA, dimensions do not match" << endl;

	for (int i = 0; i < A.size(); ++i)
	{
		for (int j = 0; j < A[0].size(); ++j)
		{
			newM[i][j] = A[i][j] + B[i][j];
		}
	}

	return newM;
}

// multiply two matrices
vvi matrixM(vvi A, vvi B)
{
	int m = A.size();
	int n = A[0].size();
	int q = B[0].size();

	vvi newM(m, vi(q, 0));

	if (B.size() != n)
		cerr << "error in matrixM, dimensions do not match" << endl;

	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < q; ++j)
		{
			for (int k = 0; k < n; ++k)
			{
				newM[i][j] = (newM[i][j] + A[i][k] * B[k][j]);
			}
		}
	}

	return newM;
}

// put matrix to a power using fast exponentiation
vvi matrixP(vvi A, int k)
{
	if (k == 1)
		return A;

	if (k % 2 == 0)
	{
		vvi B = matrixP(A, k/2);
		return matrixM(B, B);
	}
	else
	{
		return matrixM(matrixP(A, k-1), A);
	}
}

// Given an n-by-n integer matrix A, compute det(A) mod M, where M
// can be an arbitrary positive integer.
// The return value will be in the range [0, M) regardless of the
// sign of det(A).
// Complexity: O(n^3 logM).
int det(const int* matrix, int n, int M) {
    vector<vector<long long> > A;
    for (int i = 0; i < n; ++i)
        A.push_back(vector<long long>(matrix + i * n,
                    matrix + (i + 1) * n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            A[i][j] %= M;
    long long ans = 1;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            while (A[j][i] != 0) {
                int t = A[i][i] / A[j][i];
                for (int k = i; k < n; ++k) {
                    A[i][k] -= t * A[j][k];
                    A[i][k] %= M;
                }
                swap(A[i], A[j]);
                ans *= -1;
            }
        }
        if (A[i][i] == 0)
            return 0;
        ans *= A[i][i];
        ans %= M;
    }
    return ans < 0 ? ans + M : ans;
}

//returns the value of t where the 3-dimensional line from x1 to x2 has shortest distance to point x0
//note: for this to work, x1 should be the point where t = 0 and x2 where t = 1
long double time(vector<long double> x0, vector<long double> x1, vector<long double> x2)
{
	return -1 * dotProd(vMinus(x1, x0), vMinus(x2, x1)) / pow(magnitude(vMinus(x2, x1)), 2);
}

/*******************************************
********************debug*******************
*******************************************/
//print container
template
    < typename T
    , template<typename ELEM, typename ALLOC=std::allocator<ELEM> > class Container
    >
std::ostream& operator<< (std::ostream& o, const Container<T>& container)
{
    typename Container<T>::const_iterator beg = container.begin();
    o << "[";
	if (beg != container.end())
	{
		o << *beg;
		++beg;
	}
	for (; beg != container.end(); ++beg)
	{
		o << ", " << *beg;
	}
	o << "]";
	return o;
}

//print container of containers
template
    < typename T
    , template<typename ELEM, typename ALLOC=std::allocator<ELEM> > class Container
    , template<typename ELEM, typename ALLOC=std::allocator<ELEM> > class Container2
    >
std::ostream& operator<< (std::ostream& o, const Container<Container2<T> >& container)
{
	typename Container<Container2<T> >::const_iterator beg = container.begin();
	o << "[";
	if (beg != container.end())
	{
		o << *beg;
		++beg;
		if (beg != container.end())
			o << endl;
	}
	for (; beg != container.end()-1 && beg != container.end(); ++beg)
	{
		o << " ";
		o << *beg;
		o << endl;
	}
	if (beg != container.end())
	{
		cout << " ";
		o << *beg;
	}
	cout << "]";
	return o;
}

//print set
template < typename T>
std::ostream& operator<< (std::ostream& o, const set<T> container)
{
	typename set<T>::const_iterator beg = container.begin();
	o << "[";
	if (beg != container.end())
	{
		o << *beg;
		++beg;
	}
	for (; beg != container.end(); ++beg)
	{
		o << ", " << *beg;
	}
	o << "]";
	return o;
}

//print pair
template < typename T, typename V>
std::ostream& operator<< (std::ostream& o, const pair<T, V> p)
{
	o << "(" << p.first << ", " << p.second << ")";
	return o;
}

//print map
template < typename T, typename V>
std::ostream& operator<< (std::ostream& o, const map<T, V> mp)
{
	typename map<T, V>::const_iterator beg = mp.begin();
	o << "[";
	if (beg != mp.end())
	{
		o << *beg;
		++beg;
	}
	for (; beg != mp.end(); ++beg)
	{
		o << ", " << *beg;
	}
	o << "]";
	return o;
}

/*******************************************
****************combinatorics***************
*******************************************/
//naively computes nCr
long long nCr(int n, int r)
{
	if (r > n || n < 0 || r < 0)
	{
		return 0;
	}
	long long total = 1;
	for (int num = n-r+1, den = 1; den <= r; ++num, ++den)
	{
		total *= num;
		if (total < 0)
			cerr << "overflow in nCr" << endl;
		total /= den;
	}
	return total;
}

long long nPr(int n, int r)
{
	if (r > n)
	{
		return 0;
	}
	long long total = 1;
	for (int i = n; n-i < r; --i)
	{
		total *= i;
	}
	return total;
}

long long nPowr(int n, int r)
{
	long long total = 1;
	for (int i = 0; i < r; ++i)
	{
		total *= n;
	}
	return total;
}

long long factorial(int n)
{
	long long cur = 1;
	for (int i = 1; i <= n; ++i)
	{
		cur *= i;
		if (cur < 0)
		{
			cerr << "overflow in factorial" << endl;
		}
	}
	return cur;
}

#define SIZE 100
vector<vector<long long> > nCrMemo(SIZE, vector<long long>(SIZE, -1));

long long nCr(int n, int r, long long q)
{
	if (n < 0 || r < 0 || r > n)
	{
		return 0;
	}
	if (n > SIZE || r > SIZE)
	{
		cerr << "Error, n or r too large" << endl;
		return -1;
	}
	if (nCrMemo[n][r] != -1)
	{
		return nCrMemo[n][r];
	}
	if (n == r || r == 0)
	{
		return 1;
	}
	long long ans = (nCr(n-1, r-1, q) + nCr(n-1, r, q)) % q;
	nCrMemo[n][r] = ans;
	return ans;
}

long double binomial(int success, int attempts, long double pSuccess)
{
	vector<long double> nCrs;
	for (int num = attempts, den = success; den > 0; --num, --den)
	{
		nCrs.push_back(num / (long double)den);
	}
	vector<long double> ps(attempts);
	for (int i = 0; i < attempts; ++i)
	{
		if (i < success)
		{
			ps[i] = pSuccess;
		}
		else
		{
			ps[i] = 1 - pSuccess;
		}
	}

	long double prob = 1;
	int ns = 0;
	for (int i = 0; i < nCrs.size(); ++i)
	{
		prob *= nCrs[i];
		while (prob > 1)
		{
			prob *= ps[ns++];
		}
	}
	for (; ns < attempts; ++ns)
	{
		prob *= ps[ns];
	}
	return prob;
}

long double probOccurence(int total, int favorable, int selection, int needed)
{
	return nCr(favorable, needed) * nCr(total - favorable, selection - needed) / (long double)nCr(total, selection);
}

/*******************************************
****************miscellaneous***************
*******************************************/
//convert to base from decimal
vector<int> convertTo(int num, int base)
{
	vector<int> numB;
	while (num != 0)
	{
		numB.push_back(num % base);
		num /= base;
	}
	vector<int> numB2(numB.rbegin(), numB.rend());
	return numB2;
}

//convert from base to decimal
int convertFrom(vector<int> num, int base)
{
	int sum = 0;
	int power = 1;
	for (int i = num.size()-1; i >= 0; --i)
	{
		sum += power * num[i];
		power *= base;
	}
	return sum;
}

template < typename T>
void min_max(T &first, T &second)
{
	T tmp = first;
	first = min(first, second);
	second = max(tmp, second);
}

// generates a vector of subsets for the nCr subsets of n
void subsetsRec(vector<int> &subsets, int curSet, int n, int r)
{
	if (n == 0)
	{
		subsets.push_back(int(curSet));
	}
	else
	{
		if (r > 0)
		{
			curSet |= 1 << (n-1);
			subsetsRec(subsets, curSet, n-1, r-1);
			curSet ^= 1 << (n-1);
		}

		if (r < n)
		{
			subsetsRec(subsets, curSet, n-1, r);
		}
	}
}

/*******************************************
******************geometry******************
*******************************************/
//this function is incomplete
bool intersects(ii l1s, ii l1e, ii l2s, ii l2e)
{
	int dx1 = l1s.first - l1e.first;
	int dy1 = l1s.second - l1e.second;
	int dx2 = l2s.first - l2e.first;
	int dy2 = l2s.second - l2e.second;
	if (dx1 == dx2)
	{
		min_max(l1s.second, l1e.second);
		min_max(l2s.second, l2e.second);
		return l1e.second <= l2s.second || l2e.second <= l1s.second;
	}
	else if (dy1 == dy2)
	{
		min_max(l1s.first, l1e.first);
		min_max(l2s.first, l2e.first);
		return l1e.first <= l2s.first || l2e.first <= l1s.first;
	}
}

double intersectPt(double m1, double b1, double m2, double b2)
{
	if (m1 == m2)
	{
		cerr << "parallel lines in intersectPt" << endl;
	}
	return (b2-b1)/(m1-m2);
}

/*TODO: -number theory
			-sum of arithmetic sequence
			-sum of geometric sequence
			-sum of squares
			-sum of cubes
			-chinese remainder theorem
			-gcd, egcd

*/

LL gcd(LL a, LL b)
{
	if (b > a)
		return gcd(b, a);
	if (b == 0)
		return a;
	return gcd(b, a%b);
}

LL egcd(LL a, LL b, LL &x, LL &y)
{
	if (b > a)
		return egcd(b, a, y, x);
	if (b == 0)
	{
		x = 1;
		y = 0;
		return a;
	}
	LL ans = egcd(b, a%b, x, y);
	LL temp = x;
	x = y;
	y = temp - (a/b) * y;
	return ans;
}

LL inverse(LL a, LL n)
{
	LL x, y;
	egcd(a, n, x, y);
	return (x+n) % n;
}

bool isPrime(int n)
{
	for (int i = 2; i < sqrt(n); ++i)
	{
		if (n % i == 0)
		{
			return false;
		}
	}
	return true;
}

vector<int> primesToN(int n)
{
	vector<bool> sieve(n+1, true);
	vector<int> numbers;
	for (int i = 2; i <= sqrt(n)+EP; ++i)
	{
		if (sieve[i])
		{
			for (int j = i+i; j <= n; j += i)
			{
				sieve[j] = false;
			}
		}
	}

	for (int i = 2; i <= n; ++i)
	{
		if (sieve[i])
		{
			numbers.push_back(i);
		}
	}

	return numbers;
}

vector<bool> segSieve(LL start, LL end)
{
    vector<int> primes = primesToN((int)(sqrt(end)+EP));

    vector<bool> sieve(end-start+1, true);
    
    if (start <= 1)
        sieve[0] = false;

    if (start == 0)
        sieve[1] = false;

    for (int i = 0; i < (int)primes.size(); ++i)
    {
        for (LL j = primes[i] * max((start + primes[i]-1)/primes[i], 2LL); j <= end; j += primes[i])
        {
            sieve[j-start] = false;
        }
    }

    return sieve;

    /*vector<ll> numbers;
    for (int i = 0; i < sieve.size(); ++i)
    {
        numbers.push_back(sieve[i]+start);
    }

    return numbers;
    */
}

long long mult_mod(long long a, long long b, long long m) {
    long long q;
    long long r;
    asm(
            "mulq %3;"
            "divq %4;"
            : "=a"(q), "=d"(r)
            : "a"(a), "rm"(b), "rm"(m));
    return r;
}

/* Computes a^b mod m. Assumes 1 <= m <= 2^62-1 and 0^0=1.
 * The return value will always be in [0, m) regardless of the sign of a.
 */
long long pow_mod(long long a, long long b, long long m) {
    if (b == 0) return 1 % m;
    if (b == 1) return a < 0 ? a % m + m : a % m;
    long long t = pow_mod(a, b / 2, m);
    t = mult_mod(t, t, m);
    if (b % 2) t = mult_mod(t, a, m);
    return t >= 0 ? t : t + m;
}

/* A deterministic implementation of Miller-Rabin primality test.
 * This implementation is guaranteed to give the right result for n < 2^64
 * by using a 7-number magic base.
 * Alternatively, the base can be replaced with the first 12 prime numbers
 * (prime numbers <= 37) and still working correctly.
 */
bool is_prime(long long n) {
    long long small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (int i = 0; i < 12; ++i)
        if (n > small_primes[i] && n % small_primes[i] == 0)
            return false;
    long long base[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    long long d = n - 1;
    int s = 0;
    for (; d % 2 == 0; d /= 2, ++s);
    for (int i = 0; i < 7; ++i) {
        long long a = base[i] % n;
        if (a == 0) continue;
        long long t = pow_mod(a, d, n);
        if (t == 1 || t == n - 1) continue;
        bool found = false;
        for (int r = 1; r < s; ++r) {
            t = pow_mod(t, 2, n);
            if (t == n - 1) {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}


vector<int> primeFactorization(int n, vector<int> &primes)
{
	vector<int> factors;
	for (int i = 0; i <= sqrt(n)+EP; ++i)
	{
		while (n % primes[i] == 0)
		{
			factors.push_back(primes[i]);
			n /= primes[i];
		}
	}

	if (n != 1)
	{
		factors.push_back(n);
	}
	return factors;
}

LL g(LL x, LL n, LL b)
{
	return mult_mod(x, x, n) + b;
}

LL pollard_rho(LL n, LL start, LL b)
{
	LL x = start, y = start, d = 1;
	while (d == 1)
	{
		x = g(x, n, b);
		y = g(g(y, n, b), n, b);
		d = gcd(abs(x-y), n);
	}

	return d;
}

vector<LL> prime_factor_smart(LL n)
{
	vector<LL> factors;

	long long small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
	if (n <= 37*37)
	{
		for (int i = 0; i < 12; ++i)
		{
			while (n % small_primes[i] == 0)
			{
				factors.push_back(small_primes[i]);
				n /= small_primes[i];
			}
		}

		return factors;
	}

	if (is_prime(n))
	{
		factors.push_back(n);
		return factors;
	}
	else
	{
		LL factor;
		while (true)
		{
			factor = pollard_rho(n, rand()%10, rand()%10);

			if (factor != n)
				break;
		}

		vector<LL> fact1 = prime_factor_smart(n / factor);
		vector<LL> fact2 = prime_factor_smart(factor);

		fact1.insert(fact1.end(), fact2.begin(), fact2.end());
		return fact1;
	}
}

int main() {
	int arr[6] = {1, 2, 4, 5, 7, 8};
	for (int i = 0; i < 6; ++i)
	{
		cout << inverse(arr[i], 9) << endl;
	}

	int asdf;
	cin >> asdf;
}
