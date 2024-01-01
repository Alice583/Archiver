#include "Archivator.h"
#include <iostream>

bool Archivator::CountControlBit(std::bitset<kBitesInCoded> codeWord, int cur) {
    bool control_bit = false;
    for (int i = kBitesInCoded - cur; i >= 0; i -= (cur * 2)) {
        for (int j = i; j > i - cur && j >= 0; --j) {
            control_bit = control_bit ^ codeWord[j];
        }
    }
    return control_bit;
}

uint16_t Archivator::OneByte(uint8_t symb) {
    uint16_t res = 0;
    std::bitset<kBitesInByte> buff_one_byte = symb;
    std::bitset<kBitesInCoded> buff_for_code;
    bool bit;
    int power_of_two = 1;
    int cursor = 1;
    for (int i = 1; i <= kBitesInCoded; ++i) {
        if (i == power_of_two) {
            power_of_two *= 2;
        } else {
            buff_for_code[kBitesInCoded - i] = buff_one_byte[kBitesInByte - cursor];
            cursor++;
        }
    }
    for (int i = 1; i <= kBitesInByte; i *= 2) {
        buff_for_code[kBitesInCoded - i] = CountControlBit(buff_for_code, i);
    }
    for (int i = kBitesInCoded - 1; i >= 0; --i) {
        bit = buff_for_code[i];
        res |= (bit << i);
    }
    return res;
}

int Archivator::OneDecode(uint16_t symb_code) {
    uint8_t res = 0;
    std::bitset<kBitesInCoded> buff_coded_byte = symb_code;
    std::bitset<kBitesInCoded> buff_for_check_coded_byte = symb_code;
    std::bitset<kBitesInByte> buff_for_decoded_byte;
    int sum_for_control = 0;
    bool bit;
    for (int i = 1; i <= kBitesInCoded; i *= 2) {
        buff_for_check_coded_byte[kBitesInCoded - i] = false;
        if (CountControlBit(buff_for_check_coded_byte, i) != buff_coded_byte[kBitesInCoded - i]) {
            sum_for_control += i;
        }
    }
    if (sum_for_control > kBitesInCoded) {
        std::cerr << "More than two mistakes!";
        return -1;
    }
    if (sum_for_control != 0) {
        buff_coded_byte[kBitesInCoded - sum_for_control] = !(buff_coded_byte[kBitesInCoded - sum_for_control]);
    }
    int power_of_two = 1;
    int cursor = 1;
    for (int i = 1; i <= kBitesInCoded; ++i) {
        if (i == power_of_two) {
            power_of_two *= 2;
        } else {
            buff_for_decoded_byte[kBitesInByte - cursor] = buff_coded_byte[kBitesInCoded - i];
            cursor++;
        }
    }
    for (int i = kBitesInByte - 1; i >= 0; --i) {
        bit = buff_for_decoded_byte[i];
        res |= (bit << i);
    }
    return res;
}

void Archivator::CodeSizeFile(uint32_t file_size, std::ofstream& archive_ofstream) {
    int cursor = 1;
    uint16_t codeSymb = 0;
    uint8_t res;
    bool bit;
    std::bitset<kBitesInByte> one_byte;
    std::bitset<kBitsInFourBytes> buff_for_code_size = file_size;
    for (int i = 0; i < sizeof(uint32_t); ++i) {
        codeSymb = 0;
        res = 0;
        for (int j = 1; j <= kBitesInByte; ++j) {
            one_byte[kBitesInByte - j] = buff_for_code_size[kBitsInFourBytes - cursor];
            cursor++;
        }
        for (int k = kBitesInByte - 1; k >= 0; --k) {
            bit = one_byte[k];
            res |= (bit << k);
        }
        codeSymb = OneByte(res);
        archive_ofstream.write((char*) &codeSymb, 2);
    }
}

uint32_t Archivator::DecodeSizeFile(std::ifstream& archive_ifstream) {
    int cursor = 1;
    uint32_t res = 0;
    uint16_t byte_for_code;
    bool bit;
    int decoded_byte;
    for (int i = 0; i < sizeof(uint32_t); ++i) {
        archive_ifstream.read((char*) &byte_for_code, 2);
        decoded_byte = OneDecode(byte_for_code);
        std::bitset<kBitesInByte> one_byte = decoded_byte;
        if (decoded_byte == -1) {
            std::cerr << "The file isn`t correct";
            exit(0);
        } else {
            for (int j = kBitesInByte - 1; j >= 0; --j) {
                bit = one_byte[j];
                res |= (bit << (kBitsInFourBytes - cursor));
                cursor++;
            }
        }
    }
    return res;
}

uint32_t Archivator::GetSizeFile(std::ifstream& file_ifstream) {
    uint32_t file_size;
    file_ifstream.seekg(0, std::ios::end);
    file_size = file_ifstream.tellg();
    file_ifstream.seekg(0, std::ios::beg);
    return file_size;
}

std::string Archivator::GetNameOfFile(std::ifstream& archive_ifstream) {
    std::string file_name;
    uint16_t coded_byte;
    int decoded_byte = 0;
    while ((char) decoded_byte != '\n') {
        archive_ifstream.read((char*) &coded_byte, 2);
        decoded_byte = OneDecode(coded_byte);
        if (decoded_byte == -1) {
            std::cerr << "The file isn`t correct";
            exit(0);
        } else if ((char) decoded_byte != '\n') {
            file_name += (char) decoded_byte;
        } else {
            return file_name;
        }
    }
    return file_name;
}


void Archivator::CreateNewArchive(const std::vector<std::string>& files_names, const std::string& archive_name) {
    std::ofstream new_archive(archive_name, std::ios::binary);
    uint32_t size_of_file;
    uint16_t codeSymb;
    char symb;
    for (int i = 0; i < files_names.size(); ++i) {
        std::ifstream file(files_names[i], std::ios::binary);
        if (!file) {
            std::cerr << "The file`s path isn`t correct";
            exit(0);
        }
        size_of_file = GetSizeFile(file) + files_names[i].size() + 1;
        CodeSizeFile(size_of_file, new_archive);
        for (int j = 0; j < files_names[i].size(); ++j) {
            codeSymb = OneByte(files_names[i][j]);
            new_archive.write((char*) &codeSymb, 2);
        }
        codeSymb = OneByte('\n');
        new_archive.write((char*) &codeSymb, 2);
        size_of_file -= (files_names[i].size() + 1);
        for (int j = 0; j < size_of_file; ++j) {
            file.get(symb);
            codeSymb = OneByte(symb);
            new_archive.write((char*) &codeSymb, 2);
        }
    }
}

void Archivator::GetListOfFilesFromArchive(const std::string& archive_name) {
    std::ifstream archive(archive_name, std::ios::binary);
    uint32_t archive_size;
    std::string file_name;
    uint32_t cur_file_size;
    uint32_t cursor;
    if (!archive) {
        std::cerr << "The file`s path isn`t correct";
        exit(0);
    }
    archive_size = GetSizeFile(archive);
    if (archive_size == 0) {
        std::cout << "There are no files in the " << archive_name << ".";
        return;
    }
    std::cout << archive_name << ":\n";
    while (archive.tellg() != archive_size) {
        cur_file_size = DecodeSizeFile(archive);
        if (cur_file_size == 0) {
            std::cerr << "The file isn`t correct";
            exit(0);
        }
        file_name = GetNameOfFile(archive);
        cur_file_size -= (file_name.size() + 1);
        std::cout << file_name << '\n';
        cursor = archive.tellg();
        cursor += 2 * cur_file_size;
        archive.seekg(cursor);
    }
}

void Archivator::AppendFileToArchive(const std::string& file_name, const std::string& archive_name) {
    std::ifstream archive_ifstream(archive_name, std::ios::binary);
    uint32_t size_of_file;
    uint16_t codeSymb;
    char symb;
    if (!archive_ifstream) {
        std::cerr << "The file`s path isn`t correct";
        exit(0);
    }
    archive_ifstream.close();
    std::ofstream archive_ofstream(archive_name, std::ios::app | std::ios::binary);

    std::ifstream file_ifstream(file_name, std::ios::binary);
    if (!file_ifstream) {
        std::cerr << "The file`s path isn`t correct";
        exit(0);
    }

    size_of_file = GetSizeFile(file_ifstream) + file_name.size() + 1;
    CodeSizeFile(size_of_file, archive_ofstream);

    for (int i = 0; i < file_name.size(); ++i) {
        codeSymb = OneByte(file_name[i]);
        archive_ofstream.write((char*) &codeSymb, 2);
    }

    codeSymb = OneByte('\n');
    archive_ofstream.write((char*) &codeSymb, 2);

    size_of_file -= (file_name.size() + 1);
    for (int j = 0; j < size_of_file; ++j) {
        file_ifstream.get(symb);
        codeSymb = OneByte((uint8_t) symb);
        archive_ofstream.write((char*) &codeSymb, 2);
    }
    std::cout << "Files is added.";
}

void Archivator::DeleteFileFromArchive(const std::string& file_name, const std::string& archive_name) {
    std::ifstream archive_ifstream(archive_name, std::ios::binary);
    uint32_t archive_size;
    uint32_t cursor = 0;
    uint32_t cur_size_file;
    std::string cur_name_file;
    char symb;
    const std::string temp_name_of_archive = "temp";
    if (!archive_ifstream) {
        std::cerr << "The file`s path isn`t correct";
        exit(0);
    }
    std::ofstream archive_ofstream(temp_name_of_archive, std::ios::binary);
    archive_size = GetSizeFile(archive_ifstream);
    while (cursor < archive_size) {
        archive_ifstream.seekg(cursor);
        cur_size_file = DecodeSizeFile(archive_ifstream);
        if (cur_size_file == 0) {
            std::cerr << "The file isn`t correct";
            exit(0);
        }
        cur_name_file = GetNameOfFile(archive_ifstream);
        if (cur_name_file == file_name) {
            cursor += 2 * (cur_size_file + sizeof(uint32_t));
        } else {
            archive_ifstream.seekg(cursor);
            cursor += 2 * (cur_size_file + sizeof(uint32_t));
            for (int i = 0; i < 2 * (cur_size_file + sizeof(uint32_t)); ++i) {
                archive_ifstream.get(symb);
                archive_ofstream.write(&symb, 1);
            }
        }
    }
    archive_ifstream.close();
    archive_ofstream.close();
    remove(archive_name.c_str());
    rename(temp_name_of_archive.c_str(), archive_name.c_str());
    std::cout << "Files is deleted.";
}

void Archivator::ExtractAllFiles(const std::string& archive_name) {
    std::ifstream archive(archive_name, std::ios::binary);
    uint32_t size_of_archive;
    std::string name_of_file;
    if (!archive) {
        std::cerr << "The file`s path isn`t correct";
        exit(0);
    }
    size_of_archive = GetSizeFile(archive);
    uint32_t current_size_of_file;
    while (archive.tellg() != size_of_archive) {
        current_size_of_file = DecodeSizeFile(archive);
        if (current_size_of_file == 0) {
            std::cerr << "The file isn`t correct";
            exit(0);
        }
        name_of_file = GetNameOfFile(archive);
        current_size_of_file -= (name_of_file.size() + 1);
        ExtractOneFile(current_size_of_file, name_of_file, archive);
    }
}

void Archivator::ExtractFile(const std::string& file_name, const std::string& archive_name) {
    std::ifstream archive_ifstream(archive_name, std::ios::binary);
    uint32_t archive_size;
    uint32_t cur_size_file;
    std::string cur_file_name;
    uint32_t cursor;
    if (!archive_ifstream) {
        std::cerr << "The file`s path isn`t correct";
        exit(0);
    }
    archive_size = GetSizeFile(archive_ifstream);
    if (archive_size == 0) {
        std::cout << "There is nothing to extract. Archive is empty.";
        return;
    }
    while (archive_ifstream.tellg() != archive_size) {
        cur_size_file = DecodeSizeFile(archive_ifstream);
        if (cur_size_file == 0) {
            std::cerr << "The file isn`t correct";
            exit(0);
        }
        cur_file_name = GetNameOfFile(archive_ifstream);
        cur_size_file -= (cur_file_name.size() + 1);
        if (cur_file_name == file_name) {
            ExtractOneFile(cur_size_file, file_name, archive_ifstream);
            return;
        }
        cursor = archive_ifstream.tellg();
        cursor += (2 * cur_size_file);
        archive_ifstream.seekg(cursor);
    }
    std::cout << "Files extracted.";
}

void Archivator::ExtractOneFile(uint32_t size_of_file, const std::string& name_of_file, std::ifstream& archive) {
    std::ofstream file_ofstream(name_of_file, std::ios::binary);
    uint16_t byte_for_code;
    int byte_for_decode;
    for (int i = 0; i < size_of_file; ++i) {
        archive.read((char*) &byte_for_code, 2);
        byte_for_decode = OneDecode(byte_for_code);
        if (byte_for_decode == -1) {
            std::cerr << "The file isn`t correct";
            exit(0);
        }
        file_ofstream.write((char*) &byte_for_decode, 1);
    }
}

void Archivator::SplitArchives(const std::vector<std::string>& names_of_archives, const std::string& name_of_archive) {
    std::ofstream archive(name_of_archive, std::ios::binary);
    char ch;
    for (int i = 0; i < names_of_archives.size(); ++i) {
        std::ifstream current_archive(names_of_archives[i], std::ios::binary);
        while (current_archive.get(ch)) {
            archive.write(&ch, 1);
        }
    }
    std::cout << "Archives are concatenated.";
}
