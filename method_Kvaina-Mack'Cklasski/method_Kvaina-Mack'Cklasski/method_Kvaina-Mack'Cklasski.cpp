#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

//преобразование данных из файла по заданному пути в vector
std::vector<std::vector<char>> input_data(const std::string& path) {
	std::vector<std::vector<char>> vector{};
	std::ifstream fin(path);
	if (fin.is_open()) {
		std::string time{};
		while (std::getline(fin, time)) {
			std::vector<char> time_vec{};
			time_vec.push_back('0');
			for (unsigned int i = 0; i < time.size(); ++i) {
				time_vec.push_back(time[i]);
			}
			vector.push_back(time_vec);
		}
	}
	fin.close();
	return vector;
}

void print_function(const std::vector<std::vector<char>> &vector) {
	for (unsigned int i = 0; i < vector.size(); ++i) {
		for (unsigned int j = 1; j < vector[i].size(); ++j) {
			std::cout << vector[i][j];
		}
		if (i < (vector.size() - 1)) std::cout << " v ";
	}
	std::cout << "\n\n";
}

void print_system(const std::vector<std::vector<char>>& vector, const std::vector<std::vector<char>>& coefs) {
	std::cout << "Current system (start):" << std::endl;
	for (unsigned int i = 0; i < vector.size(); ++i) {
		for (unsigned int k = 1; k < vector[i].size(); ++k) {
			std::cout << vector[i][k];
		}
		std::cout << " = ";
		unsigned int count = 0;
		for (unsigned j = 0; j < coefs.size(); ++j) {
			bool flag = true;
			for (unsigned int k = 1; k < coefs[j].size(); ++k) {
				if (vector[i][k] != coefs[j][k] && coefs[j][k] != '`') {
					flag = false;
					break;
				}
			}
			if (flag) {
				if (count != 0) std::cout << " v ";
				++count;
				for (unsigned int k = 1; k < coefs[j].size(); ++k) {
					std::cout << coefs[j][k];
				}
			}
		}
		std::cout << std::endl;
	}
	std::cout << "The end of the system\n\n";
}

//проверка, включен ли included в vector (true - его нет в vector, false - иначе)
bool inclusion_check(const std::vector<char>& included, const std::vector<std::vector<char>>& vector) {
	for (unsigned int i = 0; i < vector.size(); ++i) {
		bool flag = true;
		for (unsigned int j = 1; j < included.size(); ++j) {
			if (vector[i][j] != included[j]) {
				flag = false;
				break;
			}
		}
		if (flag) {
			return false;
		}
	}
	return true;
}

//преобразование vector с 'k' числом "тильд" к 'k+1'
std::vector<std::vector<char>> minimization(std::vector<std::vector<char>>& vector) {
	std::vector<std::vector<char>> time_vector{};
	for (unsigned int i = 0; i < vector.size() - 1; ++i) {
		for (unsigned int j = i + 1; j < vector.size(); ++j) {
			unsigned int count = 0;
			for (unsigned int k = 1; k < vector[i].size(); ++k) {
				if (vector[i][k] != vector[j][k]) ++count;
			}
			if (count == 1) {
				std::vector<char> time_vec{};
				time_vec.push_back('0');
				vector[i][0] = '1'; vector[j][0] = '1';
				for (unsigned int k = 1; k < vector[i].size(); ++k) {
					if (vector[i][k] != vector[j][k]) time_vec.push_back('`');
					else time_vec.push_back(vector[i][k]);
				}
				if (!time_vector.empty()) {
					if(inclusion_check(time_vec, time_vector)) time_vector.push_back(time_vec);
				}
				else time_vector.push_back(time_vec);
			}
		}
	}
	return time_vector;
}

//вспомогательный метод для безызбыточного покрытия
std::vector<char> need_implic(std::vector<std::vector<char>>& vector, std::vector<std::vector<char>>& value) {
	unsigned int count = 0, num{};
	for (unsigned int i = 0; i < value.size(); ++i) {
		unsigned int time_count = 0, time_num = i;
		for (unsigned int j = 0; j < vector.size(); ++j) {
			bool flag = true;
			for (unsigned int k = 1; k < vector[j].size(); ++k) {
				if (value[i][k] != vector[j][k] && value[i][k] != '`') {
					flag = false;
					break;
				}
			}
			if (flag) ++time_count;
		}
		if (time_count > count) {
			count = time_count;
			num = time_num;
		}
	}
	std::vector<char> time = value[num];

	std::vector<std::vector<char>> time_vector{};
	for (unsigned int i = 0; i < vector.size(); ++i) {
		bool flag = true;
		for (unsigned j = 1; j < value[num].size(); ++j) {
			if (value[num][j] != vector[i][j] && value[num][j] != '`') {
				flag = false;
				break;
			}
		}
		if (!flag) time_vector.push_back(vector[i]);
	}
	vector = time_vector;

	std::vector<std::vector<char>> time_vector_implic{};
	for (unsigned int i = 0; i < value.size(); ++i) {
		if (i != num)time_vector_implic.push_back(value[i]);
	}
	value = time_vector_implic;

	return time;
}

//поиск безызбыточного покрытия (существенных импликант нет)
std::vector<std::vector<char>> minimal_vector_implic(const std::vector<std::vector<char>>& vector, const std::vector<std::vector<char>>& value) {
	std::vector<std::vector<char>> time_vector = vector;
	std::vector<std::vector<char>> time_value = value;
	std::vector<std::vector<char>> final{};
	while (!time_vector.empty()) {
		std::vector<char> time = need_implic(time_vector, time_value);
		final.push_back(time);
	}
	return final;
}

//поиск безызбыточного покрытия (отделение существенных импликант). Вывод - вектор импликант (финальный)
std::vector<std::vector<char>> final_minimization(const std::vector<std::vector<char>>& vector, const std::vector<std::vector<char>>& value) {
	std::vector<std::vector<char>> final{};
	//поиск существенных импликант (записываются в final)
	for (unsigned i = 0; i < vector.size(); ++i) {
		unsigned int count = 0, num{};
		for (unsigned int j = 0; j < value.size(); ++j) {
			bool flag = true;
			for (unsigned int k = 1; k < vector[i].size(); ++k) {
				if (vector[i][k] != value[j][k] && value[j][k] != '`') {
					flag = false;
					break;
				}
			}
			if (flag) {
				++count;
				num = j;
			}
		}
		if (count == 1) {
			if (!final.empty()) {
				bool flag = inclusion_check(value[num], final);
				if (flag) final.push_back(value[num]);
			}
			else final.push_back(value[num]);
		}
	}
	std::cout << "significant variables : " << std::endl;
	print_function(final);

	std::vector<std::vector<char>> changed_func{};	//вектор оставшися значений функции, оставшихся после нахождения и учитывания существенных
	for (unsigned int i = 0; i < vector.size(); ++i) {
		bool flag_ = true;
		for (unsigned int j = 0; j < final.size(); ++j) {
			bool flag = true;
			for (unsigned int k = 1; k < final[j].size(); ++k) {
				if (vector[i][k] != final[j][k] && final[j][k] != '`') {
					flag = false;
					break;
				}
			}
			if (flag) {
				flag_ = false;
				break;
			}
		}
		if (flag_) changed_func.push_back(vector[i]);
	}
	std::cout << "remaining function : " << std::endl;
	print_function(changed_func);

	std::vector<std::vector<char>> final_{};	//вектор оставшихся импликант, после нахождения существенных
	for (unsigned int i = 0; i < value.size(); ++i) {
		bool flag = inclusion_check(value[i], final);
		if (flag) final_ .push_back(value[i]);
	}
	std::cout << "remaining terms : " << std::endl;
	print_function(final_);

	//финальный вектор импликант (ответ)
	std::vector<std::vector<char>> final__ = minimal_vector_implic(changed_func, final_);
	for (unsigned int i = 0; i < final__.size(); ++i) {
		final.push_back(final__[i]);
	}

	return final;
}

std::string main_algorithm(std::vector<std::vector<char>>& vector) {
	unsigned int count = 0;
	std::cout << count << " floor : " << std::endl;
	print_function(vector);

	std::ostringstream exit{};
	std::vector<std::vector<char>> start_function = vector;
	std::vector<std::vector<char>> value{};

	//переход векторов с 'k' числом "тильд" к 'k+1' и их обработка
	while (true) {
		std::vector<std::vector<char>> time_vector = minimization(vector);
		++count;
		//проверка, включен ли данный набор в последующие преобразование, если нет, то он включается в конечную ФАЛ
		for (unsigned int i = 0; i < vector.size(); ++i) {
			if (vector[i][0] == '0') {
				if (!value.empty()) {
					if (inclusion_check(vector[i], value)) value.push_back(vector[i]);
				}
				else value.push_back(vector[i]);
			}
		}
		vector = time_vector;
		if (vector.empty()) break;
		else {
			std::cout << count << " floor : " << std::endl;
			print_function(vector);
		}
	}
	std::cout << "received terms : " << std::endl;
	print_function(value);
	print_system(start_function, value);

	value = final_minimization(start_function, value);
	for (unsigned int i = 0; i < value.size(); ++i) {
		for (unsigned int j = 1; j < value[i].size(); ++j) {
			exit << value[i][j];
		}
		exit << std::endl;
	}

	return exit.str();
}

int main()
{
	std::string path; std::cout << "Input the path to the file" << std::endl;
	std::getline(std::cin, path);
	std::vector<std::vector<char>> vector = input_data(path);

	std::string path_; std::cout << "Input the path to save data" << std::endl;
	std::getline(std::cin, path_);
	
	std::ofstream fout(path_);
	if (fout.is_open()) {
		fout << main_algorithm(vector);
		fout.close();
	}

	return 0;
}