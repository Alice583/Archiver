#pragma once

#include <bitset>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

class Archivator {
public:
    static const int kBitesInCoded = 12;
    static const int kBitesInByte = 8;
    static const int kBitsInFourBytes = 32;

    bool CountControlBit(std::bitset<kBitesInCoded> code, int i);

    uint16_t OneByte(uint8_t symbol);

    int OneDecode(uint16_t symb_code);

    void CodeSizeFile(uint32_t file_size, std::ofstream& archive_ofstream);

    uint32_t DecodeSizeFile(std::ifstream& archive_ifstream);

    uint32_t GetSizeFile(std::ifstream& file_ifstream);

    std::string GetNameOfFile(std::ifstream& archive_ifstream);

    void CreateNewArchive(const std::vector<std::string>& files_names, const std::string& archive_name);

    void GetListOfFilesFromArchive(const std::string& archive_name);

    void AppendFileToArchive(const std::string& file_name, const std::string& archive_name);

    void DeleteFileFromArchive(const std::string& file_name, const std::string& archive_name);

    void ExtractAllFiles(const std::string& archive_name);

    void ExtractFile(const std::string& file_name, const std::string& archive_name);

    void ExtractOneFile(uint32_t size_of_file, const std::string& name_of_file, std::ifstream& archive);

    void SplitArchives(const std::vector<std::string>& names_of_archives, const std::string& name_of_archive);
};