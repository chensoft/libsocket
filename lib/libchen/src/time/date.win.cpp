/**
 * Created by Jian Chen
 * @since  2016.05.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include "chen/time/date.hpp"
#include <Windows.h>

// -----------------------------------------------------------------------------
// date
chen::date::date(bool utc)
{
	::SYSTEMTIME st{};
	utc ? ::GetSystemTime(&st) : ::GetLocalTime(&st);

	this->microsecond = st.wMilliseconds * 1000;
	this->second      = st.wSecond;
	this->minute      = st.wMinute;
	this->hour        = st.wHour;
	this->day         = st.wDay;
	this->wday        = st.wDayOfWeek;
	this->month       = st.wMonth;
	this->year        = st.wYear;

	::TIME_ZONE_INFORMATION zone;
	::GetTimeZoneInformation(&zone);

	this->zone = -zone.Bias * 60;
}

#endif