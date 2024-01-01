#include "Parser.h"
#include <iostream>
#include <sstream>
#include <iterator>


bool Parser::isCreateArchive() const {
    return create_archive_;
}

bool Parser::isFilesList() const {
    return files_list_;
}

bool Parser::isExtractFiles() const {
    return extract_files_;
}

bool Parser::isAppendFile() const {
    return append_file_;
}

bool Parser::isDeleteFile() const {
    return delete_file_;
}

bool Parser::isSplitArchives() const {
    return split_archives_;
}

const std::vector<std::string>& Parser::getFilesNames() const {
    return files_names;
}

const std::string& Parser::getArchive() const {
    return archive_;
}

std::vector<std::string> Parser::SplitString(const std::string& str) {
    std::istringstream iss(str);

    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}

void Parser::Parse(int argc, const char** argv) {
    for (int i = 1; i < argc; i++) {
        std::string command = argv[i];
        if (command == kExtractLong or command == kExtractShort) {
            extract_files_ = true;
        } else if (command == kListLong or command == kListShort) {
            files_list_ = true;
        } else if (command == kAppendLong or command == kAppendShort) {
            append_file_ = true;
        } else if (command == kCreateLong or command == kCreateShort) {
            create_archive_ = true;
        } else if (command == kDeleteLong or command == kDeleteShort) {
            delete_file_ = true;
        } else if (command == kConcatenateLong or command == kConcatenateShort) {
            split_archives_ = true;
        } else if (std::strncmp(argv[i], kFileLong, 7) == 0) {
            archive_ = command.substr(7);
        } else if (command == kFileShort) {
            archive_ = argv[i + 1];
            i++;
        } else {
            files_names.emplace_back(argv[i]);
        }
    }
}

void Parser::Parse(const std::string& commands) {
    std::vector<std::string> command_l = SplitString(commands);
    for (int i = 1; i < command_l.size(); i++) {
        std::string command = command_l[i];
        if (command == kExtractLong or command == kExtractShort) {
            extract_files_ = true;
        } else if (command == kListLong or command == kListShort) {
            files_list_ = true;
        } else if (command == kAppendLong or command == kAppendShort) {
            append_file_ = true;
        } else if (command == kCreateLong or command == kCreateShort) {
            create_archive_ = true;
        } else if (command == kDeleteLong or command == kDeleteShort) {
            delete_file_ = true;
        } else if (command == kConcatenateLong or command == kConcatenateShort) {
            split_archives_ = true;
        } else if (std::strncmp(command_l[i].c_str(), kFileLong, 7) == 0) {
            archive_ = command.substr(7);
        } else if (command == kFileShort) {
            archive_ = command_l[i + 1];
            i++;
        } else {
            files_names.push_back(command_l[i]);
        }
    }
}
