/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   // TODO...
   string command;
   _dofile = new ifstream(dof.c_str());
   if (! (*_dofile) ){ // load unsuccessful 
     delete _dofile;
     if(_dofileStack.size() > 0){
       _dofile = _dofileStack.top();
     }
     else _dofile = 0;
     return  false;
   }   
   else{
     if (_dofileStack.size() > 1023 ){ // load successful but stack size >1203 ,pop all _dofilestack
      for (size_t i = 0 ; i < _dofileStack.size() ; i++ ){
        _dofileStack.pop(); 
      } 
      _dofile = 0;
      return false ;
    }
    else {  // load successful and push _dofile on top 
      _dofileStack.push(_dofile);
      _dofile = _dofileStack.top();
      return true;
    }
   }
    
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
   assert(_dofile != 0);
   _dofile->close();
   delete _dofile;
   _dofileStack.pop();
   if(_dofileStack.size() > 0){
     _dofile = _dofileStack.top();
   }
   else _dofile =0;
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);

   // execute the command
   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   // TODO...
  for ( map<const string, CmdExec*>::const_iterator index = _cmdMap.begin() ; index != _cmdMap.end() ; index ++ ){
    index->second->help();
  }
  cout<<endl;
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
   assert(_tempCmdStored == false);
   assert(!_history.empty());
   string str = _history.back();
   assert(str[0] != 0 && str[0] != ' ');
   string command;
   int end = myStrGetTok(str,command);
   //option = option.substr(end);
   if( getCmd(command) != 0 ) {
     if ( end != -1){
       option = str.substr(end);
     }
     else option ="";
     return getCmd(command);
    }
   else {
     cerr <<"Illegal command!! (" << command <<")";
     return 0;
   }
   // TODO...
}


void
CmdParser::listCmd(const string& str)
{
   // TODO...
    size_t begin = str.find_first_not_of(' ', 0);
    if ( begin == string::npos ){  // no word ,print all command 
      cout << endl ; 
      string command;
      int count =0;
      for ( map<const string, CmdExec*>::const_iterator index = _cmdMap.begin() ; index != _cmdMap.end() ; ++index  ){
        command = index->first;
        command.append(index->second->getOptCmd());
        cout << setw(12) << left << command ;
        count ++;
        if ((count % 5) == 0) cout << endl;
      }
      reprintCmd();
    }
    else{
      vector<string> tempcmd; //store command input
      mylexOptions ( str , tempcmd ,0);
      if (tempcmd.size() == 1){   // one word 
        vector<map<const string, CmdExec*>::const_iterator> temp;
        match( temp, tempcmd[0]);
        if (temp.size() == 0) {  //no match 
          mybeep();
        }
        else if (temp.size() == 1){
          if (*(_readBufPtr-1) == ' '){  // match command and at least one space between first word
            if ( _tabPressCount == 1 ){     // match command and at least one space between first word and first tab
              cout <<endl;
              temp[0]->second->usage(cout);
              _tabPressCount += 1;
              reprintCmd();
            }
            else {        // match command and at least one space between first word and two more tab
              string s;
              vector<string> files;
              listDir( files, s, "." );
              if (files.size() == 0){
                mybeep();
              }
              else {
                if (cmpfiles(files,0)) ;
                else{
                  printfile(files);
                  reprintCmd();
                }
              }
            }
          }
          else {    // match command and ptr at the first 's last 
            string command = temp[0]->first + temp[0]->second->getOptCmd();
            for (size_t i = tempcmd[0].size() ; i < command.size() ; i++ )
              insertChar(command[i], 1);
            insertChar(' ' , 1 );
            _tabPressCount =0;
          }                
        }
        else {  // multiple match 
          if ( *(_readBufPtr-1) != ' ' ){
            cout << endl;
            for (size_t i =0 ; i< temp.size() ; i++){
              cout << setw(12) << left << temp[i]->first + temp[i]->second->getOptCmd() ;
              if (i%5 == 4) cout << endl;
            }
            reprintCmd();
          }
          else mybeep();
        }
      }  
      else if (tempcmd.size() >=1 ){  //command with option 
        vector<map<const string, CmdExec*>::const_iterator> temp;
        match( temp, tempcmd[0]);
        if (temp.size() == 0){  // case 7 two or more words , but the first word doesn't match
          mybeep();
        }
        else if (temp.size() == 1) {  // match command  
          if (*(_readBufPtr-1) ==' '){  // match command and at least one space between first word
            if (_tabPressCount == 1 ){     // match command and at least one space between first word and first tab
                  cout <<endl;
                  temp[0]->second->usage(cout);
                  _tabPressCount += 1;
                  reprintCmd();
            }
            else {        // match command and at least one space between first word and two more tab
              string s;
              vector<string> files;
              listDir( files, s, "." );
              if (files.size() == 0){
                mybeep();
              }
              else {
                if (cmpfiles(files,0)) ;
                else{
                  printfile(files);
                  reprintCmd();
                }
              }
            }
          }
          else {    // match command and ptr at the second or third ...words
            if (_tabPressCount == 1 ){
              cout <<endl;
              temp[0]->second->usage(cout);
              _tabPressCount += 1;
              reprintCmd();
            }
            else{
              vector<string> files;
              listDir( files ,tempcmd[tempcmd.size()-1] ,"." );
              if (files.size() == 0 ){
                mybeep();
              }
              else {
                if (cmpfiles(files,tempcmd[tempcmd.size()-1].size())) ;
                else {
                  printfile(files);
                  reprintCmd();
                }
              }
            }
          }
        }
        else {    // case 7 two or more words , but the first word doesn't match
          mybeep();
        }
      }
    }


}   

bool CmdParser::cmpfiles(const vector<string>& files, int k){
  if ( files.size() == 0) return false;
  bool equal =false;
  if ( files.size() == 1) {
    for(size_t i = k ; i < files[0].size() ; i++){
      insertChar(files[0][i]);
    }
    insertChar(' ');
    return true ;
  }
  for(size_t i = k ; i < files[0].size() ; i++){
    for (size_t j = 0 ; j < files.size() ; j++){
      if (files[0][i] != files[j][i]) goto end;
    } 
    insertChar(files[0][i]);
    equal = true ;
  }
  end : 
  if(equal == true) mybeep();
  return equal;
}

void CmdParser::printfile(const vector<string>& files){
  int count =0;
  cout <<endl;
  for (size_t i =0 ; i < files.size() ; i++){
    cout << setw(16) << left << files[i];
    count++;
    if (count%5 == 0) cout <<endl;
  }
}

int  CmdParser::match(vector<map<const string, CmdExec*>::const_iterator> & temp , const string cmd){
  string command;
  for ( map<const string, CmdExec*>::const_iterator index = _cmdMap.begin() ; index != _cmdMap.end() ; ++index  ){
    command = index->first;
    command.append(index->second->getOptCmd());
    int match = StrCmp (  command , cmd , index->first.size() );
    if (match == 0){
      for (size_t i =0 ; i < temp.size() ; i++){
        temp.pop_back();
      }
      temp.push_back(index);
      return 0;
    }
    else if (match == 1){
      temp.push_back(index);
    }
  }
  return 0;
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
   CmdExec* e = 0;
   // TODO...
  string command;
  for ( map<const string, CmdExec*>::const_iterator index = _cmdMap.begin() ; index != _cmdMap.end() ; ++index  ){
    command = index->first;
    command.append(index->second->getOptCmd());
    if ( myStrNCmp (  command , cmd , index->first.size() ) == 0 ){
      e = index->second;
    }
  }   
  return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token
bool
CmdExec::lexNoOption(const string& option) const
{
   string err;
   myStrGetTok(option, err);
   if (err.size()) {
      errorOption(CMD_OPT_EXTRA, err);
      return false;
   }
   return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}


bool
CmdParser::mylexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
        
         return false;
      }
      if (tokens.size() > nOpts) {

         return false;
      }
   }
   return true;
}

int
CmdParser::StrCmp(const string& s1, const string& s2, unsigned n)
{
   assert(n > 0);
   unsigned n2 = s2.size();
   if (n2 == 0) return -1;
   unsigned n1 = s1.size();
   assert(n1 >= n);
   for (unsigned i = 0; i < n1; ++i) {
      if (i == n2)
         return (i < n)? 1 : 0;
      char ch1 = (isupper(s1[i]))? tolower(s1[i]) : s1[i];
      char ch2 = (isupper(s2[i]))? tolower(s2[i]) : s2[i];
      if (ch1 != ch2)
         return 2;
   }
   return n2-n1;
}
