/*  
    Quine-McCluskey and Petrick’s Method logic minimization Program
    Implemented by MineEric in January 2022
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

void Input();
//stage 1 Find all prime implicants
void SortTerm();
bool MergeTerm();
void deleteSame();
//stage 2 Petrick's method
void CreateList();
void CreatePOS();
void CalculatePOS();
void FindMinTerm();
void Translate();

void Output();
//Tool
short FindVar(char &);
short CountOne(string &);
short CanMerge(string &,string &);
void CountTerm(vector<short> &,string &,short ,short );
string MultiplePOS(string &, string &);
bool CompareTerm(string &, string &);
void DeleteSameChar(string &);

vector<char>Variable;
vector<string>Term;
vector<short>TermNumber;
vector<vector<bool> >List;
vector<string>POS;
vector<string>Result;
string FileName;

int main(){

    bool isMerge = 1;
    Input();
    deleteSame();
    do{
        SortTerm();
        isMerge = MergeTerm();
        if(isMerge) deleteSame();
    }while(isMerge);
    CreateList();
    CreatePOS();
    CalculatePOS();
    FindMinTerm();
    Translate();
    Output();
    return 0;
}

void Input(){

    short Count = 0;
    short Index = 0;
    bool isComplement = 0;
    string InputStr,TempStr;
    cout<<"Enter the file name: "<<endl;
    cin>>FileName;
    ifstream InputFile(FileName,ios::in);
    while(!InputFile){
        cout<<"File doesn't exist. Please check your file."<<endl;
        cout<<"Enter the file name again: "<<endl;
        cin>>FileName;
        InputFile.open(FileName,ios::in);
    }

    while(getline(InputFile,InputStr)){

        if(InputStr[0] == 'I'){
            for(auto i = InputStr.find('=')+1; i<InputStr.size(); i++){
                if(InputStr[i] == ' ') continue;
                else if(InputStr[i] == ';') break;
                Variable.push_back(InputStr[i]);
            }
        }
        else if(InputStr[0] == 'O') continue;
        else if(InputStr[0] == 'o'){
            InputStr = InputStr.substr(InputStr.find('('));
            for(short i = 0; i<static_cast<short>(InputStr.size()); i++){

                if(InputStr[i] == ')') Term.push_back(TempStr);
                else if(InputStr[i] == '('){
                    TempStr = "";
                    for(short i = 0; i<static_cast<short>(Variable.size()); i++) TempStr += '-';
                }
                else if(InputStr[i] == '+' || InputStr[i] == '*') continue;
                else if(InputStr[i] == '!') isComplement = 1;
                else{
                    Index = FindVar(InputStr[i]);
                    if(Index != -1){
                        if(isComplement) TempStr[Index] = '0';
                        else TempStr[Index] = '1';
                    }
                    isComplement = 0;
                }
            }
        }

        for(short i = 0; i<static_cast<short>(Term.size()); i++){
            Count = 0;
            for(short j = 0; j<static_cast<short>(Term[i].size()); j++){
                Count *= 2;
                Count += Term[i][j] - '0';
            }
            TermNumber.push_back(Count);
        }
        sort(TermNumber.begin(),TermNumber.end());
    }
}

void SortTerm(){
    string TempStr;
    vector<string>Sort;

    for(short Number = 0; Number<=static_cast<short>(Variable.size()); Number++){

        TempStr = "#" + to_string(Number);
        Sort.push_back(TempStr);

        for(short i = 0; i<static_cast<short>(Term.size()); i++){
            if(Term[i].at(0) == '#') continue;
            if(CountOne(Term[i]) == Number) Sort.push_back(Term[i]);
        }
    }
    Sort.push_back("#");
    Term = Sort;
}

bool MergeTerm(){

    bool isMerge = 0;
    short Index = 0;
    string TempStr;
    vector<string>Merge;
    vector<short>NumberIndex;
    vector<bool>hasMerge;

    for(short i = 0; i<static_cast<short>(Term.size()); i++){
        if(Term[i].at(0) == '#') NumberIndex.push_back(i);
    }

    for(short i = 0; i<static_cast<short>(Term.size()); i++){
        hasMerge.push_back(0);
    }

    for(short i = 0; i<static_cast<short>(NumberIndex.size())-1; i++){
        for(short j = NumberIndex[i] + 1; j<NumberIndex[i+1] ; j++){
            for(short k = NumberIndex[i+1] + 1; k<static_cast<short>(Term.size()) && k<NumberIndex[i+2]; k++){
                //cout<<Term[j]<<" compare "<<Term[k]<<endl;
                Index = CanMerge(Term[j],Term[k]);
                if(Index == -1) continue;
                isMerge = 1;
                TempStr = Term[j];
                TempStr[Index] = '-';
                Merge.push_back(TempStr);
                hasMerge[j] = 1;
                hasMerge[k] = 1;
               // cout<<"Can Merge to "<<TempStr<<endl;
            }
        }
    }
    //for(short i = 0; i<static_cast<short>(Merge.size()); i++) cout<<"Merge "<<Merge[i]<<endl;
    //cout<<endl;
    for(short i = 0; i<static_cast<short>(hasMerge.size()); i++){
        if(hasMerge[i] == 0){
            if(Term[i].at(0) == '#') continue;
            if(Term[i].find('*') < Term[i].size() || !isMerge) TempStr = Term[i];
            else TempStr = Term[i] + "*";
            Merge.push_back(TempStr);
        }
    }
    Term = Merge;
    return isMerge;
}

void deleteSame(){

    bool isSame = 0;
    vector<string>Sort;

    for(short i = 0; i<static_cast<short>(Term.size()); i++){

        isSame = 0;
        for(short j = 0; j<static_cast<short>(Sort.size()); j++){
            if(Term[i] == Sort[j]){
                isSame = 1;
                break;
            }
        }
        if(!isSame) Sort.push_back(Term[i]);
    }
    Term = Sort;
}

void CreateList(){

    vector<short>Number;
    vector<bool>TEMP(TermNumber.size());

    for(short i = 0; i<static_cast<short>(Term.size()); i++){
        //if(Term[i][0] == '#') continue;
        Number.clear();
        CountTerm(Number,Term[i],0,0);
        for(short j = 0; j<static_cast<short>(TEMP.size()); j++) TEMP[j] = 0;
        //cout<<endl;
        //for(short j = 0; j<static_cast<short>(Number.size()); j++) cout<<Number[j]<<" ";
        for(short j = 0; j<static_cast<short>(Number.size()); j++){
            for(short k = 0; k<static_cast<short>(TermNumber.size()); k++){
                if(Number[j] == TermNumber[k]){
                    TEMP[k] = 1;
                    break;
                }
            }
        }
        List.push_back(TEMP);
    }
    /*
    cout<<endl<<endl;
    for(short i = 0; i<static_cast<short>(TermNumber.size()); i++) cout<<TermNumber[i]<<" ";
    cout<<endl<<endl;
    for(short i = 0; i<static_cast<short>(List.size()); i++){
        for(short j = 0; j<static_cast<short>(List[i].size()); j++){
            cout<<List[i][j];
        }
        cout<<endl;
    }
    */
}

void CreatePOS(){

    string TempStr;
    for(short i = 0; i<static_cast<short>(List[0].size()); i++){
        TempStr = "";
        for(short j = 0; j<static_cast<short>(List.size()); j++){

            if(List[j][i] == 1){
                if(static_cast<short>(POS.size()) <= i ){
                    POS.push_back("");
                }
                TempStr += char(j + 'A');
                TempStr += "+";
            }
        }
        POS.at(i) += TempStr;
    }
    //cout<<endl;
    //for(short i = 0; i<static_cast<short>(POS.size()); i++) cout<<POS.at(i)<<endl;
    //cout<<endl;

}

void CalculatePOS(){
    string TEMP;
    for(short i = 0; i<static_cast<short>(POS.size()-1); i++){
        POS[i+1] = MultiplePOS(POS[i],POS[i+1]);

    }
    TEMP = POS[POS.size()-1];
    POS.clear();
    POS.push_back(TEMP);
    //cout<<POS[0]<<endl;
}

short FindVar(char &Input){

    for(short i = 0; i<static_cast<short>(Variable.size()); i++){
        if(Input == Variable[i] ) return i;
    }
    return -1;
}

short CountOne(string &Input){
    short Count = 0;
    for(short i = 0; i<static_cast<short>(Input.size()); i++){
       if(Input[i] == '1') Count++;
    }
    return Count;
}

short CanMerge(string &Input1,string &Input2){

    short Count = 0;
    short Index = 0;
    //cout<<Input1<<" compare "<<Input2<<endl;
    for(short i = 0; i<static_cast<short>(Input1.size()); i++){
        if(Input1.size() != Input2.size()) return -1;
        else if(Count>1) return -1;
        else if(Input1[i] == '*' || Input2[i] == '*') continue;
        else if(Input1[i] == Input2[i]) continue;
        else{
            Count++;
            Index = i;
        }
    }
    if(Count>1) return -1;
    //cout<<"Can Merge"<<endl;
    return Index;
}

void CountTerm(vector<short> &Number,string &InputStr,short Index,short Count){

    if(Index == static_cast<short>(InputStr.size()) ){
        Number.push_back(Count);
        return;
    }
    if(InputStr[Index] == '*'){
        Number.push_back(Count);
        return;
    }
    //cout<<InputStr[Index]<<" ";
    if(InputStr[Index] == '0') CountTerm(Number,InputStr,Index+1,Count*2);
    else if(InputStr[Index] == '1') CountTerm(Number,InputStr,Index+1,Count*2+1);
    else if(InputStr[Index] == '-'){
        CountTerm(Number,InputStr,Index+1,Count*2);
        CountTerm(Number,InputStr,Index+1,Count*2+1);
    }
    return;
}

string MultiplePOS(string &First, string &Second){

    vector<string>FirstTerm;
    vector<string>SecondTerm;
    vector<string>MultiTerm;
    string TEMP;

    for(short i = 0; i<static_cast<short>(First.size()); i++){
        if(First[i] == '+'){
            FirstTerm.push_back(TEMP);
            TEMP = "";
        }
        else{
            TEMP += First[i];
        }
    }
    for(short i = 0; i<static_cast<short>(Second.size()); i++){
        if(Second[i] == '+'){
            SecondTerm.push_back(TEMP);
            TEMP = "";
        }
        else{
            TEMP += Second[i];
        }
    }

    for(short i = 0; i<static_cast<short>(FirstTerm.size()); i++){
        for(short j = 0; j<static_cast<short>(SecondTerm.size()); j++){

            if(FirstTerm[i] == SecondTerm[j]) MultiTerm.push_back(FirstTerm[i]);
            else{
                MultiTerm.push_back(FirstTerm[i]+SecondTerm[j]);
            }
        }
    }

    for(short i = 0; i<static_cast<short>(MultiTerm.size()); i++) DeleteSameChar(MultiTerm[i]);

    for(short i = 0; i<static_cast<short>(MultiTerm.size()); i++){

        for(short j = 0; j<static_cast<short>(MultiTerm.size()); j++){
            if(i==j) continue;
            else if(MultiTerm[i][0] == '#') break;
            else if(MultiTerm[j][0] == '#') continue;
            else if(MultiTerm[i] == MultiTerm[j]) MultiTerm[j] = "#";
            else if(CompareTerm(MultiTerm[i],MultiTerm[j])){
                //cout<<"IS X+XY "<<MultiTerm[i]<<" "<<MultiTerm[j]<<endl;
                if(MultiTerm[i].size() > MultiTerm[j].size() ){
                    MultiTerm[i] = "#";
                    break;
                }
                else MultiTerm[j] = "#";
            }
            DeleteSameChar(MultiTerm[i]);
        }
    }

    for(short i = 0; i<static_cast<short>(MultiTerm.size()); i++){
        if(MultiTerm[i][0] == '#') continue;
        DeleteSameChar(MultiTerm[i]);
        TEMP += MultiTerm[i] + "+";
    }
    //cout<<TEMP<<endl;
    return TEMP;
}

void FindMinTerm(){

    vector<string>TEMP;
    vector<short>Counts;
    short Count = 0;
    short Min = 0;
    string TempStr;
    for(short i = 0; i<static_cast<short>(POS[0].size()); i++){
        if(POS[0][i] == '+'){
            Counts.push_back(Count);
            TEMP.push_back(TempStr);
            Count = 0;
            TempStr = "";
            continue;
        }
        Count++;
        TempStr += POS[0][i];
    }
    Count = 1000;
    for(short i = 0; i<static_cast<short>(Counts.size()); i++){
        if(Count> Counts[i]){
            Min = i;
            Count = Counts[i];
        }
    }
    POS[0] = TEMP[Min];
    //cout<<POS[0]<<endl;
}

void Translate(){

    string TEMP;
    for(short i = 0; i<static_cast<short>(POS[0].size()); i++){

        Result.push_back(Term[POS[0][i] - 'A']);
    }
    TEMP = "(";
    for(short i = 0; i<static_cast<short>(Result.size()); i++){
        TEMP = "(";
        for(short j = 0; j<static_cast<short>(Result[i].size()); j++){

            if(Result[i][j] == '0'){
                TEMP += "!";
                TEMP.push_back(Variable[j] ) ;
                TEMP += "*";
            }
            else if(Result[i][j] == '1'){
                TEMP.push_back(Variable[j] ) ;
                TEMP += "*";
            }
            else if(Result[i][j] == '-' || Result[i][j] == '*') continue;

        }
        TEMP.pop_back();
        TEMP += ")";
        Result[i] = TEMP;
        //cout<<Result[i]<<endl;
    }
}

void Output(){

    string TempStr;
    ofstream OutputFile;
    FileName = FileName.substr(0,FileName.size()-4) + "out.eqn";
    OutputFile.open(FileName);

    OutputFile<<"INORDER = ";
    for(short i = 0; i<static_cast<short>(Variable.size()); i++){
        OutputFile<<Variable[i]<<" ";
    }
    OutputFile<<";"<<endl;
    OutputFile<<"OUTORDER = output;"<<endl<<"output = ";

    for(short i = 0; i<static_cast<short>(Result.size()); i++){

        TempStr += Result[i] + "+";
    }
    TempStr.pop_back();
    OutputFile<<TempStr<<endl;
}

bool CompareTerm(string &Term1, string &Term2){

    short Conut = 0;

    for(short i = 0; i<static_cast<short>(Term1.size()); i++){

        for(short j = 0; j<static_cast<short>(Term2.size()); j++){
            if(Term1[i] == Term2[j]){
                Conut++;
            }
        }
    }
    if(Conut == static_cast<short>(Term1.size()) || Conut == static_cast<short>(Term2.size())) return 1;
    else return 0;
}

void DeleteSameChar(string &Str){

    bool isSame = 0;
    string Sorting;

    for(short i = 0; i<static_cast<short>(Str.size()); i++){
        isSame = 0;
        for(short j = 0; j<static_cast<short>(Sorting.size()); j++){
            if(Str[i] == Sorting[j]){
                isSame = 1;
                break;
            }
        }
        if(!isSame) Sorting.push_back(Str[i]);
    }
    Str = Sorting;
}
