#pragma once

#include<iostream>
#include<sstream>
#include<iomanip>
#include<cstring>

using namespace std;

const int MAXROWS=31;

class Sheet{
	int xP,yP; // position values used for printing to terminal
	string value[9][MAXROWS]; // 270 cell locations, columns A-I, rows 0-30
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
		}
		friend istream &operator >>(istream &in,Sheet &other){
			string temp;
			getline(in,temp); // take whole line as input
			stringstream parse(temp);
			string subTemp;
			char x;
			int i=0,y;
			if(temp.substr(0,3)=="del"){ // clear cell contents
				other.delCell(temp.substr(4),other);
				return in;
			}
			while(!parse.eof()){ // parse coords and initialize raw contents
				getline(parse,subTemp,'='); // seperate cell assignment
				if(!i){
					x=subTemp[0]; // get x coord
					y=stoi(subTemp.substr(1)); // get y coord
				}
				else other.value[x-65][y]=subTemp; // store raw contents
				i++;
			}
			return in;
		}
		friend ostream &operator <<(ostream &out,Sheet &other){
			// check for empty cell
			if(other.value[other.xP][other.yP].empty()) out << "";
			// parse contents on print (allows for live referencing)
			else out << other.contentParse(other.value[other.xP][other.yP]);
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
					int y=stoi(temp.substr(1));
					// get target cell contents and convert to double
					// this retains consistent display of double precision
					double t=stod(contentParse(value[temp[0]-65][y]));
					temp=to_string(t); // convert back to string
					return temp;
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
				double xVar,yVar;
				char math;
				while(!parse.eof()){
					getline(parse,subTemp,' ');
					if(!i){
						if(subTemp[0]>=65&&subTemp[0]<=73){ // if referencing cell
							int y=stoi(subTemp.substr(1));
							xVar=stod(contentParse(value[subTemp[0]-65][y]));
						}
						else xVar=stod(subTemp); // if double
					}
					else if(i==1){
						if(subTemp[0]>=65&&subTemp[0]<=73){ // if referencing cell
							int y=stoi(subTemp.substr(1));
							yVar=stod(contentParse(value[subTemp[0]-65][y]));
						}
						else yVar=stod(subTemp); // if double
					}
					else if(i==2) math=subTemp[0];
					i++;
				}
				if(math=='+') return to_string(xVar+yVar);
				else if(math=='-') return to_string(xVar-yVar);
				else if(math=='*') return to_string(xVar*yVar);
				else if(math=='/') return to_string(xVar/yVar);
				else return "NULL";
			}
		}
		void delCell(string temp,Sheet &other){
			int y=stoi(temp.substr(1));
			other.value[temp[0]-65][y]="";
		}
};