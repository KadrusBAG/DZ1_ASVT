#include "pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

//преобразование данных из файла по заданному пути в vector (значения булевой функции)
std::vector<std::vector<char>> input_data(const std::string& path) {
	std::vector<std::vector<char>> vector{};
	std::ifstream fin(path);
	if (fin.is_open()) {
		std::string time{};
		while (std::getline(fin, time)) {
			std::vector<char> time_vec{};
			for (unsigned int i = 0; i < time.size(); ++i) {
				time_vec.push_back(time[i]);
			}
			vector.push_back(time_vec);
		}
	}
	fin.close();
	return vector;
}

//вывод системы в консоль
void print_system(const std::vector<std::vector<char>>& vector, const std::vector<std::vector<char>>& coefs) {
	std::cout << "Current system (start):" << std::endl;
	for (unsigned int i = 0; i < vector.size(); ++i) {
		for (unsigned int k = 0; k < vector[i].size(); ++k) {
			std::cout << vector[i][k];
		}
		std::cout << " = ";
		unsigned int count = 0;
		for (unsigned j = 0; j < coefs.size(); ++j) {
			bool flag = true;
			for (unsigned int k = 0; k < coefs[j].size(); ++k) {
				if (vector[i][k] != coefs[j][k] && coefs[j][k] != '`') {
					flag = false;
					break;
				}
			}
			if (flag) {
				if (count != 0) std::cout << " v ";
				++count;
				for (unsigned int k = 0; k < coefs[j].size(); ++k) {
					std::cout << coefs[j][k];
				}
			}
		}
		std::cout << std::endl;
	}
	std::cout << "The end of the system\n\n";
}

//создание всех коэффициентов, нужных для системы
std::vector<std::vector<char>> generation_all_coefficients(const unsigned int& n) {
	std::vector<std::vector<char>> final{};
	std::vector<char> time{};
	for (unsigned int i = 0; i < n; ++i) {
		time.push_back('`');
	}
	while (true) {
		bool flag = true;
		for (int i = n - 1; i >= 0; --i) {
			if (time[i] == '`') {
				time[i] = '0';
				flag = false;
				break;
			}
			else if (time[i] == '0') {
				time[i] = '1';
				flag = false;
				break;
			}
			else if (time[i] == '1') {
				time[i] = '`';
			}
		}
		if (flag) break;
		else final.push_back(time);
	}
	return final;
}

//проверка, включен ли included в vector (true - его нет в vector, false - иначе)
bool inclusion_check(const std::vector<char>& included, const std::vector<std::vector<char>>& vector) {
	for (unsigned int i = 0; i < vector.size(); ++i) {
		bool flag = true;
		for (unsigned int j = 0; j < included.size(); ++j) {
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

//создание всех нулевых булевых функций
std::vector<std::vector<char>> generation_zero_vector(const std::vector<std::vector<char>>& vector) {
	std::vector<std::vector<char>> final{};
	std::vector<char> time{};
	for (unsigned int i = 0; i < vector[0].size(); ++i) {
		time.push_back('0');
	}
	if (inclusion_check(time, vector)) final.push_back(time);

	while (true) {
		bool flag = true;
		for (int i = vector[0].size() - 1; i >= 0; --i) {
			if (time[i] == '0') {
				time[i] = '1';
				flag = false;
				break;
			}
			else if (time[i] == '1') {
				time[i] = '0';
			}
		}
		if (flag) break;
		else {
			if (inclusion_check(time, vector)) final.push_back(time);
		}
	}
	return final;
}

//проверка, включен ли коэффициент included в vector - значения булевой функции (true - его нет в vector, false - иначе)
bool inclusion_check_coef(const std::vector<char>& included, const std::vector<std::vector<char>>& vector) {
	for (unsigned int i = 0; i < vector.size(); ++i) {
		bool flag = true;
		for (unsigned int j = 0; j < included.size(); ++j) {
			if (vector[i][j] != included[j] && included[j] != '`') {
				flag = false;
				break;
			}
		}
		if (flag) return false;
	}
	return true;
}

//удалени коэффициентов, соответствующим нулевым булевым функциям
std::vector<std::vector<char>> delete_zero_coefs(const std::vector<std::vector<char>>& coefs, const std::vector<std::vector<char>>& vector) {
	std::vector<std::vector<char>> time{};
	for (unsigned int i = 0; i < coefs.size(); ++i) {
		if (inclusion_check_coef(coefs[i], vector))	time.push_back(coefs[i]);
	}
	return time;
}

//нахождение коэффициента, покрывающего больше всего булевых функций и редактирование системы
std::vector<char> take_coef(std::vector<std::vector<char>>& vector, std::vector<std::vector<char>> coefs) {
	unsigned int count = 0, num{};
	for (unsigned int i = 0; i < coefs.size(); ++i) {
		unsigned int time_count = 0, time_num = i;
		for (unsigned int j = 0; j < vector.size(); ++j) {
			bool flag = true;
			for (unsigned int k = 0; k < vector[j].size(); ++k) {
				if (vector[j][k] != coefs[i][k] && coefs[i][k] != '`') {
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
	std::vector<char> exit = coefs[num];

	std::vector<std::vector<char>> time_vector{}, time_coefs{};
	for (unsigned int i = 0; i < vector.size(); ++i) {
		bool flag = true;
		for (unsigned j = 0; j < exit.size(); ++j) {
			if (exit[j] != vector[i][j] && exit[j] != '`') {
				flag = false;
				break;
			}
		}
		if (!flag) time_vector.push_back(vector[i]);
	}

	for (unsigned int i = 0; i < coefs.size(); ++i) {
		if (!inclusion_check_coef(coefs[i], time_vector)) time_coefs.push_back(coefs[i]);
	}

	vector = time_vector;
	coefs = time_coefs;
	return exit;
}

//поиск коэффициентов в упрощенной системе (без нулевых значений функции и коэффициентов, соответствующих этим значением)
std::vector<std::vector<char>> search_coefs(const std::vector<std::vector<char>>& vector, const std::vector<std::vector<char>>& coefs) {
	std::vector<std::vector<char>> final{}, vec = vector, coef = coefs;
	while (!vec.empty()) {
		std::vector<char> time = take_coef(vec, coef);
		final.push_back(time);
		std::cout << "finding coefficient : ";
		for (unsigned int i = 0; i < time.size(); ++i) std::cout << time[i];
		std::cout << std::endl;
		if (!vec.empty()) print_system(vec, coef);
	}
	return final;
}

std::string main_algorithm(const std::vector<std::vector<char>>& vector, const unsigned int& n) {
	std::vector<std::vector<char>> coefs = generation_all_coefficients(n);
	std::vector<std::vector<char>> zero_vector = generation_zero_vector(vector);
	std::vector<std::vector<char>> coefs_upgrade = delete_zero_coefs(coefs, zero_vector);

	print_system(vector, coefs_upgrade);

	std::vector<std::vector<char>> final = search_coefs(vector, coefs_upgrade);

	std::ostringstream exit{};
	for (unsigned int i = 0; i < final.size(); ++i) {
		for (unsigned int j = 0; j < final[i].size(); ++j) {
			exit << final[i][j];
		}
		exit << std::endl;

	}
	return exit.str();
}

int main()
{
	unsigned int n = 6;

	std::string path; std::cout << "Input the path to the file" << std::endl;
	std::getline(std::cin, path);
	std::vector<std::vector<char>> function = input_data(path);


	std::string path_; std::cout << "Input the path to save data" << std::endl;
	std::getline(std::cin, path_);
	std::ofstream fout(path_);
	if (fout.is_open()) {
		fout << main_algorithm(function, n);
		fout.close();
	}

	return 0;
}