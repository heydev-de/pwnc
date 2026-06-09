function audio_loading(audio)
{
    audio_event(audio, "loading");
};

function audio_playing(audio)
{
    audio_event(audio, "playing");
};

function audio_paused(audio)
{
    audio_event(audio, "paused");
};

function audio_buffering(audio)
{
    if (audio.className.match(/(?:^|\s)audio-loading(?:$|\s)/)) return;
    var flag = audio.className.match(/(?:^|\s)audio-playing(?:$|\s)/);
    audio_event(audio, flag ? "waiting" : "loading");
};

function audio_timeupdate(audio, range)
{
    range.value = 100 / audio.duration * audio.currentTime;
};

function audio_event(audio, value)
{
    var _class      = audio.className;
    var regex       = new RegExp("(?:^|\s)audio-" + value + "(?:$|\s)");
    if (_class.match(regex)) return;
    _class          = _class.replace(/(?:^|\s+)audio-[-a-z]+(?:$|\s+)/g, " ");
    audio.className = _class + " audio-" + value;
};

function audio_play(audio)
{
    if (! audio.paused)
    {
        audio.pause();
        audio.currentTime = 0;
    };

    var list   = document.getElementsByTagName("audio");
    var object = null;

    for (object of list)
    {
        if (object === audio) continue;
        object.pause();
    };

    audio.play();
};

function audio_pause(audio)
{
    audio.pause();
};

function audio_seek(audio, percent)
{
    audio.currentTime = audio.duration / 100 * percent;
};