#pragma once

#include <filesystem>
#include <vector>

void SelectFile(const std::filesystem::path &directory,
                std::filesystem::path &current,
                const std::vector<std::string> &filter);

void SelectFolder(const std::filesystem::path &directory,
                  std::filesystem::path &current);

bool Filter(const std::filesystem::path &entry,
            const std::vector<std::string> &filter);
