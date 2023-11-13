#include <bits/stdc++.h>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <climits>
#include <cstring>

using namespace std;
typedef long long ll;
int TotalBandwidth;
class HttpRequest
{
public:
    int ID;
    int Website_ID;
    int processing_time;
    HttpRequest(int ID, int Website_ID, int processing_time)
    {
        this->ID = ID;
        this->Website_ID = Website_ID;
        this->processing_time = processing_time;
    }
};
// Website
class Website
{
public:
    int ID;                        // Website ID
    string owner;                  // Website Owner
    queue<HttpRequest *> requests; // Requests for this website
    int processing_power;          // Processing power of this website
    int bandwidth;                 // Bandwidth of this website
    Website(int ID, string owner, int bandwidth, int processing_power)
    {
        this->ID = ID;
        this->owner = owner;
        while (this->requests.size() != 0)
        {
            this->requests.pop();
        }
        this->bandwidth = bandwidth;
        this->processing_power = processing_power;
    }
};
int factorial(int n)
{
    if (n == 0)
        return 1;
    return n * factorial(n - 1);
}

int nCr(int n, int r) { return factorial(n) / (factorial(r) * factorial(n - r)); }

void vec_input(vector<int> &v)
{
    for (auto &value : v)
    {
        cin >> value;
    }
}

// Load Balancer
class LoadBalancer
{
public:
    vector<Website *> All_Websites;        // All websites
    queue<HttpRequest *> All_HttpRequests; // All Http Requests
    Website *dequeue_from;                 // Website from which request is dequeued
    vector<int> Order;                     // Order of processing of requests
    void Add_Website(int web_id, string owner, int bandwidth, int processing_power)
    {
        Website *Web = new Website(web_id, owner, bandwidth, processing_power);
        All_Websites.push_back(Web);
    }
    void Enqueue_Request(HttpRequest *Request)
    {
        Website *Web;
        for (int i = 0; i < All_Websites.size(); i += 1)
        {
            if (All_Websites[i]->ID == Request->Website_ID)
            {
                Web = All_Websites[i];
                break;
            }
        }
        Web->requests.push(Request);
        All_HttpRequests.push(Request);
    }
    void Dequeue_Request()
    {
        dequeue_from->requests.pop();
    }
    void WFQ_Scheduling()
    {
        map<int, int> LatestProcessedTime;
        map<int, int> latest_finished_time;
        map<int, double> allocated_bandwidth;
        map<int, set<pair<int, int>>> finished;
        int tot_band = 0;
        for (auto i : All_Websites)
        {
            tot_band += (i->bandwidth);
        }
        cout << tot_band << endl;
        for (auto i : All_Websites)
        {
            allocated_bandwidth[i->ID] = ((double(i->bandwidth)) / (tot_band)) * TotalBandwidth;
        }
        // Assume all requests arrived at same time.
        while (All_HttpRequests.size() != 0)
        {
            HttpRequest *Request = All_HttpRequests.front();
            All_HttpRequests.pop();
            Website *Web;
            for (auto i : All_Websites)
            {
                if (i->ID == Request->Website_ID)
                {
                    Web = i;
                    break;
                }
            }
            int web_id = Web->ID;
            int req_id = Request->ID;
            int TotalTimeTaken = LatestProcessedTime[web_id] + Request->processing_time;
            int TotalData = (Web->processing_power) * (Request->processing_time);
            LatestProcessedTime[web_id] = TotalTimeTaken;
            double transfer_time = TotalData / allocated_bandwidth[web_id];
            if (transfer_time != int(transfer_time))
            {
                transfer_time = int(transfer_time) + 1;
            }
            TotalTimeTaken = max(TotalTimeTaken, latest_finished_time[web_id]) + transfer_time;
            latest_finished_time[web_id] = TotalTimeTaken;
            finished[TotalTimeTaken].insert({web_id, req_id});
            dequeue_from = Web;
            Dequeue_Request();
        }
        for (auto t : finished)
        {
            for (auto req : t.second)
            {
                Order.push_back({req.second});
            }
        }
    }
    void Print_Order()
    {
        printf("Order of Processing: ");
        for (auto i : Order)
        {
            printf("%d ", i);
        }
        cout << endl;
    }
};



void vec_output(vector<int> v)
{
    for (auto &value : v)
    {
        cout << value << " ";
    }
    cout << endl;
}

void vecpair_ouptut(vector<pair<int, int>> v)
{
    for (auto &value : v)
    {
        cout << value.first << " " << value.second << endl;
    }
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    LoadBalancer SuperMaster;
    int n, m;
    printf("Enter the number of Websites :\n");
    cin >> n;
    for (int i = 0; i < n; i += 1)
    {
        int ID;                                                          // Website ID
        string owner;                                                    // Website Owner
        int processing_power;                                            // Processing power of this website
        int bandwidth;                                                   // Bandwidth of this website
        cin >> ID >> owner >> bandwidth >> processing_power;             // Input
        SuperMaster.Add_Website(ID, owner, bandwidth, processing_power); // Add website to Load Balancer
    }
    printf("Enter the number of Http Requests :\n");
    cin >> m;
    for (int i = 0; i < m; i += 1)
    {
        int ID;
        int Website_ID;
        int processing_time;
        cin >> ID >> Website_ID >> processing_time;
        HttpRequest *Req = new HttpRequest(ID, Website_ID, processing_time);
        SuperMaster.Enqueue_Request(Req);
    }
    printf("Enter Total bandwidth:\n");
    cin >> TotalBandwidth;
    SuperMaster.WFQ_Scheduling();
    SuperMaster.Print_Order();

    return 0;
}