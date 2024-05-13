#pragma once
#include "Multiplier.h"
class MultiplierMPI : public Multiplier
{
	double multiplyMatrices() override;
public:
	MultiplierMPI();
	MultiplierMPI(int size, const std::string& res_path, const std::string& validator_path, const std::string& stats_path, bool validating);

	~MultiplierMPI();
};

