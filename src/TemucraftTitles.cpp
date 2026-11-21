/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

// Titles sandbox: titles from every expansion, with no requirements, for game masters to hand out.
// - The catalog (mod_temucraft_title_catalog) lists every title and how it was originally earned.
// - The titles in game (CharTitles ids 190+, chartitles_dbc + the client's CharTitles.dbc) can be given and taken:
//   .temucraft temutitle find <text>, .temucraft temutitle give <id or name> (to the selected player), .temucraft temutitle take <id or name>.

#include "Chat.h"
#include "CommandScript.h"
#include "DBCStores.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "StringConvert.h"
#include <algorithm>
#include <vector>

using namespace Acore::ChatCommands;

namespace
{
    struct Entry { std::string title, unlock, expansion; uint32 inGame = 0; };
    std::vector<Entry> catalog;

    void Load()
    {
        catalog.clear();
        if (QueryResult r = WorldDatabase.Query("SELECT `title`, `unlock`, `expansion`, `in_game` FROM `mod_temucraft_title_catalog` ORDER BY `in_game` DESC, `title`"))
            do
            {
                Field* f = r->Fetch();
                catalog.push_back({ f[0].Get<std::string>(), f[1].Get<std::string>(), f[2].Get<std::string>(), f[3].Get<uint32>() });
            } while (r->NextRow());
    }

    std::string Lower(std::string s) { std::transform(s.begin(), s.end(), s.begin(), ::tolower); return s; }

    // a title in game, by CharTitles id or (part of) its name
    CharTitlesEntry const* Find(std::string const& text, Player* who)
    {
        if (Optional<uint32> id = Acore::StringTo<uint32>(text))
            return sCharTitlesStore.LookupEntry(*id);
        std::string const want = Lower(text);
        for (Entry const& e : catalog)
            if (e.inGame && Lower(e.title).find(want) != std::string::npos)
                return sCharTitlesStore.LookupEntry(e.inGame);
        for (uint32 i = 0; i < sCharTitlesStore.GetNumRows(); ++i)
            if (CharTitlesEntry const* t = sCharTitlesStore.LookupEntry(i))
                if (Lower(t->nameMale[who ? who->GetSession()->GetSessionDbcLocale() : LOCALE_enUS]).find(want) != std::string::npos)
                    return t;
        return nullptr;
    }
}

class TemucraftTitlesWorld : public WorldScript
{
public:
    TemucraftTitlesWorld() : WorldScript("TemucraftTitlesWorld", { WORLDHOOK_ON_STARTUP }) { }
    void OnStartup() override { Load(); }
};

class TemucraftTitlesCommands : public CommandScript
{
public:
    TemucraftTitlesCommands() : CommandScript("TemucraftTitlesCommands") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable sub =
        {
            { "find",   HandleFind,   101401 /* SEC_GAMEMASTER -> RBAC */, Console::Yes },
            { "give",   HandleGive,   101402 /* SEC_GAMEMASTER -> RBAC */, Console::No },
            { "take",   HandleTake,   101403 /* SEC_GAMEMASTER -> RBAC */, Console::No },
            { "reload", HandleReload, 101404 /* SEC_ADMINISTRATOR -> RBAC */, Console::Yes },
        };
        static ChatCommandTable table = { { "temucraft temutitle", sub } };
        return table;
    }

    static bool HandleFind(ChatHandler* handler, Tail text)
    {
        std::string const want = Lower(std::string(text));
        uint32 shown = 0;
        for (Entry const& e : catalog)
        {
            if (!want.empty() && Lower(e.title).find(want) == std::string::npos)
                continue;
            if (e.inGame)
                handler->PSendSysMessage("|cff00ff00[{}]|r {} ({}) - {}", e.inGame, e.title, e.expansion, e.unlock.substr(0, 120));
            else
                handler->PSendSysMessage("|cff999999[not in game]|r {} ({}) - {}", e.title, e.expansion, e.unlock.substr(0, 120));
            if (++shown >= 30)
            {
                handler->SendSysMessage("... (narrow the search)");
                break;
            }
        }
        if (!shown)
            handler->SendSysMessage("No title matches.");
        return true;
    }

    static bool GiveOrTake(ChatHandler* handler, std::string const& text, bool give)
    {
        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
            return false;
        CharTitlesEntry const* title = Find(text, handler->GetPlayer());
        if (!title)
        {
            handler->SendSysMessage("That title isn't in game (.temucraft temutitle find lists them; green ones can be given).");
            return true;
        }
        target->SetTitle(title, !give);
        if (!give && target->GetUInt32Value(PLAYER_CHOSEN_TITLE) == title->bit_index)
            target->SetUInt32Value(PLAYER_CHOSEN_TITLE, 0);
        handler->PSendSysMessage("{} title {} for {}.", give ? "Gave" : "Took", title->ID, target->GetName());
        return true;
    }

    static bool HandleGive(ChatHandler* handler, Tail text) { return GiveOrTake(handler, std::string(text), true); }
    static bool HandleTake(ChatHandler* handler, Tail text) { return GiveOrTake(handler, std::string(text), false); }
    static bool HandleReload(ChatHandler* handler) { Load(); handler->SendSysMessage("Title catalog reloaded."); return true; }
};

void AddTemucraftTitlesScripts()
{
    new TemucraftTitlesWorld();
    new TemucraftTitlesCommands();
}
