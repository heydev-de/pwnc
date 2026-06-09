<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DESKTOP INTERFACE
//==================================================================================================

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    $time   = NULL;
    break;

case "time":
//..................................................................................................

    $object = NULL;
    $time   = $ifc_param;
    break;

case "add":
//..................................................................................................

    if (($key = $desktop->data->seek(["#subtype" => "appointment"])) !== NULL)
    {
        $desktop->data->set_buffer(
            [["name"    => CMS_L_DESKTOP_APPOINTMENT_001,
              "#type"   => "appointment",
              "time"    => $ifc_param,
              "#expire" => $ifc_param + 86400]]);

        if (
             ($_object = $desktop->data->insert($key))
             &&
             $desktop->save()
           )
        {
            $object       = $_object;
            $time         = NULL;
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "save":
//..................................................................................................

    $desktop->object_set($object, "name",        $ifc_param1);
    if (($ifc_param2 = strtotime($ifc_param2)) !== FALSE)
    {
        $desktop->object_set($object, "time",    $ifc_param2);
        $desktop->object_set($object, "#expire", isset($ifc_param3) ? $ifc_param2 + 86400 : NULL);
    }
    else
    {
        $desktop->object_set($object, "#expire", isset($ifc_param3));
    };
    $desktop->object_set($object, "content",     $ifc_param4);
    $desktop->object_set($object, "place",       $ifc_param5);
    $desktop->object_set($object, "participant", $ifc_param6);

    if ($desktop->save())
    {
        $time         = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    if ($desktop->delete_object($object))
    {
        $object       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$time_current     = time();
$day_current      = (int)date("j", $time_current);
$month_current    = (int)date("n", $time_current);
$year_current     = (int)date("Y", $time_current);
$time           ??= $desktop->object_get($object, "time") ?? $time_current;
$time            -= $time % 900;
$hour             = (int)date("G", $time);
$minute           = (int)date("i", $time);
$day              = (int)date("j", $time);
$days_per_month   = (int)date("t", $time);
$month            = (int)date("n", $time);
$year             = (int)date("Y", $time);
$list             = [];
$count            = [];

if ($hour < 6)
{
    $hour += 24;
    $day  -=  1;

    if ($day < 1)
    {
        if (--$month < 1)
        {
            $month  = 12;
            $year  -= 1;
        };

        $day = $days_per_month = (int)date("t", mktime(6, 0, 0, $month, 1, $year));
    };
};

//appointment list
$desktop->data->move("first");
while (($key = $desktop->data->move("next")) !== NULL)
{
    if (nstreq($desktop->object_type($key), "appointment")) continue;

    $_time   = (int)($desktop->object_get($key, "time") ?? $time_current);
    $_time  -= $_time % 900;
    $_hour   = (int)date("G", $_time);
    $_day    = (int)date("j", $_time);
    $_month  = (int)date("n", $_time);
    $_year   = (int)date("Y", $_time);

    if ($_hour < 6)
    {
        $_hour += 24;
        $_day  -=  1;

        if ($_day < 1)
        {
            if (--$_month < 1)
            {
                $_month  = 12;
                $_year  -=  1;
            };

            $_day = (int)date("t", mktime(6, 0, 0, $_month, 1, $_year));
        };
    };

    if (($_year === $year) && ($_month === $month) && ($_day === $day))
    {
            $_minute = (int)date("i", $_time);
            $list[$_hour][$_minute][] = $key;
    };

    $count[$_year][$_month][$_day] = ($count[$_year][$_month][$_day] ?? 0) + 1;
};

$buffer_year  = [];
$buffer_month = [];
$buffer_day   = [];
$_buffer_day  = [];
$buffer_hour  = [];

$sum_recursive = function(&$value)
{
    if (! isset($value)) return FALSE;

    $sum = 0;
    array_walk_recursive($value, function($val) use (&$sum) { $sum += $val; });
    return $sum;
};

for ($_year = $year - 5; $_year <= $year + 5; $_year++)
{
    if (($_year < 1970) || ($_year > 2037)) continue;

    $_days_per_month   = (int)date("t", mktime(6, 0, 0, $month, 1, $_year));
    $_day              = ($day > $_days_per_month) ? $_days_per_month : $day;
    $_time             = mktime(6, 0, 0, $month, $_day, $_year);
    $_count            = $sum_recursive($count[$_year]);
    $flag              = $_year === $year;
    $_flag             = $_year === $year_current;
    $class             = $flag   ? "class=\"enabled\" "   :
                         ($_flag ? "class=\"highlight\" " : "");
    $buffer_year     []= "<td $class" .
                             "style=\"TEXT-ALIGN:center\">" .
                         "<a href=\"javascript:ifc_post('time','$_time');\">" .
                         ($flag   ? sprintf("<strong>%s</strong>", $_year) : $_year) .
                         ($_count ? sprintf(" (%d)", $_count)              : "") .
                         "</a>" .
                         "</td>";

    if ($_year !== $year) continue;

    for ($_month = 1; $_month <= 12; $_month++)
    {
        $_days_per_month = (int)date("t", mktime(6, 0, 0, $_month, 1, $year));
        $_day            = ($day > $_days_per_month) ? $_days_per_month : $day;
        $_time           = mktime(6, 0, 0, $_month, $_day, $year);
        $_count          = $sum_recursive($count[$year][$_month]);
        $flag            = $_month === $month;
        $_flag           = ($_year === $year_current) && ($_month === $month_current);
        $class           = $flag   ? "class=\"enabled\" "   :
                           ($_flag ? "class=\"highlight\" " : "");
        $buffer_month[]  =
            "<td $class" .
                "style=\"TEXT-ALIGN:center\">" .
            "<a href=\"javascript:ifc_post('time','$_time');\">" .
            ($flag   ? sprintf("<strong>%s</strong>", month($_month)) : month($_month)) .
            ($_count ? sprintf(" (%d)", $_count)                      : "") .
            "</a>" .
            "</td>";

        if ($_month !== $month) continue;

        for ($_day = 1; $_day <= $days_per_month; $_day++)
        {
            $_time         = mktime(6, 0, 0, $month, $_day, $year);
            $day_week      = (int)date("w", $_time);
            $day_name      = substr(weekday($day_week), 0, 2);
            $_count        = $count[$year][$month][$_day] ?? 0;
            $flag          = $_day === $day;
            $_flag         = ($_year === $year_current) && ($_month === $month_current) && ($_day === $day_current);
            $__flag        = in_array($day_week, [0, 6]);
            $class         = $flag    ? "class=\"enabled\" "   :
                             ($_flag  ? "class=\"highlight\" " :
                             ($__flag ? "class=\"varied\" "    : ""));
            $buffer_day[]  =
                "<td $class" .
                    "style=\"TEXT-ALIGN:center\">" .
                $day_name .
                "</td>";
            $_buffer_day[] =
                "<td $class" .
                    "style=\"TEXT-ALIGN:center\">" .
                "<a href=\"javascript:ifc_post('time','$_time');\">" .
                    ($flag   ? sprintf("<strong>%s</strong>", $_day) : $_day) .
                    ($_count ? sprintf(" (%d)", $_count)             : "") .
                "</a>" .
                "</td>";

            if ($_day !== $day) continue;

            for ($_hour = 6; $_hour < 30; $_hour++)
            {
                $_varied = ifc_varied(NULL, 1);

                if ($_hour === 24)
                {
                    $_time         = mktime($_hour, 0, 0, $month, $day, $year);
                    $buffer_hour[] =
                        "<tr>" .
                        "<th colspan=\"3\" " .
                            "style=\"TEXT-ALIGN:center\">" .
                        weekday(date("w", $_time)) . ", " . date(CMS_L_DATE_FORMAT, $_time) .
                        "</th>" .
                        "</tr>";
                };

                $buffer_hour[] =
                    "<tr>" .
                    "<td$_varied rowspan=\"4\">" .
                    "<strong>" .
                    sprintf("%02d:", ($_hour >= 24) ? $_hour - 24 : $_hour) .
                    "</strong>" .
                    "</td>";

                for ($_minute = 0; $_minute <= 45; $_minute += 15)
                {
                    $_varied       = ifc_varied(NULL, 2);
                    $buffer_hour[] =
                        "<td$_varied>" .
                        sprintf("%02d", $_minute) .
                        "</td>";

                    if (isset($list[$_hour][$_minute]))
                    {
                        $buffer_hour[] = "<td class=\"highlight\">";

                        foreach ($list[$_hour][$_minute] AS $value)
                        {
                            $name          = x($desktop->object_get($value, "name"));
                            $_time         = (int)($desktop->object_get($value, "time") ?? $time_current);
                            $buffer_hour[] =
                                "<a href=\"javascript:ifc_post('select','" . qx($value) . "');\">" .
                                image("desktop/icon_appointment") . " " .
                                (streq($value, $object) ? sprintf("<strong>%s</strong>", $name) : $name) .
                                " (" . friendly_date($_time) . ")" .
                                "</a><br>";
                        };
                    }
                    else
                    {
                        $buffer_hour[] = "<td$_varied>";
                    };

                    $_time         = mktime($_hour, $_minute, 0, $month, $day, $year);
                    $buffer_hour[] =
                        "<a href=\"javascript:ifc_post('add','$_time');\">" .
                        image("desktop/command_create") . " " . CMS_L_COMMAND_ADD .
                        "</a>" .
                        "</td>" .
                        "</tr>";
                };
            };
        };
    };
};

//menu
$menu = stre($object) ? NULL :
        [CMS_L_COMMAND_SAVE .   "|desktop/command_save"   => "save",
         CMS_L_COMMAND_DELETE . "|desktop/command_delete" => "#delete"];

//display
$ifc = new ifc(
    NULL,
    $ifc_page,
    $menu,
    array_merge(
        $param,
        ["object" => $object,
         "time"   => $time]),
    NULL,
    CMS_L_DESKTOP_APPOINTMENT_002);

//year list
ifc_table_open();
echo("<tr>" .
     implode("", $buffer_year) .
     "</tr>");
ifc_table_close();

//month list
ifc_table_open();
echo("<tr>" .
     implode("", $buffer_month) .
     "</tr>");
ifc_table_close();

//day list
ifc_table_open();
echo("<tr>" .
     implode("", $buffer_day) .
     "</tr>" .
     "<tr>" .
     implode("", $_buffer_day) .
     "</tr>");
ifc_table_close();

echo("<table class=\"layout\">" .
     "<tr>" .
     "<td>");

ifc_table_open();
echo("<colgroup>" .
     "<col span=\"2\" " .
          "style=\"WIDTH:0\">" .
     "<col>" .
     "</colgroup>" .
     "<tr>" .
     "<th colspan=\"3\" " .
         "style=\"TEXT-ALIGN:center\">" .
     weekday(date("w", $time)) . ", " . date(CMS_L_DATE_FORMAT, $time) .
     "</th>" .
     "</tr>" .
     "<tr>" .
     implode("", $buffer_hour));
ifc_table_close();

echo("</td>");

//data
if ($object)
{
    $time = (int)($desktop->object_get($object, "time") ?? $time_current);

    echo("<td style=\"WIDTH:0\">");

    ifc_table_open();
    echo("<tr>" .
         "<th>");
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_005, "text 40 40", $desktop->object_get($object, "name"));
    echo("</th>" .
         "</tr>" .

         "<tr>" .
         "<td>" .
         "<span class=\"element\">" .
         x(friendly_date($time)) .
         "</span><br>");
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_007, "datetime f",      $time);
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_011, "checkbox b",      TRUE, $desktop->object_get($object, "#expire"));
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_012, "textarea 40x8 b", $desktop->object_get($object, "content"));
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_006, "textarea 40x4 b", $desktop->object_get($object, "place"));
    $ifc->set(CMS_L_DESKTOP_APPOINTMENT_013, "textarea 40x4",   $desktop->object_get($object, "participant"));
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};

echo("</tr>" .
     "</table>");

$ifc->close();