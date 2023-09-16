#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <map>
#include <cstring>
 
using namespace std;
const int CACHE_SIZE = 5; // Maximum cache size is 5
// Linked List to store the LRU Cache
template <typename T>
class MyLinkedList
{
private:
 // Structure of a Linked List Node to store Web urls
 struct Node
 {
 T data;
 Node *next;
 Node(const T &value) : data(value), next(NULL) {}
 };
 Node *_head;
 int _size;
 
public:
 // Constructor
 MyLinkedList() : _head(NULL), _size(0) {}
 
 // Destructor
 ~MyLinkedList()
 {
 while (_head)
 {
 Node *temp = _head;
 _head = _head->next;
 delete temp;
 }
 }
 // Function to insert at the front of the Linked List
 void PushFront(const T &value)
 {
 Node *new_node = new Node(value);
 new_node->next = _head;
 _head = new_node;
 _size++;
 }
 // Function to remove a node with the given url value
 bool Remove(const T &value)
 {
 Node *current = _head;
 Node *prev = NULL;
 while (current)
 {
 if (current->data == value)
 {
 if (prev)
 {
 prev->next = current->next;
 }
 else
 {
 _head = current->next;
 }
 delete current;
 _size--;
 return true;
 }
 prev = current;
 current = current->next;
 }
 return false;
 }
 // Function to check if Linked List is empty
 bool Empty() const
 {
 return _size == 0;
 }
 // Function to return Size of Linked List
 int Size() const
 {
 return _size;
 }
 // Function to print the Linked List
 void Print() const
 {
 Node *current = _head;
 int i = 1;
 while (current)
 {
 cout << i++ << ".";
 cout << current->data;
 cout << endl;
 current = current->next;
 }
 cout << endl;
 }
 // Function to find the Least Recently Used Url
 T findLRU()
 {
 Node *temp = _head;
 while (temp->next != NULL)
 {
 temp = temp->next;
 }
 return temp->data;
 }
};
 
// Structure of a WebPage
struct WebPage
{
 string url;
 string content;
};
 
// Class to implement LRU cache using a Map and Linked List
class WebCache
{
private:
 int _cacheSize;
 map<string, WebPage> _mpp;
 MyLinkedList<string> _cache;
 
 // Function to move a URL to the Front(Most Recently used) of Linked List
 void MoveToFront(const string &url)
 {
 _cache.Remove(url);
 _cache.PushFront(url);
 }
 
 // Function to add the URL to Web Cache
 void AddToCache(const string &url, const string &content)
 {
 WebPage page;
 page.url = url;
 page.content = content;
 _mpp[url] = page;
 _cache.PushFront(url);
 }
 
 // Function to fetch the web page using HTTP GET request
 string FetchWebPage(const string &url)
 {
 // Perform HTTP GET request using sockets
 
 // Creating a socket
 int sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP connection
 if (sockfd < 0)
 {
 perror("Error opening socket");
 exit(1);
 }
 // Socket Opened successfully
 struct hostent *server = gethostbyname(url.c_str());
 if (server == NULL)
 {
 cerr << "Error resolving host" << endl;
 exit(1);
 }
 // Host Resolved
 struct sockaddr_in ServerAddress; // Server Address
 memset((char *)&ServerAddress, 0, sizeof(ServerAddress));
 ServerAddress.sin_family = AF_INET; // For IPv4 addresses
 memcpy((char *)&ServerAddress.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
 ServerAddress.sin_port = htons(80); // Server Port
 if (connect(sockfd, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) < 0)
 {
 perror("Error connecting to server");
 exit(1);
 }
 // Server connected successfully.
 string request = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
 if (write(sockfd, request.c_str(), request.length()) < 0)
 {
 perror("Error sending request");
 exit(1);
 }
 // Request sent successfully.
 string MyRes;
 char buffer[1024];
 ssize_t MyBuff;
 while ((MyBuff = read(sockfd, buffer, sizeof(buffer))) > 0)
 {
 MyRes.append(buffer, MyBuff);
 }
 close(sockfd); // Closing the Socket
 return MyRes;
 }
 
public:
 WebCache() : _cacheSize(CACHE_SIZE) {}
 
 // To fetch the webpage based on the given URL
 string GetWebPage(const string &url)
 {
 if (_mpp.count(url))
 {
 // Move the accessed page to the front (most recently used)
 MoveToFront(url);
 return _mpp[url].content; // Return the Web page content.
 }
 else
 {
 // Fetch the page using HTTP GET request
 string content = FetchWebPage(url);
 if (_mpp.size() >= 5)
 {
 // Evict the least recently used page
 string val = _cache.findLRU();
 _cache.Remove(val);
 _mpp.erase(val);
 }
 // Add the new page to the cache
 AddToCache(url, content);
 return content; // Return the Web page content.
 }
 }
 // Function to display the Current cache
 void DisplayCache()
 {
 cout << "Cache Contents (Most Recently Used to Least Recently Used):" << endl;
 _cache.Print();
 }
};
int main()
{
 WebCache cache;
 printf("************************************\n");
 printf("* Welcome to the HTTP Web Cache *\n");
 printf("* Input Interface! *\n");
 printf("************************************\n");
 printf("Cache Size : 5\n");
 while (1)
 {
 string url;
 cout << "Enter URL (or 'exit' to quit): ";
 getline(cin, url);
 if (url == "exit")
 {
 printf("************************************\n");
 printf("* Session End *\n");
 printf("************************************\n");
 break;
 }
 // Access URLs to populate the cache
 string content = cache.GetWebPage(url);
 cout << "Page Content :\n"
 << content << endl;
 
 // Display the cache after accessing URLs
 cache.DisplayCache();
 }
 return 0;
}