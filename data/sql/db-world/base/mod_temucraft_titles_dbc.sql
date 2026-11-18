-- mod-temucraft-titles: no new titles in game (decided 2026-11-18). This removes the 37 that were added before;
-- the catalog (mod_temucraft_title_catalog) stays as a list. To add titles: fill PICKS in tools/build_titles.py and run it.
DELETE FROM `chartitles_dbc` WHERE `ID` BETWEEN 190 AND 226;
