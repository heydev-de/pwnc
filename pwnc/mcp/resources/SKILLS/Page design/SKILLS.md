---
name: PWNC Page Design Manual
description: Use this skill whenever you design a site or build or rework a page template — how frame, slots, components, menus, placeholders and the fluid-responsive stylesheet fit together so the site scales, stays editable and keeps working as pages and content change.
---
# PWNC Page Design Manual

Scope: the strategy of a site design. It decides where each part of a page lives so that the result works without template edits later. Element syntax, IDs and PHP: [Template manual](<pwnc:SKILLS/Template manual/SKILLS.md>). Tokens, scaling and stylesheets: [CSS guideline](<pwnc:SKILLS/CSS guideline/SKILLS.md>). Composing page content from components: [Layout](<pwnc:SKILLS/Layout/SKILLS.md>). Navigation tree: [Site structure](<pwnc:SKILLS/Site structure/SKILLS.md>).

## Principle

**A page template is a frame, not a page.** It holds only what every page shares and leaves everything that varies to slots, the navigation tree, placeholders and components. A design that looks right but has its content baked into the frame breaks at the first change: a new page that is missing from the footer, a teaser nobody can replace, a layout that stops growing on large screens.

**Values live per page.** Every editable element of the page template stores its value in the document of the page being rendered. A value set on one page is absent on every other page and on every new one. Content that is the same everywhere therefore never belongs in an editable element of the frame.

For every part of the design, decide where its content comes from:

| Part | Source | Mechanism |
|------|--------|-----------|
| Page content: sections, teasers, text, images | the page's document | the main slot (`tp_content`), filled with components ([Layout](<pwnc:SKILLS/Layout/SKILLS.md>)) |
| Optional per-page area in the frame: banner, sidebar, pre-footer | the page's document | an extra slot or element, rendered only when filled |
| Lists of page links: main navigation, footer, legal and service links, section menus | the navigation tree | `<CMS:menu>` |
| Site data: name, company, address, phone, e-mail, social links | the Placeholders app | `%%sitename%%`, `%%company%%`, `%%address%%` … |
| Fixed interface texts: "Menu", "Close", "Search" | the Placeholders app | `%%l_*%%` placeholders, one value per language |
| A recurring block on many pages that editors may replace: call to action, contact box | a component | slot with `default="<component id>"` |
| Site-wide technical parts: icons, Open Graph, structured data, tracking, language links | a shipped component (section Shipped Parts) | `<CMS:template default="<id>" edit="off"/>` |
| The same editorial block, edited once, shown on several pages | a synced element | `#reference` binding ([Content editing](<pwnc:SKILLS/Content editing/SKILLS.md>), section Synced Elements) |

## Anatomy of a Page Template

```html
<!DOCTYPE html>
<html lang="<?=x(CMS_LANGUAGE)?>">
<head>
<CMS:head title="%s · %%sitename%%"/>
<meta name="robots" content="<?=x(CMS_CONTENT_META_ROBOTS)?>">
<meta name="viewport" content="width=device-width, initial-scale=1">
<CMS:template default="<icon component>" edit="off"/>
<CMS:template default="<open graph component>" edit="off"/>
<CMS:template default="<breadcrumb structured data component>" edit="off"/>
</head>
<body><CMS:control/>

<header id="head">
  <a id="logo" href="<?=x(CMS_ROOT_URL . cms_param())?>"><CMS:image id="i_logo" default="image://<id>" width="300" edit="off"/></a>
  <nav id="menu"><CMS:menu/></nav>
</header>

<CMS:cblock idref="tp_banner"><div id="banner"><CMS:template id="tp_banner"/></div></CMS:cblock>

<main id="body"><CMS:template id="tp_content"/></main>

<footer id="foot">
  <div id="address">%%company%%<br>%%address%%</div>
  <nav id="foot-menu"><CMS:menu base="<footer node id>" show-hidden="on"/></nav>
  <CMS:template default="<language links component>" edit="off"/>
  <CMS:backlink/>
</footer>

<CMS:template default="<script loader component>" edit="off"/>
<CMS:noedit><CMS:template default="<tracking component>" edit="off"/></CMS:noedit>
</body>
</html>
```

- **Head.** `<CMS:head/>` is required once. Leave out `stylesheet`: the page then follows `data/stylesheet.css`, which also styles login, chat and the other pages without a page template, so a design switch stays one change ([CSS guideline](<pwnc:SKILLS/CSS guideline/SKILLS.md>), section Linking the design).
- **Viewport.** Write `width=device-width, initial-scale=1` and nothing more. `maximum-scale=1` or `user-scalable=no` blocks zooming for visitors who need it.
- **`lang`.** `CMS_LANGUAGE` is empty while no languages are configured; the Simple design then falls back to a fixed code: `<?=stre(CMS_LANGUAGE) ? "de" : x(CMS_LANGUAGE)?>`.
- **Explicit IDs** for every element editors fill (`i_logo`, `tp_banner`, `tp_content`). An auto-ID element added later re-addresses everything after it, and content lands in the wrong slot. Fixed technical slots (`edit="off"`) store nothing and may keep auto-IDs.
- **Main slot.** Use `tp_content` in every page template of the site, as the shipped ones do; the Template app's page skeleton writes `tp1`, so rename it. Switching a page to another page template then keeps its content. Every other frame slot that should survive a switch needs the same ID in all page templates too.
- **Home link.** Link the logo to the site root, `CMS_ROOT_URL . cms_param()`, as the Simple design does. The root redirects to the first node of the tree, so the link needs no id ([Site structure](<pwnc:SKILLS/Site structure/SKILLS.md>), section What a Node Points To).
- **Fixed links** in the frame are for single structural targets such as the action of a search form. Build them with `translate_url("content://<index>")` or `translate_url("directory://<id>")` in PHP. Never write a literal URL. Never write a list of links by hand (section Navigation).
- **Shipped page templates are examples, not the reference.** Simple sets `stylesheet` on `<CMS:head/>`, Minimal sets `maximum-scale=1`. Do not copy either as it is.

## Design Folder

`design/<name>/` is the home of every file a design brings besides its templates: the design stylesheet, fonts, images of the frame (backgrounds, patterns, decorative graphics) and scripts. One folder per design keeps it portable: copy the folder, and the design moves with everything it needs. Updates never overwrite `design/` ([CSS guideline](<pwnc:SKILLS/CSS guideline/SKILLS.md>), section Design package).

- **Layout.** `stylesheet.css` and `font/` as in `design/simple/`; add folders as needed, for example `image/` and `javascript/`. Server-side code belongs in components, not in PHP files here.
- **Paths.** `<CMS:head/>` sets `<base>` to the site root, so the page template's HTML and JS address the folder as `design/<name>/…`. A template stylesheet is served from `data/template/stylesheet/`, so its `url()` needs `../../../design/<name>/…`.
- **Not for content.** Images editors choose or replace (logo, banners, page images) belong in the Images app and are placed with `<CMS:image>`; the design folder holds only what is part of the design itself.

## Slots: Leave Room

A slot is a place where editors choose a component per page. The frame provides the places, components provide the blocks.

- **Optional areas** (banner, sidebar, pre-footer) are slots without a default. Wrap the slot and its container in `<CMS:cblock idref="<slot id>">`: an empty slot then leaves no markup, no empty box and no gap. While conditional blocks are being edited the slot stays visible, so editors can fill it.
- **A default makes a block site-wide but replaceable.** `<CMS:template id="tp_cta" default="<component id>"/>` shows that component on every page until an editor picks another one there. Changing the default in the template changes every page that kept it. To let editors switch the block off on single pages, add a switch and condition the slot on it: `<CMS:switch id="v_no_cta" title="Hide call to action"/><CMS:calt idref="v_no_cta"><CMS:template id="tp_cta" default="…"/></CMS:calt>`.
- **Never wrap a slot that has a default in a `cblock` on that slot.** A condition sees a default only after its element has rendered. The `cblock` stands before the slot, sees an empty value on pages without a stored choice and hides the default.
- **`edit="off"`** fixes the component of a slot: editors cannot swap it. Use it for technical parts that must be identical everywhere. Editable elements inside such a component still store per page, so a fixed component should not contain any.
- **Page-specific decoration** in the frame, such as a banner image, can be a plain element instead of a slot (`<CMS:image id="i_banner"/>` in a `cblock`). It is filled per page.

## Blocks: Self-Contained Components

Teasers, call-to-action bands, testimonials, contact boxes and every other recurring block are components, never HTML in the frame.

- **Placement.** A block that belongs to one page goes into the content tree of that page. A block that appears on most pages goes into a frame slot as its default. Hard-coded markup in the frame is the one placement that cannot be replaced, moved, hidden or translated per page.
- **Check what exists.** The installation ships components in categories such as Layout, Content, Text, Image / Media, Link, Navigation, Module and Page. Look in the Template app before building one; ids differ per site, so find them by name ([Layout](<pwnc:SKILLS/Layout/SKILLS.md>), section Shipped Components and Your Site).
- **Self-contained** means the component works in any slot on any page:
  - its own template with its own CSS and JS, which are linked automatically wherever it is used;
  - every CSS selector prefixed with the component's root class; no `#id` selectors, because a page can hold several instances. For HTML ids use `$cms_template_temp_id`;
  - no assumption about its width. It may sit in a full-width row or in a narrow column. Size with percentages, `min()` and flex or grid relative to its container; the phone breakpoint is its only rule based on the page width;
  - colours, fonts and spacing from `--cms-*` tokens, sizes in factors (section Fluid-Responsive Stylesheet);
  - settings (`value`, `switch`, `repeat`) in an edit-only settings table, as the shipped components do ([Layout](<pwnc:SKILLS/Layout/SKILLS.md>), section Concept);
  - no output when empty: wrap optional parts in `cblock`;
  - links as `<CMS:href>` with an editable value, not as fixed URLs.
- **Exchangeable.** Components for the same purpose with the same element IDs replace each other in a slot without losing content. Use the naming conventions of the [Template manual](<pwnc:SKILLS/Template manual/SKILLS.md>) (`t_title`, `i1`, `h1` …), so a "Teaser, image left" can become a "Teaser, image right" in place.
- **No component CSS in the page template.** The page template's stylesheet styles the frame: header, navigation, footer, main width. A rule for a block there breaks the block on a site with another page template.

## Navigation: Menus, Not Links

Every list of page links is a `<CMS:menu>` over a branch of the navigation tree. Editors then add, remove, rename, reorder and translate links in the Navigation app, and new pages appear without a template edit. A list written as `<a href>` or `translate_url()` calls in the frame freezes the site structure into code.

| Need | Menu |
|------|------|
| Main navigation | `<CMS:menu/>`, or with `depth` for a limited number of levels |
| Footer, legal and service links | a branch of its own: `<CMS:menu base="<node id>" show-hidden="on"/>` |
| Section menu beside the content | `<CMS:menu level="1" filter="active"/>`: everything below the current top-level page |
| Section menu that unfolds along the current path | `<CMS:menu level="1" filter="active-open"/>` |
| Subpages of the current page | `<CMS:menu level="+0" depth="1" filter="active-open"/>` |

`level` counts from 0 for the first level below `base`, the top level without `base`. A relative level counts from the current page: `+0` is the level directly below it, `-1` its own level. Outside the `base` branch the current page has no path, and a relative level acts as the absolute one. Without a filter a menu shows every node of the selected levels in the whole tree, not only those near the current page.

- **A branch for the footer** is a hidden container node whose children are the footer links. Hidden keeps it out of the main menu. A menu over a hidden branch needs `show-hidden="on"`: without it, hidden nodes and everything below them are removed before the menu is built, `base` no longer exists, and the menu falls back to the whole tree, so the footer repeats the main navigation. A mistyped or deleted `base` id does the same. `show-hidden="on"` also shows hidden nodes inside the branch.
- **A page in two menus** gets a second node: the footer branch holds a node whose URL is `directory://<id of the page's node>`, and the menu links it straight to the page ([Site structure](<pwnc:SKILLS/Site structure/SKILLS.md>), section What a Node Points To). The shipped `_footer` works this way: Privacy, Legal info, Sitemap and Contact point to nodes elsewhere in the tree, and Contact also stands in the main menu. Such a node, not a hand-written link, is the answer whenever a page belongs in a second list.
- **Alternative:** a visible container, kept out of the main menu with `exclude="<node id>"` on the main menu. Every other menu over the whole tree, such as the Sitemap component, then needs the same `exclude`, so prefer the hidden container.
- **Node ids** are generated when a node is created. Read the id from the tree (`data-fv-hir-index`) and write it into `base`. Shipped installations already have the hidden containers `_footer`, which the Simple design shows in its footer, and `_lp` for landing pages (section Shipped Parts).
- **Styling.** Menus render nested `UL` lists with links. The design gives lists inside `NAV` no bullets and no list styling, so the page template's CSS styles each `NAV` ([CSS guideline](<pwnc:SKILLS/CSS guideline/SKILLS.md>), section What the design does to every element). Hooks: every link on the current path has the class `active`, a link of a node with children `container`; the current page's name is wrapped in `STRONG`. The `LI` ids change when a menu is added to the page, so do not style by id.
- **Empty menus.** A section or subpage menu on a page without subpages outputs nothing, but its `NAV` stays, and `cblock` cannot test a menu. Hide it in CSS: `NAV:not(:has(LI)) { DISPLAY: none; }`.
- A menu reflects the tree everywhere it is used. A node added to the footer branch appears in every footer at once.

## Shipped Parts

Check what ships before building a frame part. These parts are site data like the components: an update replaces `pwnc/` only, so a site may have changed, renamed or deleted them. Find components by name in the Template app and read their source before relying on the descriptions below ([Layout](<pwnc:SKILLS/Layout/SKILLS.md>), section Shipped Components and Your Site).

**Components for the frame** (shipped id in brackets):

| Component | Output | Place |
|-----------|--------|-------|
| _ Icon (`A2yfYCIO`) | favicon and touch icons in eight sizes, generated from one image set in its code | head |
| _ Open Graph (`KGac19nD`) | Open Graph tags from the page's title, description and image, with a fallback image | head |
| _ Breadcrumb (structured data) (`UrRhr6QU`) | schema.org `BreadcrumbList` as JSON-LD: home page, the path, the current page | head |
| _ Script loader (`F5P2O2n5`) | loads PhotoSwipe and parallax when the page holds their markers (Scripts on demand below) | end of `BODY` |
| _ Tracking (`dmH8QLbh`) | consent dialog that inserts the code of the placeholder `tracking_code` after consent; nothing when the placeholder is empty, while editing, or on the privacy page, whose node id is written in its code. An element with `id="tracking-background"` is blurred while the dialog is open | end of `BODY`, in `<CMS:noedit>` |
| _ Disable tracking (`GBenSdEC`) | opt-out link | privacy page content |
| Language links (`zlOJPXC6`) | one link per other enabled language, with flag image and name, to the same page in that language, in `DIV.menu-language` | header or footer |
| Language codes (`MmsqbjDw`) | every enabled language, the active one included, as a two-letter link to the same page in that language, the full name as `title`, in `NAV.menu-language-codes` | header or footer |
| Language selector (`Ay8uSAmp`) | the active language as a toggle that opens the other languages, CSS only, with its own stylesheet | header |
| Landing pages (`VclQdXxH`) | three links from the hidden container `_lp` in `NAV.menu-landing-page`; each page URL gets its own selection, so the landing pages are linked across the site; nothing without landing pages | footer |
| Breadcrumb (`87Qy4HYp`) | visible trail in `NAV.menu-breadcrumb`: home page, the path, the current page with `aria-current="page"`; nothing on the home page. The structured data comes from _ Breadcrumb (structured data) | above the main content |
| Sitemap (`vhLArrvI`) | the whole menu tree | content of a sitemap page |

The language components output nothing while no languages are configured in the Languages app.

**Adapting a part.** Place the component as a fixed slot and position it with the page template's CSS, as the Simple design does with Language codes, Landing pages and _ Breadcrumb (structured data). For other markup, copy the component in the Template app and change the copy; leave the shipped one as it is, other templates use it.

**Scripts on demand.** Components only mark elements. A script in the page acts on them:

| Marker | Script | Effect |
|--------|--------|--------|
| `A.thumbnail` with `data-size`, which `<CMS:thumbnail class="… thumbnail"/>` outputs (galleries, image components) | `3rdparty/photoswipe-5.4.4/` | lightbox with navigation through all zoom images of the page |
| `[data-parallax-distance]` (the Parallax components) | `3rdparty/parallax/` | parallax shift; needs WebGL |

The component _ Script loader (`F5P2O2n5`) loads each script only when the page holds its marker. Place it once per page template as a fixed slot, at the end of `BODY`: `<CMS:template default="<script loader component>" edit="off"/>`. Without it a zoom link opens the bare image file and a parallax wrapper stays static.

## Fluid-Responsive Stylesheet

The design scales with `--cms-factor`: in the shipped design one factor is 1 px up to a 1600 px wide viewport and grows linearly above it, 1.6 px at 2560 px ([CSS guideline](<pwnc:SKILLS/CSS guideline/SKILLS.md>), section Scaling). A page template written in pixels stops at 1600 px: on a large screen the layout is a small island in the middle and the text is tiny.

- **Every size is factors.** Font sizes, padding, margins, gaps, widths, heights, radii, offsets, shadow sizes: `calc(N * var(--cms-factor))`. A literal `px` value outside a media query is a defect, except a 1 px hairline.
- **Content width:** `WIDTH: min(100%, 1280 * var(--cms-factor))` with `MARGIN: 0 auto`. It caps the column on ordinary screens, grows on large ones and never overflows narrow ones.
- **Type from tokens:** `--cms-font-size`, `--cms-h1-font-size` … are already factor-based. Use them instead of new literal sizes.
- **Spacing from tokens:** `--cms-hspacing` and `--cms-vspacing` shrink on narrow screens by themselves.
- **Narrow screens are media-query work.** Below 1600 px the factor is fixed, so phones need `@media` rules with literal pixel breakpoints (`var()` is not allowed there). Components switch at `max-width: 640px`. The frame may switch its navigation earlier (the Simple design uses 800 px); keep content rules on the components' breakpoint.
- **Images** grow with the screen: give `<CMS:image>` a `width` large enough for big screens (1920 or more for full-width banners). The `srcset` serves smaller versions to smaller screens. `sizes` defaults to `100vw`; an image that fills only part of the width sets `sizes`, or the browser loads a larger version than it needs ([Template manual](<pwnc:SKILLS/Template manual/SKILLS.md>)).
- **Full-bleed frame:** the design gives `BODY` a margin of `--cms-vspacing --cms-hspacing`. A frame with edge-to-edge header or bands sets `BODY { MARGIN: 0; }` and pads its inner containers.

## Editing Mode

Editors work in the same frame, with the editing toolbar from `<CMS:control/>` and wrapper elements around every editable element.

- Fixed or sticky headers, scroll snapping, animations and overlays get in the way of editing. Switch them off for editors: put an edit-only `<style>` into the head with `<CMS:edit>`, or switch classes with `<CMS:edit>` / `<CMS:noedit>`.
- Tracking and consent banners belong in `<CMS:noedit>`.
- Judge the visitor view in the Output view (`content_option=0`) with `view: raw`; judge editability in the Full view ([Content editing](<pwnc:SKILLS/Content editing/SKILLS.md>), section Editing Modes).

## Workflow for a New Design

1. **Inventory.** Go through the design part by part and assign each part a source from the table in Principle. Anything still marked "hard-coded" needs a reason.
2. **Navigation tree.** Create the main pages and any further branch a menu needs. Reuse the hidden `_footer` for footer links, with `directory://` nodes for pages that also stand elsewhere; create a hidden container only if it is missing. Note the node ids ([Site structure](<pwnc:SKILLS/Site structure/SKILLS.md>)).
3. **Placeholders.** Fill the Info placeholders (`sitename`, `company`, `address`, `phone`, `link_phone`, `email`, `copyright`); create `l_*` placeholders for fixed interface texts, reusing existing ones.
4. **Design folder.** Copy `design/simple/` to `design/<name>/`, change the tokens (colours, fonts, spacing), put the design's own fonts, images and scripts there and point `data/stylesheet.css` at it ([CSS guideline](<pwnc:SKILLS/CSS guideline/SKILLS.md>), section Design package). Keep `--cms-factor`.
5. **Components.** Reuse shipped components (section Shipped Parts); build the missing blocks as self-contained components.
6. **Page template.** Build the frame from the anatomy above. Its stylesheet styles only the frame, in factors; the _ Script loader loads the scripts the components need.
7. **Assign and fill.** Assign the page template to the pages ([Site structure](<pwnc:SKILLS/Site structure/SKILLS.md>)); fill `tp_content` with components ([Layout](<pwnc:SKILLS/Layout/SKILLS.md>)).
8. **Verify** with the checklist below.

## Checklist

| Check | How |
|-------|-----|
| Scales above 1600 px | No literal `px` sizes outside media queries in the page template's and the components' CSS; content width uses `min(100%, N * var(--cms-factor))` |
| Works on phones | Read the `@media` rules of the frame's and the components' CSS: the frame has narrow-screen rules, components switch at 640 px |
| Link lists follow the tree | Every navigation, footer and service list is a `<CMS:menu>`; add a test page to the branch and see it appear |
| Blocks are replaceable | No teaser or block markup in the frame; recurring blocks are components in slots |
| Site data is central | Company, address, phone and e-mail appear only as placeholders |
| Empty areas vanish | An empty optional slot leaves no markup in the Output view |
| Defaults show | Slots with a default have no `cblock` on themselves; a new page shows the default block |
| Template switch keeps content | Main slot `tp_content` and shared frame slots carry the same IDs in all page templates |
| Editors can work | The Full view shows every slot; fixed headers and animations are off while editing |
| Effects work | A zoom image opens in the lightbox, a Parallax component moves; the page template holds the _ Script loader |

## Traps

| Symptom | Cause | Fix |
|---------|-------|-----|
| Layout and text stay small on large screens | sizes in `px` instead of factors | `calc(N * var(--cms-factor))` |
| A new page is missing from the footer | footer links written by hand | `<CMS:menu base="<node id>" show-hidden="on"/>` |
| Footer repeats the main navigation | `base` is hidden and the menu has no `show-hidden`, or the `base` id is wrong | add `show-hidden="on"`, check the id |
| A page is wanted in the main menu and the footer | one node can stand in one place only | a second node with `directory://<id>` in the footer branch |
| Empty box where a section menu would be | menu without entries on this page, `NAV` stays | `NAV:not(:has(LI)) { DISPLAY: none; }` |
| Legal pages appear in the main menu | footer container visible | hide it, or `exclude` it in the main menu |
| A teaser cannot be changed on one page | block markup in the frame | component in a slot with a default |
| A block set in the frame shows on one page only | editable element in the frame filled on one page | site-wide content via placeholder, menu or default |
| Default block missing on new pages | `cblock` on the slot that has the default | remove the condition, or condition on a switch |
| Content gone after switching the page template | main slot IDs differ between page templates | same ID (`tp_content`) everywhere |
| Content in the wrong frame slot after a template edit | auto-IDs in the frame | explicit IDs |
| Second instance of a component looks broken | `#id` selectors or fixed HTML ids in the component | root-class selectors, `$cms_template_temp_id` |
| Login and chat pages in another design | `stylesheet` set on `<CMS:head/>` | leave it out; switch designs in `data/stylesheet.css` |
| Visitors cannot zoom | `maximum-scale=1` or `user-scalable=no` | `width=device-width, initial-scale=1` |
| A zoom image opens as a bare file; Parallax is static | the page template has no _ Script loader | add it as a fixed slot (section Shipped Parts) |
| A design image or font from the page template's CSS does not load | `url()` written relative to the site root | relative to `data/template/stylesheet/`: `../../../design/<name>/…` |
| A language switcher built by hand misses the page in the other language | links to the home page per language | Language links or Language codes: they link the same page |

---

[PWNC Website](https://pwnc.it)