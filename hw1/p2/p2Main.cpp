#include <iostream>
#include <cstdlib>
#include "p2Table.h"

using namespace std;

int main()
{
   Table table;

   // TODO: read in the csv file
   string csvFile,cmd;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else exit(-1); // csvFile does not exist.
   // TODO read and execute commands
   while (cin >> cmd) {
        if(cmd == "PRINT")
			table.print();
		else if (cmd == "SUM")
		{
			int col_in;
			cin >> col_in;
			table.sum(col_in);

		}
   		else if (cmd == "MAX")
		{
			int col_in;
			cin >> col_in;
			table.max(col_in);

		}
		else if (cmd == "MIN")
		{
			int col_in;
			cin >> col_in;
			table.min(col_in);

		}
		else if (cmd == "COUNT")
		{
			int col_in;
			cin >> col_in;
			table.count(col_in);

		}
		else if (cmd == "SUM")
		{
			int col_in;
			cin >> col_in;
			table.sum(col_in);

		}
		else if (cmd == "AVE")
		{
			int col_in;
			cin >> col_in;
			table.ave(col_in);
		}
		else if (cmd == "ADD")
		{
			string tmp;
			vector<string> input;
			for(int i = 0; i < table.col_t; i++)
			{
				cin >> tmp;
				input.push_back(tmp);
			}
			table.add(input);
		}
		else if (cmd == "EXIT")
			break;
	}
}
