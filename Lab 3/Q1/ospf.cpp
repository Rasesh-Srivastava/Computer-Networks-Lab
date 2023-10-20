#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>
#include <string>
#include <unistd.h>
using namespace std;

int inf = 1e9;
// Router class
class Router
{
public:
// router_id is the id of the router
    int router_id;
// routing_table is the routing table of the router
    map<int, int> routing_table;
    vector<pair<Router *, int>> neighbours;
    map<int, int> dist;
    // constructor
    Router(int id, int n)
    {
        this->router_id = id;
    }
    // add neighbour to the router
    void add_neighbor(Router *neighbour, int cost)
    {
        neighbours.push_back({neighbour, cost});
        routing_table[neighbour->router_id] = neighbour->router_id;
    }
    // update routing table of the router
    void update_routing_table(int n)
    {// dis stores the distance of the router from the router_id
        map<int, int> dis;
        for (int i = 1; i <= n; i++)
        {
            dis[i] = inf;
        }
        // distance of router_id from itself is 0
        dis[router_id] = 0;
        vector<int> parent(n + 1);
        // parent[i] stores the parent of i in the shortest path from router_id to i
        for (int i = 1; i <= n; i++)
        {
            parent[i] = i;
        }
        // pq is the priority queue used in Dijkstra's algorithm
        priority_queue<pair<int, Router *>, vector<pair<int, Router *>>, greater<pair<int, Router *>>> pq;
        pq.push({0, this});
        while (!pq.empty())
        {// Dijkstra's algorithm
            int cost = pq.top().first;
            Router *node = pq.top().second;
            pq.pop();

            for (auto &x : node->neighbours)
            {
                if (dis[x.first->router_id] > dis[node->router_id] + x.second)
                {
                    dis[x.first->router_id] = dis[node->router_id] + x.second;
                    pq.push(make_pair(dis[x.first->router_id], x.first));

                    parent[x.first->router_id] = node->router_id;
                }
            }
        }
        // paths[i] stores the path from router_id to i
        vector<vector<int>> paths(n + 1);
        // update paths
        for (int i = 1; i <= n; i++)
        {
            vector<int> path;
            int node = i;
            while (parent[node] != node)
            {
                path.push_back(node);
                node = parent[node];
            }
            path.push_back(this->router_id);
            reverse(path.begin(), path.end());
            paths[i] = path;
        }
        // update routing table
        for (int i = 1; i <= n; i++)
        {
            if (i != this->router_id)
            {
                routing_table[i] = paths[i][1];
            }
            else
            {
                routing_table[i] = this->router_id;
            }
        }
        dist = dis;
    }
    // print routing table of the router
    void print_routing_table()
    {// print routing table
        cout << "***********************************************\n";
        cout << "          Routing table for Router " << this->router_id << "          " << endl;
        for (auto &x : routing_table)
        {
            if (dist[x.first] == inf)
            {
                cout << "\nDestination Router " << x.first << " -> Next-Hop not possible"
                     << " Total Cost: infinity";
            }
            else
            {
                cout << "\nDestination Router " << x.first << " -> Next-Hop " << x.second << " Total Cost " << dist[x.first];
            }
        }
        cout << "\n\n";
        cout << "***********************************************\n";
    }
};
// simulate OSPF
void simulate_OSPF(int src, int dest, vector<Router *> &Routers)
{
    // n is the number of routers
    int n = Routers.size();
    Router *temp = Routers[src];
    vector<int> path;
    // path stores the path taken by the packet to reach from src to dest
    path.push_back(src);
    temp->update_routing_table(n - 1);
    // next_hop is the next router to which the packet is to be sent
    int next_hop = temp->routing_table[dest];
    path.push_back(next_hop);
    // print routing table of the router
    temp->print_routing_table();
    if (temp->dist[dest] == inf || next_hop==0)
    {// if next_hop is 0 then destination is not reachable
        cout << "Destination not reachable\n\n";
        return;
    }
    cout << endl
         << "Jumping to next Router which is " << next_hop << endl
         << endl;
    sleep(2);
    while (next_hop != dest)
    {// update routing table of the router
        temp = Routers[next_hop];
        temp->update_routing_table(n - 1);
        temp->print_routing_table();
        // next_hop is the next router to which the packet is to be sent
        next_hop = temp->routing_table[dest];
        path.push_back(next_hop);
        if (temp->dist[dest] == inf || next_hop==0)
        {       // if next_hop is 0 then destination is not reachable
            cout << "Destination not reachable\n\n";
            return;
        }
        cout << endl
             << "Jumping to next Router which is " << next_hop << endl
             << endl;
        sleep(2);
    }
    cout << endl
         << "Destination reached which is " << dest << endl
         << endl;
    cout << "Path taken by the packet to reach from " << src << " to " << dest << " is  ";
    for (int i = 0; i < path.size() - 1; i++)
    {
        cout << path[i] << "->";
    }
    cout << path[path.size() - 1] << endl
         << endl;
    cout << "Total cost to reach the destination is " << Routers[src]->dist[dest] << endl
         << endl;
    return;
}
// main function
int main()
{
    // n is the number of routers
    cout << "Enter number of Routers and links to be included in Network" << endl;
    // l is the number of links
    int n, l;
    cin >> n >> l;
    // Routers is the vector of all the routers
    vector<Router *> Routers(n + 1);
    // create n routers
    for (int i = 1; i <= n; i++)
    {
        Routers[i] = new Router(i, n);
    }
    // add neighbours to the routers
    for (int i = 1; i <= l; i++)
    {
        int r1, r2;
        int cost;
        cout << "Enter link costs associated with the Routers in the form (Router 1,Router 2,Link Cost) \n";
        cin >> r1 >> r2 >> cost;
        Routers[r1]->add_neighbor(Routers[r2], cost);
        Routers[r2]->add_neighbor(Routers[r1], cost);
    }
    // update routing table of all the routers
    while (1)
    {
        cout << "Enter 'y' to continue 'exit' to quit " << endl
             << endl;
        string s;
        cin >> s;
        if (s != "y" && s != "exit")
        {
            cout << "Command not recognised" << endl
                 << endl;
            continue;
        }
        if (s == "exit")
        {
            break;
        }
        cout << "Enter '1' to simulate OSPF protocol\nEnter '2' to print a Router's routing table\n\n";
        char c;
        cin >> c;
        if (c == '1')
        {// simulate OSPF
            cout << "Enter source and destination Routers to simulate OSPF " << endl;
            int src, dest;
            cin >> src >> dest;
            simulate_OSPF(src, dest, Routers);
        }
        else if (c == '2')
        {// print routing table of a router
            cout << "Enter the Router to print its routing table " << endl;
            int node;
            cin >> node;
            Routers[node]->update_routing_table(n);
            Routers[node]->print_routing_table();
        }
        else
        {
            cout << "Command not recognised" << endl
                 << endl;
            continue;
        }
    }

    return 0;
}