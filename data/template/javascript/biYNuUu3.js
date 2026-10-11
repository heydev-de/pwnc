class slider
{
    object    = null;
    container = null;
    control   = null;
    slides    = [];
    buttons   = [];
    current   = 0;
    before    = 0;
    interval  = 0;

    constructor(object, interval = 0)
    {
        this.object    = object;
        this.container = object.querySelector(":scope > DIV.slider-container");
        this.control   = object.querySelector(":scope > DIV.slider-control");

        fx_event_listen(object.querySelector(":scope > BUTTON.slider-prev"), "click", () => this.prev());
        fx_event_listen(object.querySelector(":scope > BUTTON.slider-next"), "click", () => this.next());

        this.container.querySelectorAll(":scope > DIV").forEach(slide =>
        {
            let n = this.slides.length;
            this.slides.push(slide);

            let button = document.createElement("button");
            button.textContent = n + 1;
            this.buttons.push(button);

            this.control.appendChild(button);
            fx_event_listen(button, "click", () => { this.to(n); });
        });

        this.container.firstElementChild.classList.add("active");
        this.control.firstElementChild.classList.add("active");

        fx_swipe(this.container, (object, dir) => { this.swipe(dir); });

        if (interval > 0)
        {
            this.interval = interval;
            setTimeout(() => this.auto(), this.interval);
        };
    };

    prev()
    {
        this.to(this.current - 1);
    };

    next()
    {
        this.to(this.current + 1);
    };

    swipe(dir)
    {
        if      (dir === "r") this.prev();
        else if (dir === "l") this.next();
    };

    to(n)
    {
        if (n === this.current) return;
        this.object.style.setProperty("--slider-direction", (n > this.current) ? 1 : -1);

        let c = this.slides.length;
        if      (n <  0) n += c;
        else if (n >= c) n  = 0;

        let slide = this.slides[this.before];
        slide.classList.remove("before");

        slide = this.slides[this.current];
        slide.classList.remove("active");
        slide.classList.add("before");

        this.slides[n].classList.add("active");

        let button = this.buttons[this.current];
        button.classList.remove("active");
        this.buttons[n].classList.add("active");

        this.before  = this.current;
        this.current = n;
    };

    auto()
    {
        if (! this.object.matches(":focus-within, :hover")) this.next();
        setTimeout(() => this.auto(), this.interval);
    };
};