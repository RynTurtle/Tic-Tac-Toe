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
const int ROWS = 3;
const int COLS = 3;
// grid of noughts and crosses 
char grid[ROWS][COLS];
int current_player = 1; // 1 or 2 

string welcome_message =
R"(
    XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO
    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-X
    X Welcome to noughts and crosses O
    O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-X
    X       1. Start new game        O
    O       2. Learn how to play     X
    X       3. View leaderboard      O
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
    O       2. Against X-O-Bot       X
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
	else if (wanted_colour == "underline") {
		original_text = "\x1b[4m" + original_text;
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


class tic_tac_toe {
	private:
		char input;


	public:
		bool is_against_human;
		// map within a map containing the usernames player1:name1, player2:name2
		std::unordered_map<int, std::string> player_usernames;

		void Change_Player() {
			if (current_player == 1)
				current_player = 2;
			else current_player = 1;
		}

		void Clear_Grid() {
			// reset the grid 
			for (int row = 0; row < ROWS; row++) {
				for (int col = 0; col < COLS; col++) {
					grid[row][col] = ' ';
				}
			}
		}

		void Write_Stats(string username, int wins, int losses, int draws) {
			json data = Read_Stats_File();
			string vs;
			if (is_against_human) {
				vs = "vs-humans";
			}
			else { vs = "vs-bot"; }

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

		void Play_Sound(bool is_winner) {
			string file;
			string keypress;
			if (is_winner) {
				cout << coloured_text("green", "Congratulations " + player_usernames[current_player] + ", you have won!!!") << endl;
				file = "celebration.wav";
			}
			else {
				file = "loludied.wav";
			}

			// playing the sound in async mode so it can also handle input at the same time 
			PlaySoundA(file.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

			// when a the user enters any key it will
			cout << "Press any key to stop the music and return to the main menu." << endl;
			cin >> keypress;

			// stop the sound 
			PlaySound(NULL, 0, 0);
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

			// loop through table and add the colours for X/O 
			for (string::size_type i = 0; i < whole_grid.size(); ++i) {
				// since theres an XO pattern around the game table I need to check if the surrounding characters have | on either side.
				if (i >= 3 && i < whole_grid.size() - 3) { 
					if ((whole_grid[i - 3] == '|' || whole_grid[i + 3] == '|') && whole_grid[i] == 'X') {
						whole_grid.replace(i, 1, coloured_text("red", "X"));
					}
					if ((whole_grid[i - 3] == '|' || whole_grid[i + 3] == '|') && whole_grid[i] == 'O') {
						whole_grid.replace(i, 1, coloured_text("blue", "O"));
					}
				}
			}
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
			}

			else if (current_player == 2) {
				grid[row][col] = 'O';
			};

			return true;
		}

		int Check_Winner() {
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
				}
				// if theres a match then a winner has been found 
				if (check == "XXX") return 1;  
				if (check == "OOO") return -1; 
			}

			// now we do the same for rows (left to right) 
			for (int row = 0; row < ROWS; row++) {
				check = "";
				for (int col = 0; col < COLS; col++) {
					check += grid[row][col];
				}
				if (check == "XXX") return 1;
				if (check == "OOO") return -1;
			}

			// now we need to check diagonal  0,0 1,1 2,2 its just the same number repeating from 0,3 
			check = "";
			for (int i = 0; i < 3; i++) {
				check += grid[i][i];
			}
			if (check == "XXX") return 1;
			if (check == "OOO") return -1;

			// check the other diagonal 0,2, 1,1 2,0 
			check = "";
			for (int i = 0; i < 3; i++) {
				check += grid[i][COLS - i - 1]; // 3 - 0 - 1 = (2),  3 - 1 - 1 = (1), 3 - 2 - 1 = (0)
			}
			
			if (check == "XXX") return 1;
			if (check == "OOO") return -1;


			int filled = 0;
			for (int row = 0; row < ROWS; row++) {
				for (int col = 0; col < COLS; col++) {
					if (grid[row][col] != ' ') {
						filled += 1;
					}
				}
			}
			if (filled == ROWS * COLS) {
				// its a draw!
				return 3;
			}


			return 0;
		}

		// maximiser is the AI - it wants the highest possible score
		// minimiser is the human - they want to choose the ai's lowest score so they have the higher chances
		// returns the game state (how good a move is) based on recursion of itself reaching a terminal state (10 being good, 0 being okay, -10 being bad)
		int minimax(bool is_maximising) {
			// if there is a current winner then terminate the board tree recursion (terminal state)
			
			// Print the current board state
			//cout << "Current board state:" << endl;
			//for (int i = 0; i < ROWS; i++) {
			//	for (int j = 0; j < COLS; j++) {
			//		cout << grid[i][j] << " ";
			//	}
			//	cout << endl;
			//}
			//cout << "is_maximising " << is_maximising << endl;

			int result = Check_Winner();
			if (result == 1) {
				// the AI won so it gets 10 points, points can be more than 10 just as long as its positive
				return 10;
			}
			else if (result == -1){
				//cout << "human win " << endl;
				return -10; // human won
			}
			else if (result == 3) {
				// it was a draw so it gets 0 points
				//cout << "draw" << endl; 
				return 0;
			}
			//cout << "Check_Winner returned: " << result << endl;


			int best_score;
			// here i am recursively checking each possible move in the board given
			if (is_maximising) {
				cout << "max" << endl;
				// the best score for the AI is the highest possible number, we assume it can get a very low score first and compare it
				best_score = -1000;
				for (int row = 0; row < ROWS; row++) {
					for (int col = 0; col < COLS; col++) {
						// if theres an available move then get the score for the move
						if (grid[row][col] == ' ') {
							grid[row][col] = 'X';  
							//cout << "setting to false " << endl;
							int score = minimax(false); // minimise next turn for human
							best_score = max(best_score, score);
							grid[row][col] = ' '; // undo the move
						}
					}
				}

			}
			else {
				//cout << "min" << endl;
				// the best score for the human is the lowest score of minmax due to it being lowest chance of the AI winning
				// we assume that the human can have a very high score to compare to get the lowest score
				best_score = 1000;
				for (int row = 0; row < ROWS; row++) {
					for (int col = 0; col < COLS; col++) {
						// if theres an available move then get the score for the move
						if (grid[row][col] == ' ') {
							grid[row][col] = 'O';
							cout << "setting to true " << endl;
							int score = minimax(true); // maximise next for ai 
							best_score = min(best_score, score);
							grid[row][col] = ' '; // undo the move
						}
					}
				}
			}

			return best_score;
		}

		// now i go through the main tree 
		pair<int, int> best_move() {
			pair<int, int> move;
			int best_score = -1000; // find the maximum number again 
			// I like the middle to be always chosen first 
			if (grid[1][1] == ' ') {
				move = { 1,1 };
				return move;
			}

			for (int row = 0; row < ROWS; row++) {
				for (int col = 0; col < COLS; col++) {
					if (grid[row][col] == ' ') {
						grid[row][col] = 'X';
						// now i want to see if the move is good 
						int score = minimax(false);
						grid[row][col] = ' ';

						if (score > best_score) {
							move = { row,col };
							best_score = score;
						}
					}

				}
			}
			return move;
		}
		

		void Check_Input() {
			// input should be handled by minimax 
			if (current_player == 1 && is_against_human == false) {
				pair<int, int> ai_move = best_move();
				bool make_move = Fill_Square(ai_move.first, ai_move.second);
				cout << "(" << ai_move.first << ", " << ai_move.second << ")" << endl;
				if (make_move == false) {
					cout << "Minimax algorithm Couldn't determine the right move, it decided (" << ai_move.first << "," << ai_move.second << ")" << endl;
				}
				system("pause");
			}
			else {

				while (true) { // keep checking input  until its correct 
					cin >> input;
					input = toupper(input);
					//  checks if the input is a valid move
					if (input == 'Q' && Fill_Square(0, 0) == true) { break; }
					else if (input == 'W' && Fill_Square(0, 1) == true) { break; }
					else if (input == 'E' && Fill_Square(0, 2) == true) { break; }
					else if (input == 'A' && Fill_Square(1, 0) == true) { break; }
					else if (input == 'S' && Fill_Square(1, 1) == true) { break; }
					else if (input == 'D' && Fill_Square(1, 2) == true) { break; }
					else if (input == 'Z' && Fill_Square(2, 0) == true) { break; }
					else if (input == 'X' && Fill_Square(2, 1) == true) { break; }
					else if (input == 'C' && Fill_Square(2, 2) == true) { break; }
					else if (input == '2') { system("cls "); cout << controls_message << endl; system("pause"); break; }
					else if (input == '1') { break; }

					else {
						// clear previous attempts, show grid and point user in the right direction 
						system("cls");

						Display_Grid();
						cout << "character invalid or invalid placement. \npress 2 to see the controls. \ngameplay will resume on correct move. " << endl;
					}

				}
			}

		}


		string Get_Username() {
			string username;
			while (true) {
				cin >> username;
				// convert username to lowercase
				for (char& c : username) {
					c = std::tolower(c);
				}

				if (username == "x-o-bot") {
					cout << "Your username cant be the bots name" << endl;
				}
				if (username.length() <= 9) { break; }
				else { cout << "your username is too long, it needs to be less than 9 characters" << endl; }
			}

			return username;
		}




		int Game_Start() {
			Clear_Grid();
			if (is_against_human) {
				cout << "Player 1 username = ";
				player_usernames[1] = Get_Username();
				cout << "Player 2 username = ";
				player_usernames[2] = Get_Username();
			}
			else {
				player_usernames[1] = "X-O-Bot";
				cout << "Whats your username? ";
				player_usernames[2] = Get_Username();

			}

			do {
				system("cls"); // clear previous inputs
				cout << "    1. back to main menu \n    2. to see controls \n    " + coloured_text("underline","enter controls to start playing \n") << endl;
				Display_Grid();
				Check_Input();
 

				int win_check = Check_Winner();
				// win/draw
				if (win_check != 0) {
					system("cls");
					Display_Grid();
					// update stats 
					if (win_check == 1 || win_check == -1) {
						if (current_player == 1) {
							// player 1 wins then player 2 loses 
							Write_Stats(player_usernames[1], 1, 0, 0);
							Write_Stats(player_usernames[2], 0, 1, 0);
						}
						else { // player 2  wins, player 1 loses 
							Write_Stats(player_usernames[2], 1, 0, 0);
							Write_Stats(player_usernames[1], 0, 1, 0);
						}
					}
					else if (win_check == 3) {
						cout << "draw!" << endl;
						// write draw to both users 
						Write_Stats(player_usernames[1], 0, 0, 1);
						Write_Stats(player_usernames[2], 0, 0, 1);
						system("pause");
						break;
					}

					// play loser sound only if your against the bot and you lost, otherwise there will always be a winner
					if (is_against_human == false && current_player == 1) { Play_Sound(false); }
					else { Play_Sound(true); }
					// break game loop once a game state has ended 
					break;
				}
				Change_Player();
			} while (input != '1');
			// user entered 1 therfore they want to go back to the main menu, return to the main loop 
				return 0;
		}
};



class Game_menu {
	private:
		char input;
		// makes sure the length of the word is whats wanted by adding spaces on either side 
		string ensure_length(int characters, string word) {
			for (int i = 0; i < word.length() - characters; i++) {
				word += " ";
			}
			return word;
		}

		void Display_LeaderBoard() {
			json data = Read_Stats_File();
			//cout << data.dump(4) << endl
			json humans = data["vs-humans"];
			json bot = data["vs-bot"];
			cout << "   Against Human:" << endl;
			cout << "   ____________________________________" << endl;
			cout << "   " << coloured_text("underline", "| Username | Wins | losses | Draws |") << endl;
			// iterate the json object
			for (auto it = humans.begin(); it != humans.end(); ++it) {
				string username = ensure_length(9, it.key());
				auto value = it.value();
				string wins = ensure_length(5, to_string(value["wins"]));
				string draws = ensure_length(6, to_string(value["draws"]));
				string losses = ensure_length(7, to_string(value["losses"]));

				cout << "   " << coloured_text("underline", "| " + username + "| " + wins + "| " + losses + "| " + draws + "|") << endl;
			}


			cout << "\n   Against Bot:" << endl;
			cout << "   ____________________________________" << endl;
			cout << "   " << coloured_text("underline", "| Username | Wins | losses | Draws |") << endl;
			for (auto it = bot.begin(); it != bot.end(); ++it) {
				string username = ensure_length(9, it.key());
				auto value = it.value();
				string wins = ensure_length(5, to_string(value["wins"]));
				string draws = ensure_length(6, to_string(value["draws"]));
				string losses = ensure_length(7, to_string(value["losses"]));
				cout << "   " << coloured_text("underline", "| " + username + "| " + wins + "| " + losses + "| " + draws + "|") << endl;
			}
		}

		void HowToPlay() {
			tic_tac_toe tic_tac;
			string answer;
			tic_tac.player_usernames[1] = "user1";
			tic_tac.player_usernames[2] = "user2";

			do {
				system("cls");
				cout << coloured_text("purple", "The aim of noughts and crosses is \nTo match either X or O in a line of 3.") << endl;
				cout << "Choose what example you would like to see:" << endl;
				cout << coloured_text("purple", "vertical,horizontal,diagonal") << endl;
				cin >> answer;
				tic_tac.Clear_Grid();
				if (answer == "vertical") {
					for (int i = 0; i < 3; i++) {
						system("cls");
						tic_tac.Fill_Square(0, i);
						tic_tac.Display_Grid();
						Sleep(1000);
					}
				}

				if (answer == "horizontal") {
					for (int i = 0; i < 3; i++) {
						system("cls");
						tic_tac.Fill_Square(i, 0);
						tic_tac.Display_Grid();
						Sleep(1000);
					}
				}

				if (answer == "diagonal") {
					for (int i = 0; i < 3; i++) {
						system("cls");
						tic_tac.Fill_Square(i, i);
						tic_tac.Display_Grid();
						Sleep(1000);
					}
				}
				cout << coloured_text("purple", "Thats an example of a possible \n" + answer + " line combination") << endl;
				cout << "Would you like to quit? (y/n)" << endl;
				cin >> answer;
			} while (answer != "y");
		}

	public:
		void Menu_Loop() {
			do {
				system("cls");
				cout << welcome_message << endl;
				cin >> input;

				// User wants to start the game, need to check if they want to play against AI or human 
				if (input == '1') {
					system("cls");

					cout << game_message << endl;
					cin >> input;
					tic_tac_toe tic_tac;

					// human
					if (input == '1') {
						tic_tac.is_against_human = true;
						tic_tac.Game_Start();
					}// AI
					else if (input == '2') {
						tic_tac.is_against_human = false;
						tic_tac.Game_Start();
					}


				}// show them how to play 
				else if (input == '2') {
					HowToPlay();
				}

				else if (input == '3') {// display the leaderboard reading the json file
					system("cls");
					cout << "LeaderBoard" << endl;
					// back to main menu
					Display_LeaderBoard();
					system("pause");

				}

			} while (input != '4');
		}
};


int main() {
	change_font_size(30);
	// resize console window 8;rows;cols
	std::cout << "\x1b[8;30;42t" << std::endl;
	// line wrapping 
	std::cout << "\x1b[?7h";

	
	Game_menu menu;
	menu.Menu_Loop();

}
