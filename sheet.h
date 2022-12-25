#pragma once

#include<iostream>
#include<sstream>
#include<iomanip>
#include<cstring>
#include<fstream>
#include<stack>

using namespace std;

const int MAXROWS=31;
bool STAY=1;

class Sheet{
	int xP,yP; // position values used for printing to terminal
	string value[9][MAXROWS]; // 270 cell locations, columns A-I, rows 0-30
	stack<double> internal; // internal stack for parsing equations
	bool disp; // display flag, 0=default (display values) 1=flagged (display raw cell contents)
	string menu[2];
	bool menuFlag; // flag, 0=default (default menu) 1=flagged (file/quit operations)
	public:
		Sheet(){
			xP=0; // init print positions
			yP=0;
			for(int i=0;i<9;i++) value[i][0]=(char)i+65; // init column headers
			for(int i=0;i<9;i++){
				for(int j=1;j<MAXROWS;j++){
					value[i][j]=""; // init all other cells
				}
			}
			// internal.push(0); // provision for a "bottom out" value
			disp=0;
			menuFlag=0;
			menu[0]="Menu toggle (menu), delete cell (del A1), toggle display (tog), or input cell contents (A1=...): ";
			menu[1]="Menu toggle (menu), quit (quit), save (save), or load (load): ";
		}
		void menuFcn(){
			cout << menu[menuFlag];
		}
		friend istream &operator >>(istream &in,Sheet &other){
			string temp;
			getline(in,temp); // take whole line as input
			stringstream parse(temp);
			string subTemp;
			char x;
			int i=0,y;
			if(!other.menuFlag&&temp.substr(0,3)=="del"){ // clear cell contents
				other.delCell(temp.substr(4),other);
				return in;
			}
			else if(!other.menuFlag&&temp.substr(0,3)=="tog"){ // toggle cell display mode
				other.disp=!other.disp; // toggle disp variable
				return in;
			}
			else if(temp.substr(0,4)=="menu"){ // toggle menu
				other.menuFlag=!other.menuFlag;
				return in;
			}
			else if(other.menuFlag&&temp.substr(0,4)=="quit"){
				STAY=0;
				return in;
			}
			else if(other.menuFlag&&temp.substr(0,4)=="save"){
				// TODO: implement save to single file option
				return in;
			}
			else if(other.menuFlag&&temp.substr(0,4)=="load"){
				//  gets contents of dataFile line-by-line
				//  inf is the ifstream object
				ifstream data;
				string dIn;
				data.open("sheet.csv"); // data file to store/read from
				for(int i=0;i<MAXROWS;i++){
					getline(data,dIn,','); // comma delimit
					// TODO insert into current sheet
				}
				data.close();
				return in;
			}
			else if(!other.menuFlag&&temp.find("=")!=string::npos){
				try{
					while(!parse.eof()){ // parse coords and initialize raw contents
						getline(parse,subTemp,'='); // seperate cell assignment
						if(!i){
							x=subTemp[0]; // get x coord
							y=stoi(subTemp.substr(1)); // get y coord
						}
						else other.value[x-65][y]=subTemp; // store raw contents
						i++;
					}
				} catch(exception &e){
					// invalid input, do nothing
				}
			}
			return in;
		}
		friend ostream &operator <<(ostream &out,Sheet &other){
			// check for empty cell
			if(other.value[other.xP][other.yP].empty()) out << "";
			// parse contents on print (allows for live referencing)
			else{
				if(!other.disp){
					try{
						out << other.contentParse(other.value[other.xP][other.yP]);
					} catch(exception &e){
						/* this section of code alters the cell with an invalid reference, replacing prior contents
						// output & set cell to "REF ERR" (signify reference error) but allow continued printing
						stringstream errIn;
						errIn << (char)(other.xP+65) << other.yP << "=REF ERR"; // build new string in format "A1=REF ERR"
						errIn >> other; // update cell with error state
						*/
						out << "REF_ERR<<113"; // print error message to cell location
					}
				}
				else out << other.value[other.xP][other.yP]; // output raw cell contents
			}
			if(other.xP==8){ // increment print coords, reset if
				other.xP=0;
				if(other.yP==MAXROWS-1) other.yP=0;
				else other.yP++;
			}
			else other.xP++;
			return out;
		}
		string contentParse(string temp){ // parse cell contents
			// string case (non equation):
			if(temp[0]=='\"') return temp.substr(1,temp.length()-2);
			// double case (non equation):
			else if(temp.find(' ')==string::npos){
				// need to resolve single cell references (no equation)
				if((temp.length()>1)&&(temp[0]>=65&&temp[0]<=73)){
					try{
						int y=stoi(temp.substr(1));
						// get target cell contents and convert to double
						// this retains consistent display of double precision
						double t=stod(contentParse(value[temp[0]-65][y]));
						temp=to_string(t); // convert back to string
						return temp;
					} catch(exception &e){
						// reference error likely, return error
						return "REF_ERRcp142";
					}
				}
				else{
					if(temp[0]>=48&&temp[0]<=57){
						double t=stod(temp); // convert to double
						temp=to_string(t); // convert back to string
					}
					return temp;
				}
			}
			// equation cases:
			else{
				stringstream parse(temp);
				string subTemp;
				int i=0;
				while(!parse.eof()){ // parse stringstream and use stack to allow chaining equations
					getline(parse,subTemp,' ');
					if(subTemp[0]>=65&&subTemp[0]<=73){ // if referencing cell
						int x=subTemp[0]-65;
						int y=stoi(subTemp.substr(1));
						try{
							internal.push(stod(contentParse(value[x][y]))); // push parse to stack
						} catch(exception &e){
							// reference error likely, return error
							return "REF_ERRcp167";
						}
					}
					else if(subTemp=="+"||
							subTemp=="-"||
							subTemp=="*"||
							subTemp=="/"){
						try{
							if(subTemp=="+"){
								double a=internal.top();
								internal.pop();
								double b=internal.top();
								internal.pop();
								internal.push(a+b);	
							} else if(subTemp=="-"){
								double b=internal.top();
								internal.pop();
								double a=internal.top();
								internal.pop();
								internal.push(a-b);
							} else if(subTemp=="*"){
								double b=internal.top();
								internal.pop();
								double a=internal.top();
								internal.pop();
								internal.push(a*b);
							} else if(subTemp=="/"){
								double b=internal.top();
								internal.pop();
								double a=internal.top();
								internal.pop();
								internal.push(a/b);
							}
						} catch(exception &e){
							// return "REF ERR" (reference error) but allow continued input
							return "REF_ERRcp202";
						}
					}
					else internal.push(stod(subTemp)); // if double push to stack
					i++;
				}
				string retOut=to_string(internal.top());
				internal.pop();
				return retOut;
			}
		}
		void delCell(string temp,Sheet &other){
			int y=stoi(temp.substr(1));
			other.value[temp[0]-65][y]="";
		}
};