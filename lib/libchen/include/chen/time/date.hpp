/**
 * Created by Jian Chen
 * @since  2015.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

namespace chen
{
    class date
    {
    public:
        explicit date(bool utc = false);

    public:
        int microsecond = 0;  // range from 0 ~ 999999
        int second      = 0;  // range from 0 ~ 59
        int minute      = 0;  // range from 0 ~ 59
        int hour        = 0;  // range from 0 ~ 23
        int day         = 0;  // range from 1 ~ 31
        int wday        = 0;  // 0: Sunday, 1: Monday ~ 6: Saturday
        int month       = 0;  // 1: January ~ 12: December
        int year        = 0;  // the true year value, e.g: 2017
        long zone       = 0;  // timezone offset in seconds
    };
}