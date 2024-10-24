#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <list>
using namespace std;
const int MIN_X = 40;
const int MAX_X = 80;
const int MIN_Y = 5;
const int MAX_Y = 25;
//Объект
struct Object {
	char body;
	int colorid;
	HANDLE console;
	COORD placement;
	Object(int x, int y, int id) {
		console = GetStdHandle(STD_OUTPUT_HANDLE);
		placement.X = x;
		placement.Y = y;
		colorid = id;
		body = '*';
	}
	friend ostream& operator <<(ostream& out, Object& o) {
		SetConsoleTextAttribute(o.console, o.colorid);
		SetConsoleCursorPosition(o.console, o.placement);
		out << o.body;
		return out;
	}
};
//Змейка
class Snake {
	HANDLE console;
	COORD* part;
	COORD last_tail_position;
	int size;
	char body;
	int colorid;
	int speed;
	int score = 0;
	string current_direction;
	string opposite_direction;
	bool is_crashed = false;
public:
	vector<COORD> body_placement;
	Snake(char body, int size, int color, int speed, int levelid) {
		console = GetStdHandle(STD_OUTPUT_HANDLE);
		this->body = body;
		this->size = size;
		colorid = color;
		for (int i = 0; i < this->size; i++) {
			part = new COORD();
			if (levelid == 3) {
				part->X = MIN_X + 8 - i;
				part->Y = MAX_Y - 2;
			}
			else if (levelid == 4) {
				part->X = MIN_X + 8 - i;
				part->Y = MAX_Y - 1;
			}
			else if (levelid == 5) {
				part->X = MIN_X + 8 - i;
				part->Y = MIN_Y + 1;
			}
			else {
				part->X = (MIN_X + MAX_X) / 2 - i;
				part->Y = (MIN_Y + MAX_Y) / 2;
			}
			body_placement.push_back(*part);
			delete part;
		}
		this->speed = speed;
		current_direction = "right";
		opposite_direction = "left";
	}
	void Paint() {
		SetConsoleTextAttribute(console, colorid);
		for (int i = 0; i < size; i++) {
			SetConsoleCursorPosition(console, body_placement[i]);
			cout << body;
		}
		SetConsoleTextAttribute(console, 15);
	}
	void Move(string dir) {
		part = new COORD();
		*part = body_placement[0];
		if (dir == "right") {
			part->X++;
		}
		if (dir == "down") {
			part->Y++;
		}
		if (dir == "left") {
			part->X--;
		}
		if (dir == "up") {
			part->Y--;
		}
		body_placement.insert(body_placement.begin() + 0, *part);
		Clear();
		last_tail_position = body_placement[body_placement.size() - 1];
		body_placement.erase(body_placement.begin() + body_placement.size() - 1);
	}
	void Clear() {
		SetConsoleCursorPosition(console, body_placement[body_placement.size() - 1]);
		cout << ' ';
	}
	void Turn() {
		int code;
		if (_getch() == 224) {
			code = _getch();
			if (code == 75 && opposite_direction != "left") {
				current_direction = "left";
				opposite_direction = "right";
			}
			if (code == 72 && opposite_direction != "up") {
				current_direction = "up";
				opposite_direction = "down";
			}
			if (code == 77 && opposite_direction != "right") {
				current_direction = "right";
				opposite_direction = "left";
			}
			if (code == 80 && opposite_direction != "down") {
				current_direction = "down";
				opposite_direction = "up";
			}
		}
	}
	void Interaction(vector<Object*>& o_list) {
		for (int i = 0; i < o_list.size(); i++) {
			if (body_placement[0].X == o_list[i]->placement.X && body_placement[0].Y == o_list[i]->placement.Y) {
				if (o_list[i]->colorid == 8)
					is_crashed = true;
				if (o_list[i]->colorid == 3) {
					part = new COORD();
					part->X = last_tail_position.X;
					part->Y = last_tail_position.Y;
					body_placement.push_back(*part);
					delete part;
					delete o_list[i];
					o_list.erase(o_list.begin() + i);
					score += 100;
				}
			}
		}
		for (int i = 1; i < body_placement.size(); i++)
			if (body_placement[0].X == body_placement[i].X && body_placement[0].Y == body_placement[i].Y)
				is_crashed = true;
	}
	void Action(vector<Object*>& o_list, int max_score, bool& map_state, int& level_score) {
		while (!is_crashed && score < max_score) {
			Sleep(1000 - speed);
			if (_kbhit())
				Turn();
			Move(current_direction);
			Paint();
			Interaction(o_list);
		}
		level_score = score;
		if (is_crashed)
			map_state = false;
		else
			map_state = true;
	}
};
//Уровень
class Level {
	vector<Object*> objects;
	Snake* snake;
	int max_score = 0;
	int color_chosen = 0;
	int food_count;
public:
	bool map_completed;
	int level_score;
	Level(int size, int colorid, int levelid) {
		color_chosen = colorid;
		food_count = size;
		switch (levelid) {
		case 1:Build_1st_map(); break;
		case 2:Build_2nd_map(); break;
		case 3:Build_3rd_map(); break;
		case 4:Build_4th_map(); break;
		case 5:Build_5th_map(); break;
		}
	}
	void Set_Wall(int x, int y) {
		objects.push_back(new Object(x, y, 8));
	}
	void Set_Food() {
		int x, y, n;
		while (true) {
			x = (MIN_X + 1) + rand() % (MAX_X - (MIN_X + 1));
			y = (MIN_Y + 1) + rand() % (MAX_Y - (MIN_Y + 1));
			n = 0;
			for (auto& element : snake->body_placement)
				if (x == element.X && y == element.Y)
					n++;
			for (auto& element : objects)
				if (x == element->placement.X && y == element->placement.Y)
					n++;
			if (n == 0)
				break;
		}
		objects.push_back(new Object(x, y, 3));
		max_score += 100;
	}
	void Build_1st_map() {
		snake = new Snake('*', 5, color_chosen, 800, 1);
		for (int y = MIN_Y; y <= MAX_Y; y++) {
			for (int x = MIN_X; x <= MAX_X; x++) {
				if (y == MIN_Y || y == MAX_Y || x == MIN_X || x == MAX_X)
					Set_Wall(x, y);
			}
		}
		for (int i = 0; i < food_count; i++) {
			Set_Food();
		}
	}
	void Build_2nd_map() {
		snake = new Snake('*', 5, color_chosen, 800, 2);
		for (int y = MIN_Y; y <= MAX_Y; y++) {
			for (int x = MIN_X; x <= MAX_X; x++) {
				if ((y == MIN_Y || y == MAX_Y || x == MIN_X || x == MAX_X) ||
					(((y == MIN_Y + 4 || y == MIN_Y + 5) || (y == MAX_Y - 4 || y == MAX_Y - 5)) && (x > MIN_X + 4 && x < MAX_X - 4)))
					Set_Wall(x, y);
			}
		}
		for (int i = 0; i < food_count; i++) {
			Set_Food();
		}
	}
	void Build_3rd_map() {
		snake = new Snake('*', 5, color_chosen, 800, 3);
		for (int y = MIN_Y; y <= MAX_Y; y++) {
			for (int x = MIN_X; x <= MAX_X; x++) {
				if ((y == MIN_Y || y == MAX_Y || x == MIN_X || x == MAX_X) ||
					((x == (MIN_X + MAX_X) / 2 - 1 || x == (MIN_X + MAX_X) / 2 || x == (MIN_X + MAX_X) / 2 + 1) && (y > MIN_Y + 2 && y < MAX_Y - 2)) ||
					((y == (MIN_Y + MAX_Y) / 2 - 1 || y == (MIN_Y + MAX_Y) / 2 || y == (MIN_Y + MAX_Y) / 2 + 1) && (x > MIN_X + 4 && x < MAX_X - 4)))
					Set_Wall(x, y);
			}
		}
		for (int i = 0; i < food_count; i++) {
			Set_Food();
		}
	}
	void Build_4th_map() {
		snake = new Snake('*', 5, color_chosen, 800, 4);
		int n = 0;
		for (int y = MIN_Y; y <= MAX_Y; y++) {
			for (int x = MIN_X; x <= MAX_X; x++) {
				if ((y == MIN_Y || y == MAX_Y || x == MIN_X || x == MAX_X) ||
					(((x == MIN_X + n - 1 || x == MIN_X + n || x == MIN_X + n + 1) || (x == MAX_X - n - 1 || x == MAX_X - n || x == MAX_X - n + 1)) && (y > MIN_Y + 2 && y < MAX_Y - 2)) ||
					((x == (MIN_X + MAX_X) / 2 && (y == (MIN_Y + MAX_Y) / 2 - 1 || y == (MIN_Y + MAX_Y) / 2 + 1)) || (y == (MIN_Y + MAX_Y) / 2) && (x == (MIN_X + MAX_X) / 2 - 3 || x == (MIN_X + MAX_X) / 2 - 2 || x == (MIN_X + MAX_X) / 2 + 2 || x == (MIN_X + MAX_X) / 2 + 3)) ||
					((x >= (MIN_X + MAX_X) / 2 - 1 && x <= (MIN_X + MAX_X) / 2 + 1) && (y == MIN_Y + 2 || y == MIN_Y + 3 || y == MAX_Y - 2 || y == MAX_Y - 3)) ||
					((y >= (MIN_Y + MAX_Y) / 2 - 1 && y <= (MIN_Y + MAX_Y) / 2 + 1) && (x == MIN_X + 4 || x == MIN_X + 5 || x == MAX_X - 4 || x == MAX_X - 5))
					)
					Set_Wall(x, y);
			}
			n += 2;
		}
		for (int i = 0; i < food_count; i++) {
			Set_Food();
		}
	}
	void Build_5th_map() {
		snake = new Snake('*', 5, color_chosen, 800, 5);
		for (int y = MIN_Y; y <= MAX_Y; y++) {
			for (int x = MIN_X; x <= MAX_X; x++) {
				if ((y == MIN_Y || y == MAX_Y || x == MIN_X || x == MAX_X) ||
					(y == MIN_Y + 3 && ((x >= MIN_X + 1 && x <= MIN_X + 10) || (x >= MAX_X - 5 && x <= MAX_X - 1))) ||
					(y == MIN_Y + 5 && ((x >= MIN_X + 3 && x <= MIN_X + 16) || (x >= MAX_X - 6 && x <= MAX_X - 2))) ||
					(y == MIN_Y + 8 && ((x >= MIN_X + 9 && x <= MIN_X + 30))) ||
					(y == MIN_Y + 11 && ((x >= MIN_X + 9 && x <= MIN_X + 13) || (x >= MIN_X + 15 && x <= MIN_X + 20) || (x >= MIN_X + 25 && x <= MIN_X + 30))) ||
					(y == MIN_Y + 14 && ((x >= MIN_X + 5 && x <= MIN_X + 11) || (x >= MAX_X - 13 && x <= MAX_X - 6))) ||
					(y == MIN_Y + 16 && ((x >= MIN_X + 12 && x <= MIN_X + 33))) ||
					(x == MIN_X + 5 && ((y >= MIN_Y + 5 && y <= MIN_Y + 10) || (y >= MAX_Y - 5 && y <= MAX_Y - 2))) ||
					(x == MIN_X + 10 && ((y >= MIN_Y + 6 && y <= MIN_Y + 16))) ||
					(x == MIN_X + 20 && ((y >= MIN_Y + 1 && y <= MIN_Y + 6) || (y >= MAX_Y - 10 && y <= MAX_Y - 3))) ||
					(x == MIN_X + 31 && ((y >= MIN_Y + 4 && y <= MIN_Y + 14 || (y >= MAX_Y - 3 && y <= MAX_Y - 1)))) ||
					(x == MIN_X + 37 && ((y >= MIN_Y + 7 && y <= MIN_Y + 15)))
					)
					Set_Wall(x, y);
			}
		}
		for (int i = 0; i < food_count; i++) {
			Set_Food();
		}
	}
	void Start_Map() {
		system("cls");
		for (auto& element : objects)
			cout << *element;
		snake->Action(objects, max_score, map_completed, level_score);
	}
};
//Игра
class Game {
	list <Level> campaing;
	Level* current;
	int user_choice;
	int chosen_level;
	int chosen_color;
	int overall_score = 0;
	void(*build_current_map)();
public:
	Game(string gamemode) {};
	void MainMenu() {
		do {
			system("cls");
			cout << "1. New game \n2. Choose level \n3. Exit\n";
			cin >> user_choice;
			switch (user_choice) {
			case 1: StartGame(); ShowResults(); Reset(); break;
			case 2: 
				LevelSelection(); 
				if(user_choice != 0)
					ShowResults(); 
				Reset(); 
				break;
			case 3: exit(0); break;
			default: system("cls"); cout << "Wrong choice\n"; Sleep(1000); user_choice = 0; break;
			}
		} while (user_choice != 3);
	}
	void Intro(int levelid) {
		system("cls");
		HANDLE intro_console = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD intro_coordinates;
		intro_coordinates.X = (MIN_X + MAX_X) / 2;
		intro_coordinates.Y = (MIN_Y + MAX_Y) / 2;
		SetConsoleCursorPosition(intro_console, intro_coordinates);
		cout << "Level " << levelid;
		Sleep(1000);
	}
	void LevelSelection() {
		do {
			system("cls");
			cout << "1. Level 1 \n2. Level 2 \n3. Level 3 \n4. Level 4\n5. Level 5\n6. Cancel\n";
			cin >> chosen_level;
			switch (chosen_level) {
			case 1: StartLevel(1); break;
			case 2: StartLevel(2); break;
			case 3: StartLevel(3); break;
			case 4: StartLevel(4); break;
			case 5: StartLevel(5); break;
			case 6: system("cls"); user_choice = 0; break;
			default: system("cls"); cout << "Wrong choice\n"; Sleep(1000); chosen_level = 0; break;
			}
		} while (chosen_level == 0);
	}
	void ColorSelection() {
		do {
			system("cls");
			cout << "Choose snake color:\n1.Blue\n2.Green\n3.Red\n";
			cin >> user_choice;
			switch (user_choice) {
			case 1: chosen_color = 1; break;
			case 2: chosen_color = 2; break;
			case 3: chosen_color = 4; break;
			default: system("cls"); cout << "Wrong choice\n"; Sleep(1000); chosen_color = 0;
			}
		} while (chosen_color == 0);
	}
	void StartGame() {
		int n = 1;
		ColorSelection();
		cout << "\033[?25l";
		for (int i = 0; i < 5; i++) {
			current = new Level(20, chosen_color, i + 1);
			campaing.push_back(*current);
			delete current;
		}
		for (auto& element : campaing) {
			Intro(n);
			element.Start_Map();
			overall_score += element.level_score;
			if (!element.map_completed)
				break;
			n++;
		}
	}
	void StartLevel(int number) {
		int n = 1;
		ColorSelection();
		cout << "\033[?25l";
		for (int i = 0; i < 5; i++) {
			current = new Level(20, chosen_color, i + 1);
			campaing.push_back(*current);
			delete current;
		}
		for (auto it = campaing.begin(); it != campaing.end(); it++) {
			if (n == number) {
				Intro(number);
				it->Start_Map();
				overall_score += it->level_score;
				break;
			}
			n++;
		}
	}
	void Reset() {
		system("cls");
		cout << "\033[?25h";
		campaing.clear();
		overall_score = 0;
		user_choice = 0;
		chosen_level = 0;
		chosen_color = 0;
	}
	void ShowResults() {
		system("cls");
		HANDLE res_console = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD res_coordinates;
		res_coordinates.X = (MIN_X + MAX_X) / 2;
		res_coordinates.Y = (MIN_Y + MAX_Y) / 2;
		SetConsoleCursorPosition(res_console, res_coordinates);
		cout << "Your score - " << overall_score;
		Sleep(1000);
	}
};
//Главная функция
int main() {
	srand(time(NULL));
	HANDLE common_console = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD common_coordinates;
	common_coordinates.X = 0;
	common_coordinates.Y = 99;
	Game G("Singleplayer");
	G.MainMenu();
	SetConsoleCursorPosition(common_console, common_coordinates);
}
