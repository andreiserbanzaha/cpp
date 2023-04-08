#include <iostream>
#include <string>

class OpenFile
{
public:
    OpenFile(const std::string& fileName)
    {
        std::cout << "File name: " << fileName << '\n';
    }

    void destroyMe()
    {
        delete this;
    }

private:
    ~OpenFile()
    {
        std::cout << "OpenFile destroyed\n";
    }
};

int main()
{
    //OpenFile f{std::string{"az.txt"}};
    //f.destroyMe(); // stack unwinding calls the destructor anyway and since
                    // it's private, it cannot be called

    OpenFile* f = new OpenFile{std::string{"az.txt"}};
    f->destroyMe(); // good for embedded programming where stack is limited

    return 0;
}
