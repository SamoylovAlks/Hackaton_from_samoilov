#include <iostream>
#include <thread>
#include <random>
#include <mutex>
using namespace std;

mutex m;
int table;//общий стол
int table_updated = 0;//если 0 - на общем столе не обновились карты, мастера не могут чекать стол
int master_finished = 0;//если 0 - мастер не закончил, создатель не может обновлять стол

int global_work = 1;//чтобы стопнуть бесконечную работу

vector<string> table_names = {"unicorn horn and phoenix feather",
							  "unicorn horn and snake scale",
							  "phoenix feather and snake scale"};
/*
Варианты объектов на столе
1 2
1 3
2 3
*/


class Master{
public:
	int id;
	std::string name;
	Master(int i){
		id = i;
		if(i == 1)
			name = "unicorn horn";
		else if(i == 2)
			name = "phoenix feather";
		else
			name = "snake scale";
	}

	void delay(){//Мастер кладет свой ингридиент на стол
		cout << "Dungeon Master put " << name << '\n' << endl;
		master_finished = 1;
	}

	void check(){//проверка столоа мастером
		const lock_guard<mutex> lock(m);
		if (id+table==6 && table_updated == 1){
			table_updated = 0;
			delay();
		}
	}
};

class Creature{
public:

	int give(){
		int table1, table2;
		srand(time(NULL));
		table1 = rand() % 3 + 1;
		table2 = rand() % 3 + 1;
		if (table1 == table2){
			table1 = (table1 + 1)%3;
			if (table1 == 0) table1+=1;
		}

		return table1+table2;
	}
};



int main(){

	vector<thread> vec;
	Creature C;
	table = C.give();
	std::cout << "Creature put " << table_names[table - 3] << endl;

	table_updated = 1;

	vec.push_back(std::thread([](){
		Master M1(1);
		while(global_work)
			M1.check();
	}));

	vec.push_back(std::thread([](){
		Master M2(2);
		while(global_work)
			M2.check();
	}));

	vec.push_back(std::thread([](){
		Master M3(3);
		while(global_work)
			M3.check();
	}));

	// this_thread::sleep_for(1000ms);

	for(int i = 1; i < 10; i++)
	{	
		while(!master_finished){}//ждем пока мастер не закончит, только после этого обновляем стол
		master_finished = 0;
		table = C.give();
		std::cout << "Creature put " << table_names[table - 3] << endl;
		table_updated = 1;		
		}
	
	while(!master_finished){}//ждем последнего мастера	
	global_work = 0;//останавливаем работу функции во всех потоках
	
	for(int i =0; i < 3; i++)
		vec[i].join();

	return 0;
}