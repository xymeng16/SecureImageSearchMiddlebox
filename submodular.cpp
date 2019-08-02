//
// Created by xiangyi on 8/1/19.
//

#include "submodular.h"
#include "data_handler.h"
#include <cstdint>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;
Graph::Graph(data_pack *data) : numOfSCC(0)
{
    this->V = data->img_num;
    this->adj = new list<int>[V];
    this->scc = new int[this->V];
    memset(this->scc, 0, sizeof(int) * this->V);

    this->dist_mat = new int *[V];

    for (int i = 0; i < V; i++)
    {
        this->dist_mat[i] = new int[V];

        for (int j = 0; j < V; j++)
        {
            this->dist_mat[i][j] = dist(data->a_hash[i], data->a_hash[j], LSH_L, DM_LEN);
//            this->dist_mat[j][i] = dist(data->a_hash[i], data->a_hash[j], LSH_L, DM_LEN);

        }
    }
    for (int i = 0; i < data->img_num - 1; i++)
    {
        for (int j = i + 1; j < data->img_num; j++)
        {

            if(this->dist_mat[i][j] >= dist_threshold)
            {
//                cout << "dist between " << i << " and " << j << ":"
//                     << dist(data->a_hash[i], data->a_hash[j], LSH_L, DM_LEN) << endl;
                this->addEdge(i, j);
                this->addEdge(j, i);
            }
        }
    }

}

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w);
}

// A recursive function that finds and prints strongly connected
// components using DFS traversal
// u --> The vertex to be visited next
// disc[] --> Stores discovery times of visited vertices
// low[] -- >> earliest visited vertex (the vertex with minimum
//			 discovery time) that can be reached from subtree
//			 rooted with current vertex
// *st -- >> To store all the connected ancestors (could be part
//		 of SCC)
// stackMember[] --> bit/index array for faster check whether
//				 a node is in stack
void Graph::SCCUtil(int u, int disc[], int low[], stack<int> *st,
                    bool stackMember[])
{
    // A static variable is used for simplicity, we can avoid use
    // of static variable by passing a pointer.
    static int time = 0;

    // Initialize discovery time and low value
    disc[u] = low[u] = ++time;
    st->push(u);
    stackMember[u] = true;

    // Go through all vertices adjacent to this
    list<int>::iterator i;
    for (i = adj[u].begin(); i != adj[u].end(); ++i)
    {
        int v = *i; // v is current adjacent of 'u'

        // If v is not visited yet, then recur for it
        if (disc[v] == -1)
        {
            SCCUtil(v, disc, low, st, stackMember);

            // Check if the subtree rooted with 'v' has a
            // connection to one of the ancestors of 'u'
            // Case 1 (per above discussion on Disc and Low value)
            low[u] = min(low[u], low[v]);
        }

            // Update low value of 'u' only of 'v' is still in stack
            // (i.e. it's a back edge, not cross edge).
            // Case 2 (per above discussion on Disc and Low value)
        else if (stackMember[v] == true)
            low[u] = min(low[u], disc[v]);
    }

    // head node found, pop the stack and print an SCC
    int w = 0; // To store stack extracted vertices
    if (low[u] == disc[u])
    {
        while (st->top() != u)
        {
            w = (int) st->top();
//            cout << w << " ";
            this->scc[w] = this->numOfSCC;
            stackMember[w] = false;
            st->pop();
        }
        w = (int) st->top();
//        cout << w << "\n";
        stackMember[w] = false;
        st->pop();
        this->numOfSCC++;
    }
}

// The function to do DFS traversal. It uses SCCUtil()
int Graph::SCC()
{
    int *disc = new int[V];
    int *low = new int[V];
    bool *stackMember = new bool[V];
    stack<int> *st = new stack<int>();

    // Initialize disc and low, and stackMember arrays
    for (int i = 0; i < V; i++)
    {
        disc[i] = NIL;
        low[i] = NIL;
        stackMember[i] = false;
    }

    // Call the recursive helper function to find strongly
    // connected components in DFS tree with vertex 'i'
    for (int i = 0; i < V; i++)
        if (disc[i] == NIL)
            SCCUtil(i, disc, low, st, stackMember);

    return this->numOfSCC;
}

inline int coverage(Graph *g, vector<int> selectedImgs)
{
    int val = 0;
    for (int i = 0; i < g->V; ++i)
    {
        int tmp = INT32_MIN;
        for (int j = 0; j < selectedImgs.size(); j++)
        {
            tmp = max(tmp, g->dist_mat[i][selectedImgs[j]]);
        }
        val += tmp;
    }

    return val;
}

inline int diversity(Graph *g, vector<int> selectedImgs)
{
    int val = 0;
    bool *visSCC = new bool[g->V];
    memset(visSCC, 0, sizeof(bool) * g->V);

    for (int i = 0; i < selectedImgs.size(); i++)
    {
        int sccid = g->scc[selectedImgs[i]];
        if (visSCC[sccid])
        {
            continue;
        }
        else
        {
            visSCC[sccid] = true;
            val++;
        }
    }

    return val;
}

double submodular_func(Graph *g, vector<int> selectedImgs)
{
    return kCov * coverage(g, selectedImgs) + kDiv * diversity(g, selectedImgs);
}

vector<int> submodular(Graph *g)
{
    int imgUploadAmount = g->SCC();
    vector<int> selectedImgs;
    bool *selected = new bool[g->V];

    if(imgUploadAmount > 0)
    {
        srand(time(NULL)); // Randomly generate the initial img
        int init = rand() % g->V;
        selectedImgs.push_back(init);
        double score = submodular_func(g, selectedImgs);
        selected[init] = true;
        cout << "Amount: " << imgUploadAmount << endl;
        for(int i = 1; i < imgUploadAmount; i++)
        {
            double preScore = -1, curScore;
            int select;
            for (int j = 0; j < g->V; j++)
            {
                if (selected[j])
                {
                    continue;
                }
                selectedImgs.push_back(j);
                curScore = submodular_func(g, selectedImgs);
                if(curScore > preScore)
                {
                    select = j;
                    preScore = curScore;
                }
                selectedImgs.pop_back();
            }
            if(preScore < score)
            {
                imgUploadAmount = i - 1;
                break;
            }
            else
            {
                selectedImgs.push_back(select);
                selected[select] = true;
                score = preScore;
            }
        }
    }

    return selectedImgs;
}