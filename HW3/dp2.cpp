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
void ThreeSequenceSimilarity(const string&, const string&, const string&, unsigned, vector<vector<vector<int>>>&, vector<vector<vector<pair<unsigned,unsigned>>>>&);
void TwoSequenceSimilarity(const string&, const string&, vector<vector<int>>&, vector<vector<pair<unsigned,unsigned>>>&);
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
    
    // readMemoryUsage();
    auto startTime = chrono::steady_clock::now();
    int score = ThreeSequenceSimilarity(s2, s1, s3);
    cout << "Alighnemnt Score: " << score <<endl;
    // cout << readMemoryUsage()<<endl;
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

void TwoSequenceSimilarity(const string& x, const string& y, vector<vector<int>>& plane, vector<vector<pair<unsigned,unsigned>>>&score)
{
    // initializizing the for x = "-"
    for(unsigned i = 1; i < plane.size(); i++)
    {
        plane.at(i).at(0) = plane.at(i-1).at(0) + INDEL;

        pair<unsigned, unsigned> cur = score.at(i-1).at(0);
        score.at(i).at(0) = make_pair(cur.first, cur.second+1); 
    }
    // initializizing the for y = "-"
    for(unsigned j = 1; j < plane.at(0).size(); j++)
    {
        plane.at(0).at(j) = plane.at(0).at(j-1) + INDEL;

        pair<unsigned, unsigned> cur = score.at(0).at(j-1);
        score.at(0).at(j) = make_pair(cur.first, cur.second+1); 
    }

    // fill in teh face/plane

    for(unsigned row = 1; row < plane.size(); row++)
    {
        for(unsigned col = 1; col < plane.at(0).size(); col++)
        {
            int matchOrMis = plane.at(row-1).at(col-1) + getCost(x.at(row-1), y.at(col-1));
            int deletion   = plane.at(row-1).at(col  ) + getCost(x.at(row-1), '-');
            int insertion  = plane.at(row  ).at(col-1) + getCost('-', y.at(col-1));

            // int maxVal = max(max(deletion, insertion), matchOrMis);
            if(deletion > matchOrMis && deletion > insertion)
            {
                plane.at(row).at(col) = deletion;

                pair<unsigned, unsigned> cur = score.at(row-1).at(col  );
                score.at(row).at(col) = make_pair(cur.first, cur.second+1); 
            }
            else if(insertion > matchOrMis && insertion > deletion)
            {
                plane.at(row).at(col) = insertion;

                pair<unsigned, unsigned> cur = score.at(row  ).at(col-1);
                score.at(row).at(col) = make_pair(cur.first, cur.second+1); 
            }
            else
            {
                plane.at(row).at(col) = matchOrMis;

                pair<unsigned, unsigned> cur = score.at(row-1).at(col-1);
                score.at(row).at(col) = make_pair(cur.first, cur.second+1); 
            }
        }
    }
}

int ThreeSequenceSimilarity(const string& x, const string& y, const string& z)
{
    // initialize the two sliding vectors
    vector<vector<vector<int>>> plane(2, vector<vector<int>>(x.size()+1, vector<int>(y.size()+1, 0)));
    vector<vector<vector<pair<unsigned,unsigned>>>> scores(2, vector<vector<pair<unsigned,unsigned>>>(x.size()+1, vector<pair<unsigned,unsigned>>(y.size()+1, make_pair(0,0))));
    // vector<vector<int>> plane2(x.size()+1, vector<int>(y.size()+1, 0));
    TwoSequenceSimilarity(x,y,plane.at(0), scores.at(0));

    for(unsigned i = 1; i<z.size()+1; i++)
    {
        // for(int a = 0; a < scores.at(0).size(); a++)
        // {
        //     for(int b = 0; b < scores.at(0).at(0).size(); b++)
        //     {
        //         cout << "("<<scores.at(0).at(a).at(b).first<<", "<<scores.at(0).at(a).at(b).second<<") ";
        //     }
        //     cout<<endl;
        // }
        // cout<<endl;

        ThreeSequenceSimilarity(x,y,z, i, plane,scores);
        plane.at(0) = plane.at(1);
        scores.at(0) = scores.at(1);

        if(i == z.size()/2)
            cout << "50% done"<<endl;

    }
    cout << readMemoryUsage()<<endl;
    cout << "# of Perfect Matches: "<<scores.back().back().back().first << endl;
    cout << "Length of Alighnemnt: "<<scores.back().back().back().second << endl;
    return plane.back().back().back();
    
}

void ThreeSequenceSimilarity(const string& x, const string& y, const string&z, unsigned len, vector<vector<vector<int>>>& plane, vector<vector<vector<pair<unsigned,unsigned>>>>&score)
{   
    unsigned row,col;
    plane.at(1).at(0).at(0) = plane.at(0).at(0).at(0) + INDEL;

    pair<unsigned, unsigned> cur; 
    score.at(1).at(0).at(0) = make_pair(cur.first, cur.second+1);
    // initialize first row of the 2nd plane
    for(col = 1; col < plane.at(0).at(0).size(); col++)
    {
        int matchOrMis = plane.at(0).at(0).at(col-1) + getCost(y.at(col-1), z.at(len-1));
        int deletion   = plane.at(0).at(0).at(col) + getCost('-', z.at(len-1));
        int insertion  = plane.at(1).at(0).at(col-1) + getCost(y.at(col-1), '-');

        int maxVal = max(max(deletion, insertion), matchOrMis);
        plane.at(1).at(0).at(col) = maxVal;

        if(maxVal == matchOrMis)
        {
           cur =  score.at(0).at(0).at(col-1); 
        }
        else if(maxVal == deletion)
        {
            cur =  score.at(0).at(0).at(col); 
        }
        else if(maxVal == insertion)
        {
            cur =  score.at(1).at(0).at(col-1);
        }
        score.at(1).at(0).at(col) = make_pair(cur.first, cur.second+1);
    }
    // initialize first col of the 2nd plane
    for(row = 1; row < plane.at(0).size(); row++)
    {
        int matchOrMis = plane.at(0).at(row-1).at(0) + getCost(x.at(row-1), z.at(len-1));
        int deletion   = plane.at(0).at(row).at(0) + getCost('-', z.at(len-1));
        int insertion  = plane.at(1).at(row-1).at(0) + getCost(x.at(row-1), '-');

        int maxVal = max(max(deletion, insertion), matchOrMis);
        plane.at(1).at(row).at(0) = maxVal;

        if(maxVal == matchOrMis)
        {
           cur =  score.at(0).at(row-1).at(0); 
        }
        else if(maxVal == deletion)
        {
            cur =  score.at(0).at(row).at(0); 
        }
        else if(maxVal == insertion)
        {
            cur =  score.at(1).at(row-1).at(0);
        }
        score.at(1).at(row).at(0) = make_pair(cur.first, cur.second+1);
    }

    for(row = 1; row < plane.at(0).size(); row++)
    {
        for(col = 1; col < plane.at(0).at(0).size(); col++)
        {
            vector<int> pos;

            int o1, o2, o3, o4, o5, o6, o7;
            o1 = plane.at(0).at(row-1).at(col-1) + getCost(x.at(row-1), y.at(col-1), z.at(len-1));
            o2 = plane.at(1).at(row-1).at(col-1) + getCost(x.at(row-1), y.at(col-1), '-');
            o3 = plane.at(0).at(row-1).at(col) + getCost(x.at(row-1), '-', z.at(len-1));
            o4 = plane.at(0).at(row).at(col-1) + getCost('-', y.at(col-1), z.at(len-1));
            o5 = plane.at(1).at(row-1).at(col) + getCost(x.at(row-1), '-', '-');
            o6 = plane.at(1).at(row).at(col-1) + getCost('-', y.at(col-1), '-');
            o7 = plane.at(0).at(row).at(col) + getCost('-', '-', z.at(len-1));
            // no indels
            pos.push_back(o1);

            // one indel
            pos.push_back(o2);
            pos.push_back(o3);
            pos.push_back(o4);
            
            // two indel
            pos.push_back(o5);
            pos.push_back(o6);
            pos.push_back(o7);

            int maxVal = *max_element(pos.begin(), pos.end());

            pair<unsigned, unsigned> cur; 
            bool perfectMatch = false;
            if( maxVal == o1) 
            {
                cur = score.at(0).at(row-1).at(col-1);
                if(x.at(row-1) == y.at(col-1) && x.at(row-1) == z.at(len-1)) //perfect match
                {
                    perfectMatch = true;
                }
            }
            else if (maxVal == o2)
            {   cur = score.at(1).at(row-1).at(col-1);  }
            else if (maxVal == o3)
            {   cur = score.at(0).at(row-1).at(col);  }
            else if (maxVal == o4)
            {   cur = score.at(0).at(row).at(col-1);  }
            else if (maxVal == o5)
            {   cur = score.at(1).at(row-1).at(col);  }
            else if (maxVal == o6)
            {   cur = score.at(1).at(row).at(col-1);  }
            else if (maxVal == o7)
            {   cur = score.at(0).at(row).at(col);  }

            plane.at(1).at(row).at(col) = maxVal;
            score.at(1).at(row).at(col) = make_pair(cur.first+perfectMatch, cur.second+1);


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
        formattedTime += to_string(hours) + " hr";
        if (hours > 1)
            formattedTime += "s";
        formattedTime += " ";
        
    }
    if (minutes > 0) {
        formattedTime += to_string(minutes) + " min";
        formattedTime += " ";
    }
    if (seconds > 0 || (hours == 0 && minutes == 0)) {
        formattedTime += to_string(seconds) + " s";
    }
    return formattedTime;
}