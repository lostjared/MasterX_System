#ifndef CFG_H_X 
#define CFG_H_X

#include<iostream>
#include<fstream>
#include<string>
#include<unordered_map>
namespace mx {

    class Item {
    public:
        std::string key, value;
    };

    class ConfigFile {
        std::unordered_map<std::string, std::unordered_map<std::string, Item>> values;
        void loadFIle(const std::string  &f);
        void saveFile(const std::string &f2);

    public:
        explicit ConfigFile(const std::string &filex);
       ~ConfigFile();


    };
}

#endif

















