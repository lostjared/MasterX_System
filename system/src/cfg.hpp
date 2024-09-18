#ifndef CFG_H_X 
#define CFG_H_X

#include<iostream>
#include<fstream>
#include<string>
#include<unordered_map>
#include<vector>

namespace mx {

    class Item {
    public:
        std::string key, value;
    };

    class ConfigFile {
        std::unordered_map<std::string, std::unordered_map<std::string, Item>> values;
        std::string filename;
        void loadFIle(const std::string  &f);
        void saveFile(const std::string &f2);
        Item itemAtKey(const std::string &section, const std::string &key);
        void setItem(const std::string &section, const std::string &key, const std::string value);

    public:
        explicit ConfigFile(const std::string &filex);
       ~ConfigFile();


    };
}

#endif

















