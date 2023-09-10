#include <cstdlib>
#include <sys/socket.h>
#include <bits/stdc++.h>
#include <cstring>
#include <arpa/inet.h>
#include <unordered_map>
#include <map>
#include<vector>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;
char dns_servers[10][100];
int dns_server_count = 0;
unordered_map<string,vector<string>> mp;
// Types of DNS resource records
#define T_A 1  // Ipv4 address
#define T_NS 2  // Nameserver
#define T_CNAME 5  // Canonical name
#define DNS_SERVER "172.17.1.1"
#define DNS_PORT 53

void GetTheHostByName(unsigned char*, int);
void ConvertToDNS_NameFormat(unsigned char*, unsigned char*);
unsigned char* ReadTheDNS_Name(unsigned char*, unsigned char*, int*);

struct QUESTION {
    unsigned short qtype;
    unsigned short qclass;
};

// DNS header structure
struct DNS_HEADER {
    unsigned short id;
    unsigned char rd : 1; 
    unsigned char tc : 1; 
    unsigned char aa : 1; 
    unsigned char opcode : 4;
    unsigned char qr : 1; 

    unsigned char rcode : 4; 
    unsigned char cd : 1; 
    unsigned char ad : 1;
    unsigned char z : 1;
    unsigned char ra : 1; 

    unsigned short QuestionCount; 
    unsigned short AnswerCount;
    unsigned short AuthorityCount; 
    unsigned short ResourceCount;
};

#pragma pack(push, 1)
struct R_DATA {
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD {
    unsigned char* name;
    struct R_DATA* resource;
    unsigned char* rdata;
};

typedef struct {
    unsigned char* name;
    struct QUESTION* ques;
} QUERY;

class LinkedList{
private:
    struct Node{
        string data;
        Node* next;
        Node(const string& value) : data(value), next(NULL) {}
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
    void PushFront(const string& value) {
        Node* new_node = new Node(value);
        new_node->next = _head;
        _head = new_node;
        _size++;
    }
    bool Remove(const string& value) {
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
    string findLRU()
    {
        Node * temp = _head;
        while(temp->next!= NULL)
        {
            temp=temp->next;
        }
        return temp->data;
    }
};
LinkedList _cache;
void GetTheHostByName(unsigned char* host, int query_type) {
    string temphost(reinterpret_cast<char*>(host));
    if(mp.find(temphost)!=mp.end())
    {
        _cache.Remove(temphost);
        _cache.PushFront(temphost);
        for(int i=0;i<mp[temphost].size();i++)
        {
            cout << "Name: "<<temphost<<" has IPv4 address: "<<mp[temphost][i]<<endl;
        }
        return;
    }
    else
    {
        if(_cache.Size()>=5)
        {
            string val = _cache.findLRU();
            _cache.Remove(val);
            mp.erase(val);
        }
        unsigned char buf[65536], *qname, *reader;
        int i, j, stop, s;
        struct sockaddr_in a;
        struct RES_RECORD answers[20], auth[20], addit[20];
        struct sockaddr_in dest;
        struct DNS_HEADER* dns = nullptr;
        struct QUESTION* qinfo = nullptr;
        printf("Resolving ");
        cout << host;
        s = socket(AF_INET, SOCK_DGRAM, 0);
        dest.sin_family = AF_INET;
        dest.sin_port = htons(DNS_PORT);
        dest.sin_addr.s_addr = inet_addr(DNS_SERVER);
        dns = (struct DNS_HEADER*)&buf;
        dns->id = (unsigned short)htons(getpid());
        dns->qr = 0; 
        dns->opcode = 0; 
        dns->aa = 0; 
        dns->tc = 0; 
        dns->rd = 1; 
        dns->ra = 0;
        dns->z = 0;
        dns->ad = 0;
        dns->cd = 0;
        dns->rcode = 0;
        dns->QuestionCount = htons(1);
        dns->AnswerCount = 0;
        dns->AuthorityCount = 0;
        dns->ResourceCount = 0;
        qname = (unsigned char*)&buf[sizeof(struct DNS_HEADER)];

        ConvertToDNS_NameFormat(qname, host);
        qinfo = (struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];
        qinfo->qtype = htons(query_type); // Type of the query (A, MX, CNAME, NS, etc.)
        qinfo->qclass = htons(1); 
        printf("\nSending Packet...");
        if (sendto(s, (char*)buf, sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct QUESTION), 0, (struct sockaddr*)&dest, sizeof(dest)) < 0) {
            perror("Sending Packet failed :(");
        }
        printf("Done");

        // Receive the answer
        i = sizeof(dest);
        printf("\nReceiving answer...");
        if (recvfrom(s, (char*)buf, 65536, 0, (struct sockaddr*)&dest, (socklen_t*)&i) < 0) {
            perror("recvfrom failed");
        }
        printf("Done");

        dns = (struct DNS_HEADER*)buf;
        reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct QUESTION)];

        // cout << "\nThe response contains: ";
        // cout << "\n" << ntohs(dns->QuestionCount) << " Questions.";
        // cout << "\n" << ntohs(dns->AnswerCount) << " Answers.";
        // cout << "\n" << ntohs(dns->AuthorityCount) << " Authoritative Servers.";
        // cout << "\n" << ntohs(dns->ResourceCount) << " Additional records.\n\n";

        stop = 0;
        for (i = 0; i < ntohs(dns->AnswerCount); i++) {
            answers[i].name = ReadTheDNS_Name(reader, buf, &stop);
            reader = reader + stop;

            answers[i].resource = (struct R_DATA*)(reader);
            reader = reader + sizeof(struct R_DATA);

            if (ntohs(answers[i].resource->type) == 1)
            {
                answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));

                for (j = 0; j < ntohs(answers[i].resource->data_len); j++) {
                    answers[i].rdata[j] = reader[j];
                }

                answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';

                reader = reader + ntohs(answers[i].resource->data_len);
            } else {
                answers[i].rdata = ReadTheDNS_Name(reader, buf, &stop);
                reader = reader + stop;
            }
        }

        // Read Authorities
        for (i = 0; i < ntohs(dns->AuthorityCount); i++) {
            auth[i].name = ReadTheDNS_Name(reader, buf, &stop);
            reader += stop;

            auth[i].resource = (struct R_DATA*)(reader);
            reader += sizeof(struct R_DATA);

            auth[i].rdata = ReadTheDNS_Name(reader, buf, &stop);
            reader += stop;
        }

        // Read Additional Resource Records
        for (i = 0; i < ntohs(dns->ResourceCount); i++) {
            addit[i].name = ReadTheDNS_Name(reader, buf, &stop);
            reader += stop;
            addit[i].resource = (struct R_DATA*)(reader);
            reader += sizeof(struct R_DATA);
            if (ntohs(addit[i].resource->type) == 1) {
                addit[i].rdata = (unsigned char*)malloc(ntohs(addit[i].resource->data_len));
                for (j = 0; j < ntohs(addit[i].resource->data_len); j++)
                    addit[i].rdata[j] = reader[j];

                addit[i].rdata[ntohs(addit[i].resource->data_len)] = '\0';
                reader += ntohs(addit[i].resource->data_len);
            } else {
                addit[i].rdata = ReadTheDNS_Name(reader, buf, &stop);
                reader += stop;
            }
        }
        // Print Answer Records
        cout << "\nAnswer Records: " << ntohs(dns->AnswerCount) << "\n";
        vector<string> ips;
        for (i = 0; i < ntohs(dns->AnswerCount); i++) {
            cout << "Name: " << answers[i].name;

            if (ntohs(answers[i].resource->type) == T_A)
            {
                long* p;
                p = (long*)answers[i].rdata;
                a.sin_addr.s_addr = (*p);
                cout << " has IPv4 address: " << inet_ntoa(a.sin_addr);
                string s2 = inet_ntoa(a.sin_addr);
                ips.push_back(s2);
            }

            if (ntohs(answers[i].resource->type) == 5) {
                // Canonical name for an alias
                cout << " has alias name: " << answers[i].rdata;
            }
            cout << "\n";
        }
        mp.insert({temphost,ips});
        _cache.PushFront(temphost);
        // // Print authorities
        // cout << "\nAuthoritative Records: " << ntohs(dns->AuthorityCount) << "\n";
        // for (i = 0; i < ntohs(dns->AuthorityCount); i++) {

        //     cout << "Name: " << auth[i].name;
        //     if (ntohs(auth[i].resource->type) == 2) {
        //         cout << " has nameserver: " << auth[i].rdata;
        //     }
        //     cout << "\n";
        // }

        // // Print additional resource records
        // cout << "\nAdditional Records: " << ntohs(dns->ResourceCount) << "\n";
        // for (i = 0; i < ntohs(dns->ResourceCount); i++) {
        //     cout << "Name: " << addit[i].name;
        //     if (ntohs(addit[i].resource->type) == 1) {
        //         long* p;
        //         p = (long*)addit[i].rdata;
        //         a.sin_addr.s_addr = (*p);
        //         cout << " has IPv4 address: " << inet_ntoa(a.sin_addr);
        //     }
        //     cout << "\n";
        // }
        return;
    }
}
int main(int argc, char* argv[]) {
    unsigned char hostname[100];
    // Get the hostname from the terminal
    char ch='Y';
    printf("************************************\n");
    printf("*     Welcome to the DNS Lookup    *\n");
    printf("*       Input Interface!           *\n");
    printf("************************************\n");
    printf("\n");
    while(ch=='Y' || ch=='y')
    {
        printf("Cache Contents (Most Recently Used to Least Recently Used):\n");
        if(_cache.Size()==0)
        {
            cout<<"Empty"<<endl;
        }
        else
        {
            _cache.Print();
        }
        printf("\n");
        printf("Enter Hostname to Lookup: ");
        cin >> hostname;
        // Now get the IP of this hostname, A record
        GetTheHostByName(hostname, T_A);
        printf("************************************\n");
        printf("* Would you like to continue the session ? *\n");
        printf("* Enter 'Y' to continue! and 'N' to exit   *\n");
        printf("************************************\n");
        char UserChoice; 
        cin >> UserChoice;
        ch=UserChoice;
    }
    printf("************************************\n");
    printf("*           Session End            *\n");
    printf("************************************\n");
    return 0;
}
/*
 * Read the DNS name format
 */
unsigned char* ReadTheDNS_Name(unsigned char* reader, unsigned char* buffer, int* count) {
    unsigned char* name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    name = (unsigned char*)malloc(256);

    name[0] = '\0';

    while (*reader != 0) {
        if (*reader >= 192) {
            offset = (*reader) * 256 + *(reader + 1) - 49152;
            reader = buffer + offset - 1;
            jumped = 1;
        } else {
            name[p++] = *reader;
        }

        reader = reader + 1;

        if (jumped == 0) {
            *count = *count + 1;
        }
    }

    name[p] = '\0';
    if (jumped == 1) {
        *count = *count + 1;
    }

    for (i = 0; i < (int)strlen((const char*)name); i++) {
        p = name[i];
        for (j = 0; j < (int)p; j++) {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0';
    return name;
}

/*
 * Convert hostname to DNS name format
 */
void ConvertToDNS_NameFormat(unsigned char* dns, unsigned char* host) {
    int lock = 0, i;
    strcat((char*)host, ".");

    for (i = 0; i < strlen((char*)host); i++) {
        if (host[i] == '.') {
            *dns++ = i - lock;
            for (; lock < i; lock++) {
                *dns++ = host[lock];
            }
            lock++; // or lock=i+1;
        }
    }
    *dns++ = '\0';
}
