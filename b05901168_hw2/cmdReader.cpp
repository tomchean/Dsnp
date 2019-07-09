/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"
using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();
   int size = _readBufEnd - _readBufPtr ;
   while (1) {
      
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY :if(moveBufPtr(_readBufPtr-1))  deleteChar();
                               /* TODO */
                              break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr+1);/* TODO */ break;
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr-1);/* TODO */ break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : insertChar(' ',TAB_POSITION);/* TODO */ break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable characte
            insertChar(char(pch)); break;
      }
      /*
      cout<<"\r";
      size = _readBufEnd - _readBuf+15;
      for (int j =0; j <size ; j++) 
        cout <<" ";
      cout <<"\rcmd> "<<_readBuf ;
      size = _readBufEnd - _readBufPtr ; 
      for (int j =0; j <size ; j++) 
        cout <<"\b";
      */
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
//ok
bool
CmdParser::moveBufPtr(char* const ptr)
{ 
  
   // TODO...
  if (ptr>=_readBuf && ptr <=_readBufEnd){
    
    if (ptr>_readBufPtr){
      for (int i = 0 ; i< ptr - _readBufPtr  ; i++) cout <<*(_readBufPtr+i) ; 
    }
    else if (ptr<_readBufPtr) {
      for (int i =  _readBufPtr - ptr  ; i > 0 ; i--) cout <<"\b"; 
    }
    _readBufPtr = ptr;
    return true;
  }
  else {
    mybeep();
    return false;
  }
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
// ok
bool
CmdParser::deleteChar()
{
   // TODO...
   
   if(_readBufPtr != _readBufEnd) {
     for (char* i = _readBufPtr ; i< _readBufEnd ; i++ ){
      cout << *(i+1);      
     }
     cout <<" ";
     for (char* i = _readBufPtr ; i< _readBufEnd ; i++ ){
       cout << "\b";
     }
     
     for (char* i=_readBufPtr; i<=_readBufEnd;i++)
     {
        *i = *(i+1) ;
     }
     _readBufEnd -=1;
     return true;
   }
   else {
     mybeep();
     return false ;
  }

}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//ok
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...   
   assert(repeat >= 1);
   for (int i = repeat ; i >0 ; i--) cout << ch ;
   for (char* i = _readBufPtr ; i< _readBufEnd ; i++ ) cout << *i;
   for (char* i = _readBufPtr ; i< _readBufEnd ; i++ ) cout << "\b";
   for (char* i=_readBufEnd; i>=_readBufPtr;i--)
    {
      *(i+repeat) = * i ;
    }
   for (int i =0 ; i<repeat ; i++)
    {
      *(_readBufPtr+i) = ch ;
    }
    _readBufEnd+=repeat;
   
    _readBufPtr = _readBufPtr+repeat;
    //moveBufPtr(_readBufPtr+repeat);
  
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
   cout <<"\rcmd> " ;
   for (char* i =_readBuf ; i <_readBufEnd ; i++ ) cout<<" ";
   for (char* i =_readBuf ; i <_readBufEnd ; i++ ) cout<<"\b";
   for (char* i =_readBuf ; i <_readBufEnd ; i++ ){
     *i = 0;
   }
   _readBufPtr = _readBufEnd = _readBuf;
   *_readBufPtr = 0;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
  if (_history.size()!=0){
    if (index < _historyIdx){  //moving up 
      if (index <0) index =0;
      if (_historyIdx == 0){
         mybeep();
         goto nothing;
     }
      else if (_historyIdx == _history.size()){
       _history.push_back(_readBuf);
       _tempCmdStored = true;
      }
    }
    else {   //moving down  
     if (!_tempCmdStored )
     {
     if(_historyIdx >=_history.size()-1 ){
        mybeep();
        goto nothing;
     }
     if(index >= _history.size())
        index = _history.size() - 1;
     }
     else {
       if(index == _history.size()-1)  //enter temp 
         {
           _history.pop_back();
           _tempCmdStored = false ;
         } 
     }
    }
   _historyIdx = index ;
    retrieveHistory();
  }
  else mybeep();
  nothing : ;
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
  char* start = _readBuf;
  char* end = _readBufEnd;
  while( *start == ' '){
    start=start+1;
  }
  while ( *end == ' '){
    end = end -1;
  }
  int start_index = start-_readBuf;
  int size = end-start;
  string temp = _readBuf;
  temp=temp.substr(start_index,size);
  
  if (_tempCmdStored == false) 
  {
    if (temp.size() != 0 ) _history.push_back(temp);
  }
  else 
  {
    if (temp.size() != 0 ) _history[_history.size()-1] =  temp;
    else _history.pop_back();
    _tempCmdStored = false;
  }
  
  _historyIdx = _history.size() ;
  //deleteLine();
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
   
}
