fx_event_listen("document_load", () =>
{
    //dynamically load photoswipe
    if (document.querySelector("A.thumbnail"))
    {
        load_css("3rdparty/photoswipe-5.4.4/photoswipe.css");
        import(new URL("3rdparty/photoswipe-5.4.4/photoswipe.esm.min.js", document.baseURI).href).then((mod) => { mod.photoswipe_init(); });
    };

    //dynamically load parallax
    if (document.querySelector("[data-parallax-distance]"))
        load_script("3rdparty/parallax/parallax.js", () => parallax_init());
});