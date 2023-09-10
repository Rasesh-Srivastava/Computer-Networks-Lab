#include <iostream>

#include <string>

#include <map>

#include <cstring>

#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

 

using namespace std;

 

const int CACHE_SIZE = 5;

 

 

 

template <typename T>

class LinkedList{

private:

    struct Node{

        T data;

        Node* next;

 

        Node(const T& value) : data(value), next(NULL) {}

    };

    Node* _head;

    int _size;

 

public:

    LinkedList() : _head(NULL), _size(0) {}

 

    ~LinkedList() {

        while (_head) {

            Node* temp = _head;

            _head = _head->next;

            delete temp;

        }

    }

 

    void PushFront(const T& value) {

        Node* new_node = new Node(value);

        new_node->next = _head;

        _head = new_node;

        _size++;

    }

 

    bool Remove(const T& value) {

        Node* current = _head;

        Node* prev = NULL;

 

        while (current) {

            if (current->data == value) {

                if (prev) {

                    prev->next = current->next;

                } else {

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

 

    bool Empty() const {

        return _size == 0;

    }

 

    int Size() const {

        return _size;

    }

 

    void Print() const {

        Node* current = _head;

        while (current) {

            cout << current->data << " ";

            current = current->next;

        }

        cout << endl;

    }

 

    T findLRU()

    {

        Node * temp = _head;

        while(temp->next!= NULL)

        {

            temp=temp->next;

        }

        return temp->data;

    }

 

};

 

struct WebPage {

    string url;

    string content;

};

 

class WebCache {

private:

    int _cacheSize;

    map<string, WebPage> _mpp;

    LinkedList<string> _cache;

 

    void MoveToFront(const string& url) {

        _cache.Remove(url);

        _cache.PushFront(url);

    }

 

    void AddToCache(const string& url, const string& content) {

        WebPage page;

        page.url = url;

        page.content = content;

        _mpp[url] = page;

        _cache.PushFront(url);

    }

 

    string FetchWebPage(const string& url) {

        // Perform HTTP GET request using sockets

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) {

            perror("Error opening socket");

            exit(1);

        }

 

        struct hostent* server = gethostbyname(url.c_str());

        if (server == NULL) {

            cerr << "Error resolving host" << endl;

            exit(1);

        }

 

        struct sockaddr_in server_addr;

        memset((char*)&server_addr, 0, sizeof(server_addr));

        server_addr.sin_family = AF_INET;

        memcpy((char*)&server_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

        server_addr.sin_port = htons(80);

 

        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {

            perror("Error connecting to server");

            exit(1);

        }

 

        string request = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";

        if (write(sockfd, request.c_str(), request.length()) < 0) {

            perror("Error sending request");

            exit(1);

        }

 

        string response;

        char buffer[1024];

        ssize_t bytes_received;

        while ((bytes_received = read(sockfd, buffer, sizeof(buffer))) > 0) {

            response.append(buffer, bytes_received);

        }

 

        close(sockfd);

 

        return response;

    }

 

public:

    WebCache() : _cacheSize(CACHE_SIZE) {}

 

    string GetWebPage(const string& url) {

        if (_mpp.count(url)) {

            // Move the accessed page to the front (most recently used)

            MoveToFront(url);

            return _mpp[url].content;

        } else {

            // Fetch the page using HTTP GET request

            string content = FetchWebPage(url);

            if (_mpp.size() >=5) {

                // Evict the least recently used page

                // cout<<"Evict called"<<endl;

                string val= _cache.findLRU();

                // cout<<val<<endl;

                _cache.Remove(val);

                _mpp.erase(val);

            }

            // Add the new page to the cache

            AddToCache(url, content);

            // cout<<_mpp.size()<<endl;

            return content;

        }

    }

 

    void DisplayCache() {

        cout << "Cache Contents (Most Recently Used to Least Recently Used):" << endl;

        _cache.Print();

    }

};

 

int main() {

    WebCache cache;

    while(1)

    {

    string url;

    getline(cin,url);

 

    // string url1 = "quietsilverfreshmelody.neverssl.com";

    // string url2 = "example.com";

    // string url3 = "example.org";

    // string url4 = "example.net";

    // string url5 = "example.edu";

    // string url6 = "google.com";

 

    // Access URLs to populate the cache

    string content1 = cache.GetWebPage(url);

    // string content2 = cache.GetWebPage(url2);

    // string content3 = cache.GetWebPage(url3);

    // string content4 = cache.GetWebPage(url4);

    // string content5 = cache.GetWebPage(url5);

    // string content6 = cache.GetWebPage(url6);

 

    cout << "Content 1:\n" << content1 << endl;

    // cout << "Content 2:\n" << content2 << endl;

    // cout << "Content 3:\n" << content3 << endl;

    // cout << "Content 4:\n" << content4 << endl;

    // cout << "Content 5:\n" << content5 << endl;

    // cout << "Content 6:\n" << content6 << endl;

 

    // Access an existing URL to demonstrate cache behavior

    // string content7 = cache.GetWebPage(url2);

    // cout << "Content 7:\n" << content7 << endl;

 

    // Display the cache after accessing URLs

    cache.DisplayCache();

    }

    return 0;

}