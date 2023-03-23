#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>
#include<fstream>

class FileLoader
{
private:
    std::string file_;
public:
    void ReadFile(const char* file_name);
    std::string File() const;
};


#endif // FILELOADER_H
