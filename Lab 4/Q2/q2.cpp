#include <bits/stdc++.h> // Include commonly used C++ libraries.
#include <unistd.h>       // Include the unistd library for sleep functionality.
#include <random>         // Include the random library for generating random numbers.
#include <chrono>         // Include the chrono library for time-related functions.
#include <thread>         // Include the thread library for thread-related operations.
#include <deque>          // Include the deque library for double-ended queues.
#include <queue>          // Include the queue library for priority queues.
#include <vector>         // Include the vector library for dynamic arrays.
#include <climits>        // Include the climits library for integer limits.
#include <iostream>       // Include the input/output stream library.
#include <algorithm>      // Include the algorithm library for various algorithms.
#include <iomanip>        // Include the iomanip library for output formatting.
#include <cmath>          // Include the cmath library for mathematical operations.
#include <fstream>        // Include the fstream library for file operations.
#include <string>         // Include the string library for string manipulation.
#include <sstream>        // Include the sstream library for string stream operations.
#include <ctime>          // Include the ctime library for time functions.
using namespace std;
int PacketDropped;
// Structure for a packet.
struct packet
{
    double arrival;
    double departure;
    double serviceTime;
    packet()
    {
        arrival = 0;
        departure = 0;
        serviceTime = -1;
    }
};
// Structure for a server.
struct server
{
    // Queue for storing packets.
    deque<packet *> Service_Queue;
    int size;
    // Exponential distribution for generating service times.
    exponential_distribution<double> distribution;
    // Random number generator.
    default_random_engine generator;
    double prev, total, summation, utilisation, avgQueueSize, lambda;
    // Priority queue for storing packets in order of their departure times.
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    // Constructor for a server.
    server(int Buffer_Size, double Lambda)
    {
        Service_Queue.clear();
        size = Buffer_Size;
        lambda = Lambda;
        distribution.param(std::exponential_distribution<double>::param_type(lambda));
        prev = 0;
        total = 0;
        summation = 0;
        utilisation = 0;
    }
    // Function to randomise the generator.
    void randomise(int j)
    {
        generator.seed(int(chrono::system_clock::now().time_since_epoch().count()) + j);
    }
    // Function to return the size of the queue.
    int Service_Queue_Size()
    {
        return Service_Queue.size();
    }
    // Function to calculate the average queue length.
    void Average_Queue_Length(double maxTot)
    {
        while (!Service_Queue.empty())
        {
            dequePacket();
        }
        avgQueueSize = summation / maxTot;
        utilisation = (utilisation / maxTot) * 100;
    }
    // Function to dequeue a packet.
    void dequePacket()
    {
        total += Service_Queue.front()->departure - prev;
        utilisation += Service_Queue.front()->departure - prev;
        summation += double(Service_Queue.size()) * (Service_Queue.front()->departure - prev);
        prev = Service_Queue.front()->departure;
        Service_Queue.pop_front();
    }
    // Function to enqueue a packet.
    double enquePacket(packet *p)
    {
        if (Service_Queue.size() >= size)
        {
            p->departure = -1;
            PacketDropped++;
            return -1;
        }
        total = total + p->arrival - prev;
        if (Service_Queue.size() != 0)
            utilisation += p->arrival - prev;
        summation += double(Service_Queue.size()) * (p->arrival - prev);
        prev = p->arrival;
        if (!Service_Queue.empty())
        {
            p->departure = Service_Queue.back()->departure + distribution(generator);
            cout << "Service Time = " << p->departure - Service_Queue.back()->departure << endl;
            p->serviceTime = p->departure - Service_Queue.back()->departure;
        }
        else
        {
            p->departure = p->arrival + distribution(generator);
            cout << "Service Time = " << p->departure - p->arrival << endl;
            p->serviceTime = p->departure - p->arrival;
        }
        Service_Queue.push_back(p);
        return p->departure;
    }
};
// Function to input a vector.
void vecpair_ouptut(vector<pair<int, int>> v)
{
    for (auto &value : v)
    {
        cout << value.first << " " << value.second << endl;
    }
}
// Function to output a vector.
int factorial(int n)
{
    if (n == 0)
        return 1;
    return n * factorial(n - 1);
}
// Function to calculate nCr.
int nCr(int n, int r) { return factorial(n) / (factorial(r) * factorial(n - r)); }
// Function to simulate the model.
void simulate(vector<server> &Servers, vector<packet> &Packets, double &avgWaiting)
{
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    int i = 0;
    while (i < Packets.size())
    {
        if (pq.empty() || Packets[i].arrival < pq.top().first)
        {
            int index = -1, length = INT_MAX;
            for (int j = 0; j < Servers.size(); j++)
            {
                if (Servers[j].Service_Queue_Size() < length)
                {
                    index = j;
                    length = Servers[j].Service_Queue_Size();
                }
            }
            cout << "Packet " << i << " Enqued at Server " << index + 1 << endl;
            pq.push({Servers[index].enquePacket(&Packets[i]), index});
            if (pq.top().first == -1)
            {
                cout << "***Packet dropped due to insufficient Buffer***" << endl;
                PacketDropped++;
            }
            i++;
        }
        else
        {
            int index = pq.top().second;
            if (pq.top().first > 0)
            {
                cout << "***Job finished at Server*** " << index + 1 << endl;
                Servers[index].dequePacket();
            }
            pq.pop();
        }
    }
    while (!pq.empty())
    {
        int index = pq.top().second;
        if (pq.top().first > 0)
        {
            cout << "***Job finished at Server*** " << index + 1 << endl;
            Servers[index].dequePacket();
        }
        pq.pop();
    }
    double maxTot = 0;
    for (int i = 0; i < Servers.size(); i++)
    {
        maxTot = max(maxTot, Servers[i].total);
    }
    for (int i = 0; i < Servers.size(); i++)
    {
        Servers[i].Average_Queue_Length(maxTot);
    }
    double avg = 0;
    int total = 0;
    for (auto x : Packets)
    {
        if (x.departure != -1)
        {
            avg += x.departure - x.arrival - x.serviceTime;
            total++;
        }
    }
    avgWaiting = avg / double(total);
}
// Function to input a vector.
void vec_input(vector<int> &v)
{
    for (auto &value : v)
    {
        cin >> value;
    }
}
 
void vec_output(vector<int> v)
{
    for (auto &value : v)
    {
        cout << value << " ";
    }
    cout << endl;
}
// Function to simulate the model.
void simulate_model(int population_size, double lambda, int num_of_servers, int buffer_size, double mu, int choice)
{
    cout << "\n\n****** Running Simulation with :: ********\n\n";
    cout << "Population Size = " << population_size << endl;
    cout << "Number of Servers = " << num_of_servers << endl;
    cout << "Arrival Rate = " << lambda << endl;
    cout << "Service Rate = " << mu << endl;
    if (choice == 1 || choice == 3)
    {
        cout << "Buffer Size = " << 0 << endl
             << endl;
    }
    else
    {
        cout << "Buffer Size = " << buffer_size << endl
             << endl;
    }
    sleep(3);
    PacketDropped = 0;
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    exponential_distribution<double> distribution(lambda);
    vector<server> Servers(num_of_servers, {buffer_size, mu});
    for (int i = 0; i < num_of_servers; i++)
    {
        Servers[i].randomise(i);
    }
    vector<packet> v(population_size);
    double prev = 0;
    for (int i = 0; i < population_size; ++i)
    {
        v[i].arrival = distribution(generator) + prev;
        prev = v[i].arrival;
        cout << "Packet " << i << " Arrival: " << v[i].arrival << endl;
    }
    double avgWaiting;
    simulate(Servers, v, avgWaiting);
    for (int i = 0; i < population_size; ++i)
    {
        cout << "Packet " << i << " Departure: " << v[i].departure << endl;
    }
    if (choice == 1 || choice == 3)
    {
        for (int i = 0; i < Servers.size(); i++)
        {
            cout << "Server " << i + 1 << " Utitlisation: " << Servers[i].utilisation << endl;
        }
    }
    else
    {
        for (int i = 0; i < Servers.size(); i++)
        {
            cout << "Server " << i + 1 << " Avg Queue Length :" << Servers[i].avgQueueSize << " Utitlisation: " << Servers[i].utilisation << endl;
        }
    }
    cout << "Total Packets dropped: " << PacketDropped/2 << endl;
    double time_taken = avgWaiting > 0 ? avgWaiting : 0;
    cout << "Avg Waiting Time: " << time_taken << endl;
    return;
}
int main()
{
    // Input the choice.
    while (1)
    {
        // Input the choice.
        int choice;
        cout << "******************* Airport Security Queue Models *******************\n\n";
        cout << "              Simulate the Airport Security Screening Process               \n\n";
        cout << "1.)  With only one Scanner -- Single Server Model " << endl;
        cout << "2.)  With only one Scanner -- Buffered Single-Server Model" << endl;
        cout << "3.)  With multiple Scanner -- Multi-Server Model" << endl;
        cout << "4.)  With multiple Scanner -- Buffered Multi-Server Model"<< endl;
        cout << "5.)  To Exit the Simulation" << endl<<endl;
        cin >> choice;
        // Input the parameters.
        if (choice == 1)
        {
            int population_size;
            cout << "Enter Population size\n";
            cin >> population_size;
            double lambda, mu;
            cout << "Enter lambda and mu you want for your model\n";
            cin >> lambda >> mu;
            simulate_model(population_size, lambda, 1, 1, mu, 1);
        }
        else if (choice == 2)
        {
            int population_size;
            cout << "Enter Population size\n";
            cin >> population_size;
            double lambda, mu;
            cout << "Enter lambda and mu you want for your model\n";
            cin >> lambda >> mu;
            int buffer_size;
            cout << "Enter Buffer size you want for a single server model\n";
            cin >> buffer_size;
            simulate_model(population_size, lambda, 1, buffer_size, mu, 2);
        }
        else if (choice == 3)
        {
            int population_size;
            cout << "Enter Population size\n";
            cin >> population_size;
            double lambda, mu;
            cout << "Enter lambda and mu you want for your model\n";
            cin >> lambda >> mu;
            int num_of_servers;
            cout << "Enter number of servers you want to incorporate in model\n";
            cin >> num_of_servers;
            simulate_model(population_size, lambda, num_of_servers, 1, mu, 3);
        }
        else if (choice == 4)
        {
            int population_size;
            cout << "Enter Population size\n";
            cin >> population_size;
            double lambda, mu;
            cout << "Enter lambda and mu you want for your model\n";
            cin >> lambda >> mu;
            int num_of_servers;
            cout << "Enter number of servers you want to incorporate in model\n";
            cin >> num_of_servers;
            int buffer_size;
            cout << "Enter Buffer size you want for a multiple server model\n";
            cin >> buffer_size;
            simulate_model(population_size, lambda, num_of_servers, buffer_size, mu, 4);
        }
        else
        {
            break;
        }
    }
    // Return 0 to signify normal termination.
    return 0;
}