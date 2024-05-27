#include <string>
#include <vector>
// #include <iterator>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iostream>

#include<windows.h>
#include <Psapi.h>
#include <chrono> // for time measurement


using namespace std;
namespace fs = filesystem;

const int MATCH = 5;
const int MISMATCH = -4;
const int INDEL = -8;
// read in the files / memory
void readFilesFromFolder(const string&, string& , string& , string& );
double readMemoryUsage();
double measureTime(chrono::steady_clock::time_point);

int ThreeSequenceSimilarity(const string&, const string&, const string&);
void ThreeSequenceSimilarity(const string&, const string&, const string&, unsigned, vector<vector<vector<int>>>&);
void TwoSequenceSimilarity(const string&, const string&, vector<vector<int>>&);
int getCost(char, char);
int getCost(char, char, char);
string formatTime(double);

int main(int argc, char* argv[]) 
{    
    if(argc != 2) {
        cout << "Missing Test Case/ too many cases" << endl;
        return 1;
    }

    string folderName = argv[1];
    string s1, s2, s3 = "";

    readFilesFromFolder(folderName, s1, s2, s3);
    // cout<<s1;

    // cout << getCost('A', 'A', '-');
    // cout << ThreeSequenceSimilarity("AA", "AA", "AC")<<endl;
    
    readMemoryUsage();
    auto startTime = chrono::steady_clock::now();
    cout << ThreeSequenceSimilarity(s2, s1, s3) <<endl;
    cout << readMemoryUsage()<<endl;
    cout << formatTime(measureTime(startTime)) << endl;
}


int getCost(char a, char b)
{
    if(a == '-' && b == '-')
    {
        return 0;
    }
    if(a == '-' || b == '-')
    {
        return INDEL;
    }
    else if(a == b)
    {
        return MATCH;
    }
    else
    {
        return MISMATCH;
    }
}

int getCost(char a, char b, char c)
{
    int score = 0;
    score += getCost(a,b);
    score += getCost(a,c);
    score += getCost(c,b);
    return score;
}

void TwoSequenceSimilarity(const string& x, const string& y, vector<vector<int>>& plane)
{
    // initializizing the for x = "-"
    for(unsigned i = 1; i < plane.size(); i++)
    {
        plane.at(i).at(0) = plane.at(i-1).at(0) + INDEL;
    }
    // initializizing the for y = "-"
    for(unsigned j = 1; j < plane.at(0).size(); j++)
    {
        plane.at(0).at(j) = plane.at(0).at(j-1) + INDEL;
    }

    // fill in teh face/plane

    for(unsigned row = 1; row < plane.size(); row++)
    {
        for(unsigned col = 1; col < plane.at(0).size(); col++)
        {
            int matchOrMis = plane.at(row-1).at(col-1) + getCost(x.at(row-1), y.at(col-1));
            int deletion   = plane.at(row-1).at(col  ) + getCost(x.at(row-1), '-');
            int insertion  = plane.at(row  ).at(col-1) + getCost('-', y.at(col-1));

            int maxVal = max(max(deletion, insertion), matchOrMis);
            plane.at(row).at(col) = maxVal;
        }
    }
}

int ThreeSequenceSimilarity(const string& x, const string& y, const string& z)
{
    // initialize the two sliding vectors
    vector<vector<vector<int>>> plane(2, vector<vector<int>>(x.size()+1, vector<int>(y.size()+1, 0)));
    // vector<vector<int>> plane2(x.size()+1, vector<int>(y.size()+1, 0));
    TwoSequenceSimilarity(x,y,plane.at(0));

    for(unsigned i = 1; i<z.size()+1; i++)
    {
        ThreeSequenceSimilarity(x,y,z, i, plane);
        plane.at(0) = plane.at(1);
    }
    cout << readMemoryUsage()<<endl;
    return plane.back().back().back();
    
}

void ThreeSequenceSimilarity(const string& x, const string& y, const string&z, unsigned len, vector<vector<vector<int>>>& plane)
{   
    unsigned row,col;
    plane.at(1).at(0).at(0) = plane.at(0).at(0).at(0) + INDEL; 
    // initialize first row of the 2nd plane
    for(col = 1; col < plane.at(0).at(0).size(); col++)
    {
        int matchOrMis = plane.at(0).at(0).at(col-1) + getCost(y.at(col-1), z.at(len-1));
        int deletion   = plane.at(0).at(0).at(col) + getCost('-', z.at(len-1));
        int insertion  = plane.at(1).at(0).at(col-1) + getCost(y.at(col-1), '-');

        int maxVal = max(max(deletion, insertion), matchOrMis);
        plane.at(1).at(0).at(col) = maxVal;
    }
    // initialize first col of the 2nd plane
    for(row = 1; row < plane.at(0).size(); row++)
    {
        int matchOrMis = plane.at(0).at(row-1).at(0) + getCost(x.at(row-1), z.at(len-1));
        int deletion   = plane.at(0).at(row).at(0) + getCost('-', z.at(len-1));
        int insertion  = plane.at(1).at(row-1).at(0) + getCost(x.at(row-1), '-');

        int maxVal = max(max(deletion, insertion), matchOrMis);
        plane.at(1).at(row).at(0) = maxVal;
    }

    for(row = 1; row < plane.at(0).size(); row++)
    {
        for(col = 1; col < plane.at(0).at(0).size(); col++)
        {
            vector<int> pos;
            // no indels
            pos.push_back(plane.at(0).at(row-1).at(col-1) + getCost(x.at(row-1), y.at(col-1), z.at(len-1)));

            // one indel
            pos.push_back(plane.at(1).at(row-1).at(col-1) + getCost(x.at(row-1), y.at(col-1), '-'));
            pos.push_back(plane.at(0).at(row-1).at(col) + getCost(x.at(row-1), '-', z.at(len-1)));
            pos.push_back(plane.at(0).at(row).at(col-1) + getCost('-', y.at(col-1), z.at(len-1)));
            
            // two indel
            pos.push_back(plane.at(1).at(row-1).at(col) + getCost(x.at(row-1), '-', '-'));
            pos.push_back(plane.at(1).at(row).at(col-1) + getCost('-', y.at(col-1), '-'));
            pos.push_back(plane.at(0).at(row).at(col) + getCost('-', '-', z.at(len-1)));

            plane.at(1).at(row).at(col) = *max_element(pos.begin(), pos.end());
        }

    }
}







// file reads:
void readFilesFromFolder(const string& folderName, string& s1, string& s2, string& s3) {
    fs::path folderPath = fs::current_path() / folderName;

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        cout << "Folder '" << folderName << "' does not exist." << endl;
        return;
    }

    int count = 0;
    for (const auto& entry : fs::directory_iterator(folderPath)) {

        if (entry.is_regular_file()) {
            ifstream file(entry.path());
            if (file.is_open()) {
                string line;
                getline(file, line);
                while(getline(file, line))
                {
                    getline(file, line);
                    if(count == 0)
                        s1.append(line);
                    else if(count == 1)
                        s2.append(line);
                    else if(count == 2)
                        s3.append(line);
                    else
                    {
                        cout<<"TOO MANY FILES!"<<endl;
                        return;
                    }
                }    
                file.close();
            } else {
                cout << "Unable to open file '" << entry.path().filename() << "'" << endl;
            }
        }
        count++;
    }
}

double readMemoryUsage()
{
    HANDLE process = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS_EX pmc;

    double totalMemUsedInMB = 0;
     if (GetProcessMemoryInfo(process, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        SIZE_T totalMemUsedByMe = pmc.PrivateUsage + pmc.WorkingSetSize;
        totalMemUsedInMB = static_cast<double>(totalMemUsedByMe) / (1024.0 * 1024.0);

        cout << "Total memory used by process: " <<setprecision(3) << totalMemUsedInMB << " Mb" << endl;
    } else {
        cerr << "Error: Unable to get process memory information." << endl;
        return EXIT_FAILURE;
    }
    return totalMemUsedInMB;
}

double measureTime(chrono::steady_clock::time_point started) {
    auto ending = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(ending - started).count();
    return static_cast<double>(elapsedTime);
}

string formatTime(double timeInMilliseconds) {
    int mili = static_cast<int>(timeInMilliseconds);

    int seconds = mili / 1000;
    mili %= 1000;

    int minutes = seconds / 60;
    seconds %= 60;

    int hours = minutes / 60;
    minutes %= 60;

    string formattedTime;
    if (hours > 0) {
        formattedTime += to_string(hours) + " hour";
        if (hours > 1)
            formattedTime += "s";
        formattedTime += " ";
        
    }
    if (minutes > 0) {
        formattedTime += to_string(minutes) + " minute";
        if (minutes > 1)
            formattedTime += "s";
        formattedTime += " ";
    }
    if (seconds > 0 || (hours == 0 && minutes == 0)) {
        formattedTime += to_string(seconds) + " second";
        if (seconds != 1)
            formattedTime += "s";
    }
    return formattedTime;
}