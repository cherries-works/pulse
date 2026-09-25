#include <stdlib.h>

const char *PROC_DIR = "/proc";
const char *PROC_UPTIME_FILE = "/proc/uptime";
const char *PROC_MEM_FILE = "/proc/meminfo";
const char *PROC_CPU_FILE = "/proc/cpuinfo";
const char *PROC_LOAD_FILE = "/proc/loadavg";
const char *PROC_STAT_FILE = "/proc/stat";
const char *PROC_DISK_FILE = "/proc/diskstats";
const char *PROC_NET_FILE = "/proc/net/dev";

const char *ETC_OS_RELEASE = "/etc/os-release";

const char *SYS_THERMAL_ZONE = "/sys/class/thermal/thermal_zone";

const char SPACE_IN_ASCII = ' ';
const unsigned ASCII_INT_TO_STRING_NUMNER = 48;
const size_t BUFFER_ONE_KB = 1024;
const size_t BUFFER_ONE_MB = BUFFER_ONE_KB * BUFFER_ONE_KB;

const char* CHERRIES_FOLDER = "~/.cherries-works";
const char* CHERRIES_FOLDER_PULSE = "~/.cherries-works/pulse";
const char* R_CHERRIES_FOLDER = ".cherries-works";
const char* R_CHERRIES_FOLDER_PULSE = ".cherries-works/pulse";

const char* CHERRIES_PULSE_SHM = "/cherries_pulse";
const char* CHERRIES_PULSE_READY_SEM = "/cherries_pulse_ready";

const char *CHERRIES_DEFAULT_TOML =
    "[alerts]\n"
    "enabled = true\n"
    "\n"
    "[alerts.cpu]\n"
    "threshold = 99\n"
    "operator = \"<\"\n"
    "duration = \"1s\"\n"
    "cooldown = \"2s\"\n"
    "\n"
    "[alerts.ram]\n"
    "threshold = 10\n"
    "operator = \"<\"\n"
    "duration = \"5s\"\n"
    "cooldown = \"10s\"\n"
    "\n"
    "[alerts.disk]\n"
    "threshold = 90\n"
    "operator = \"<\"\n"
    "duration = \"15s\"\n"
    "cooldown = \"30s\"\n"
    "\n"
    "[notifications]\n"
    "\n"
    "[notifications.desktop]\n"
    "enabled = true\n"
    "title = \"Pulse Alert\"\n"
    "message = \"{resource} is above {value}%\"\n"
    "\n"
    "[notifications.discord]\n"
    "enabled = false\n"
    "webhook = \"\"\n"
    "message = \"**Pulse:** {resource} reached {value}%\"\n"
    "\n"
    "[notifications.command]\n"
    "enabled = false\n"
    "command = \"\"\n";