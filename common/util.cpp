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
#include "util.hpp"

#include <switch.h>

namespace util {

    bool IsErista() {
        u64 type=0;

        if (R_FAILED(splGetConfig(SplConfigItem_HardwareType, &type)))
            return false;

        return type == 0 /* SplHardwareType_Icosa */ || type == 1 /* SplHardwareType_Copper */;
    }

}
