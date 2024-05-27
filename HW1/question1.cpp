#include <set>
#include <map>
#include <iostream>
#include <vector>
using namespace std;

void printMultiSet(const multiset<int>& m)
{
    map<int, int> lookup;
    set<int> temp;
    for(auto x : m)
    {
        if(lookup.find(x) == lookup.end())
        {
            lookup[x] = 1;
            temp.insert(x);
        }
        else
        {
            lookup[x] = lookup[x] + 1;
        }
    }
    for(auto x : temp)
    {
        cout << x <<"("<<lookup[x]<<"), ";
    }
    cout<<endl;

}
void printMultiSetLong(const multiset<int>& m)
{
    for(auto x : m)
    {
        cout<<x<<", ";
    }
    cout<<endl;
}

void printSet(const set<int>& s)
{
    
    for(auto x : s)
    {
        cout<<x<<", ";
    }
    cout<<endl;
}

void place(multiset<int> L, set<int> X, int width, set<set<int>>& Solutions)
{
    // cout<<"L: ";
    // printMultiSetLong(L);
    // cout<<"X: ";
    // printSet(X);
    if( L.empty() )             // 1 if L is empty
    {
        // printSet(X);
        Solutions.insert(X);    // 2 output X
        return;                 // 3 return
    }                
    int y = *(L.rbegin());      // 4 y ← Maximum element in L
    multiset<int> delta;        // 5 if ∆(y, X) ⊆ L
    bool isSubset = true;
    
    delta = {};                        
    isSubset = true;
    for(auto x : X)                 // need to calculate multiset
    { delta.insert(abs(x - y)); }   // where distance from y to each value in X
    
    for(auto d : delta)             // then I got to check if that resulting multiset is a subset of L
    {
        if(L.count(d) < delta.count(d))
        {
            isSubset = false;
        }
    }
    if(isSubset)
    {
        X.insert(y);           // 6 Add y to X and remove lengths ∆(y, X) from L
        for( auto d : delta)
        {
            L.erase(L.find(d));
        }
        place(L, X, width, Solutions);           // 7 PLACE(L, X)
        X.erase(y);          // 8 Remove y from X and add lengths ∆(y, X) to L
        for( auto d : delta)
        {
            L.insert(d);
        }
    }
    delta = {};                         // 9 if ∆(width − y, X) ⊆ L
    isSubset = true;
    for(auto x : X)                         // need to calculate multiset
    { delta.insert(abs(x - (width - y))); } // where distance from y to each value in X
    for(auto d : delta)             // then I got to check if that resulting multiset is a subset of L
    {
        if(L.count(d) < delta.count(d))
        {
            isSubset = false;
        }
    }
    if(isSubset)
    {
        X.insert(width-y);           // 10 Add width − y to X and remove lengths ∆(width − y, X) from L
        for( auto d : delta)
        {
            L.erase(L.find(d));
        }
        place(L, X, width, Solutions);           // 11 PLACE(L, X)
        X.erase(width-y);              // 12 Remove width − y from X and add lengths ∆(width − y, X) to L
        for( auto d : delta)
        {
            L.insert(d);
        }
    }
                    
    return;
}

void partitianDigest(const multiset<int>& L_Origin, set<set<int>>& Solutions)
{
    multiset<int> Copy = L_Origin;
    int width = *(Copy.rbegin());           // 1 width ← Maximum element in L
    Copy.erase(width);                      // 2 DELETE(width, L)
    set<int> X = {0, width};                // 3 X ← {0, width}
    place(Copy, X, width, Solutions);                         // 4 PLACE(L, X)
}

void removeSymetricSolutions(set<set<int>>& Solutions)
{
    map<vector<int>, bool> found; 
    // cout<<min<<" "<<max<<endl;
    for(auto sol : Solutions)
    {   
        vector<int> cpy((*Solutions.begin()).begin(), (*Solutions.begin()).end()); //convery solution to vectro

        if( found.find(cpy) == found.end())
        {
            vector<int> symetric;   //will containt the reflextion
            for(int i = cpy.size()-1; i >= 0; i--)
            {        
                symetric.push_back( abs(cpy.at(i)-cpy.at((cpy.size()-1)-i) ));
            }
            found[cpy] = true;  // add solution out list
            found[symetric] = true;  // add solution out list
        }
        else
        {
            Solutions.erase(sol);
        }
    }

}
int main(int argc, char *argv[])
{
    // L1 = {(1(1), 2(1), 3(1), 4(1), 5(1), 6(1)}
    // L2 = {(1(2), 2(3), 3(3), 4(1), 5(3), 6(1), 7(2), 8(2), 9(1), 10(1), 11(1), 12(1)}
    // L3 = {1(9), 2(8), 3(7), 4(6), 5(5), 6(4), 7(3), 8(2), 9(1)}
    // L4 = {1(16), 2(15), 3(14), 4(13), 5(12), 6(11), 7(10), 8(9), 9(8), 10(9), 11(8), 12(7), 13(6), 14(5), 15(4), 16(3), 17(2), 18(1)}
    char chooseCase = *argv[1];
    multiset<int> current;

    // cout << char(chooseCase);
    switch(*argv[1]) {
    case '1':
        current = {1,2,3,4,5,6};
        break;
    case '2':
        current = {1,1,2,2,2,3,3,3,4,5,5,5,6,7,7,8,8,9,10,11,12};
        break;
    case '3':
        current = {1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,4,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,7,8,8,9};
        break;
    case '4':
        for(int i = 0; i < 16; i++) {current.insert(1);} for(int i = 0; i < 15; i++) {current.insert(2);}
        for(int i = 0; i < 14; i++) {current.insert(3);} for(int i = 0; i < 13; i++) {current.insert(4);}
        for(int i = 0; i < 12; i++) {current.insert(5);} for(int i = 0; i < 11; i++) {current.insert(6);}
        for(int i = 0; i < 10; i++) {current.insert(7);} for(int i = 0; i < 9; i++) {current.insert(8);}
        for(int i = 0; i <  8; i++) {current.insert(9);} for(int i = 0; i < 9; i++) {current.insert(10);}
        for(int i = 0; i < 8; i++) {current.insert(11);} for(int i = 0; i < 7; i++) {current.insert(12);}
        for(int i = 0; i < 6; i++) {current.insert(13);} for(int i = 0; i < 5; i++) {current.insert(14);}
        for(int i = 0; i < 4; i++) {current.insert(15);} for(int i = 0; i < 3; i++) {current.insert(16);}
        for(int i = 0; i < 2; i++) {current.insert(17);} for(int i = 0; i < 1; i++) {current.insert(18);}
        break;
    case '5':
        current =  {2,2,3,3,4,5,6,7,8,10};
        break;
    default:
        current = {1,2,3,4,5,6};
        break;
    }//end of switch;

    set<set<int>> Solutions;
    partitianDigest(current, Solutions);
    
    removeSymetricSolutions(Solutions);

    int counter = 0;
    cout<<" Test Case: ";
    printMultiSet(current);
    for( auto set : Solutions)
    {
        counter++;
        cout<<"Solution "<<counter<<": ";
        for(auto n : set)
            cout<<n<<", ";
        cout<<endl;
    }


}