/*
 * Copyright (c) 2020-2021 Studious Pancake
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <payload.hpp>
#include <util.hpp>

#include <cstdio>
#include <cstdlib>
#include <switch.h>
#include <vector>

namespace {

    typedef void (*TuiCallback)(void *user);

    struct TuiItem {
        std::string text;
        TuiCallback cb;
        void *user;
        bool selectable;
    };

    void BootConfigCallback(void *user) {
        auto config = reinterpret_cast<Payload::BootConfig *>(user);

        Payload::RebootToConfig(*config, false);
    }

    void IniConfigCallback(void *user) {
        auto config = reinterpret_cast<Payload::BootConfig *>(user);

        Payload::RebootToConfig(*config, true);
    }

    void UmsCallback(void *user) {
        Payload::RebootToUMS(Payload::UmsTarget_Sd);

        (void)user;
    }

    void PayloadCallback(void *user) {
        auto config = reinterpret_cast<Payload::PayloadConfig *>(user);

        Payload::RebootToPayload(*config);
    }
}

extern "C" void userAppInit(void) {
    splInitialize();
}

extern "C" void userAppExit(void) {
    splExit();
}

int main(int argc, char **argv) {
    std::vector<TuiItem> items;

    /* Load available boot configs */
    auto boot_config_list = Payload::LoadBootConfigList();

    /* Load available ini configs */
    auto ini_config_list = Payload::LoadIniConfigList();

    /* Load available payloads */
    auto payload_config_list = Payload::LoadPayloadList();

    /* Build menu item list */
    if (util::IsErista()) {
        items.reserve(3 + boot_config_list.empty() ? 0 : 1 + boot_config_list.size()
                        + ini_config_list.empty()  ? 0 : 1 + ini_config_list.size()
                        + payload_config_list.empty()  ? 0 : 1 + payload_config_list.size());

        if (!boot_config_list.empty()) {
            items.emplace_back("Boot Configs", nullptr, nullptr, false);
            for (auto &entry : boot_config_list)
                items.emplace_back(entry.name, BootConfigCallback, &entry, true);
        }

        if (!ini_config_list.empty()) {
            items.emplace_back("Ini Configs", nullptr, nullptr, false);
            for (auto &entry : ini_config_list)
                items.emplace_back(entry.name, IniConfigCallback, &entry, true);
        }

        items.emplace_back("Miscellaneous", nullptr, nullptr, false);
        items.emplace_back("Reboot to UMS", UmsCallback, nullptr, true);

        if (!payload_config_list.empty()) {
            items.emplace_back("Payloads", nullptr, nullptr, false);
            for (auto &entry : payload_config_list)
                items.emplace_back(entry.name, PayloadCallback, &entry, true);
        }

    } else {
        items.reserve(2);

        items.emplace_back("Mariko consoles unsupported", nullptr, nullptr, false);
    }

    items.emplace_back("Exit", nullptr, nullptr, true);

    std::size_t index = 0;

    for (auto &item : items) {
        if (item.selectable)
            break;

        index++;
    }

    PrintConsole *console = consoleInit(nullptr);

    /* Configure input */
    padConfigureInput(8, HidNpadStyleSet_NpadStandard);

    /* Initialize pad */
    PadState pad;
    padInitializeAny(&pad);

    while (appletMainLoop()) {
        {
            /* Update padstate */
            padUpdate(&pad);

            u64 kDown = padGetButtonsDown(&pad);

            if ((kDown & (HidNpadButton_Plus | HidNpadButton_B | HidNpadButton_L)))
                break;

            if ((kDown & HidNpadButton_A)) {
                auto &item = items[index];

                if (item.selectable && item.cb)
                    item.cb(item.user);

                break;
            }

            if ((kDown & HidNpadButton_Minus)) {
                Payload::RebootDefault();
            }

            if ((kDown & HidNpadButton_AnyDown) && (index + 1) < items.size()) {
                for (std::size_t i = index; i < items.size(); i++) {
                    if (!items[i + 1].selectable)
                        continue;

                    index = i + 1;
                    break;
                }
            }

            if ((kDown & HidNpadButton_AnyUp) && index > 0) {
                for (std::size_t i = index; i > 0; i--) {
                    if (!items[i - 1].selectable)
                        continue;

                    index = i - 1;
                    break;
                }
            }
        }

        consoleClear();

        printf("Studious Pancake\n----------------\n");

        for (std::size_t i = 0; i < items.size(); i++) {
            auto &item    = items[i];
            bool selected = (i == index);

            if (!item.selectable)
                console->flags |= CONSOLE_COLOR_FAINT;

            printf("%c %s\n", selected ? '>' : ' ', item.text.c_str());

            if (!item.selectable)
                console->flags &= ~CONSOLE_COLOR_FAINT;
        }

        consoleUpdate(nullptr);
    }

    consoleExit(nullptr);

    (void)argc;
    (void)argv;

    return EXIT_SUCCESS;
}
