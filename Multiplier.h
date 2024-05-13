#pragma once
#include <string>
#include <fstream>
#include <direct.h>
#include <iostream>

class Multiplier {
protected:
    int* a = nullptr;
    int* b = nullptr;
	int* result = nullptr;

	std::string resultFolder;
    std::string validatorPath;
    std::string statsPath;

	int matricesSize;
	bool validating;

	int* createMatrix() const;
	static void deleteMatrix(const int* matrix);

	void writeResults(const std::string& path, int* matrix, double time = -1) const;
	static void validateResult(const std::string& script_path, const std::string& first_matrix_path, const std::string& second_matrix_path, const std::string& result_path);

	void defineMatrices();

    virtual double multiplyMatrices() = 0;
public:

	double makeExperiment();

	void setMatricesSize(int size);
	std::string getStatsPath();
};
