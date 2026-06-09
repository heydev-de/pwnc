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
    case 0: return $abbr ? CMS_L_SUNDAY_ABBR    : CMS_L_SUNDAY;
    case 1: return $abbr ? CMS_L_MONDAY_ABBR    : CMS_L_MONDAY;
    case 2: return $abbr ? CMS_L_TUESDAY_ABBR   : CMS_L_TUESDAY;
    case 3: return $abbr ? CMS_L_WEDNESDAY_ABBR : CMS_L_WEDNESDAY;
    case 4: return $abbr ? CMS_L_THURSDAY_ABBR  : CMS_L_THURSDAY;
    case 5: return $abbr ? CMS_L_FRIDAY_ABBR    : CMS_L_FRIDAY;
    case 6: return $abbr ? CMS_L_SATURDAY_ABBR  : CMS_L_SATURDAY;
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function month($number,
               $abbr = TRUE)
{
    switch ($number)
    {
    case  1: return $abbr ? CMS_L_JANUARY_ABBR   : CMS_L_JANUARY;
    case  2: return $abbr ? CMS_L_FEBRUARY_ABBR  : CMS_L_FEBRUARY;
    case  3: return $abbr ? CMS_L_MARCH_ABBR     : CMS_L_MARCH;
    case  4: return $abbr ? CMS_L_APRIL_ABBR     : CMS_L_APRIL;
    case  5: return $abbr ? CMS_L_MAY_ABBR       : CMS_L_MAY;
    case  6: return $abbr ? CMS_L_JUNE_ABBR      : CMS_L_JUNE;
    case  7: return $abbr ? CMS_L_JULY_ABBR      : CMS_L_JULY;
    case  8: return $abbr ? CMS_L_AUGUST_ABBR    : CMS_L_AUGUST;
    case  9: return $abbr ? CMS_L_SEPTEMBER_ABBR : CMS_L_AUGUST;
    case 10: return $abbr ? CMS_L_OCTOBER_ABBR   : CMS_L_OCTOBER;
    case 11: return $abbr ? CMS_L_NOVEMBER_ABBR  : CMS_L_NOVEMBER;
    case 12: return $abbr ? CMS_L_DECEMBER_ABBR  : CMS_L_DECEMBER;
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function friendly_date($time      = NULL,
                       $date_only = FALSE)
{
    if ($time === NULL) $time = time();
    $date      = getdate($time);
    $_time     = time();
    $now       = getdate($_time);
    $future    = $time > $_time;

    //interval in seconds
    $seconds = $_time - $time;

    //right now
    if ($seconds === 0) return CMS_L_COMMON_027;

    //within the minute
    if ((int)($seconds / 60) === 0)
        return sprintf($future ? CMS_L_COMMON_012 : CMS_L_COMMON_002,
                       abs($seconds));

    //interval in hours
    $hours = (int)($seconds / 3600);

    //within the hour
    if (abs($hours) === 0)
        return sprintf($future ? CMS_L_COMMON_013 : CMS_L_COMMON_003,
                       abs((int)($seconds / 60)));

    //within 6 hours
    if (abs($hours) <= 6)
    {
        //interval in minutes
        $minutes = (int)($seconds % 3600 / 60);

        //first minute of hour
        if ($minutes === 0)
            return sprintf($future ? CMS_L_COMMON_019 : CMS_L_COMMON_018,
                           abs($hours));

        //ony other minute
        return sprintf($future ? CMS_L_COMMON_017 : CMS_L_COMMON_015,
                       abs($hours),
                       abs($minutes));
    };

    //interval in days
    $days = (int)($_time / 86400) - (int)($time / 86400);

    //today
    if ($days === 0)
        return $date_only ?
               CMS_L_COMMON_026 :
               date(CMS_L_SHORT_TIME_FORMAT, $time);

    //yesterday / tomorrow
    if (abs($days) === 1)
        return $date_only ?
                      ($future ? CMS_L_COMMON_021 : CMS_L_COMMON_020) :
               sprintf($future ? CMS_L_COMMON_014 : CMS_L_COMMON_004,
                       date(CMS_L_SHORT_TIME_FORMAT, $time));

    //week interval
    $weeks = (int)($_time / 604800) - (int)($time / 604800);

    //this week
    if ($weeks === 0)
        return $date_only ?
               sprintf(CMS_L_COMMON_022,
                       weekday($date["wday"])) :
               sprintf(CMS_L_COMMON_005,
                       weekday($date["wday"]),
                       date(CMS_L_SHORT_TIME_FORMAT, $time));

    //this year
    if ($date["year"] === $now["year"])
    {
        //this month
        if ($date["mon"] === $now["mon"])
            return $date_only ?
                   sprintf($future ? CMS_L_COMMON_023 : CMS_L_COMMON_024,
                           weekday($date["wday"]),
                           abs($days)) :
                   sprintf($future ? CMS_L_COMMON_016 : CMS_L_COMMON_006,
                           weekday($date["wday"]),
                           abs($days),
                           date(CMS_L_SHORT_TIME_FORMAT, $time));

        //any other month
        return $date_only ?
               sprintf(CMS_L_COMMON_025,
                       $date["mday"],
                       month($date["mon"])) :
               sprintf(CMS_L_COMMON_007,
                       $date["mday"],
                       month($date["mon"]),
                       date(CMS_L_SHORT_TIME_FORMAT, $time));
    };

    //any other year
    return $date_only ?
           sprintf(CMS_L_COMMON_022,
                   date(CMS_L_DATE_FORMAT, $time)) :
           sprintf(CMS_L_COMMON_005,
                   date(CMS_L_DATE_FORMAT, $time),
                   date(CMS_L_SHORT_TIME_FORMAT, $time));
};