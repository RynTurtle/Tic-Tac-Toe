#include <iostream>
#include <string>
#include <unordered_map>
#include <windows.h>
#include <mmsystem.h>
#include <fstream>
#include "include/nlohmann/json.hpp"


// link windm .libs for playsound function to work 
#pragma comment( lib, "winmm" )

using namespace std;
using json = nlohmann::json;



// global variables 
const int ROWS = 3; // number of rows
const int COLS = 3; // number of columns 
char grid[ROWS][COLS]; // the grid of naughts and crosses 
char input;
int current_player = 1;
int total_squares;

string welcome_message =
R"(
    XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO
    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-X
    X Welcome to noughts and crosses O
    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-X
    X       1. Start new game        O
    O       2. Learn how to play     X
    X       3. View stats            O
    O       4. Quit game             X
    X+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-O
    OXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOX
)";

	

string game_message =
R"(
    XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO
    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-X
    X          New game              O
    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-X
    X       1. 2 Players             O
    O       2. Against Cross-o-Bot   X
    X       3. Return to main menu   O
    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-X
	XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO
)";

	

string grid_display =
R"(
	XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO
	O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ X
	X         Player 1: Ryan         O
	O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ X
	X                                O
	O            |     |             X
	X        r   |  r  |  r          O
	O       _____|_____|_____        X
	X            |     |             O
	O        r   |  r  |  r          X
	X       _____|_____|_____        O
	O            |     |             X
	X        r   |  r  |  r          O
	O            |     |             X
	X                                O
	O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ X
	XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO
	
    
)";

	

// map within a map containing the usernames player1:name1, player2:name2
std::unordered_map<int, std::string> player_usernames;


void clear_table() {
	// reset the table 
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			grid[row][col] = ' ';
		}
	}
}

string coloured_text(string wanted_colour, string original_text) {
	// clour output using ansi colour codes.


	if (wanted_colour == "red") {
		// bold high intensity blue
		original_text = "\x1b[1;91m" + original_text;
	}

	else if (wanted_colour == "blue") {
		// bold high intensity blue
		original_text = "\x1b[1;94m" + original_text;
	}

	else if (wanted_colour == "purple") {
		original_text = "\x1b[0;35m" + original_text;
	}

	else if (wanted_colour == "green") {
		original_text = "\x1b[0;32m" + original_text;
	}

	else if (wanted_colour == "yellow") {
		original_text = "\x1b[0;33m" + original_text;
	}

	else if (wanted_colour == "white") {
		// bold high intensity white
		original_text = "\x1b[1;97m" + original_text;
	}
 
	// ads the reset colour code so it doesnt continue
	return original_text + "\x1b[0m";
}


string DisplayControls() {
	string controls = "";
	controls += "------\n";
	controls += "controls \n";
	controls += "------ \n";
	controls += "Q|W|E \n";
	controls += "-|-|- \n";
	controls += "A|S|D \n";
	controls += "-|-|- \n";
	controls += "Z|X|C \n";

	return coloured_text("purple", controls);
}


void DisplayGrid() {
	string whole_grid = "";

	whole_grid += "    XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOX\n";
	whole_grid += "    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+O\n    X";
	
	string player_line = "         Player " + std::to_string(current_player) + ": " + player_usernames[current_player];
	// changes user colour line based on if they are player 1 or 2 (x or o)
	if (current_player == 1) {
		whole_grid += coloured_text("red", player_line);
	}
	else { whole_grid += coloured_text("blue", player_line); }

	int spaces_needed = 12 - player_usernames[current_player].length();
	for (int i = 0; i < spaces_needed; i++) {
		whole_grid += " ";
	}
	whole_grid += "X\n";
	whole_grid += "    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+O\n";
	whole_grid += "    X            |     |            X\n";
	whole_grid += "    O         " + string(1, grid[0][0]) + "  |  " + string(1, grid[0][1]) + "  |  " + string(1, grid[0][2]) + "         O\n";
	whole_grid += "    X       _____|_____|_____       X\n";
	whole_grid += "    O            |     |            O\n";
	whole_grid += "    X         " + string(1, grid[1][0]) + "  |  " + string(1, grid[1][1]) + "  |  " + string(1, grid[1][2]) + "         X\n";
	whole_grid += "    O       _____|_____|_____       O\n";																	  
	whole_grid += "    X            |     |            X\n";																	  	
	whole_grid += "    O         " + string(1, grid[2][0]) + "  |  " + string(1, grid[2][1]) + "  |  " + string(1, grid[2][2]) + "         O\n";
	whole_grid += "    X            |     |            X\n";
	whole_grid += "    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+O\n";
	whole_grid += "    XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOX\n";

	// need to figure out how to highlight 3 winning characters and display input x/o's in colour 

	cout << whole_grid << endl;

}



void winner_sound() {
	cout << coloured_text("green", "congratulations " + player_usernames[current_player] + " you have won!!! ") << endl;
	// async play sound so i can ask the user if they want to quit the song (repeat song if they dont type anything)
	// song file located in relative directory from where the exe was run 
	PlaySound(TEXT("celebration.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	cout << "press any key to stop music and return to the main menu " << endl;
	cin >> input;
	PlaySound(0, 0, 0); // stop music 

}



bool FillSquare(int row, int col) {
	// places the players symbol on the square selected
	// check if square is empty, if its not then the square has been filled already 
	// check if all the squares have been placed, if they have then the users have drawn
	if (grid[row][col] != ' ') {
		// squares already filled 
		return false; 
	}
	if (current_player == 1) {
		grid[row][col] = 'X';
		total_squares += 1; 

	}
	else if (current_player == 2) {

		grid[row][col] = 'O';
		total_squares += 1;

	};

	if (total_squares == ROWS * COLS) {
		// show new grid and break the loop 
		system("cls");
		DisplayGrid();
		input = '1';
		cout << "draw!" << endl;
	}

	return true;
}

bool check_winner(){
	string check = "";
	// 0,0 0,1 0,2 
	// 1,0 1,1 1,2 
	// 2,0 2,1 2,2 
	// 
	// for every colum (top to bottom) check if three characters match 
	for (int col = 0; col < COLS; col++) {
		check = ""; // after each column reset the 3 characters
		for (int row = 0; row < ROWS; row++) {
			// append to check variable 
			check += grid[row][col];
			// if theres a match then a winner has been found 
			if (check == "XXX" || check ==  "OOO") {
				return true;
			}
		}
	}

	// now we do the same for rows (left to right) 
	for (int row = 0; row < ROWS; row++) {
		check = "";
		for (int col = 0; col < COLS; col++) {
			check += grid[row][col];
			if (check == "XXX" || check == "OOO") {
				return true; 
			}
		}
	}

	// now we need to check horizontal  0,0 1,1 2,2 its just the same number repeating from 0,3 
	check = "";
	for (int i = 0; i < 3; i++) {
		check += grid[i][i];
		if (check == "XXX" || check == "OOO") {
			return true;
		}
	}

	// check the other horizontal 0,2, 1,1 2,0 
	check = "";
	for (int i = 0; i < 3; i++) {
		check += grid[i][COLS - i - 1]; // 3 - 0 - 1 = (2),  3 - 1 - 1 = (1), 3 - 2 - 1 = (0)
		if (check == "XXX" || check == "OOO") {
			return true;
		}
	}

	return false; 
}


void check_input() {
	while (true) { // keep checking input  until its correct 
		cin >> input;
		input = toupper(input);

		//  checks if the input is a valid move
		if (input == 'Q' && FillSquare(0,0) == true) {break;}
		else if (input == 'W' && FillSquare(0, 1) == true) {break; }
		else if (input == 'E' && FillSquare(0, 2) == true) {break; }
		else if (input == 'A' && FillSquare(1, 0) == true) {break; }
		else if (input == 'S' && FillSquare(1, 1) == true) {break; }
		else if (input == 'D' && FillSquare(1, 2) == true) {break; }
		else if (input == 'Z' && FillSquare(2, 0) == true) {break; }
		else if (input == 'X' && FillSquare(2, 1) == true) {break; }
		else if (input == 'C' && FillSquare(2, 2) == true) {break; }
		else if (input == '2') { cout << DisplayControls();}
		else if (input == '1') { break; }

		else {
			// clear previous attempts, show grid and point user in the right direction 
			system("cls");

			DisplayGrid();
			cout << "character invalid or invalid placement. \npress 2 to see the controls. \ngameplay will resume on correct move. " << endl;
		}

	}

}


json read_stats_file() {
	// reads the file, if it doesnt exist create it, if it does then return the json object 
	json data;
	ifstream f("data.json");

	if (f.fail()) {
		cout << "Creating stats file." << endl;
		data["vs-humans"] = json{};
		data["vs-bot"] = json{};

		ofstream file("data.json");
		file << data.dump(4); // dump(4) means write it with 4 indents 
		file.close();
	}
	else {
		f >> data;
		f.close();
	}

	return data;
}

void View_Stats() {
	json data = read_stats_file(); 
	cout << data.dump(4) << endl;

}


void Write_stats(string username, string vs, int wins, int losses, int draws) {
	json data = read_stats_file();
	if (data[vs].contains(username)) {
		// if user exists then add the values instead 

		data[vs][username]["wins"] = data[vs][username]["wins"].get<int>() + wins;
		data[vs][username]["losses"] = data[vs][username]["losses"].get<int>() + losses;
		data[vs][username]["draws"] = data[vs][username]["draws"].get<int>() + draws;
	}
	else {
		// create the new json key/values
		data[vs][username] = json{
			{"wins", wins},
			{"losses", losses},
			{"draws", draws}
		};
	}

	ofstream file("data.json");
	file << data.dump(4); 
	file.close();

}


int StartGame() {
	while (true) {
		if (player_usernames[1].length() > 12 || player_usernames[1] == "") {
			cout << "Player 1 username = ";
			cin >> player_usernames[1];
		}

		if (player_usernames[2].length() > 12 || player_usernames[2] == "") {
			cout << "Player 2 username = ";
			cin >> player_usernames[2];
		}

		if (player_usernames[1].length() < 12 && player_usernames[2].length() < 12) {
			break;
		}

	}
	do {// starts the game loop 
		system("cls"); // clear previous inputs

		cout << "    1. back to main menu \n    2. to see controls \n    \x1b[4menter controls to start playing\x1b[0m \n" << endl;
		// display the game grid 
		DisplayGrid();
		check_input();
		
		if (check_winner() == true){
			// show new grid and break the loop
			system("cls");
			DisplayGrid();
			winner_sound();
			break;

		}


		// after each game change the player 
		if (current_player == 1)
			current_player = 2;
		else current_player = 1;

	} while (input != '1');
	// user entered 1 therfore they want to go back to the main menu, return to the main loop 
	return 0;
}



void change_font_size(int size) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE) {
		std::cerr << "Error: Unable to get console handle." << std::endl;
		return;
	}
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	if (!GetCurrentConsoleFontEx(hConsole, FALSE, &cfi)) {
		std::cerr << "Error: Unable to retrieve font information." << std::endl;
		return;
	}

	cfi.dwFontSize.Y = size; // Font height
	cfi.dwFontSize.X = 0;    // width is sependant on height 
	wcscpy_s(cfi.FaceName, L"Consolas"); //  change the font 
	if (!SetCurrentConsoleFontEx(hConsole, FALSE, &cfi)) {
		std::cerr << "Error: Unable to set font size." << std::endl;
	}
}




int main() {
	change_font_size(30);

	// resize console window 8;rows;cols
	std::cout << "\x1b[8;30;42t" << std::endl;
	// line wrapping 
	std::cout << "\x1b[?7h";  

	do {
		clear_table();
		total_squares = 0;

		cout << welcome_message << endl;
		cin >> input;
		system("cls");

		if (input == '1') {
			StartGame();

		}
		else if (input == '3') {
			View_Stats();
		}

	} while (input != '4');
}





/*
	play against robot
	if you lose then play https://www.youtube.com/watch?v=NT4S8A7Vcsk


	classes??
	class bot 
	class human
	Class game_logic

*/
