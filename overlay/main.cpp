/*
 * Copyright (c) 2020 Studious Pancake
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
#include "hekate.hpp"

#include <tesla.hpp>

class PancakeGui : public tsl::Gui {
  private:
    Hekate::BootConfigList const config_list;

  public:
    PancakeGui()
        : config_list(Hekate::LoadBootConfigList()) {
    }

    virtual tsl::elm::Element *createUI() override {
        auto frame = new tsl::elm::OverlayFrame("Studious Pancake", "v0.1.0");

        auto list = new tsl::elm::List();

        /* Append boot config entries. */
        list->addItem(new tsl::elm::CategoryHeader("Boot configs"));

        for (auto &config : config_list) {
            auto entry = new tsl::elm::ListItem(config.name);
            entry->setClickListener([&](u64 keys) -> bool { return (keys & KEY_A) && Hekate::RebootToConfig(config); });
            list->addItem(entry);
        }

        /* Miscellaneous. */
        list->addItem(new tsl::elm::CategoryHeader("Miscellaneous"));

        auto ums = new tsl::elm::ListItem("Reboot to UMS");
        ums->setClickListener([](u64 keys) -> bool { return (keys & KEY_A) && Hekate::RebootToUMS(Hekate::UmsTarget_Sd); });
        list->addItem(ums);

        frame->setContent(list);
        return frame;
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) {
        if (keysDown & KEY_MINUS)
            Hekate::RebootDefault();

        return false;
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
        return std::make_unique<PancakeGui>();
    }
};

int main(int argc, char **argv) {
    return tsl::loop<PancakeOverlay>(argc, argv);
}