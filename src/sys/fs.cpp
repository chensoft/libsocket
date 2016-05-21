/**
 * Created by Jian Chen
 * @since  2016.01.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/fs.hpp>
#include <chen/base/str.hpp>
#include <fstream>
#include <climits>

using namespace chen;

// -----------------------------------------------------------------------------
// path
std::string fs::realpath(const std::string &path)
{
    char buf[PATH_MAX] = {0};
    ::realpath(path.c_str(), buf);

    std::string ret(buf);
    return fs::isExist(ret) ? ret : "";
}

std::string fs::absolute(const std::string &path)
{
    if (fs::isRelative(path))
        return fs::current() + fs::separator() + fs::normalize(path);
    else
        return fs::normalize(path);
}

std::string fs::normalize(const std::string &path)
{
    if (path.empty())
        return "";

    auto sep = fs::separator();
    auto abs = (path[0] == sep);

    std::size_t ptr = 0;  // current segment cursor
    std::size_t len = 0;  // current segment length

    std::vector<std::pair<std::size_t, std::size_t>> store;  // segments cache

    for (std::size_t i = 0, l = path.size(); (i < l) || len; ++i)
    {
        if ((i == l) || (path[i] == sep))
        {
            if (len > 0)
            {
                // store a segment
                if (str::equal(&path[ptr], len, "..", 2))
                {
                    if (!store.empty() && !str::equal(&path[store.back().first], store.back().second, "..", 2))
                        store.pop_back();
                    else
                        store.push_back(std::make_pair(ptr, len));
                }
                else if (!str::equal(&path[ptr], len, ".", 1))
                {
                    store.push_back(std::make_pair(ptr, len));
                }

                ptr = len = 0;
            }
        }
        else
        {
            if (!len)
            {
                ptr = i;
                len = 1;
            }
            else
            {
                ++len;
            }
        }
    }

    // concat
    std::string ret(abs ? "/" : "");

    for (std::size_t i = 0, l = store.size(); i < l; ++i)
    {
        auto &pair = store[i];
        ret.append(&path[pair.first], pair.second);

        if (i < l - 1)
            ret.append(1, sep);
    }

    return ret;
}

std::string fs::dirname(const std::string &path)
{
    if (path.empty())
        return "";

    auto sep = fs::separator();
    auto beg = path.rbegin();
    auto end = path.rend();
    auto idx = beg;

    auto flag = false;

    for (; idx != end; ++idx)
    {
        if (*idx == sep)
        {
            auto next = idx + 1;

            if (flag && (*next != sep))
            {
                ++idx;
                break;
            }
        }
        else if (!flag)
        {
            flag = true;
        }
    }

    if (flag)
    {
        if (idx != end)
        {
            return path.substr(0, static_cast<std::size_t>(end - idx));
        }
        else
        {
            auto first = path[0];
            return (first == sep) ? "/" : ".";  // using root directory or current directory
        }
    }
    else
    {
        return "/";  // all chars are '/'
    }
}

std::string fs::basename(const std::string &path)
{
    auto sep = fs::separator();
    auto beg = path.rbegin();
    auto end = path.rend();

    auto flag_a = end;  // before
    auto flag_b = end;  // after

    for (auto idx = beg; idx != end; ++idx)
    {
        if (*idx == sep)
        {
            if (flag_b != end)
            {
                flag_a = idx;
                break;
            }
        }
        else if (flag_b == end)
        {
            flag_b = idx;
        }
    }

    if (flag_b != end)
        return path.substr(static_cast<std::size_t>(end - flag_a), static_cast<std::size_t>(flag_a - flag_b));
    else
        return "";
}

std::string fs::extname(const std::string &path, std::size_t dots)
{
    if (!dots)
        return "";

    auto sep = fs::separator();
    auto beg = path.rbegin();
    auto end = path.rend();
    auto idx = beg;
    auto cur = beg;
    auto num = 0;

    for (; idx != end; ++idx)
    {
        if (*idx == sep)
        {
            break;
        }
        else if (*idx == '.')
        {
            cur = idx + 1;

            if (++num == dots)
                break;
        }
    }

    return num ? path.substr(static_cast<std::size_t>(end - cur), static_cast<std::size_t>(cur - beg)) : "";
}

// exist
bool fs::isAbsolute(const std::string &path)
{
    auto sep = fs::separator();
    return !path.empty() && (path[0] == sep);
}

bool fs::isRelative(const std::string &path)
{
    return !fs::isAbsolute(path);
}

// copy
bool fs::copy(const std::string &path_old, const std::string &path_new)
{
    if (fs::isFile(path_old))
    {
        auto folder = fs::dirname(path_new);
        if (folder.empty())
            return false;

        if (!fs::create(folder))
            return false;

        std::ifstream in(path_old, std::ios_base::binary);
        if (!in)
            return false;

        std::ofstream out(path_new, std::ios_base::binary);
        if (!out)
            return false;

        out << in.rdbuf();
        out.close();

        return true;
    }
    else if (fs::isDir(path_old, true))
    {
        if (!fs::create(path_new))
            return false;

        auto size = path_old.size();
        auto ret  = true;

        fs::visit(path_old, [size, &ret, path_new] (const std::string &name) -> bool {
            auto sub = name.substr(size, name.size() - size);

            if (fs::isFile(name))
                ret = fs::copy(name, path_new + sub);
            else
                ret = fs::create(path_new + sub, 0, false);

            // exit if occur an error
            return ret;
        });

        return ret;
    }
    else
    {
        return false;
    }
}

// visit
std::vector<std::string> fs::collect(const std::string &directory, bool recursive)
{
    std::vector<std::string> store;

    fs::visit(directory, [&store] (const std::string &path) -> bool {
        store.push_back(path);
        return true;
    }, recursive);

    return store;
}

// count
std::size_t fs::count(const std::string &directory,
                        bool recursive,
                        bool contain_file,
                        bool contain_dir)
{
    if (!contain_file && !contain_dir)
        return 0;

    std::size_t num = 0;

    fs::visit(directory, [&] (const std::string &path) -> bool {
        if ((contain_file && contain_dir) ||
            (contain_file && fs::isFile(path)) ||
            (contain_dir && fs::isDir(path)))
        {
            ++num;
        }

        return true;
    }, recursive);

    return num;
}