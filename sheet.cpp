/*******************************************************************************
 *	INSTRUCTIONS: Write a program that allows a person to create a spreadsheet.
 *	BY: Jacob B. Seman, 3/31/2022
*******************************************************************************/
#include"sheet.h"

using namespace std;

void display(Sheet &sheet){ // display current sheet
	// should call all cell-contained post-fix equations
	for(int i=0;i<MAXROWS+2;i++) printf("\033[A\33[2K"); // clear 33 lines
	cout << "Post-Fix_Spreadsheet" << setw(100) << setfill('_') // print title bar status
		 << right << sheet.titleStat << setfill(' ') << endl; // print top border
	for(int i=0;i<MAXROWS;i++){
		printf("%02i",i); // print row numbers
		for(int j=0;j<9;j++){
			// sheet will auto increment with successive insertion overloads
			cout << '|' << setw(12) << left << sheet;
			if(j==8) cout << '|' << endl;
		}
	}
}

int main(){
	Sheet sheet;
	while(STAY){ // from sheet.h, global bool flag modified by extraction overload case event
		display(sheet);
		sheet.menuFcn();
		cin >> sheet;
	}
	return 0;
}