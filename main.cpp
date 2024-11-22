#include <iostream>
#include <string>
#include <unordered_map>
#include <windows.h>
#include <mmsystem.h>
// link windm .libs for playsound function to work 
#pragma comment( lib, "winmm" )

using namespace std;



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


/*

	XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO
	O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-X
	X		Player 1: Ryan           O 15 character limit on username
	O+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-X
				 |     |
			  X  |  O  |  X
			_____|_____|_____
				 |     |
			  O  |  O  |  X
			_____|_____|_____
				 |     |
			  X  |  O  |  O
				 |     |


*/



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
	// black borders white background, text red for x, blue for o  

	string whole_grid = "";
	for (int row = 0; row < ROWS; row++) {
		// on each row add a newline and the boarder lines
		whole_grid.append("\n");
		if (row > 0) whole_grid.append("-|-|-\n");

		for (int col = 0; col < COLS; col++) {
			if (grid[row][col] == 'x')
				whole_grid.append(coloured_text("red", "x"));

			else if (grid[row][col] == 'o')
				whole_grid.append(coloured_text("blue", "o"));

			else (whole_grid.append(" "));

			// add the  boarder on the first two colums 
			if (col < 2) whole_grid.append("|"); 
		}
	}

	cout << "-----" << endl;
	if (current_player == 1) {
		cout << coloured_text("red", "Player 1: " + player_usernames[1]) << endl;;
	}
	else cout << coloured_text("blue", "player 2: " + player_usernames[2]) << endl;;
	cout << "-----" << endl;

	cout << whole_grid + "\n";


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
		grid[row][col] = 'x';
		total_squares += 1; 

	}
	else if (current_player == 2) {

		grid[row][col] = 'o';
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
			if (check == "xxx" || check ==  "ooo") {
				return true;
			}
		}
	}

	// now we do the same for rows (left to right) 
	for (int row = 0; row < ROWS; row++) {
		check = "";
		for (int col = 0; col < COLS; col++) {
			check += grid[row][col];
			if (check == "xxx" || check == "ooo") {
				return true; 
			}
		}
	}

	// now we need to check horizontal  0,0 1,1 2,2 its just the same number repeating from 0,3 
	check = "";
	for (int i = 0; i < 3; i++) {
		check += grid[i][i];
		if (check == "xxx" || check == "ooo") {
			return true;
		}
	}

	// check the other horizontal 0,2, 1,1 2,0 
	check = "";
	for (int i = 0; i < 3; i++) {
		check += grid[i][COLS - i - 1]; // 3 - 0 - 1 = (2),  3 - 1 - 1 = (1), 3 - 2 - 1 = (0)
		if (check == "xxx" || check == "ooo") {
			return true;
		}
	}

	// winner_position = the winning line coordinates 

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

		else {
			// clear previous attempts, show grid and point user in the right direction 
			system("cls");

			DisplayGrid();
			cout << "character invalid or invalid placement, press 2 to see the controls, gameplay will resume on correct move " << endl;
		}

	}

}





void ViewStats() {
	// displays the username statistics 
	// displays whatever user is winning in gold  
	cout << "yo" << endl;
}



void Write_stats() {
	/*
		username | total wins | total losses | KDR (number of wins / deaths)
		abc | 1 | 3 |
		123 | 3 | 1 |

		write to .txt file or .json file
	*/
}


int StartGame() {
	cout << "Player 1 username = ";
	cin >> player_usernames[1];
	cout << "Player 2 username = ";
	cin >> player_usernames[2];

	do {// starts the game loop 
		system("cls"); // clear previous inputs

		cout << "1. back to main menu 2. to see controls \nenter controls to start playing" << endl;
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






int main() {
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
			ViewStats();
		}

	} while (input != '4');
}




/*
	todo resize text based on terminal?
	when a winner has been found select the 3 aligning ones and highlight the background colour to yellow or something like that
	when a player wins play a celebration sound
	play against robot
	if you lose then play https://www.youtube.com/watch?v=NT4S8A7Vcsk
	when updating stats have seperate section against the robot

	human scores, robot scores (wins and losses of user)


*/
