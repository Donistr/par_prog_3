#include "Multiplier.h"

int* Multiplier::createMatrix() const {
	int* matrix = new int [matricesSize * matricesSize];
	for (size_t i = 0; i < matricesSize * matricesSize; ++i) {
		matrix[i] = rand() % 100;
	}
	return matrix;
}

void Multiplier::deleteMatrix(const int* matrix) {
	delete matrix;
}

void Multiplier::writeResults(const std::string& path, int* matrix, double time) const {
	std::ofstream file(path);

	file << "{\"matrix\": [";
	for (size_t i = 0; i < matricesSize; ++i) {
		file << "[";
		for (size_t j = 0; j < matricesSize; ++j) {
			file << matrix[i * matricesSize + j];
			if (j < matricesSize - 1)
				file << ",";
		}
		if (i < matricesSize - 1) {
            file << "],";
        }
	}
	file << "]],\"size\": " << matricesSize * matricesSize;
	if (time > 0) {
        file << ", \"time\": " << time;
    }
	file << "}";

	file.close();
}

void Multiplier::validateResult(const std::string& script_path, const std::string& first_matrix_path, const std::string& second_matrix_path, const std::string& result_path) {
	system(std::string("py " + script_path + " " + first_matrix_path + " " + second_matrix_path + " " + result_path).c_str());
}

void Multiplier::defineMatrices() {
    a = createMatrix();
    b = createMatrix();
    result = createMatrix();

}

double Multiplier::makeExperiment() {
	_mkdir(std::string(resultFolder + "\\" + std::to_string(matricesSize)).c_str());
	std::string a_path = std::string(resultFolder + "\\" + std::to_string(matricesSize) + "\\first_matrix_" + std::to_string(matricesSize) + ".json");
	std::string b_path = std::string(resultFolder + "\\" + std::to_string(matricesSize) + "\\second_matrix_" + std::to_string(matricesSize) + ".json");
	std::string res_path = std::string(resultFolder + "\\" + std::to_string(matricesSize) + "\\result_" + std::to_string(matricesSize) + ".json");

    defineMatrices();
    writeResults(a_path, a);
    writeResults(b_path, b);
	double time = multiplyMatrices();
    writeResults(res_path, result, time);

    deleteMatrix(a);
    deleteMatrix(b);
    deleteMatrix(result);

	if (validating) {
        validateResult(validatorPath, a_path, b_path, res_path);
    }

	return time;
}

void Multiplier::setMatricesSize(int size) {
	this->matricesSize = size;
}

std::string Multiplier::getStatsPath()
{
	return statsPath;
}
