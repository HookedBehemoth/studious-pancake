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
#define TESLA_INIT_IMPL
#include <hekate.hpp>
#include <util.hpp>

#include <tesla.hpp>

namespace {

    constexpr const char AppTitle[] = "Studius Pancake";
    constexpr const char AppVersion[] = "0.2.0";

}

class PancakeGui : public tsl::Gui {
  private:
    Hekate::BootConfigList const boot_config_list;
    Hekate::BootConfigList const ini_config_list;

  public:
    PancakeGui()
        : boot_config_list(Hekate::LoadBootConfigList()),
          ini_config_list(Hekate::LoadIniConfigList()) {
    }

    virtual tsl::elm::Element *createUI() override {
        auto frame = new tsl::elm::OverlayFrame(AppTitle, AppVersion);

        auto list = new tsl::elm::List();

        /* Append boot config entries. */
        if (!boot_config_list.empty()) {
            list->addItem(new tsl::elm::CategoryHeader("Boot configs"));

            for (auto &config : boot_config_list) {
                auto entry = new tsl::elm::ListItem(config.name);
                entry->setClickListener([&](u64 keys) -> bool { return (keys & HidNpadButton_A) && Hekate::RebootToConfig(config, false); });
                list->addItem(entry);
            }
        }

        /* Append ini config entries. */
        if (!ini_config_list.empty()) {
            list->addItem(new tsl::elm::CategoryHeader("Ini configs"));

            for (auto &config : ini_config_list) {
                auto entry = new tsl::elm::ListItem(config.name);
                entry->setClickListener([&](u64 keys) -> bool { return (keys & HidNpadButton_A) && Hekate::RebootToConfig(config, true); });
                list->addItem(entry);
            }
        }

        /* Miscellaneous. */
        list->addItem(new tsl::elm::CategoryHeader("Miscellaneous"));

        auto ums = new tsl::elm::ListItem("Reboot to UMS");
        ums->setClickListener([](u64 keys) -> bool { return (keys & HidNpadButton_A) && Hekate::RebootToUMS(Hekate::UmsTarget_Sd); });
        list->addItem(ums);

        frame->setContent(list);
        return frame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) {
        if (keysDown & HidNpadButton_Minus)
            Hekate::RebootDefault();

        return false;
    }
};

class MarikoMenu : public tsl::Gui {
  public:
    virtual tsl::elm::Element *createUI() override {
        auto frame = new tsl::elm::OverlayFrame(AppTitle, AppVersion);

        /* Display incompatibility error */
        auto drawer = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer* r, s32 x, s32 y, s32 w, s32 h) {
            r->drawString("\uE150", false, x + (w / 2) - (90 / 2), 300, 90, 0xffff);
            r->drawString("Mariko consoles unsupported", false, x, 380, 25, 0xffff);
        });

        frame->setContent(drawer);
        return frame;
    }
};

class PancakeOverlay : public tsl::Overlay {
  public:
    virtual void initServices() override {
        fsdevMountSdmc();
        splInitialize();
    }

    virtual void exitServices() override {
        splExit();
        fsdevUnmountAll();
    }

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        if (util::IsErista()) {
            return std::make_unique<PancakeGui>();
        } else {
            return std::make_unique<MarikoMenu>();
        }
    }
};

int main(int argc, char **argv) {
    return tsl::loop<PancakeOverlay>(argc, argv);
}