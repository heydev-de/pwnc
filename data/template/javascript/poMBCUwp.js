//menu
var scroll_object   = null;
var scroll_position = 0;
var scroll_time     = 0;
var scroll_flag     = false;

function scroll_func()
{
    var i       = fx_window_top;
    var j       = i - scroll_position;
    scroll_time = (j < 0) ? new Date().getTime() : 0;
    if ((i < 50) || (j < -50)) //in top area or scrolling up
    {
        if (scroll_object.className !== "") //show
        {
            scroll_object.className = "";

            //start hide timer
            if (! scroll_flag)
            {
                scroll_flag = true;
                setTimeout(function scroll_timer()
                {
                    scroll_flag = false;
                    if (fx_window_top < 50) return;
                    if ((new Date().getTime() - scroll_time) < 2000)
                    {
                        scroll_flag = true;
                        setTimeout(scroll_timer, 1000);
                        return;
                    };
                    scroll_position = 0;
                    scroll_func();
                }, 1000);
            };
        };
        scroll_position = i;
    }
    else if (j > 50) //scrolling down
    {
        if (scroll_object.className === "") //hide
            scroll_object.className = "minimize";
        scroll_position = i;
    };
};

//top button
function goto_top_func()
{
    var object = document.getElementById("goto-top");
    if (fx_window_top > 250)
    {
        if (object.className === "") object.className = "goto-top-show";
    }
    else
    {
        if (object.className !== "") object.className = "";
    };
};

fx_event_listen("document_load", () =>
{
    //menu switch
    var container = document.getElementById("menu");
    var link      = container.getElementsByClassName("container");
    var _link     = null;
    var _switch   = null;

    for (var i = 0; i < link.length; i++)
    {
        _link   = link[i];
        _switch = document.createElement("button");
        _switch.setAttribute("tabindex", "-1");
        _switch.setAttribute("aria-hidden", "true");
        _link.parentElement.insertBefore(_switch, _link.nextSibling);
    };

    scroll_object = document.getElementById("sticky");
    goto_top_func();

    fx_event_listen("window_scroll", () =>
    {
        scroll_func();
        goto_top_func();
    });
});