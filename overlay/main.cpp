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
#include <payload.hpp>
#include <util.hpp>

#include <tesla.hpp>

namespace {

    constexpr const char AppTitle[] = APP_TITLE;
    constexpr const char AppVersion[] = APP_VERSION;

}

class PancakeGui : public tsl::Gui {
  private:
    Payload::HekateConfigList const boot_config_list;
    Payload::HekateConfigList const ini_config_list;
    Payload::PayloadConfigList const payload_config_list;

  public:
    PancakeGui()
        : boot_config_list(Payload::LoadHekateConfigList()),
          ini_config_list(Payload::LoadIniConfigList()),
          payload_config_list(Payload::LoadPayloadList()) {
    }

    virtual tsl::elm::Element *createUI() override {
        auto const frame = new tsl::elm::OverlayFrame(AppTitle, AppVersion);

        auto const list = new tsl::elm::List();

        /* Append boot config entries. */
        if (!boot_config_list.empty()) {
            list->addItem(new tsl::elm::CategoryHeader("Boot configs"));

            for (auto const &config : boot_config_list) {
                auto const entry = new tsl::elm::ListItem(config.name);
                entry->setClickListener([&](u64 const keys) -> bool { return (keys & HidNpadButton_A) && Payload::RebootToHekateConfig(config, false); });
                list->addItem(entry);
            }
        }

        /* Append ini config entries. */
        if (!ini_config_list.empty()) {
            list->addItem(new tsl::elm::CategoryHeader("Ini configs"));

            for (auto const &config : ini_config_list) {
                auto const entry = new tsl::elm::ListItem(config.name);
                entry->setClickListener([&](u64 const keys) -> bool { return (keys & HidNpadButton_A) && Payload::RebootToHekateConfig(config, true); });
                list->addItem(entry);
            }
        }

        /* Miscellaneous. */
        list->addItem(new tsl::elm::CategoryHeader("Miscellaneous"));

        auto const ums = new tsl::elm::ListItem("Reboot to UMS");
        ums->setClickListener([](u64 const keys) -> bool { return (keys & HidNpadButton_A) && Payload::RebootToHekateUMS(Payload::UmsTarget_Sd); });
        list->addItem(ums);

        /* Payloads */
        if (!payload_config_list.empty()) {
            list->addItem(new tsl::elm::CategoryHeader("Payloads"));

            for (auto const &config : payload_config_list) {
                auto const entry = new tsl::elm::ListItem(config.name);
                entry->setClickListener([&](u64 const keys) -> bool { return (keys & HidNpadButton_A) && Payload::RebootToPayload(config); });
                list->addItem(entry);
            }
        }

        frame->setContent(list);
        return frame;
    }

    virtual bool handleInput(u64 const keysDown, u64 const keysHeld, const HidTouchState &touchPos, HidAnalogStickState const joyStickPosLeft, HidAnalogStickState const joyStickPosRight) {
        if (keysDown & HidNpadButton_Minus)
            Payload::RebootToHekate();

        (void)keysHeld;
        (void)touchPos;
        (void)joyStickPosLeft;
        (void)joyStickPosRight;

        return false;
    }
};

class MarikoMenu final : public tsl::Gui {
  public:
    virtual tsl::elm::Element *createUI() override {
        auto const frame = new tsl::elm::OverlayFrame(AppTitle, AppVersion);

        /* Display incompatibility error */
        auto const drawer = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer* const r, s32 const x, s32 const y, s32 const w, s32 const h) {
            r->drawString("\uE150", false, x + (w / 2) - (90 / 2), 300, 90, 0xffff);
            r->drawString("Mariko consoles unsupported", false, x, 380, 25, 0xffff);
        });

        frame->setContent(drawer);
        return frame;
    }
};

class PancakeOverlay final : public tsl::Overlay {
  public:
    virtual void initServices() override {
        fsdevMountSdmc();
        splInitialize();
        spsmInitialize();
        amsBpcInitialize();
    }

    virtual void exitServices() override {
        amsBpcExit();
        spsmExit();
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
