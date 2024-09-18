#include"cfg.hpp"
#include <algorithm>

namespace mx {
    void ConfigFile::loadFIle(const std::string &f) {
        std::ifstream in(f);
        if (!in.is_open()) {
            return;
        }

        std::string line, currentSection;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue;
            }

            if (line.front() == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }

            size_t pos = line.find('=');
            if (pos != std::string::npos && !currentSection.empty()) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);

                Item item;
                item.key = key;
                item.value = value;

                values[currentSection][key] = item;
            }
        }

        in.close();
    }
    void ConfigFile::saveFile(const std::string &f2) {
        std::ofstream out(f2);
        if (!out.is_open()) {
            return;
        }

        std::vector<std::string> sectionNames;
        for (const auto &section : values) {
            sectionNames.push_back(section.first);
        }
        std::sort(sectionNames.begin(), sectionNames.end());

        for (const auto &section : sectionNames) {
            out << "[" << section << "]\n";

            std::vector<std::string> keys;
            for (const auto &pair : values[section]) {
                keys.push_back(pair.first);
            }
            std::sort(keys.begin(), keys.end());

            for (const auto &key : keys) {
                out << key << "=" << values[section][key].value << "\n";
            }

            out << "\n";
        }

        out.close();
    }

    Item ConfigFile::itemAtKey(const std::string &section, const std::string &key) {
        return values[section][key];
    }
        
    void ConfigFile::setItem(const std::string &section, const std::string &key, const std::string value) {
        values[section][key] = {key, value};
    }


    ConfigFile::ConfigFile(const std::string &filex) {
        loadFIle(filex);
    }

    ConfigFile::~ConfigFile() {
        saveFile(filename);
    }

}








