/**
 * Created by Jian Chen
 * @since  2016.01.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <string>

namespace chen
{
    class path
    {
    public:
        /**
         * Separator on this platform
         * @result "/" on Unix or Linux, "\" on Windows
         */
        static std::string separator();

        /**
         * todo
         * Normalize path, remove ".", ".." and redundant separators
         * e.g: a/./b to a/b, a///b to a/b
         */
        static std::string normalize(const std::string &path);

        /**
         * todo
         * Absolute path, append to current working folder, without ".", ".." and separators
         * e.g: cwd is /home/staff/Downloads, then path "file.txt" will be appended to cwd
         */
        static std::string absolute(const std::string &path);

        /**
         * Directory name of the path, without the trailing slash
         * e.g: /home/staff/Downloads/file.txt, dirname is "/home/staff/Downloads"
         * e.g: /usr/., dirname is "/usr", because "." is represent current folder
         * e.g: /usr/, dirname is "/", not "/usr", because single "/" isn't a effective name
         * e.g: /usr///, dirname is "/", because the trailing slash will be ignored
         * e.g: /, dirname is "/", because it's the root folder
         * e.g: file.txt, dirname is ".", because it doesn't has a absolute dir
         */
        static std::string dirname(const std::string &path);

        /**
         * Base name of the path
         * e.g: /home/staff/Downloads/file.txt, basename is "file.txt"
         */
        static std::string basename(const std::string &path);

        /**
         * Extension name of the path
         * e.g: /home/staff/Downloads/file.txt, extname is ".txt"
         * @param path full path
         * @param dots max dots count of the extension
         */
        static std::string extname(const std::string &path, std::size_t dots = 1);
    };
}