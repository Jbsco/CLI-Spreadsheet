#pragma once

#include<iostream>
#include<sstream>
#include<iomanip>
#include<cstring>
#include<fstream>
#include<stack>
#include<time.h>

using namespace std;

const int MAXROWS=31;
bool STAY=1;

class Sheet{
	int xP,yP; // position values used for printing to terminal
	string value[9][MAXROWS]; // 270 cell locations, columns A-I, rows 0-30
	stack<double> internal; // internal stack for parsing equations
	bool disp; // display flag, 0=default (display values) 1=flagged (display raw cell contents)
	string menu[2]; // two states for menu options display
	bool menuFlag; // flag, 0=default (default menu) 1=flagged (file/quit operations)
	public:
		string titleStat; // title bar status
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
			menu[1]="Menu toggle (menu), quit (quit), save (save filename), or load (load filename): ";
			titleStat="";
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
				string filename=temp.substr(5); // get filename contents
				filename.append(".csv"); // append file type
				other.yP=1; // skip column headers
				ofstream data;
				data.open(filename); // use prior filename input
				for(int i=1;i<MAXROWS;i++){
					while(other.xP<9){
						data << (char)(other.xP+65) << other.yP << '=' << other.value[other.xP][other.yP] << ',';
						other.xP++;
					}
					other.xP=0,other.yP++;
					data << endl;
				}
				other.xP=0,other.yP=0;
				data.close();
				// modify title bar string to include name of file saved and local time
				other.titleStat="Saved ";
				other.titleStat.append(filename);
				other.titleStat.append(" at ");
				// get local time
				time_t rawtime;
				struct tm *timeinfo;
				char tempTime[10];
				time(&rawtime);
				timeinfo=localtime(&rawtime);
				strftime(tempTime,10,"%I:%M%p!",timeinfo);
				// append
				other.titleStat.append(tempTime);
				other.menuFlag=!other.menuFlag;
				return in;
			}
			else if(other.menuFlag&&temp.substr(0,4)=="load"){
				// clear sheet
				for(int i=0;i<9;i++){
					for(int j=1;j<MAXROWS;j++){
						other.value[i][j]=""; // init all other cells
					}
				}
				// gets contents of dataFile line-by-line
				// data is the ifstream object
				string dIn,dRowIn,dEntry,errCell;
				string filename=temp.substr(5); // get filename contents
				filename.append(".csv"); // append file type
				try{
					ifstream data;
					data.open(filename); // data file to store/read from
					while(getline(data,dIn)){ // parse all lines in file
						stringstream dataRow(dIn);
						while(!dataRow.eof()){ // parse entire row
							getline(dataRow,dRowIn,','); // comma delimit, get cell entries
							stringstream dataEntry(dRowIn); // create stream
							// cout << dRowIn << ' ' << dRowIn.find('=') << ' ' << dRowIn.length()-1 << endl; // testing cell entries and characteristics
							int pos=dRowIn.find('='),len=dRowIn.length()-1;
							if(pos!=len){ // if cell contains anything
								i=0; // reset i to use for seperating two parts of cell entry
								while(!dataEntry.eof()){ // parse coords and initialize raw contents
									getline(dataEntry,dEntry,'='); // seperate cell assignment
									if(!i){
										x=dEntry[0]; // get x coord
										y=stoi(dEntry.substr(1)); // get y coord
										errCell=dEntry; // update errCell for title bar status display of last cell if exception caught
									}
									else other.value[x-65][y]=dEntry; // store raw contents
									i++;
								}
							}
						}
					}
					data.close();
					// modify title bar string to add load success status
					other.titleStat="Loaded ";
					other.titleStat.append(filename);
					other.titleStat.append(" successfully!");
					other.menuFlag=!other.menuFlag;
				} catch(exception &e){
					// load exception, cell may have contained a comma (the delimiting character)
					// modify title bar string to add load error status
					other.titleStat="Load error in ";
					other.titleStat.append(filename);
					other.titleStat.append(" at cell ");
					other.titleStat.append(errCell);
					other.titleStat.append("! Check for errant comma and try again.");
				}
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
						out << other.contentParse(other.value[other.xP][other.yP]).substr(0,12);
					} catch(exception &e){
						/* this section of code alters the cell with an invalid reference, replacing prior contents
						// output & set cell to "REF ERR" (signify reference error) but allow continued printing
						stringstream errIn;
						errIn << (char)(other.xP+65) << other.yP << "=REF ERR"; // build new string in format "A1=REF ERR"
						errIn >> other; // update cell with error state
						*/
						out << "REF_ERR<<145"; // print error message to cell location
					}
				}
				else out << other.value[other.xP][other.yP].substr(0,12); // output raw cell contents
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
						return "REF_ERRcp174";
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
							return "REF_ERRcp199";
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
							return "REF_ERRcp234";
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