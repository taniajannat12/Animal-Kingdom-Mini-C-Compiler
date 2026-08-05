#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <set>
#include <map>
#include <fstream>
#include <windows.h>
#include <algorithm>

using namespace std;

vector<string> sourceCode;
vector<string> tacCode, optimizedTAC, assemblyCode;

struct Symbol {
    string name, type, value, scope;
    int line;
};
vector<Symbol> symbolTableData;

bool tokenCrystal=false, grammarCrystal=false, logicCrystal=false,
     codeCrystal=false, speedCrystal=false, royalCrystal=false;

int score=100, lexicalErrorsCount=0, syntaxErrorsCount=0, semanticErrorsCount=0, tempCounter=1;
vector<string> keywords = {"int","float","double","char","string","bool","void","if","else","for","while","return"};

// ===== Helper Functions =====
bool isKeyword(string w) { for(auto k:keywords) if(k==w) return true; return false; }
bool isOperatorChar(char c) { string op="+-*/=%<>!&|"; return op.find(c)!=string::npos; }
bool isSeparatorChar(char c) { string sep=";,(){}[]"; return sep.find(c)!=string::npos; }
bool isDataType(string w) { return w=="int"||w=="float"||w=="double"||w=="char"||w=="string"||w=="bool"; }

void loading(string text) {
    cout << "\n" << text;
    for(int i=0;i<5;i++){ cout<<"."; Sleep(250); }
    cout<<" Done!\n";
}

// ===== TAC Generator =====
string generateTAC(string expr) {
    expr.erase(remove_if(expr.begin(), expr.end(), ::isspace), expr.end());
    if(!expr.empty() && expr.back()==';') expr.pop_back();
    bool simple=true;
    for(char c:expr) if(c=='+'||c=='-'||c=='*'||c=='/') { simple=false; break; }
    if(simple) return expr;

    auto findLowest = [&](const string& s, const string& ops)->int {
        int paren=0;
        for(int i=s.size()-1; i>=0; --i) {
            if(s[i]==')') paren++;
            else if(s[i]=='(') paren--;
            else if(paren==0 && ops.find(s[i])!=string::npos) return i;
        }
        return -1;
    };
    int opPos = findLowest(expr, "+-");
    if(opPos==-1) { opPos = findLowest(expr, "*/"); if(opPos==-1) { if(expr[0]=='(' && expr[expr.size()-1]==')') return generateTAC(expr.substr(1, expr.size()-2)); return expr; } }
    char op=expr[opPos];
    string left=expr.substr(0,opPos), right=expr.substr(opPos+1);
    string l=generateTAC(left), r=generateTAC(right);
    string temp="t"+to_string(tempCounter++);
    tacCode.push_back(temp+" = "+l+" "+op+" "+r);
    return temp;
}

// ===== Functions =====
void welcome() {
    system("cls");
    cout<<"============================================================\n";
    cout<<"          ANIMAL KINGDOM MINI C++ COMPILER\n";
    cout<<"============================================================\n\n";
    cout<<"Story\n\nThe Royal Compiler Gate is Locked.\nComplete all compiler phases and collect\nall 6 Magic Crystals to open the gate.\n\n";
    cout<<"Animals\n\n[ANT]    Lexical Analysis\n[OWL]    Syntax Analysis\n[EAGLE]  Semantic Analysis\n[BEE]    Intermediate Code & TAC\n[FOX]    Code Optimization\n[LION]   Final Code Generation\n\n";
    system("pause");
}

void inputProgram() {
    system("cls");
    sourceCode.clear(); symbolTableData.clear();
    int n;
    cout<<"=============================================\n";
    cout<<"            SOURCE CODE INPUT\n";
    cout<<"=============================================\n\n";
    cout<<"How many lines of code? : ";
    cin>>n; cin.ignore(); cout<<endl;
    for(int i=0;i<n;i++){ string line; cout<<"Line "<<i+1<<" : "; getline(cin,line); sourceCode.push_back(line); }
    loading("Saving Program");
}

void displayCode() {
    system("cls");
    cout<<"=============================================\n";
    cout<<"            My SOURCE CODE\n";
    cout<<"=============================================\n\n";
    for(int i=0;i<sourceCode.size();i++) cout<<i+1<<". "<<sourceCode[i]<<endl;
    cout<<"\n"; system("pause");
}

// ==============================================
// LEXICAL ANALYSIS
// ==============================================
void lexicalAnalysis() {
    system("cls");
    cout<<"========================================\nANT COLONY - LEXICAL ANALYSIS\n========================================\n\n";
    loading("Collecting Tokens");
    int totalTokens=0, keywordCount=0, idCount=0, numCount=0, opCount=0, sepCount=0;
    lexicalErrorsCount=0;
    cout<<left<<setw(25)<<"TOKEN"<<setw(20)<<"TYPE"<<endl<<"----------------------------------------\n";
    for(int i=0;i<sourceCode.size();i++){
        string line=sourceCode[i], token="";
        for(int j=0;j<=line.length();j++){
            char ch=(j<line.length())?line[j]:' ';
            if(isalnum(ch)||ch=='_') token+=ch;
            else {
                if(token!=""){
                    totalTokens++;
                    if(isKeyword(token)){ cout<<setw(25)<<token<<setw(20)<<"Keyword"<<endl; keywordCount++; }
                    else if(isdigit(token[0])){ cout<<setw(25)<<token<<setw(20)<<"Number"<<endl; numCount++; }
                    else {
                        cout<<setw(25)<<token<<setw(20)<<"Identifier"<<endl; idCount++;
                        bool found=false;
                        for(auto &s:symbolTableData) if(s.name==token){ found=true; break; }
                        if(!found){ Symbol s; s.name=token; s.type="Unknown"; s.value="NULL"; s.line=i+1; s.scope="Global"; symbolTableData.push_back(s); }
                    }
                    token="";
                }
                if(ch==' '||ch=='\t') continue;
                string op="";
                if(isOperatorChar(ch)){
                    op+=ch;
                    if(j+1<line.length() && isOperatorChar(line[j+1])){
                        string two=op+line[j+1];
                        if(two=="=="||two=="!="||two==">="||two=="<="||two=="++"||two=="--"||two=="&&"||two=="||"){ op=two; j++; }
                    }
                    totalTokens++; cout<<setw(25)<<op<<setw(20)<<"Operator"<<endl; opCount++;
                } else if(isSeparatorChar(ch)){
                    totalTokens++; string sep=""; sep+=ch; cout<<setw(25)<<sep<<setw(20)<<"Separator"<<endl; sepCount++;
                } else { lexicalErrorsCount++; cout<<setw(25)<<ch<<setw(20)<<"Invalid Token"<<endl; }
            }
        }
    }
    cout<<"\nTotal Tokens : "<<totalTokens<<"\nKeywords: "<<keywordCount<<", Identifiers: "<<idCount<<", Numbers: "<<numCount<<", Operators: "<<opCount<<", Separators: "<<sepCount<<endl;
    if(lexicalErrorsCount>0){ cout<<"\n[ERROR] Lexical Errors Found: "<<lexicalErrorsCount<<"\n"; tokenCrystal=false; score-=lexicalErrorsCount*5; if(score<0) score=0; }
    else { cout<<"\n[SUCCESS] Token Crystal Collected\n"; tokenCrystal=true; }
    system("pause");
}

void showSymbolTable() {
    system("cls");
    cout<<"========================================\n             SYMBOL TABLE\n========================================\n\n";
    cout<<left<<setw(20)<<"NAME"<<setw(15)<<"TYPE"<<setw(15)<<"VALUE"<<setw(10)<<"LINE"<<"SCOPE"<<endl;
    cout<<"---------------------------------------------------------\n";
    for(auto &s:symbolTableData)
        cout<<setw(20)<<s.name<<setw(15)<<s.type<<setw(15)<<s.value<<setw(10)<<s.line<<s.scope<<endl;
    system("pause");
}

// ==============================================
// SYNTAX ANALYSIS
// ==============================================
void syntaxAnalysis() {
    system("cls");
    cout<<"========================================\nOWL FOREST - SYNTAX ANALYSIS\n========================================\n\n";
    loading("Checking Grammar");
    syntaxErrorsCount=0;
    for(int i=0;i<sourceCode.size();i++){
        string line=sourceCode[i]; if(line.empty()) continue;
        if(line.back()!=';'){ cout<<"[ERROR] Line "<<i+1<<" : Missing ';'\n"; syntaxErrorsCount++; }
        int openP=0,closeP=0,openB=0,closeB=0,openS=0,closeS=0;
        bool inString=false,inChar=false;
        for(char c:line){
            if(c=='"' && !inChar) inString=!inString;
            else if(c=='\'' && !inString) inChar=!inChar;
            else if(!inString && !inChar){
                if(c=='(') openP++; else if(c==')') closeP++;
                else if(c=='{') openB++; else if(c=='}') closeB++;
                else if(c=='[') openS++; else if(c==']') closeS++;
            }
        }
        if(openP!=closeP){ cout<<"[ERROR] Line "<<i+1<<" : Parentheses mismatch ()\n"; syntaxErrorsCount++; }
        if(openB!=closeB){ cout<<"[ERROR] Line "<<i+1<<" : Braces mismatch {}\n"; syntaxErrorsCount++; }
        if(openS!=closeS){ cout<<"[ERROR] Line "<<i+1<<" : Brackets mismatch []\n"; syntaxErrorsCount++; }
        if(!inString && count(line.begin(),line.end(),'"')%2==1){ cout<<"[ERROR] Line "<<i+1<<" : Missing closing double quote \"\n"; syntaxErrorsCount++; }
        if(!inChar && count(line.begin(),line.end(),'\'')%2==1){ cout<<"[ERROR] Line "<<i+1<<" : Missing closing single quote '\n"; syntaxErrorsCount++; }
        if(line.find("===")!=string::npos){ cout<<"[ERROR] Line "<<i+1<<" : Invalid operator '==='\n"; syntaxErrorsCount++; }
    }
    if(syntaxErrorsCount==0){ cout<<"\n[SUCCESS] Grammar Correct\n[CRYSTAL] Grammar Crystal Collected\n"; grammarCrystal=true; }
    else { grammarCrystal=false; score-=syntaxErrorsCount*10; if(score<0) score=0; cout<<"\n[CRYSTAL] Grammar Crystal Lost\n"; }
    system("pause");
}

// ==============================================
// SEMANTIC ANALYSIS
// ==============================================
void semanticAnalysis() {
    system("cls");
    cout<<"========================================\nEAGLE MOUNTAIN - SEMANTIC ANALYSIS\n========================================\n\n";
    loading("Checking Program Meaning");

    semanticErrorsCount=0;
    set<string> declared;
    map<string,string> varType;

    for(int i=0;i<sourceCode.size();i++){
        string line=sourceCode[i];
        stringstream ss(line);
        vector<string> words; string w;
        while(ss>>w) words.push_back(w);
        if(words.empty()) continue;

        if(isDataType(words[0]) && words.size()>=2){
            string var=words[1];
            if(!var.empty() && var.back()==';') var.pop_back();
            if(!var.empty() && var.back()=='=') var.pop_back();
            if(!declared.count(var)){
                declared.insert(var);
                varType[var]=words[0];
                for(auto &s:symbolTableData) if(s.name==var){ s.type=words[0]; s.line=i+1; s.value="0"; }
            }
        }
    }

    for(int i=0;i<sourceCode.size();i++){
        string line=sourceCode[i];
        stringstream ss(line);
        vector<string> words; string w;
        while(ss>>w) words.push_back(w);
        if(words.empty()) continue;

        size_t eq=line.find('=');
        if(eq!=string::npos){
            string lhs=line.substr(0,eq);
            lhs.erase(remove_if(lhs.begin(),lhs.end(),::isspace),lhs.end());
            string var=lhs;
            for(string t:{"int","float","double","char","string","bool"}){
                if(var.find(t)==0){ var.erase(0,t.length()); break; }
            }
            if(!var.empty() && var.back()=='=') var.pop_back();

            if(!var.empty() && !declared.count(var)){
                declared.insert(var);
                varType[var]="int";
                bool found=false;
                for(auto &s:symbolTableData) if(s.name==var){ s.type="int"; s.line=i+1; s.value="0"; found=true; }
                if(!found){ Symbol s; s.name=var; s.type="int"; s.value="0"; s.line=i+1; s.scope="Global"; symbolTableData.push_back(s); }
            }

            string rhs=line.substr(eq+1);
            rhs.erase(remove_if(rhs.begin(),rhs.end(),::isspace),rhs.end());
            if(!rhs.empty() && rhs.back()==';') rhs.pop_back();

            bool isConst=true;
            for(char c:rhs) if(!isdigit(c) && c!='.'){ isConst=false; break; }
            if(isConst && !rhs.empty()){
                for(auto &s:symbolTableData) if(s.name==var){ s.value=rhs; }
            }
            else {
                string rhsVar=rhs;
                for(char c:rhs){ if(c=='+'||c=='-'||c=='*'||c=='/'){ size_t pos=rhs.find(c); rhsVar=rhs.substr(0,pos); break; } }
                rhsVar.erase(remove_if(rhsVar.begin(),rhsVar.end(),::isspace),rhsVar.end());
                if(!rhsVar.empty()){
                    string val="0";
                    for(auto &s:symbolTableData) if(s.name==rhsVar){ val=s.value; break; }
                    for(auto &s:symbolTableData) if(s.name==var){ s.value=val; }
                }
            }
        }
    }

    for(int i=0;i<sourceCode.size();i++){
        string line=sourceCode[i];
        stringstream ss(line);
        vector<string> words; string w;
        while(ss>>w) words.push_back(w);
        if(words.empty()) continue;

        for(string word:words){
            string var=word;
            while(!var.empty() && (ispunct(var.back()) || var.back()==';')) var.pop_back();
            if(!var.empty() && !isKeyword(var) && !isDataType(var) && !isdigit(var[0]) && var!="if" && var!="while" && var!="return"){
                if(!declared.count(var)){
                    cout<<"[ERROR] Line "<<i+1<<" : Undeclared variable -> "<<var<<endl;
                    semanticErrorsCount++;
                }
            }
        }
    }

    if(semanticErrorsCount==0){ cout<<"\n[SUCCESS] Semantic Analysis Passed\n[CRYSTAL] Logic Crystal Collected\n"; logicCrystal=true; }
    else { cout<<"\n[ERROR] Semantic Errors Found: "<<semanticErrorsCount<<"\n[CRYSTAL] Logic Crystal Lost\n"; logicCrystal=false; score-=semanticErrorsCount*10; if(score<0) score=0; }
    system("pause");
}

// ==============================================
// INTERMEDIATE CODE & TAC
// ==============================================
void intermediateCode() {
    system("cls");
    cout<<"========================================\nBEE HIVE - INTERMEDIATE CODE & TAC\n========================================\n\n";
    loading("Building Honey Cells");
    tacCode.clear(); tempCounter=1;
    for(int i=0;i<sourceCode.size();i++){
        string line=sourceCode[i]; if(line.empty()) continue;
        string l=line; if(l.back()==';') l.pop_back();
        size_t eq=l.find('='); if(eq==string::npos) continue;
        string lhs=l.substr(0,eq), rhs=l.substr(eq+1);
        lhs.erase(remove_if(lhs.begin(),lhs.end(),::isspace),lhs.end());
        for(string t:{"int","float","double","char","string","bool"}) if(lhs.find(t)==0) lhs.erase(0,t.length());
        string tempVar=generateTAC(rhs);
        tacCode.push_back(lhs+" = "+tempVar);
    }
    cout<<"Generated TAC:\n"; for(auto &s:tacCode) cout<<s<<endl;
    cout<<"\n[CRYSTAL] Code Crystal Collected\n"; codeCrystal=true;
    system("pause");
}

// ==============================================
// CODE OPTIMIZATION
// ==============================================
void codeOptimization() {
    system("cls");
    cout<<"========================================\nFOX DEN - CODE OPTIMIZATION\n========================================\n\n";
    loading("Fox is Optimizing Code");
    optimizedTAC.clear();
    map<string,string> constVals;
    for(string &line:tacCode){
        string l=line; l.erase(remove_if(l.begin(),l.end(),::isspace),l.end());
        size_t eq=l.find('='); if(eq==string::npos) continue;
        string lhs=l.substr(0,eq), rhs=l.substr(eq+1);
        bool isNum=true; for(char c:rhs) if(!isdigit(c) && c!='.'){ isNum=false; break; }
        if(isNum && !rhs.empty()) constVals[lhs]=rhs;
    }
    set<string> seen;
    for(string &line:tacCode){
        string l=line; l.erase(remove_if(l.begin(),l.end(),::isspace),l.end());
        size_t eq=l.find('='); if(eq==string::npos) continue;
        string lhs=l.substr(0,eq), rhs=l.substr(eq+1);
        if(lhs==rhs) continue;
        bool hasOp=false; char op=0; size_t opPos=string::npos;
        for(char c:rhs){ if(c=='+'||c=='-'||c=='*'||c=='/'){ hasOp=true; op=c; opPos=rhs.find(c); break; } }
        if(hasOp){
            string left=rhs.substr(0,opPos), right=rhs.substr(opPos+1);
            if(constVals.count(left)) left=constVals[left];
            if(constVals.count(right)) right=constVals[right];
            bool leftConst=true, rightConst=true;
            for(char c:left) if(!isdigit(c) && c!='.'){ leftConst=false; break; }
            for(char c:right) if(!isdigit(c) && c!='.'){ rightConst=false; break; }
            if(leftConst && rightConst){
                double lv=stod(left), rv=stod(right), result;
                switch(op){ case '+': result=lv+rv; break; case '-': result=lv-rv; break; case '*': result=lv*rv; break; case '/': if(rv!=0) result=lv/rv; else result=0; break; }
                string res=to_string(result); res.erase(res.find_last_not_of('0')+1,string::npos); if(res.back()=='.') res.pop_back();
                string newLine=lhs+" = "+res;
                if(seen.count(newLine)) continue;
                seen.insert(newLine); optimizedTAC.push_back(newLine); constVals[lhs]=res;
            } else {
                string newRhs=left+op+right, newLine=lhs+" = "+newRhs;
                if(seen.count(newLine)) continue;
                seen.insert(newLine); optimizedTAC.push_back(newLine);
            }
        } else {
            if(constVals.count(rhs)){ string newLine=lhs+" = "+constVals[rhs]; if(seen.count(newLine)) continue; seen.insert(newLine); optimizedTAC.push_back(newLine); constVals[lhs]=constVals[rhs]; }
            else { if(seen.count(l)) continue; seen.insert(l); optimizedTAC.push_back(l); }
        }
    }
    set<string> usedVars;
    for(string &line:optimizedTAC){
        string l=line; l.erase(remove_if(l.begin(),l.end(),::isspace),l.end());
        size_t eq=l.find('='); if(eq==string::npos) continue;
        string rhs=l.substr(eq+1);
        stringstream ss(rhs); string token;
        while(getline(ss,token,'+') || getline(ss,token,'-') || getline(ss,token,'*') || getline(ss,token,'/')){
            token.erase(remove_if(token.begin(),token.end(),::isspace),token.end());
            if(!token.empty() && token[0]=='t' && isdigit(token[1])) usedVars.insert(token);
        }
        if(!rhs.empty() && rhs[0]=='t' && isdigit(rhs[1])) usedVars.insert(rhs);
    }
    vector<string> finalOptimized;
    for(string &line:optimizedTAC){
        string l=line; l.erase(remove_if(l.begin(),l.end(),::isspace),l.end());
        size_t eq=l.find('='); if(eq==string::npos) continue;
        string lhs=l.substr(0,eq);
        if(lhs[0]=='t' && isdigit(lhs[1]) && usedVars.count(lhs)==0) continue;
        finalOptimized.push_back(line);
    }
    optimizedTAC=finalOptimized;
    cout<<"\nOptimized TAC:\n"; for(auto &s:optimizedTAC) cout<<s<<endl;
    cout<<"\n[CRYSTAL] Speed Crystal Collected\n"; speedCrystal=true;
    system("pause");
}

// ==============================================
// FINAL CODE GENERATION
// ==============================================
void finalCodeGeneration() {
    system("cls");
    cout<<"========================================\nLION THRONE - FINAL CODE GENERATION\n========================================\n\n";
    loading("Lion is Generating Final Code");
    assemblyCode.clear();
    for(string &line:optimizedTAC){
        string l=line; l.erase(remove_if(l.begin(),l.end(),::isspace),l.end());
        size_t eq=l.find('='); if(eq==string::npos) continue;
        string lhs=l.substr(0,eq), rhs=l.substr(eq+1);
        bool hasOp=false; char op=0; size_t opPos=string::npos;
        for(char c:rhs){ if(c=='+'||c=='-'||c=='*'||c=='/'){ hasOp=true; op=c; opPos=rhs.find(c); break; } }
        if(hasOp){
            string left=rhs.substr(0,opPos), right=rhs.substr(opPos+1);
            assemblyCode.push_back("LOAD R1, "+left); assemblyCode.push_back("LOAD R2, "+right);
            string asmOp; switch(op){ case '+': asmOp="ADD"; break; case '-': asmOp="SUB"; break; case '*': asmOp="MUL"; break; case '/': asmOp="DIV"; break; }
            assemblyCode.push_back(asmOp+" R1, R2"); assemblyCode.push_back("STORE "+lhs+", R1");
        } else { assemblyCode.push_back("LOAD R1, "+rhs); assemblyCode.push_back("STORE "+lhs+", R1"); }
        assemblyCode.push_back("");
    }
    cout<<"Generated Assembly:\n"; for(auto &s:assemblyCode) cout<<s<<endl;
    royalCrystal=true; cout<<"\n[CRYSTAL] Royal Crystal Collected\n";
    system("pause");
}

// ==============================================
// GATE, STATISTICS, ERROR SUMMARY, REWARD, REPORT
// ==============================================
void showGate() {
    system("cls");
    cout<<"========================================\nROYAL COMPILER GATE\n========================================\n\n";
    cout<<"Token Crystal      : "<<(tokenCrystal?"[YES]":"[NO]")<<endl;
    cout<<"Grammar Crystal    : "<<(grammarCrystal?"[YES]":"[NO]")<<endl;
    cout<<"Logic Crystal      : "<<(logicCrystal?"[YES]":"[NO]")<<endl;
    cout<<"Code Crystal       : "<<(codeCrystal?"[YES]":"[NO]")<<endl;
    cout<<"Speed Crystal      : "<<(speedCrystal?"[YES]":"[NO]")<<endl;
    cout<<"Royal Crystal      : "<<(royalCrystal?"[YES]":"[NO]")<<endl;
    cout<<"\n";
    if(tokenCrystal && grammarCrystal && logicCrystal && codeCrystal && speedCrystal && royalCrystal){
        cout<<"ROYAL GATE OPENED\nCompilation Successful!\n";
    } else { cout<<"ROYAL GATE CLOSED\nCompilation Failed! Collect all crystals.\n"; }
    system("pause");
}

void statistics() {
    system("cls");
    cout<<"========================================\nCOMPILER STATISTICS\n========================================\n\n";
    cout<<"Total Lines : "<<sourceCode.size()<<endl;
    if(score<0) score=0;
    cout<<"Score       : "<<score<<"/100"<<endl;
    int collected=0; if(tokenCrystal) collected++; if(grammarCrystal) collected++; if(logicCrystal) collected++; if(codeCrystal) collected++; if(speedCrystal) collected++; if(royalCrystal) collected++;
    cout<<"Crystals    : "<<collected<<" / 6"<<endl;
    system("pause");
}

void errorSummary() {
    system("cls");
    cout<<"========================================\nERROR SUMMARY\n========================================\n\n";
    cout<<"Lexical Errors  : "<<lexicalErrorsCount<<endl;
    cout<<"Syntax Errors   : "<<syntaxErrorsCount<<endl;
    cout<<"Semantic Errors : "<<semanticErrorsCount<<endl;
    cout<<"----------------------------------------\n";
    cout<<"Total Errors    : "<<(lexicalErrorsCount+syntaxErrorsCount+semanticErrorsCount)<<endl;
    system("pause");
}

void rewardSystem() {
    system("cls");
    cout<<"========================================\nREWARD SYSTEM\n========================================\n\n";
    if(score>=90) cout<<"Reward : Golden Rose\n";
    else if(score>=80) cout<<"Reward : Sunflower\n";
    else if(score>=70) cout<<"Reward : Tulip\n";
    else if(score>=60) cout<<"Reward : Apple\n";
    else if(score>=50) cout<<"Reward : Orange\n";
    else cout<<"Reward : Cactus\n";
    system("pause");
}

void generateReport() {
    ofstream file("report.txt");
    file<<"=====================================\nAnimal Kingdom Mini C++ Compiler Report\n=====================================\n\n";
    file<<"--- Input Program ---\n"; for(int i=0;i<sourceCode.size();i++) file<<i+1<<". "<<sourceCode[i]<<endl; file<<"\n";
    file<<"--- Symbol Table ---\n"; file<<left<<setw(20)<<"NAME"<<setw(15)<<"TYPE"<<setw(15)<<"VALUE"<<setw(10)<<"LINE"<<"SCOPE"<<endl;
    file<<"---------------------------------------------------------\n";
    for(auto &s:symbolTableData) file<<setw(20)<<s.name<<setw(15)<<s.type<<setw(15)<<s.value<<setw(10)<<s.line<<s.scope<<endl;
    file<<"\n";
    file<<"--- Lexical Result ---\n"; file<<"Lexical Errors: "<<lexicalErrorsCount<<"\nToken Crystal: "<<(tokenCrystal?"Collected":"Not Collected")<<"\n\n";
    file<<"--- Syntax Result ---\n"; file<<"Syntax Errors: "<<syntaxErrorsCount<<"\nGrammar Crystal: "<<(grammarCrystal?"Collected":"Not Collected")<<"\n\n";
    file<<"--- Semantic Result ---\n"; file<<"Semantic Errors: "<<semanticErrorsCount<<"\nLogic Crystal: "<<(logicCrystal?"Collected":"Not Collected")<<"\n\n";
    file<<"--- Intermediate Code (TAC) ---\n"; for(auto &s:tacCode) file<<s<<endl; file<<"\n";
    file<<"--- Optimized Code ---\n"; for(auto &s:optimizedTAC) file<<s<<endl; file<<"\n";
    file<<"--- Final Code (Assembly) ---\n"; for(auto &s:assemblyCode) file<<s<<endl; file<<"\n";
    file<<"--- Statistics ---\n"; file<<"Total Lines: "<<sourceCode.size()<<"\nScore: "<<score<<"/100\n";
    int collected=0; if(tokenCrystal) collected++; if(grammarCrystal) collected++; if(logicCrystal) collected++; if(codeCrystal) collected++; if(speedCrystal) collected++; if(royalCrystal) collected++;
    file<<"Crystals Collected: "<<collected<<"/6\n\n";
    file<<"--- Errors ---\n"; file<<"Lexical: "<<lexicalErrorsCount<<"\nSyntax: "<<syntaxErrorsCount<<"\nSemantic: "<<semanticErrorsCount<<"\nTotal: "<<(lexicalErrorsCount+syntaxErrorsCount+semanticErrorsCount)<<"\n\n";
    file<<"--- Reward ---\n"; if(score>=90) file<<"Golden Rose\n"; else if(score>=80) file<<"Sunflower\n"; else if(score>=70) file<<"Tulip\n"; else if(score>=60) file<<"Apple\n"; else if(score>=50) file<<"Orange\n"; else file<<"Cactus\n";
    file.close();
    cout<<"\nreport.txt Generated Successfully!\n"; system("pause");
}

// ==============================================
// AUTO COMPILE
// ==============================================
void autoCompile() {
    system("cls");
    cout<<"========================================\n         AUTO COMPILE STARTED\n========================================\n\n";
    tokenCrystal=grammarCrystal=logicCrystal=codeCrystal=speedCrystal=royalCrystal=false;
    score=100; lexicalErrorsCount=syntaxErrorsCount=semanticErrorsCount=0;
    symbolTableData.clear(); tacCode.clear(); optimizedTAC.clear(); assemblyCode.clear();
    lexicalAnalysis(); syntaxAnalysis(); semanticAnalysis(); intermediateCode(); codeOptimization(); finalCodeGeneration();
    showGate(); statistics(); errorSummary(); rewardSystem(); generateReport();
    cout<<"\nAuto Compile Completed!\n"; system("pause");
}

// ==============================================
// MAIN FUNCTION
// ==============================================
int main() {
    welcome();
    int choice;
    do {
        system("cls");
        cout<<"=============================================\n          MAIN CONTROL PANEL                 \n=============================================\n";
        cout<<" 1. Input Source Code\n 2. View Source Code\n 3. Run Lexical Analysis (Ant Colony)\n 4. View Symbol Table\n 5. Run Syntax Analysis (Owl Forest)\n 6. Run Semantic Analysis (Eagle Mountain)\n 7. Generate Intermediate Code & TAC (Bee Hive)\n 8. Run Code Optimization (Fox Den)\n 9. Generate Final Code (Lion Throne)\n10. Check Royal Gate Status\n11. Show Statistics\n12. Show Error Summary\n13. Show Rewards\n14. Generate Text Report\n15. Auto Compile (All Phases)\n16. Exit\n";
        cout<<"=============================================\nEnter your choice (1-16): ";
        cin>>choice; cin.ignore();
        switch(choice){
            case 1: inputProgram(); break; case 2: displayCode(); break;
            case 3: lexicalAnalysis(); break; case 4: showSymbolTable(); break;
            case 5: syntaxAnalysis(); break; case 6: semanticAnalysis(); break;
            case 7: intermediateCode(); break; case 8: codeOptimization(); break;
            case 9: finalCodeGeneration(); break; case 10: showGate(); break;
            case 11: statistics(); break; case 12: errorSummary(); break;
            case 13: rewardSystem(); break; case 14: generateReport(); break;
            case 15: autoCompile(); break; case 16: cout<<"\nExiting compiler. Goodbye!\n"; break;
            default: cout<<"\nInvalid choice! Try again.\n"; system("pause");
        }
    } while(choice!=16);
    return 0;
}
