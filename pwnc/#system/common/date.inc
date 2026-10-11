<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function weekday($number,
                 $abbr = TRUE)
{
    $number %= 7;

    switch ($number)
    {
    case 0: return $abbr ? t("sunday_abbr")    : t("sunday");
    case 1: return $abbr ? t("monday_abbr")    : t("monday");
    case 2: return $abbr ? t("tuesday_abbr")   : t("tuesday");
    case 3: return $abbr ? t("wednesday_abbr") : t("wednesday");
    case 4: return $abbr ? t("thursday_abbr")  : t("thursday");
    case 5: return $abbr ? t("friday_abbr")    : t("friday");
    case 6: return $abbr ? t("saturday_abbr")  : t("saturday");
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function month($number,
               $abbr = TRUE)
{
    switch ($number)
    {
    case  1: return $abbr ? t("january_abbr")   : t("january");
    case  2: return $abbr ? t("february_abbr")  : t("february");
    case  3: return $abbr ? t("march_abbr")     : t("march");
    case  4: return $abbr ? t("april_abbr")     : t("april");
    case  5: return $abbr ? t("may_abbr")       : t("may");
    case  6: return $abbr ? t("june_abbr")      : t("june");
    case  7: return $abbr ? t("july_abbr")      : t("july");
    case  8: return $abbr ? t("august_abbr")    : t("august");
    case  9: return $abbr ? t("september_abbr") : t("september");
    case 10: return $abbr ? t("october_abbr")   : t("october");
    case 11: return $abbr ? t("november_abbr")  : t("november");
    case 12: return $abbr ? t("december_abbr")  : t("december");
    };

    return FALSE;
};

function local_day_index($timestamp)
{
    return (int)(($timestamp + (int)date("Z", $timestamp)) / 86400);
}

//--------------------------------------------------------------------------------------------------

function friendly_date($time      = NULL,
                       $date_only = FALSE)
{
    $_time  = time();
    $time   = ($time === NULL) ? $_time : (int)$time;
    $date   = getdate($time);
    $now    = getdate($_time);
    $future = $time > $_time;

    //interval in seconds
    $seconds = abs($_time - $time);

    //right now
    if ($seconds === 0) return t("now");

    //within the minute
    if ((int)($seconds / 60) === 0)
        return sprintf($future ? t("seconds_ahead") : t("seconds_ago"),
                       $seconds);

    //interval in hours
    $hours = (int)($seconds / 3600);

    //within the hour
    if ($hours === 0)
        return sprintf($future ? t("minutes_ahead") : t("minutes_ago"),
                       (int)($seconds / 60));

    //within 6 hours
    if ($hours <= 6)
    {
        //interval in minutes
        $minutes = (int)($seconds % 3600 / 60);

        //first minute of hour
        if ($minutes === 0)
            return sprintf($future ? t("hours_ahead") : t("hours_ago"),
                           $hours);

        //ony other minute
        return sprintf($future ? t("hours_minutes_ahead") : t("hours_minutes_ago"),
                       $hours,
                       $minutes);
    };

    //interval in days
    $day  = (int)(($time  + (int)date("Z", $time )) / 86400);
    $_day = (int)(($_time + (int)date("Z", $_time)) / 86400);
    $days = abs($_day - $day);

    //today
    if ($days === 0)
        return $date_only ?
               t("today") :
               date(t("short_time_format"), $time);

    //yesterday / tomorrow
    if ($days === 1)
        return $date_only ?
                      ($future ? t("tomorrow") : t("yesterday")) :
               sprintf($future ? t("tomorrow_time") : t("yesterday_time"),
                       date(t("short_time_format"), $time));

    //this week
    if ($days < 7)
        return $date_only ?
               sprintf(t("weekday_only"),
                       weekday($date["wday"])) :
               sprintf(t("weekday_time"),
                       weekday($date["wday"]),
                       date(t("short_time_format"), $time));

    //this year
    if ($date["year"] === $now["year"])
    {
        //this month
        if ($date["mon"] === $now["mon"])
            return $date_only ?
                   sprintf($future ? t("weekday_days_ahead") : t("weekday_days_ago"),
                           weekday($date["wday"]),
                           $days) :
                   sprintf($future ? t("weekday_days_ahead_time") : t("weekday_days_ago_time"),
                           weekday($date["wday"]),
                           $days,
                           date(t("short_time_format"), $time));

        //any other month
        return $date_only ?
               sprintf(t("day_month"),
                       $date["mday"],
                       month($date["mon"])) :
               sprintf(t("day_month_time"),
                       $date["mday"],
                       month($date["mon"]),
                       date(t("short_time_format"), $time));
    };

    //any other year
    return $date_only ?
           sprintf(t("weekday_only"),
                   date(t("date_format"), $time)) :
           sprintf(t("weekday_time"),
                   date(t("date_format"), $time),
                   date(t("short_time_format"), $time));
};

//--------------------------------------------------------------------------------------------------

function short_datetime($time = NULL)
{
    return date(t("short_datetime_format"), $time ?? time());
};