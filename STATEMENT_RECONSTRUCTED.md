# Reconstructed problem statement

> **Not the original document.** No problem-statement file (PDF, DOC, …) exists in this
> repository or anywhere in its git history. This page is a reverse-engineered reconstruction
> assembled from `README.md` and from the four implementations (`c/`, `cpp/`, `rust/`,
> `python/`), which agree with each other on every rule except where explicitly noted.
>
> Statements marked *(derived)* are consequences of the constants in the code, not quotes from
> a spec. Section [Open questions](#open-questions-what-the-original-document-must-have-said)
> lists what the code cannot tell us.

---

## 1. Task

Given a square image encoding a **heightmap (terrain)**, find a traversable route from the
**top-left corner to the bottom-right corner** such that the terrain never gets steeper than a
fixed maximum incline; among legal routes, prefer the cheapest one according to a given cost
function. Print the route to the console and render it back onto the image.

---

## 2. Input — the map

| Property | Value | Evidence |
|---|---|---|
| Format | image, **must be exactly square** (enforced, otherwise abort) | `--map`; `pixels.width() != pixels.height()` check |
| Default size | **100 × 100 px** (`--mapsize`) | `MAP_SIZE = 100` |
| Channels used | **red channel only**, normalised `h = R / 255` ∈ [0, 1] | `map[i][j] = pixels[i][j].r / 255.f` |
| Vertical scale | `height(h) = h × 3.0` → **3 m at R = 255** (R = 1 ⇒ ≈ 1.2 cm) | `#define height(x) (x * 3.f)`, `fn height(x) { x * 3.0 }` |
| Horizontal scale | `length(d) = d × 0.1` → **0.1 m per pixel** (100 px = 10 m); diagonal step = 0.1·√2 m | `#define length(x) (x * 0.1f)` |
| Supported formats | C/C++: 24-bit BMP; Rust/Python: BMP, PNG (any lossless format via `--format`) | README, `bmp_lib.*`, `image` crate, `pillow` |
| Provided sample | `map_example.bmp` / `map_example.png` — 100×100, red-only, values ≈ 0–48, known traversable | files in each subfolder |

---

## 3. Movement and traversability

- Start `(0, 0)`, destination `(N-1, N-1)`; coordinates are `(i, j)`.
- Moves go to a neighbouring cell: **4-connected** by default, **8-connected** with `-8d`.
- Direction order is right → down → left → up (clockwise); with 8 directions, right →
  down-right → down → … This order also breaks ties toward the bottom-right.
- A transition `p1 → p2` is legal **iff the slope angle between the two cells does not exceed
  `MAX_ANGLE = π/6` = 30°**:

  ```
  atan( height(|h1 - h2|) / length(euclidean(p1, p2)) ) <= π/6
  ```

  where `h = R / 255`.

Consequences of the constants *(derived)*:

- 4-connected: legal only when **|ΔR| ≤ 4** (ΔR = 5 ⇒ 30.46°, illegal).
- 8-connected: legal only when **|ΔR| ≤ 6** (ΔR = 7 ⇒ 30.21°, illegal).
- Maximum climb per orthogonal step ≈ **0.0577 m**; per diagonal step ≈ 0.0817 m.

---

## 4. Objective

- **C++ / Rust / Python:** Dijkstra over the legal graph, per-edge cost

  ```
  cost(p1, p2) = slope_angle(p1, p2) [radians] + euclidean_distance_in_px(p1, p2) / 10
  ```

  Minimise total cost. Since `px / 10 == length(px)`, this is exactly
  **cost = slope angle (rad) + horizontal metres travelled** — i.e. it rewards flat, short
  segments. Rust source comment: *"Dijkstra's using (the absolute value of the angle in
  radians) + (the horizontal distance between the points) / 10.0 as a cost map"*.
- **C:** plain BFS — any shortest-in-steps legal path, **no cost map** (stated in README).

  History: cost was originally `angle` only; commit `f0e394a` ("Changed the cost function to
  something more sensible") added the distance term to Rust, later mirrored in C++.

---

## 5. Output

1. **Console:** `Path size: N`, then one line per step, 1-indexed, start → destination:

   ```
   Path size: 199
   1: (0; 0)
   2: (0; 1)
   ...
   199: (99; 99)
   ```

   (C prints the header lowercase: `path size: N`.)
2. **No route exists:** print `No path avaliable` *(typo is in the source; Python prints
   `No path avaliavle`)*.
3. **Image:** save the map with the route painted by **setting the blue channel to 255** and
   keeping R and G unchanged (route over red terrain renders magenta) — `path.bmp` (C/C++) or
   `path.{format}`, default `path.png` (Rust/Python).

---

## 6. Map generator (required deliverable)

When no `--map` is supplied, the program must synthesise a map and save it as `map.bmp` /
`map.png`:

- for each pixel, with probability `density` (default **0.5**) set
  `R = uniform[0, amplitude]` (`amplitude` default **48**), `G = B = 0`; otherwise black;
- then **Gaussian-blur** the result so the terrain is continuous: C = 5×5 Gaussian kernel
  (`KERNEL_SIZE 5`), C++ = same 5×5 kernel (`Pixels::blur`), Rust = `blur(1.0)`,
  Python = `GaussianBlur(radius=2)`;
- defaults imply R ≤ 48 ⇒ elevations ≤ ≈ 0.56 m.

> README: *"The default map generation settings may not always produce a traversable map. Use
> the included map_example file if you want to be sure it is traversable."*

---

## 7. Deliverables and CLI contract

Four independent implementations — `c/`, `cpp/`, `rust/`, `python/` — each in its own
subfolder with build/run instructions (plus venv setup scripts for Python), all exposing:

```
--help, -h                    print help screen
--map,  -m {file}             load a pre-generated square map
--mapsize, -s {int:100}       map size in px
--mapdens, -d {float:0.50}    generator density
--mapamplitude, -a {int:48}   generator max elevation (0-255)
-8d                           8-directional movement (default: 4)
--format, -f {"bmp"|"png"}    image format for generated map + output  [Rust, Python only]
```

---

## Open questions (what the original document must have said)

1. **Exact wording/units of the scales** — whether 3 m full scale and 0.1 m/px were *given* by
   the task or chosen by the author (the comment `// from color values, to meters` suggests
   given).
2. Whether **30°** was imposed by the task or is an author-chosen parameter — there is no CLI
   flag for it, only the `// 30 degrees` comment.
3. Which algorithm was actually required: "shortest path", "minimum-slope path", or "any legal
   path". The cost function was changed twice, which hints the original spec was vague here.
4. Provenance of `rust/maps/real_map.png` — 70×70, 8-bit RGB, untracked and gitignored
   (`/rust/maps`), tagged `Software: Paint.NET 5.1.2`, 1205 pure-red `(255,0,0)` pixels ⇒ far
   steeper than `map_example`. It is byte-different from `map_example.png` and was almost
   certainly cropped out of the task material — the only surviving fragment of that document.
5. Any hard output-format requirements (exact strings; whether the blue channel specifically
   was mandated for the route overlay).

---

## Source map (where each rule lives)

| Rule | C | C++ | Rust | Python |
|---|---|---|---|---|
| scale constants, 30° limit | `c/main.c` (`MAX_ANGLE`, `height`, `length`, `ok_slope`) | `cpp/utils.cpp` / `utils.hpp` | `rust/src/utils.rs` | `python/utils.py` |
| search + objective | `c/main.c` (BFS + `c/queue.c`) | `cpp/main.cpp` (`cost`, `std::priority_queue`) | `rust/src/main.rs` (`cost`, `BinaryHeap`) | `python/main.py` (`cost`, `heapq`) |
| direction tables, bounds | `c/main.c` (`di4/dj4`, `di8/dj8`) | `cpp/utils.hpp` (`D4`, `D8`) | `rust/src/position.rs` | `python/position.py` |
| generator + blur | `c/main.c` `gen_map`, `c/bmp_lib.c` `blur` | `cpp/main.cpp` `gen_map`, `Pixels::blur` | `rust/src/main.rs` `gen_map` | `python/main.py` `gen_map` |
| image I/O (BMP) | `c/bmp_lib.c` | `cpp/bmp_lib.cpp` | `image` crate | `pillow` |
| CLI parsing | `main.*` / `main.py` `--help` blocks | | | |
