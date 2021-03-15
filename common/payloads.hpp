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
#pragma once

#include <string>
#include <switch.h>
#include <vector>

namespace Payloads {

    constexpr char const *const PayloadDirs[] = {
        "/",
        "/bootloader/payloads/",
        "/payloads/",
    };

    struct PayloadConfig {
        std::string name;
        std::string path;
    };

    using PayloadConfigVector = std::vector<PayloadConfig>;

    bool RebootToPayload(PayloadConfig const &config);
    std::vector<PayloadConfig> LoadPayloadList();

}
