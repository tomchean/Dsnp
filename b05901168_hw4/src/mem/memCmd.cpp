/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
    int num= -1;
    int arraysize = -1;
   	bool array = false;
    vector<string> token;   
	if (!CmdExec::lexOptions(option, token))
		return CMD_EXEC_ERROR;
	if (token.empty()){
			return CmdExec::errorOption(CMD_OPT_MISSING,"");
	}
	for (size_t i =0; i <token.size();i++){
		if(myStrNCmp("-Array",token[i],2) == 0){
			if (array) return CmdExec::errorOption(CMD_OPT_EXTRA,token[i]);
			else array = true;
			if (i == token.size()-1) return CmdExec::errorOption(CMD_OPT_MISSING,token[i]);
			else {
				i++;
				if (myStr2Int(token[i],arraysize) && arraysize >0)	;		
				else  return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[i]);
			}
		}
		else {
			if ( (num == -1)){
				if (myStr2Int(token[i],num) && num >0) ;
				else  return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[i]);
			}
			else	return CmdExec::errorOption(CMD_OPT_EXTRA,token[i]);

		}
	}
	if (num==-1) return CmdExec::errorOption(CMD_OPT_MISSING,"");
	if (array){
		try {
			mtest.newArrs(num , arraysize);
		} catch (bad_alloc& ){
			return CMD_EXEC_DONE;
		}
	}
	else {
		try {
			mtest.newObjs(num);
		} catch (bad_alloc& ){
			return CMD_EXEC_DONE;
		}
	}
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO

	int num=-1;
	int mode=-1;
    vector<string> token;
	bool array;
	bool order = true;  
	if (!CmdExec::lexOptions(option, token))
		return CMD_EXEC_ERROR;
	if (token.empty()){
			return CmdExec::errorOption(CMD_OPT_MISSING,"");
	}
	for (size_t i =0; i <token.size();i++){
		if (myStrNCmp("-Index",token[i],2)==0){
			if (mode != -1) return CmdExec::errorOption(CMD_OPT_EXTRA,token[i]);
			else	mode =1;
			if (i == token.size()-1) return CmdExec::errorOption(CMD_OPT_MISSING,token[i]);
			else {
				i++;
				if (myStr2Int(token[i],num) && num >=0) ;
				else  return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[i]);
			}
		}
		else if (myStrNCmp("-Random",token[i],2)==0){
			if (mode != -1) return CmdExec::errorOption(CMD_OPT_EXTRA,token[i]);
			else	mode =2;
			if (i == token.size()-1) return CmdExec::errorOption(CMD_OPT_MISSING,token[i]);
			else {
				i++;
				if (myStr2Int(token[i],num) && num >=0) ;
				else  return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[i]);
			}
		}
		else if(myStrNCmp("-Array",token[i],2) == 0){
			if (array) return CmdExec::errorOption(CMD_OPT_EXTRA,token[i]);
			else array = true;
			if (i == 0) order = false;
		}
		else	return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[i]);
	}
	if (mode == -1) return CmdExec::errorOption(CMD_OPT_MISSING,"");
	if (array){
		if (mode ==1){
			if ((size_t)num >= mtest.getArrListSize()) {
				cerr <<"Size of array list ("<<mtest.getArrListSize()<<") is <= "<<num <<"!!"<<endl; 
			if (order)return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[1]);
			else return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[2]);
			}	
			mtest.deleteArr(num);
		}
		else{
			if (mtest.getArrListSize()==0) {
				cerr <<"Size of array list is 0!!"<<endl;
			if (order)return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[0]);
			else return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[1]);				
			}
			for (int i=0;i<num;i++ ){
				mtest.deleteArr(rnGen(mtest.getArrListSize()));
			}
		}
	}
	else {
		if (mode ==1){
			if (((size_t)num) >= mtest.getObjListSize() ){
				cerr <<"Size of object list ("<<mtest.getObjListSize()<<") is <= "<<num <<"!!"<<endl; 
				return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[1]);
			}
			mtest.deleteObj(num);
		}  
		else {
			if (mtest.getObjListSize()==0) {
				cerr <<"Size of object list is 0!!"<<endl;
				return CmdExec::errorOption(CMD_OPT_ILLEGAL,token[0]);	
			}
			for (int i=0;i<num;i++ ){
				mtest.deleteObj(rnGen(mtest.getObjListSize()));
			}
		}
	}

    return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


