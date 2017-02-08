#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#define  RARDLL
#include  "dll.hpp"

#pragma comment(lib, "UnRAR.lib")

using namespace std;

enum CrackMethod
{
    NumberOnly,
    LetterOnly,
    WebAddress,
    All
};

void get_available_characters(CrackMethod &method, vector<char> &availableCharacters)
{
    if (method == LetterOnly || method == WebAddress || method == All)
    {
        for (char i = 'a'; i <= 'z'; i++)
        {
            availableCharacters.push_back(i);
        }
    }

    if (method == LetterOnly || method == All)
    {
        for (char i = 'A'; i <= 'Z'; i++)
        {
            availableCharacters.push_back(i);
        }
    }
    
    if (method == NumberOnly || method == All)
    {
        for (char i = '0'; i <= '9'; i++)
        {
            availableCharacters.push_back(i);
        }
    }

    if (method == All)
    {
        availableCharacters.push_back('.');
        availableCharacters.push_back('_');
    }
}
static int CALLBACK _unrar_test_password_callback(UINT msg, LPARAM data, LPARAM P1, LPARAM P2)
{
    switch(msg) {
          case UCM_NEEDPASSWORD:
            *(int *)data = 1;
          break;
          default:
            return 0;
    }
    return 0;
}


static int unrar_test_password(const char * file, const char * pwd) 
{
    void                      * unrar;
    struct RARHeaderData        headerdata;
    struct RAROpenArchiveData   archivedata;
    int                         password_incorrect;
    
    password_incorrect = 0;
    memset(&headerdata,0, sizeof(headerdata));
    memset(&archivedata,0, sizeof(archivedata));
    archivedata.ArcName  = (char *) file;
    archivedata.OpenMode = RAR_OM_EXTRACT;
    
    unrar = RAROpenArchive(&archivedata);
    if (!unrar || archivedata.OpenResult)
        return -2;
    RARSetPassword(unrar, (char *) pwd);
    
    RARSetCallback(unrar, _unrar_test_password_callback, (long) &password_incorrect);
    
    if (RARReadHeader(unrar, &headerdata) != 0 || RARProcessFile(unrar, RAR_TEST, NULL, NULL) != 0 || password_incorrect)
    {
        RARCloseArchive(unrar);
        return -1;
    }

    RARCloseArchive(unrar);
    return 0;   
}

bool next_choice(vector<int> &max_choice_count, vector<int> &choice){
    for (int i = max_choice_count.size() - 1; i >= 0; --i){
        if (choice[i] != max_choice_count[i] - 1){
            ++choice[i];
            fill(choice.begin() + i + 1, choice.end(), 0);
            return true;
        }
    }
    return false;
}

void get_combination_string(size_t &currentCharacterCount, string& password, 
    vector<char>& availableCharacters, vector<int>& combinations)
{
    for (size_t i = 0; i < currentCharacterCount; i++)
    {
        password[i] = availableCharacters[combinations[i]];
    }
}

bool check_file_exists(const char* path)
{
    fstream ifs(path, ios::in);
    if (ifs)
    {
        ifs.close();
        return true;
    }
    return false;
}

string concat(const string &v1, const string &v2)
{
    stringstream ss;
    ss << v1 << v2;
    return ss.str();
}

bool test_password_by_crack_method(string& file, string& password, CrackMethod method){
    if (method == WebAddress)
    {
        if (unrar_test_password(file.c_str(), concat(concat("www.", password), ".com").c_str()) == 0)
            return true;
        else if (unrar_test_password(file.c_str(), concat(concat("http://www.", password), ".com").c_str()) == 0)
            return true;
    }
    else 
    {
        return unrar_test_password(file.c_str(), password.c_str()) == 0;
    }
    return false;
}

int main ()
{
    size_t maxPasswordLength = 3;
    int methodIndex = 0;
    CrackMethod method = NumberOnly;
    bool found = false;
    string file{ "C:/test.rar" };
    cout << "input your crack method:" << endl
        << "0. NumberOnly" << endl
        << "1. LetterOnly" << endl
        << "2. WebAddress" << endl
        << "3. All" << endl;
    cin >> methodIndex;
    cout << "input your crack file path:" << endl;
    cin >> file;
    cout << "input max password length:" << endl;
    cin >> maxPasswordLength;
    method = CrackMethod(methodIndex);
    vector<char> availableCharacters;
    cout << "Starting pass cracking on file " << file << " Max password length: " << maxPasswordLength << endl;

    if (check_file_exists(file.c_str()))
    {
        get_available_characters(method, availableCharacters);
        size_t availableCharacterCount = availableCharacters.size(), currentCharacterCount = 0;
        string password{ "" }, actualPassword{ "" };

        vector<int> combinations;
        vector<int> max_choice_count;

        for (currentCharacterCount = 1; currentCharacterCount <= maxPasswordLength; currentCharacterCount++)
        {
            cout << "crack with " << currentCharacterCount << " bits ..." << endl;
            password += availableCharacters[0];
            combinations.resize(currentCharacterCount);
            fill(combinations.begin(), combinations.end(), 0);
            max_choice_count.resize(currentCharacterCount, availableCharacterCount);

            do {
                get_combination_string(currentCharacterCount, password, availableCharacters, combinations);
                // cout << password << endl;

                if (test_password_by_crack_method(file, password, method)){
                    cout << "password is : " << password << endl;
                    ofstream ofs("password.txt", ofstream::out | ofstream::app);
                    ofs << password << endl << "crack method: " << method << endl;
                    ofs.close();
                    found = true;
                    break;
                }
            } while (next_choice(max_choice_count, combinations));

            if (found)
                break;
        }
    }
    else
    {
        cout << "RAR file not existed" << endl;
    }
    return 1;  
}
