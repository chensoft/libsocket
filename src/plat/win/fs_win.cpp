/**
 * Created by Jian Chen
 * @since  2016.06.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/fs.hpp>

using namespace chen;

// -----------------------------------------------------------------------------
// fs
std::string fs::root()
{
    return "";
}

std::string fs::home()
{
    return "";
}

std::string fs::temp()
{
    return "";
}

std::string fs::current()
{
    return "";
}

std::vector<std::string> fs::drives()
{
    return {};
}

char fs::separator()
{
    return '\\';
}

std::string fs::realpath(const std::string &path)
{
    return "";
}

// exist
bool fs::isExist(const std::string &path)
{
    return false;
}

bool fs::isDir(const std::string &path, bool strict)
{
    return false;
}

bool fs::isFile(const std::string &path, bool strict)
{
    return false;
}

bool fs::isLink(const std::string &path)
{
    return false;
}

bool fs::isReadable(const std::string &path)
{
    return false;
}

bool fs::isWritable(const std::string &path)
{
    return false;
}

bool fs::isExecutable(const std::string &path)
{
    return false;
}

// time
time_t fs::atime(const std::string &path)
{
    return 0;
}

time_t fs::mtime(const std::string &path)
{
    return 0;
}

time_t fs::ctime(const std::string &path)
{
    return 0;
}

// size
off_t fs::filesize(const std::string &file)
{
    return 0;
}

// create
bool fs::touch(const std::string &file, time_t mtime, time_t atime)
{
    return false;
}

bool fs::create(const std::string &dir, std::uint16_t mode, bool recursive)
{
    return false;
}

bool fs::rename(const std::string &path_old, const std::string &path_new)
{
    return false;
}

bool fs::remove(const std::string &path)
{
    return false;
}

// change
bool fs::change(const std::string &directory)
{
    return false;
}

// visit
void fs::visit(const std::string &directory,
               std::function<bool (const std::string &path)> callback,
               bool recursive)
{
}