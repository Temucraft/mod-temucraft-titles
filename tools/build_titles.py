#!/usr/bin/env python3
"""Builds mod-temucraft-titles' data from tools/wow_titles.csv:
- data/csv/titles.csv: every title (id, name, how it was earned)
- data/sql/db-world/base/mod_temucraft_titles_catalog.sql: the catalog table (all titles, which are in game)
- data/sql/db-world/base/mod_temucraft_titles_dbc.sql: chartitles_dbc rows for the titles in game (server)
- with --client: the same rows into the client's CharTitles.dbc (patch-T.MPQ)
The client and server know 192 title bits (3.3.5 limit); 155-191 are free, so 37 titles can be in game at once:
the PICKS list below (edit it and run again to change them)."""
import csv, os, sys, re
sys.path.insert(0, os.path.expanduser('~/tc'))
import dbctool as T

HERE = os.path.dirname(os.path.abspath(__file__))
MOD = os.path.dirname(HERE)
FIRST_ID, FIRST_BIT, LAST_BIT = 190, 155, 191

# (csv Title as written, male name, female name); %s = the character's name
PICKS = []   # no new titles in game for now (2026-11-18); the 37 tried before are in git history
assert len(PICKS) <= LAST_BIT - FIRST_BIT + 1

rows = list(csv.DictReader(open(HERE + '/wow_titles.csv', encoding='utf-8-sig')))
by_title = {r['Title']: r for r in rows}
for p in PICKS:
    assert p[0] in by_title, p[0]

def q(t):
    return "'" + (t or '').replace('\\', '\\\\').replace("'", "''") + "'"

# CSV for people
with open(MOD + '/data/csv/titles.csv', 'w', newline='', encoding='utf-8') as f:
    w = csv.writer(f)
    w.writerow(['Title ID', 'Title', 'Original unlock', 'Achievement', 'Expansion', 'In game (CharTitles id)'])
    in_game = {p[0]: FIRST_ID + i for i, p in enumerate(PICKS)}
    for r in rows:
        w.writerow([r['Title ID'], r['Title'], r['How to Unlock'], r['Achievement Name'], r['Expansion Added'], in_game.get(r['Title'], '')])

# catalog table
sql = ["-- mod-temucraft-titles: every title from tools/wow_titles.csv, with how it was originally earned; `in_game` is",
       "-- the CharTitles id when the title exists on this server (give it with .temutitle give or .titles add).",
       "CREATE TABLE IF NOT EXISTS `mod_temucraft_title_catalog` (",
       "    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT, `retail_id` INT UNSIGNED NOT NULL DEFAULT 0, `title` VARCHAR(255) NOT NULL,",
       "    `achievement` VARCHAR(255) NOT NULL DEFAULT '', `unlock` TEXT, `expansion` VARCHAR(64) NOT NULL DEFAULT '',",
       "    `faction` VARCHAR(32) NOT NULL DEFAULT '', `source` VARCHAR(64) NOT NULL DEFAULT '', `in_game` INT UNSIGNED NOT NULL DEFAULT 0,",
       "    PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;",
       "DELETE FROM `mod_temucraft_title_catalog`;",
       "INSERT INTO `mod_temucraft_title_catalog` (`retail_id`, `title`, `achievement`, `unlock`, `expansion`, `faction`, `source`, `in_game`) VALUES"]
vals = []
for r in rows:
    rid = re.sub(r'\D', '', r['Title ID'] or '') or '0'
    vals.append('(%s, %s, %s, %s, %s, %s, %s, %d)' % (rid, q(r['Title']), q(r['Achievement Name']), q(r['How to Unlock']),
                q(r['Expansion Added']), q(r['Faction']), q(r['Source Type']), in_game.get(r['Title'], 0)))
open(MOD + '/data/sql/db-world/base/mod_temucraft_titles_catalog.sql', 'w', encoding='utf-8').write('\n'.join(sql) + '\n' + ',\n'.join(vals) + ';\n')

def build(dbc):
    template = dbc.rows[189]
    new = []
    for i, (src, male, female) in enumerate(PICKS):
        r = [0] * dbc.fc
        r[0] = FIRST_ID + i
        r[2] = dbc.add(male)
        r[18] = template[18]
        r[19] = dbc.add(female or male)
        r[35] = template[35]
        r[36] = FIRST_BIT + i
        dbc.put(r)
        new.append(r)
    return new

server = T.DBC(T.SERVER_DBC + '/CharTitles.dbc')
new = build(server)
open(MOD + '/data/sql/db-world/base/mod_temucraft_titles_dbc.sql', 'w', encoding='utf-8').write(
    '-- mod-temucraft-titles: the titles in game, for the server (the client has the same rows in CharTitles.dbc).\n' +
    T.sql_insert('chartitles_dbc', new, server))

if '--client' in sys.argv:
    tmp = '/tmp/CharTitles_client.dbc'
    assert T.client_get('DBFilesClient\\CharTitles.dbc', tmp)
    client = T.DBC(tmp)
    build(client)
    client.save(tmp)
    T.client_put({'DBFilesClient\\CharTitles.dbc': tmp})
    print('client CharTitles.dbc updated')
print(len(rows), 'titles in the catalog,', len(PICKS), 'in game')
