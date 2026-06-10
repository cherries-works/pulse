struct Cpu {
    unsigned long idle;
    unsigned long total;
    unsigned long processes;
};

struct Memory {
    unsigned long total;
    unsigned long available;
};

struct Disk {
    unsigned long long total;
    unsigned long long available;
    unsigned long long read;
    unsigned long long write;
};

struct IoAverage {
    float r;
    float w;
};

struct Load {
    float load1;
    float load5;
    float load15;
};

struct Network {
    unsigned long rx;
    unsigned long tx;
};

struct NetworkAverage {
    float rx;
    float tx;
};

struct Process {
    char name[64];
    int pid;
    
    unsigned long ram;
    unsigned long cpu;
};

struct System {
    struct Cpu cpu;
    struct Memory memory;
    struct Disk disk;
    
    struct Load load;
    struct Network network;

    struct Process processes[3];

    unsigned long uptime;
};

extern struct NetworkAverage parseNetworkUsage(struct Network snapshot2, struct Network snapshot1);
extern struct IoAverage parseIoUsage(struct Disk snapshot2, struct Disk snapshot1);
extern float parseCpuUsage(struct Cpu snapshot2, struct Cpu snapshot1);

extern struct System getSystem();
