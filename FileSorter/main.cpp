#include "FileSorter.h"
#include "dependencies.h"

int main()
{
    fs::create_directory(Paths::LOG_DIRECTORY);
    fs::create_directory(Paths::MAIN_PATH_TO_WORK);

    std::unique_ptr<FileSorter> fileSorter = std::make_unique<FileSorter>(Paths::MAIN_PATH_TO_WORK);
    fileSorter->Start();
    
    return 0;
}
