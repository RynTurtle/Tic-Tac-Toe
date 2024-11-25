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

	

string controls_message =
R"(
	XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO
	O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ X
	X         Player 1: player_name  O
	O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ X
	X                                O
	O            |     |             X
	X        Q   |  W  |  E          O
	O       _____|_____|_____        X
	X            |     |             O
	O        A   |  S  |  D          X
	X       _____|_____|_____        O
	O            |     |             X
	X        Z   |  X  |  C          O
	O            |     |             X
	X                                O
	O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ X
	XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO
	
)";


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

 
class tic_tac_toe() {
	public:
		const int ROWS = 3; 
		const int COLS = 3; 
		// grid of noughts and crosses 
		char grid[ROWS][COLS]; 
		char input;
		int current_player = 1;
		int total_squares;

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

		void Display_Grid() {
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


		bool Fill_Square(int row, int col) {
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
		
		void Write_Stats(string username, string vs, int wins, int losses, int draws) {
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


		json Read_Stats_File() {
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

		void Display_LeaderBoard(){
			json data = read_stats_file();
			cout << data.dump(4) << endl;
		}

		void Display_Controls() {
			cout << controls_message << endl;
		}
		
		
		void Play_Sound(bool is_winner) {
			string file; 
			if (is_winner){
				cout << coloured_text("green", "congratulations " + player_usernames[current_player] + " you have won!!! ") << endl;
				file = "celebration.wav";
			else {file = "loludied.wav" }


			// async play sound so i can ask the user if they want to quit the song (repeat song if they dont type anything)
			// song file located in relative directory from where the exe was run 
			PlaySound(TEXT(file), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			cout << "press any key to stop music and return to the main menu " << endl;
			cin >> input;
			PlaySound(0, 0, 0); // stop music 

		}


}


class Game_menu() {
	public:

		int how_to_play() {
		}

		int leaderboard() {
		}

		int new_game() {
		}


		int main_menu() {
		}



}




int main() {
	change_font_size(30);
	// resize console window 8;rows;cols
	std::cout << "\x1b[8;30;42t" << std::endl;
	// line wrapping 
	std::cout << "\x1b[?7h";


	menu = Game_menu();
	menu.leaderboard();


}





//play against robot if you lose then play https://www.youtube.com/watch?v=NT4S8A7Vcsk
