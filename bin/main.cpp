#include "lib/Parser.h"
#include "lib/Archivator.h"
#include <iostream>


int main(int argc, const char** argv) {
    Parser parser;
//    parser.Parse("./labwork6 --create --file=arc1 file1.txt");
//    parser.Parse("./labwork6 --list --file=arc");
//    parser.Parse("./labwork6 --delete --file=arc file1.txt");
//    parser.Parse("./labwork6 --list --file=arc");
    parser.Parse("./labwork6 --extract --file=arc file1.txt");
    Archivator archivator;

    if (parser.isCreateArchive()) {
        archivator.CreateNewArchive(parser.getFilesNames(), parser.getArchive());
    } else if (parser.isFilesList()) {
        archivator.GetListOfFilesFromArchive(parser.getArchive());
    } else if (parser.isDeleteFile() && !parser.getFilesNames().empty()) {
        archivator.DeleteFileFromArchive(parser.getFilesNames()[0], parser.getArchive());
    } else if (parser.isAppendFile() && !parser.getFilesNames().empty()) {
        archivator.AppendFileToArchive(parser.getFilesNames()[0], parser.getArchive());
    } else if (parser.isExtractFiles() && parser.getFilesNames().empty()) {
        archivator.ExtractAllFiles(parser.getArchive());
    } else if (parser.isExtractFiles() && !parser.getFilesNames().empty()) {
        archivator.ExtractFile(parser.getFilesNames()[0], parser.getArchive());
    } else if (parser.isSplitArchives()) {
        archivator.SplitArchives(parser.getFilesNames(), parser.getArchive());
    }
    return 0;
}