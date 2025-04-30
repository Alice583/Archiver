#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

class Parser {
private:
    bool create_archive_ = false;
    bool files_list_ = false;
    bool extract_files_ = false;
    bool append_file_ = false;
    bool delete_file_ = false;
    bool split_archives_ = false;
    std::vector<std::string> files_names{};
    std::string archive_;
public:
    std::string kCreateLong = "--create";
    std::string kCreateShort = "-c";

    std::string kExtractLong = "--extract";
    std::string kExtractShort = "-x";

    std::string kListLong = "--list";
    std::string kListShort = "-l";

    std::string kAppendLong = "--append";
    std::string kAppendShort = "-a";

    std::string kDeleteLong = "--delete";
    std::string kDeleteShort = "-d";

    std::string kConcatenateLong = "--concatenate";
    std::string kConcatenateShort = "-A";

    const char* kFileLong = "--file=";
    std::string kFileShort = "-f";

    bool isCreateArchive() const;

    bool isFilesList() const;

    bool isExtractFiles() const;

    bool isAppendFile() const;

    bool isDeleteFile() const;

    bool isSplitArchives() const;

    const std::vector<std::string>& getFilesNames() const;

    const std::string& getArchive() const;

    void Parse(int argc, const char** argv);

    void Parse(const std::string& commands);

    std::vector<std::string> SplitString(const std::string& string);
};
