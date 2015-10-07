/**
 * Created by Jian Chen
 * @since  2015.10.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "net/dns/dns_tool.h"
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <mutex>
#include <cmath>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// tool

// nameservers
std::vector<chen::ip::address_v4> tool::nameservers()
{
    static std::vector<chen::ip::address_v4> list;
    static std::mutex mutex;
    static time_t time = 0;

    struct stat st;
    if (::stat("/etc/resolv.conf", &st))
        return list;

    std::lock_guard<std::mutex> lock(mutex);

    if (time == st.st_mtime)
        return list;

    // parse /etc/resolv.conf
    // like "nameserver 172.24.4.16"
    std::ifstream file("/etc/resolv.conf");
    std::string line;

    if (!file)
        return list;

    // reset
    list.clear();
    time = st.st_mtime;

    // parse
    std::string prefix;
    unsigned a = 0, b = 0, c = 0, d = 0;
    char dot_a, dot_b, dot_c;

    while (std::getline(file, line))
    {
        if (line.empty() || (line[0] != 'n'))
            continue;

        std::istringstream ss(line);

        if (!(ss >> prefix) || (prefix != "nameserver"))
            continue;

        if (!(ss >> a >> dot_a >> b >> dot_b >> c >> dot_c >> d))
            continue;

        if ((a > 255) || (b > 255) || (c > 255) || (d > 255))
            continue;

        if ((dot_a != '.') || (dot_b != '.') || (dot_c != '.'))
            continue;

        list.push_back(chen::ip::address_v4(a * 256 * 256 * 256 + b * 256 * 256 + c * 256 + d));
    }

    return list;
}

#endif  // CHEN_OS_UNIX