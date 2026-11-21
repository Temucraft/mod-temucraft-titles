# mod-temucraft-titles

A titles sandbox: titles from every expansion, with no requirements, for game masters to pick and hand out.

- **The catalog**: all 931 titles from `tools/wow_titles.csv`, with how each was originally earned, in
  `mod_temucraft_title_catalog` (world) and as a sheet in `data/csv/titles.csv` (id, title, original unlock,
  achievement, expansion, CharTitles id when in game).
- **In game**: none for now (decided 2026-11-18). Up to 37 could exist at once (CharTitles 190-226: the 3.3.5 client and
  server know 192 title bits and 155 are taken). To add some: fill `PICKS` in `tools/build_titles.py`, run it, and run
  it with `--client` (game closed) for the client's CharTitles.dbc. `mod_temucraft_titles_dbc.sql` now only removes
  the 37 that were tried before.
- **Commands** (game masters): `.temucraft temutitle find <text>` (green = in game, with its id), `.temucraft temutitle give <id or name>`
  and `.temucraft temutitle take <id or name>` on the selected player, `.temucraft temutitle reload`. AzerothCore's `.titles add <id>` works
  too.


## Install

Copy the module into `modules/`, re-run CMake and build. World SQL: the catalog, and the removal of the earlier
`chartitles_dbc` rows. The client's CharTitles.dbc is back to the original.
