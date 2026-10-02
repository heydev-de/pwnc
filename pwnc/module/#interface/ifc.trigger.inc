<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   INTERFACE
//==================================================================================================

//permission
ifc_permission(["" => t("access")]);

//instantiate trigger data
$data = new data(CMS_TRIGGER_DATA_FILE);

//retrieve selected object from permanent cache
cms_cache_init($object, "trigger." . CMS_USER . ".object");
$_object = $object;

//==================================================================================================
//   MESSAGE HANDLING
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    break;

case "add":
//..................................................................................................

    $data->set(
        ["name"    => t("new_event"),
         "message" => t("default_notification_text")],
        $object = unique_id());

    $ifc_response = $data->save() ? t("msg_done") : t("msg_error");
    break;

case "set":
//..................................................................................................

    if (stre($object) || ! $data->has($object)) break;

    $data->set(
        ["name"       => stripspaces($name),
         "disabled"   => isset($disabled),
         "url"        => $url,
         "_url"       => trigger::compile($url),
         "param"      => $param,
         "_param"     => trigger::compile($param, TRUE),
         "inclusion"  => $inclusion,
         "_inclusion" => trigger::compile($inclusion),
         "exclusion"  => $exclusion,
         "_exclusion" => trigger::compile($exclusion),
         "schedule"   => $schedule,
         "target"     => blank($target) ? NULL : implode("\n", $target),
         "message"    => $message],
        $object);

    trigger::due($object, isset($disabled) ? "" : $schedule, TRUE);

    $ifc_response = $data->save() ? t("msg_done") : t("msg_error");
    break;

case "del":
//..................................................................................................

    if (blank($list)) break; //no selection

    //delete
    foreach ($list AS $value) $data->del($value);

    //save
    if ($data->save())
    {
        trigger::cleanup();
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//validate and cache selected object
if (! $data->has($object))     $object = NULL;
if (nstreq($object, $_object)) cms_cache("trigger." . CMS_USER . ".object", $object, TRUE);

//sync list order settings with permanent cache
cms_cache_sync($order, "trigger." . CMS_USER . ".order", "name");
cms_cache_sync($desc,  "trigger." . CMS_USER . ".desc",  FALSE);

//menu
$menu = [t("command_add") .             "|trigger/command_add"    => "add",
         t("command_delete_selected") . "|trigger/command_delete" => "#del"];

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["object" => $object,
     "order"  => $order,
     "desc"   => $desc]);

?><script>

function s(index) //select
{
    ifc_post("select", index);
};

function o(value) //order
{
    ifc_set("desc",  ((value === ifc_get("order")) && (ifc_get("desc") === "")) ? 1 : "");
    ifc_set("order", value);
    ifc_post();
};

function trigger_get_selection(name)
{
    return Array.from(
        document.querySelectorAll("INPUT[name=\"" + name + "\"]:checked"),
        (object) => object.value)
        .join(",");
};

function trigger_schedule_type()
{
    const list = {
        interval: { month: 0, monthday: 0, day: 1, weekday: 0, hour: 1, minute: 1, time: 0 },
        day:      { month: 0, monthday: 0, day: 0, weekday: 0, hour: 1, minute: 1, time: 0 },
        week:     { month: 0, monthday: 0, day: 0, weekday: 1, hour: 1, minute: 1, time: 0 },
        month:    { month: 0, monthday: 1, day: 0, weekday: 0, hour: 1, minute: 1, time: 0 },
        year:     { month: 1, monthday: 1, day: 0, weekday: 0, hour: 1, minute: 1, time: 0 },
        time:     { month: 0, monthday: 0, day: 0, weekday: 0, hour: 0, minute: 0, time: 1 }}
        [ifc_get("type")];

    for (const key in list) fx_style("trigger-" + key, "display", list[key] ? "" : "none");
};

function trigger_schedule_add()
{
    let value = "";

    switch (ifc_get("type"))
    {
    case "interval":

        [["day", "d"], ["hour", "h"], ["minute", "m"]].forEach((v) =>
        {
            const count = parseInt(ifc_get(v[0]));
            if (count) value += count + v[1];
        });
        break;

    case "day":

        value = ifc_get("minute") + " " + ifc_get("hour") + " * * *";
        break;

    case "week":

        value = ifc_get("minute") + " " + ifc_get("hour") + " * * " +
                (trigger_get_selection("weekday") || "*");
        break;

    case "month":

        value = ifc_get("minute") + " " + ifc_get("hour")  + " " +
                (trigger_get_selection("monthday") || "1") + " * *";
        break;

    case "year":

        value = ifc_get("minute") + " " + ifc_get("hour")  + " " +
                (trigger_get_selection("monthday") || "1") + " " +
                (trigger_get_selection("month")    || "1") + " *";
        break;

    case "time":

        value = ifc_get("time").replace("T", " ");
    };

    if (value) ifc_set("schedule", ifc_get("schedule") + "\n" + value);
};

</script><?php

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     ($object ? "<col style=\"WIDTH:0\">" : "") .
     "</colgroup>" .

     "<tr>" .
     "<td>");

//display object list
//..................................................................................................

ifc_table_open("varied");
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col span=\"3\">" .
     "</colgroup>" .

     "<tr>");

//helper function
$f = function($value) use ($order, $desc)
{
    if (nstreq($value, $order)) return "";
    return $desc ? " ↘" : " ↗";
};

//retrieve metrics and due dates
$array  = $data->get() ?? [];
$metric = [];
$due    = [];
foreach (array_keys($array) AS $key)
{
    $metric[$key] = trigger::metric($key);
    $due[$key]    = trigger::due($key);
};

//compute metric factor
$metric_max    = $metric     ? max($metric)     : 0;
$metric_factor = $metric_max ? 65 / $metric_max : 0;

//sort
$_order = streq($order, "due") ? 1 : (streq($order, "metric") ? 2 : 0);
uksort($array, function($a_key, $b_key) use ($array, $metric, $due, $_order, $desc)
{
    switch ($_order)
    {
    case 1: //due

        $value = ($due[$a_key] ?: PHP_INT_MAX) <=> ($due[$b_key] ?: PHP_INT_MAX);
        break;

    case 2: //metric

        $value = $metric[$a_key] <=> $metric[$b_key];
        break;

    default: //name

        $value = 0;
    };

    if ($value === 0)
        $value = strnatcasecmp($array[$a_key]["name"] ?? "", $array[$b_key]["name"] ?? "");

    return $desc ? -$value : $value;
});

//header
echo("<td class=\"select\">" .
     "</td>" .
     "<th>");
$ifc->set(t("description") . $f("name"), "button", "javascript:o('name');");
echo("</th>" .
     "<th>");
$ifc->set(t("next_run") . $f("due"), "button", "javascript:o('due');");
echo("</th>" .
     "<th>");
$ifc->set(t("executions_day") . $f("metric"), "button", "javascript:o('metric');");
echo("</th>" .
     "</tr>");

foreach (array_keys($array) AS $key)
{
    //name
    $name = x($data->get($key, "name") ?? $key);
    if ($flag = streq($object, $key)) $name = "<strong>$name</strong>";

    echo("<tr>" .

    //selection
         "<td>");
    $ifc->set(NULL, "checkbox", $key, $flag, "list[]");
    echo("</td>" .

    //name
         "<td>" .
         "<a href=\"javascript:s('" . qx($key) . "');\">" .
         "<span class=\"br\">" .
         image("trigger/icon_" . ($data->get($key, "disabled") ? "disabled" : "trigger"), 16, 16) . " $name" .
         "</a>" .
         "</span>" .
         "</td>" .

    //next triggering time
         "<td>" .
         x($due[$key] ? friendly_date($due[$key]) : "–") .
         "</td>" .

    //executions per hour
         "<td>" .
         "<div class=\"beam\" " .
              "style=\"--width:" . (int)($metric[$key] * $metric_factor) . "\">" .
         "</div> " .
         x(format_number($metric[$key])) .
         "</td>" .
         "</tr>");
};

//selection control
echo("<tr>" .
     "<td colspan=\"4\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(t("all"),    "button", "javascript:ifc_list_activate();");
$ifc->set(t("invert"), "button", "javascript:ifc_list_invert();");
$ifc->set(t("none"),   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

//display object data
//..................................................................................................

if ($object)
{
    echo("<td>");
    ifc_table_open();

    //header
    echo("<tr>" .
         "<th>" .
         t("settings") .
         "</th>" .
         "</tr>" .

    //settings
         "<tr>" .
         "<td>");

    //name
    $ifc->set(t("description"), "textarea 40x4 w", $data->get($object, "name"), NULL, "name");

    //disabled
    echo("<div class=\"bar\">" .
         "<div>");
    $ifc->set(t("disabled"), "checkbox", TRUE, $data->get($object, "disabled"), "disabled");
    echo("</div>" .

    //save button
         "<div>");
    $ifc->set(t("command_save"), "button", "set");
    echo("</div>" .
         "</div>");

//data
//..................................................................................................

    ifc_tab_open(t("data"));

    //collect targets
    $permission = new data("#system/permission");
    $array      = [];

    $permission->move("first");
    while (($key = $permission->move("next")) !== NULL)
    {
        if (! permission_is_user($key))         continue;
        if ($permission->get($key, "disabled")) continue;

        $target = permission_remove_prefix($key);

        if (

        //no local agent
             stre($permission->get($key, "api"))
             &&

        //no email set
             stre($permission->get($key, "email"))
             &&

        //no desktop access
             ! cms_permission("desktop", NULL, NULL, $target)
           )
            continue; //skip

        //deduplicate
        $name = permission_get_name($target);
        if (isset($array[$name]))
        {
            $array["$name (" . $array[$name] . ")"] = $array[$name];
            unset($array[$name]);
            $name .= " ($target)";
        };

        $array[$name] = $target;
    };

    //sort
    asort($array, SORT_NATURAL | SORT_FLAG_CASE);

    //target selection
    $ifc->set(t("target_users"), "label");
    $ifc->set($array, "multiselect 40x8 w", trigger::line($data->get($object, "target") ?? ""), NULL, "target");

    //trigger message
    $ifc->set(t("notification_text"), "textarea 40x6 lw", $data->get($object, "message"), NULL, "message");

    //placeholders
    $array = [t("insert_placeholder") => "",
              t("description")        => "name",
              t("application")        => "application",
              t("url")                => "url",
              t("time")               => "time",
              t("user")               => "user"];
    $ifc->set($array, "select 40 w", NULL, NULL, "insert");

?><script>

fx_event_listen(ifc_object("insert"), "change", function()
{
    textcontrol_set("[name=\"l_message\"]", "#replace", "[ %" + this.value + "% ]");
    this.value = "";
});

</script><?php

//condition
//..................................................................................................

    ifc_tab_next(t("conditions"));

    $ifc->set(t("url"),                    "textarea 40x4 lw", $data->get($object, "url"),   NULL, "url");
    $ifc->set(t("parameters"),             "textarea 40x4 lw", $data->get($object, "param"), NULL, "param");
    $ifc->set(t("hint_parameter_rules"),   "info b");
    $ifc->set(t("output_must_contain"),    "textarea 40x4 lw", $data->get($object, "inclusion"), NULL, "inclusion");
    $ifc->set(t("output_may_not_contain"), "textarea 40x4 lw", $data->get($object, "exclusion"), NULL, "exclusion");
    $ifc->set(t("hint_output_rules"),      "info");

//schedule
//..................................................................................................

    ifc_tab_next(t("schedule"));

    echo("<div class=\"element\">" .
         "<div class=\"bar\">" .
         "<div>");

    //type selector
    $array = [t("schedule_interval") => "interval",
              t("schedule_day")      => "day",
              t("schedule_week")     => "week",
              t("schedule_month")    => "month",
              t("schedule_year")     => "year",
              t("point_in_time")     => "time"];
    $ifc->set($array, "select", "interval", NULL, "type");

    //month selector
    echo("<span id=\"trigger-month\">");
    ifc_popover_open(t("month"));
    echo("<div class=\"trigger-month\">");
    foreach ([t("january"), t("february"), t("march"),
              t("april"),   t("may"),      t("june"),
              t("july"),    t("august"),   t("september"),
              t("october"), t("november"), t("december")]
             AS $index => $label)
        $ifc->set($label, "checkbox w", $index + 1, NULL, "month");
    echo("</div>");
    ifc_popover_close();
    echo("</span>");

    //month day selector
    $array = [0 => 0];
    echo("<span id=\"trigger-monthday\">");
    ifc_popover_open(t("day"));
    echo("<div class=\"trigger-monthday\">");
    for ($i = 1; $i <= 31; $i++)
    {
        $ifc->set(sprintf("%02d", $i), "checkbox w", $i, NULL, "monthday");
        $array[$i] = $i;
    };
    echo("</div>");
    ifc_popover_close();
    echo("</span>");

    //interval day selector
    echo("<span id=\"trigger-day\">");
    $ifc->set($array, "select", 0, NULL, "day");
    echo("<div class=\"label\">" .
         t("days") .
         "</div>" .
         "</span>");

    //weekday selector
    echo("<span id=\"trigger-weekday\">");
    ifc_popover_open(t("weekday"));
    foreach ([1 => t("monday"),   2 => t("tuesday"), 3 => t("wednesday"),
              4 => t("thursday"), 5 => t("friday"),  6 => t("saturday"),
              0 => t("sunday")]
             AS $value => $label)
        $ifc->set($label, "checkbox w", $value, NULL, "weekday");
    ifc_popover_close();
    echo("</span>");

    //hour selector
    $array = [];
    for ($i = 0; $i <= 59; $i++) $array[sprintf("%02d", $i)] = $i;
    echo("<span id=\"trigger-hour\">");
    $ifc->set(array_slice($array, 0, 24, TRUE), "select", 0, NULL, "hour");
    echo("</span>" .

    //minute selector
         "<span id=\"trigger-minute\">" .
         "<div class=\"label\">" .
         ":" .
         "</div>");
    $ifc->set($array, "select", 0, NULL, "minute");
    echo("</span>");

    //point in time selector
    echo("<span id=\"trigger-time\">");
    $ifc->set(NULL, "datetime", NULL, NULL, "time");
    echo("</span>" .
         "</div>" .

    //add button
         "<div>");
    $ifc->set(t("command_add"), "button b", "javascript:trigger_schedule_add();");
    echo("</div>" .
         "</div>");

    //schedule list
    $ifc->set(NULL, "textarea 40x8 w", $data->get($object, "schedule"), NULL, "schedule");

    //info
    $ifc->set(t("hint_schedule_rules"), "info");

    echo("</div>");

?><style>

DIV:is(.trigger-month, .trigger-monthday)
{
    --repeat: 6;
    DISPLAY: grid;
    GRID-AUTO-FLOW: column;
    GRID-TEMPLATE-ROWS: repeat(var(--repeat), auto);

    &.trigger-monthday
    {
        --repeat: 7;
    }

    & > LABEL.fullwidth
    {
        MARGIN-BOTTOM: var(--ifc-margin-half);
    }
}

</style><script>

fx_event_listen(ifc_object("type"), "change", trigger_schedule_type);
trigger_schedule_type();

</script><?php

    ifc_tab_close();

    echo("</td>" .
         "</tr>");
    ifc_table_close();
    echo("</td>");
};

echo("</tr>" .
     "</table>");

$ifc->close();