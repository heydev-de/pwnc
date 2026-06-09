function video_loading(video)
{
    video_event(video, "loading");
};

function video_playing(video)
{
    video_event(video, video.muted ? "preview" : "playing");
};

function video_paused(video)
{
    video_event(video, "paused");
};

function video_buffering(video)
{
    if (video.className.match(/(?:^|\s)video-loading(?:$|\s)/)) return;
    var flag = video.className.match(/(?:^|\s)video-(?:preview|playing)(?:$|\s)/);
    video_event(video, (video.muted ? "preview-" : "") + (flag ? "waiting" : "loading"));
};

function video_event(video, value)
{
    var _class      = video.className;
    var regex       = new RegExp("(?:^|\s)video-" + value + "(?:$|\s)");
    if (_class.match(regex)) return;
    _class          = _class.replace(/(?:^|\s+)video-[-a-z]+(?:$|\s+)/g, " ");
    video.className = _class + " video-" + value;
};

function video_play(video, flag = 0) //1 = preview, 2 = reset
{
    if (! video.paused)
    {
        video.pause();
        video.currentTime = 0;
    };

    var preview = flag === 1;
    var reset   = flag === 2;
    var list    = document.getElementsByTagName("video");
    var object  = null;

    for (object of list)
    {
        if (object === video) continue;
        object.pause();
    };

    video.muted       = (preview || reset) ? true : false;
    video.currentTime = (preview || reset) ? parseFloat(video.getAttribute("data-start")) : 0;
    if (reset) return;
    video.play();
};

function video_pause(video)
{
    video.pause();
};

function video_fullscreen(video)
{
    if (video.requestFullscreen)            video.requestFullscreen();
    else if (video.mozRequestFullScreen)    video.mozRequestFullScreen();
    else if (video.webkitRequestFullscreen) video.webkitRequestFullscreen();
    else if (video.msRequestFullscreen)     video.msRequestFullscreen();
    else if (video.webkitEnterFullScreen)   video.webkitEnterFullScreen();
};