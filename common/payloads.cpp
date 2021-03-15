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
#include "payloads.hpp"

#include "reboot_to_payload.h"

#include <cstring>

namespace Payloads {

    namespace {

        bool LoadPayload(const char* path) {
            /* Clear payload buffer. */
            std::memset(g_reboot_payload, 0xFF, sizeof(g_reboot_payload));

            /* Open payload. */
            auto file = fopen(path, "r");
            if (file == nullptr)
                return false;

            /* Read payload to buffer. */
            std::size_t ret = fread(g_reboot_payload, 1, sizeof(g_reboot_payload), file);

            /* Close file. */
            fclose(file);

            /* Verify payload loaded successfully. */
            if (ret == 0)
                return false;

            return true;
        }

    }

    bool RebootToPayload(PayloadConfig const &config) {
        /* Load payload. */
        if (!LoadPayload(config.path.c_str()))
            return false;

        /* Reboot */
        reboot_to_payload();

        return true;
    }

    std::vector<PayloadConfig> LoadPayloadList() {
        std::vector<PayloadConfig> res;

        /* Iterate through all the payload folders */
        for (const auto& path : Payloads::PayloadDirs) {
            /* Open Sd card filesystem. */
            FsFileSystem fsSdmc;
            if (R_FAILED(fsOpenSdCardFileSystem(&fsSdmc)))
                continue;

            /* Open Directory */
            FsDir dir;
            if (R_FAILED(fsFsOpenDirectory(&fsSdmc, path, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &dir))) {
                continue;
            }

            /* Get the number of files in the directory */
            s64 entry_count = 0;
            if (R_FAILED(fsDirGetEntryCount(&dir, &entry_count))) {
                continue;
            }

            /* Get the entries from the directory */
            std::vector<FsDirectoryEntry> entries;
            entries.resize(entry_count);
            if (R_FAILED(fsDirRead(&dir, nullptr, static_cast<size_t>(entry_count), entries.data()))) {
                continue;
            }

            /* Select the payloads from the entries */
            for (const auto& entry : entries) {
                std::string name(entry.name);
                if(name.substr(name.size() - 4) == ".bin")
                    res.push_back({name, (path + name)});
            }
            fsDirClose(&dir);
        }
        return res;
    }

}