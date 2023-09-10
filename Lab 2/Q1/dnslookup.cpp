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
unordered_map<string,vector<string>> mp;
// Types of DNS resource records
#define T_A 1  // Ipv4 address
#define T_NS 2  // Nameserver
#define T_CNAME 5  // Canonical name
#define DNS_SERVER "172.17.1.1"
#define DNS_PORT 53
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
    unsigned char RecursionDesired : 1; 
    unsigned char TruncatedMessage : 1; 
    unsigned char AuthorityAnswer : 1; 
    unsigned char purpose : 4;
    unsigned char QueryResponseFlag : 1; 

    unsigned char ResponseCode : 4; 
    unsigned char CheckingDisabled : 1; 
    unsigned char AuthenicatedData : 1;
    unsigned char z : 1;
    unsigned char RecursionAvailable : 1; 

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
        int i;
        int j;
        int stop;
        int s;
        struct sockaddr_in TheDestination;
        struct DNS_HEADER* DomainNameSystem = nullptr;
        struct QUESTION* QuestionInformation = nullptr;
        struct sockaddr_in a;
        struct RES_RECORD MyAuthoratativeRecords[20];
        struct RES_RECORD MyAdditionalRecords[20];
        struct RES_RECORD answers[20];  
        printf("Resolving ");
        cout << host;
        s = socket(AF_INET, SOCK_DGRAM, 0);
        TheDestination.sin_family = AF_INET;
        TheDestination.sin_port = htons(DNS_PORT);
        TheDestination.sin_addr.s_addr = inet_addr(DNS_SERVER);
        DomainNameSystem = (struct DNS_HEADER*)&buf;
        DomainNameSystem->id = (unsigned short)htons(getpid());
        DomainNameSystem->QueryResponseFlag = 0; 
        DomainNameSystem->purpose = 0; 
        DomainNameSystem->AuthorityAnswer = 0; 
        DomainNameSystem->TruncatedMessage = 0; 
        DomainNameSystem->RecursionDesired = 1; 
        DomainNameSystem->RecursionAvailable = 0;
        DomainNameSystem->z = 0;
        DomainNameSystem->AuthenicatedData = 0;
        DomainNameSystem->CheckingDisabled = 0;
        DomainNameSystem->ResponseCode = 0;
        DomainNameSystem->QuestionCount = htons(1);
        DomainNameSystem->AnswerCount = 0;
        DomainNameSystem->AuthorityCount = 0;
        DomainNameSystem->ResourceCount = 0;
        qname = (unsigned char*)&buf[sizeof(struct DNS_HEADER)];

        ConvertToDNS_NameFormat(qname, host);
        QuestionInformation = (struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];
        QuestionInformation->qtype = htons(query_type); // Type of the query (A, MX, CNAME, NS, etc.)
        QuestionInformation->qclass = htons(1); 
        printf("\nSending Packet...");
        if (sendto(s, (char*)buf, sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct QUESTION), 0, (struct sockaddr*)&TheDestination, sizeof(TheDestination)) < 0) {
            perror("Sending Packet failed :(");
        }
        printf("Done");

        // Receive the answer
        i = sizeof(TheDestination);
        printf("\nReceiving answer...");
        if (recvfrom(s, (char*)buf, 65536, 0, (struct sockaddr*)&TheDestination, (socklen_t*)&i) < 0) {
            perror("recvfrom failed");
        }
        printf("Done");

        DomainNameSystem = (struct DNS_HEADER*)buf;
        reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct QUESTION)];

        // cout << "\nThe response contains: ";
        // cout << "\n" << ntohs(DomainNameSystem->QuestionCount) << " Questions.";
        // cout << "\n" << ntohs(DomainNameSystem->AnswerCount) << " Answers.";
        // cout << "\n" << ntohs(DomainNameSystem->AuthorityCount) << " Authoritative Servers.";
        // cout << "\n" << ntohs(DomainNameSystem->ResourceCount) << " Additional records.\n\n";

        stop = 0;
        for (i = 0; i < ntohs(DomainNameSystem->AnswerCount); i++) {
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
        for (i = 0; i < ntohs(DomainNameSystem->AuthorityCount); i++) {
            MyAuthoratativeRecords[i].name = ReadTheDNS_Name(reader, buf, &stop);
            reader += stop;

            MyAuthoratativeRecords[i].resource = (struct R_DATA*)(reader);
            reader += sizeof(struct R_DATA);

            MyAuthoratativeRecords[i].rdata = ReadTheDNS_Name(reader, buf, &stop);
            reader += stop;
        }

        // Read Additional Resource Records
        for (i = 0; i < ntohs(DomainNameSystem->ResourceCount); i++) {
            MyAdditionalRecords[i].name = ReadTheDNS_Name(reader, buf, &stop);
            reader += stop;
            MyAdditionalRecords[i].resource = (struct R_DATA*)(reader);
            reader += sizeof(struct R_DATA);
            if (ntohs(MyAdditionalRecords[i].resource->type) == 1) {
                MyAdditionalRecords[i].rdata = (unsigned char*)malloc(ntohs(MyAdditionalRecords[i].resource->data_len));
                for (j = 0; j < ntohs(MyAdditionalRecords[i].resource->data_len); j++)
                    MyAdditionalRecords[i].rdata[j] = reader[j];

                MyAdditionalRecords[i].rdata[ntohs(MyAdditionalRecords[i].resource->data_len)] = '\0';
                reader += ntohs(MyAdditionalRecords[i].resource->data_len);
            } else {
                MyAdditionalRecords[i].rdata = ReadTheDNS_Name(reader, buf, &stop);
                reader += stop;
            }
        }
        // Print Answer Records
        cout << "\nAnswer Records: " << ntohs(DomainNameSystem->AnswerCount) << "\n";
        vector<string> ips;
        for (i = 0; i < ntohs(DomainNameSystem->AnswerCount); i++) {
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
        // cout << "\nAuthoritative Records: " << ntohs(DomainNameSystem->AuthorityCount) << "\n";
        // for (i = 0; i < ntohs(DomainNameSystem->AuthorityCount); i++) {

        //     cout << "Name: " << MyAuthoratativeRecords[i].name;
        //     if (ntohs(MyAuthoratativeRecords[i].resource->type) == 2) {
        //         cout << " has nameserver: " << MyAuthoratativeRecords[i].rdata;
        //     }
        //     cout << "\n";
        // }

        // // Print additional resource records
        // cout << "\nAdditional Records: " << ntohs(DomainNameSystem->ResourceCount) << "\n";
        // for (i = 0; i < ntohs(DomainNameSystem->ResourceCount); i++) {
        //     cout << "Name: " << MyAdditionalRecords[i].name;
        //     if (ntohs(MyAdditionalRecords[i].resource->type) == 1) {
        //         long* p;
        //         p = (long*)MyAdditionalRecords[i].rdata;
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
void ConvertToDNS_NameFormat(unsigned char* DomainNameSystem, unsigned char* host) {
    int lock = 0, i;
    strcat((char*)host, ".");

    for (i = 0; i < strlen((char*)host); i++) {
        if (host[i] == '.') {
            *DomainNameSystem++ = i - lock;
            for (; lock < i; lock++) {
                *DomainNameSystem++ = host[lock];
            }
            lock++; // or lock=i+1;
        }
    }
    *DomainNameSystem++ = '\0';
}
