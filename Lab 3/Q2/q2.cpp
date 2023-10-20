#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
using namespace std;
#define all(x) (x).begin(), (x).end()
#define pyes cout << "YES" << endl;
#define pno cout << "NO" << endl;
#define outArray(a)        \
    for (auto x : a)       \
    {                      \
        cout << x << endl; \
    }
#define print(a)          \
    for (auto x : a)      \
    {                     \
        cout << x << " "; \
    }                     \
    cout << endl;
#define forn for (ll i = 0; i < n; i++)
// Define constants for simulation
const int maximumBackOffInterval = 1024;  // Adjustable backoff interval
const int packetTransmissionTime = 5; // Packets per time unit
const int maximumPacketSize = 50;         // Maximum packet size in bytes
const int maximumPacketArrivalTime = 10; // Maximum packet arrival time
const int propagationSpeed = 1;        // Propagation speed

// Define the states for the FSM
enum StateOfTheNode
{
    Ready = 0,
    Backoff,
    Transmitting
};
class Frame
{
public:
    bool isNew = 1;
    int start;
    int end;
    int size;
    Frame() : start(-1), end(-1), size(0) {}
    Frame(int s, int e) : start(s), end(e) {}
    Frame(int s, int e, bool isN, int size) : start(s), end(e), isNew(isN), size(size) {}
};
// Node class
class Node
{
public:
    int UniqueIdentifier;
    int NumberOfSuccessfulTransmissions;
    int NumberOfBackOffs;
    int NumberOfCollisions;
    bool DetectionOfA_Collision;
    StateOfTheNode state = StateOfTheNode::Ready;
    int TotalNumberOfCollisions;
    int ThePosition;
    bool fromBackoff = 0;
    Frame TheTransmittingFrame;
    vector<Frame> QueueWhichHoldsReadyFrames;
    vector<Frame> QueueWhichHoldsTheTransmittingFrames;
    int BackOffTime;
    Node(int UniqueIdentifier) : UniqueIdentifier(UniqueIdentifier), NumberOfSuccessfulTransmissions(0), NumberOfBackOffs(0), NumberOfCollisions(0), TotalNumberOfCollisions(0), ThePosition(0), fromBackoff(false), TheTransmittingFrame(Frame()) {}
    int TotalTimeTaken(int PresentTime)
    {
        return abs(PresentTime - QueueWhichHoldsTheTransmittingFrames[0].start);
    }
    StateOfTheNode AttemptingToTransmitFrame(int PresentTime, vector<Node> &nodes)
    {
        switch (state)
        {
        case StateOfTheNode::Ready:
            if (fromBackoff)
            {
                QueueWhichHoldsTheTransmittingFrames.erase(QueueWhichHoldsTheTransmittingFrames.begin());
                QueueWhichHoldsReadyFrames.insert(QueueWhichHoldsReadyFrames.begin(), TheTransmittingFrame);
                fromBackoff = false;
            }
            if (QueueWhichHoldsReadyFrames.size() && QueueWhichHoldsReadyFrames[0].start <= PresentTime)
            {
                NumberOfCollisions = 0;
                // Check for other nodes transmitting, if yes, this means that the channel is busy
                for (Node &node : nodes)
                {
                    if (node.QueueWhichHoldsTheTransmittingFrames.empty())
                    {
                        continue;
                    }
                    int frameDisplacement = propagationSpeed * node.TotalTimeTaken(PresentTime);
                    if (node.UniqueIdentifier != UniqueIdentifier && node.state == StateOfTheNode::Transmitting &&
                        frameDisplacement >= abs(node.ThePosition - ThePosition))
                    {
                        std::cout << "Node " << UniqueIdentifier << ": Transmission detected by Node " << node.UniqueIdentifier << ". Waiting for transmission to complete." << std::endl;
                        NumberOfBackOffs++;
                        return StateOfTheNode::Ready;
                    }
                }
                // If the channel is idle, start transmitting the frame(s)
                Frame frame = QueueWhichHoldsReadyFrames[0];
                frame.start = PresentTime;
                frame.end = PresentTime + (frame.size / packetTransmissionTime);
                QueueWhichHoldsTheTransmittingFrames.push_back(frame);
                QueueWhichHoldsReadyFrames.erase(QueueWhichHoldsReadyFrames.begin());
                if (frame.isNew)
                {
                    std::cout << "Node " << UniqueIdentifier << ":"
                              << " Transmission started of packet size " << frame.size << std::endl;
                }
                else
                {
                    std::cout << "Node " << UniqueIdentifier << ":"
                              << " Retransmitting the previous packet of size " << frame.size << std::endl;
                }
                return StateOfTheNode::Transmitting;
            }
            else
            {
                return StateOfTheNode::Ready;
            }
            break;
        case StateOfTheNode::Backoff:
            if (fromBackoff)
            {
                QueueWhichHoldsTheTransmittingFrames.erase(QueueWhichHoldsTheTransmittingFrames.begin());
                QueueWhichHoldsReadyFrames.insert(QueueWhichHoldsReadyFrames.begin(), TheTransmittingFrame);
                fromBackoff = false;
            }
            BackOffTime--;
            std::cout << "Node " << UniqueIdentifier << ":"
                      << " Backoff remaining: " << BackOffTime << " time units." << std::endl;
            if (BackOffTime - 1 == 0)
            {
                return StateOfTheNode::Ready;
            }
            else
            {
                return StateOfTheNode::Backoff;
            }
            break;
        case StateOfTheNode::Transmitting:
            // Check for collisions during transmission
            DetectionOfA_Collision = false;
            vector<int> NodesWhichCollided;
            for (Node &node : nodes)
            {
                if (node.QueueWhichHoldsTheTransmittingFrames.empty())
                {
                    continue;
                }
                int frameDisplacement = propagationSpeed * node.TotalTimeTaken(PresentTime);
                if (node.UniqueIdentifier != UniqueIdentifier && node.state == StateOfTheNode::Transmitting &&
                    frameDisplacement >= abs(node.ThePosition - ThePosition))
                {

                    DetectionOfA_Collision = true;
                    NodesWhichCollided.push_back(node.UniqueIdentifier);
                }
            }
            if (DetectionOfA_Collision)
            {
                std::cout << "Node " << UniqueIdentifier << ": Collision detected with these node(s) -> ";
                for (int i = 0; i < NodesWhichCollided.size(); i += 1)
                {
                    if (i != NodesWhichCollided.size() - 1)
                    {
                        std::cout << "Node: " << NodesWhichCollided[i] << ", ";
                    }
                    else
                    {
                        std::cout << "Node: " << NodesWhichCollided[i] << " ";
                    }
                }
                std::cout << "\n";
                Frame RetransmitTheFrame = QueueWhichHoldsTheTransmittingFrames[0];
                // Generate Backoff
                NumberOfCollisions += 1;
                if (NumberOfCollisions <= 10)
                {
                    BackOffTime = rand() % (1 << NumberOfCollisions);
                }
                else
                {
                    BackOffTime = rand() % maximumBackOffInterval;
                }
                if (BackOffTime == 0)
                {
                    BackOffTime += 2;
                }
                std::cout << "Node " << UniqueIdentifier << ": Waiting for Backoff time of : " << BackOffTime << " time units." << std::endl;
                TotalNumberOfCollisions++;
                NumberOfBackOffs++;
                RetransmitTheFrame.start = PresentTime + BackOffTime;
                RetransmitTheFrame.end = RetransmitTheFrame.start + (RetransmitTheFrame.size / packetTransmissionTime);
                RetransmitTheFrame.isNew = 0;
                // Do this in next iteration only - thats why fromBackoff variable
                // QueueWhichHoldsTheTransmittingFrames.erase(QueueWhichHoldsTheTransmittingFrames.begin());
                // QueueWhichHoldsReadyFrames.insert(QueueWhichHoldsReadyFrames.begin(), RetransmitTheFrame);
                if (BackOffTime == 1)
                {
                    fromBackoff = true;
                    TheTransmittingFrame = RetransmitTheFrame;
                    return StateOfTheNode::Ready;
                }
                else
                {
                    fromBackoff = true;
                    TheTransmittingFrame = RetransmitTheFrame;
                    return StateOfTheNode::Backoff;
                }
            }
            else
            {
                std::cout << "Node " << UniqueIdentifier << ": Transmitting the frames... " << std::endl;
                return StateOfTheNode::Transmitting;
            }
            break;
        }
        return StateOfTheNode::Ready;
    }
    int getTheTotalNumberOfSuccessfulTransmissions() const
    {
        return NumberOfSuccessfulTransmissions;
    }
    int getBackoffCount() const
    {
        return NumberOfBackOffs;
    }
    int getTheTotalNumberOfCollisions() const
    {
        return TotalNumberOfCollisions;
    }
    int getTheUniqueIdentifier() const
    {
        return UniqueIdentifier;
    }
};
// Function to generate a random integer between min and max (both inclusive)
int RandomIntegerGenerator(int min, int max)
{
    return rand() % (max - min + 1) + min;
}
void RandomPointsGenerator(int n, int L, int MIN_DIST, vector<Node> &nodes)
{
    vector<int> points;
    if (n <= 0 || L <= 0 || MIN_DIST <= 0 || MIN_DIST >= L)
    {
        std::cout << "Invalid input." << std::endl;
        return;
    }
    srand(static_cast<unsigned int>(time(nullptr)));
    // Generate n random points
    for (int i = 0; i < n; i+=1)
    {
        points.push_back(RandomIntegerGenerator(0, L));
    }
    sort(points.begin(), points.end());
    // Adjust the points to ensure the minimum distance constraint
    for (int i = 1; i < n; i+=1)
    {
        if (points[i] - points[i - 1] < MIN_DIST)
        {
            points[i] = points[i - 1] + MIN_DIST;
        }
    }
    // Assign the points to the nodes
    for (int i = 0; i < n; i+=1)
    {
        nodes[i].ThePosition = points[i];
    }
}
int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    int DurationOfTheSimulation;
    std::cout << "Enter the duration of the simulation: ";
    cin >> DurationOfTheSimulation;
    int numNodes;
    std::cout << "Enter the number of nodes for the simulation: ";
    cin >> numNodes;
    // Create nodes
    vector<Node> nodes;
    for (int i = 0; i < numNodes; i++)
    {
        nodes.emplace_back(i);
    }
    // Generate random points for the nodes
    RandomPointsGenerator(numNodes, 2 * numNodes + 1, 2, nodes);
    // Print the initial positions of the nodes
    std::cout << "\nInitial positions of the nodes:\n";
    for (Node &node : nodes)
    {
        std::cout << "Node " << node.getTheUniqueIdentifier() << ": " << node.ThePosition << std::endl;
    }
    // Set initial packets
    for (auto &node : nodes)
    {
        int ArrivalTimeOfPacket = rand() % 5;
        node.state = StateOfTheNode::Ready;
        int PacketSize = (rand() % (maximumPacketSize / packetTransmissionTime)) * packetTransmissionTime + packetTransmissionTime;
        std::cout << "First Frame Arrival Time of Node " << node.getTheUniqueIdentifier() << ": " << ArrivalTimeOfPacket << std::endl;
        Frame frame = Frame(ArrivalTimeOfPacket, ArrivalTimeOfPacket + (PacketSize / packetTransmissionTime), 1, PacketSize);
        node.QueueWhichHoldsReadyFrames.push_back(frame);
    }
    // Simulate the network
    for (int PresentTime = 0; PresentTime < DurationOfTheSimulation; PresentTime += 1)
    {
        std::cout << "\nAt Time " << PresentTime <<","<< std::endl;
        for (Node &node : nodes)
        {
            if (node.QueueWhichHoldsTheTransmittingFrames.size() && node.QueueWhichHoldsTheTransmittingFrames[0].end == PresentTime)
            {
                int ArrivalTimeOfPacket = rand() % maximumPacketArrivalTime + 1;
                node.NumberOfSuccessfulTransmissions++;
                std::cout << "Node " << node.UniqueIdentifier << ": Transmission complete. Next packet will arrive in " << ArrivalTimeOfPacket << " time units." << std::endl;
                node.state = StateOfTheNode::Ready;
                int PacketSize = (rand() % (maximumPacketSize / packetTransmissionTime)) * packetTransmissionTime + packetTransmissionTime;
                Frame frame = Frame(PresentTime + ArrivalTimeOfPacket, PresentTime + ArrivalTimeOfPacket + (PacketSize / packetTransmissionTime), 1, PacketSize);
                node.QueueWhichHoldsTheTransmittingFrames.erase(node.QueueWhichHoldsTheTransmittingFrames.begin());
                node.QueueWhichHoldsReadyFrames.push_back(frame);
            }
        }
        vector<StateOfTheNode> NextStates(numNodes);
        for (Node &node : nodes)
        {
            NextStates[node.UniqueIdentifier] = node.AttemptingToTransmitFrame(PresentTime, nodes);
        }
        for (Node &node : nodes)
        {
            node.state = NextStates[node.UniqueIdentifier];
        }
    }
    // Print statistics for each node
    std::cout << "\nStatistics:\n";
    for (const Node &node : nodes)
    {
        std::cout << "\nNode Number " << node.getTheUniqueIdentifier() << ":\nNumber of Successful Transmissions: " << node.getTheTotalNumberOfSuccessfulTransmissions()
                  << "\nNumber of Collisions: " << node.getTheTotalNumberOfCollisions() << "\nBackoff Count: " << node.getBackoffCount() << std::endl;
    }
    return 0;
}