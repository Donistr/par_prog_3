#include "MultiplierMPI.h"
#include "mpi.h"

double MultiplierMPI::multiplyMatrices()
{
	int procsCount, procsRank;
	int partSize, part;
	double start, end;

	MPI_Comm_rank(MPI_COMM_WORLD, &procsRank);
	MPI_Comm_size(MPI_COMM_WORLD, &procsCount);

	start = MPI_Wtime();

    partSize = matricesSize / procsCount;
	part = partSize * matricesSize;

	MPI_Status stat;

	int* localA = new int[part], * localB = new int[part], * localC = new int[part];
	int tmp, ind;
	int nextProc, prevProc;

	if (procsRank == 0) {
		for (size_t i = 0; i < partSize; ++i) {
			for (size_t j = 0; j < partSize; ++j) {
				tmp = b[i * matricesSize + j];
                b[i * matricesSize + j] = b[j * matricesSize + i];
                b[j * matricesSize + i] = tmp;
			}
		}
	}

	MPI_Scatter(a, part, MPI_INT, localA, part, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(b, part, MPI_INT, localB, part, MPI_INT, 0, MPI_COMM_WORLD);

	for (size_t i = 0; i < partSize; ++i) {
		for (size_t j = 0; j < partSize; ++j) {
			tmp = 0;
			for (size_t k = 0; k < partSize; ++k) {
                tmp += localA[i * matricesSize + k] * localB[k * matricesSize + j];
            }
            localC[i * matricesSize + j + partSize * procsRank] = tmp;
		}
	}

	for (int p = 1; p < procsCount; ++p) {
        nextProc = procsRank + 1;
		if (procsRank == procsCount - 1) {
            nextProc = 0;
        }
        prevProc = procsRank - 1;
		if (procsRank == 0) {
            prevProc = procsCount - 1;
        }

		MPI_Sendrecv_replace(localB, part, MPI_INT, nextProc, 0, prevProc, 0, MPI_COMM_WORLD, &stat);

		for (int i = 0; i < partSize; ++i) {
			for (int j = 0; j < partSize; ++j) {
				tmp = 0;
				for (int k = 0; k < part; ++k) {
                    tmp += localA[i * matricesSize + k] * localB[k * matricesSize + j];
                }

				if (procsRank - p >= 0) {
                    ind = procsRank - p;
                } else {
                    ind = procsCount - p + procsRank;
                }

                localC[i * matricesSize + j + ind * partSize] = tmp;
			}
		}
	}

	MPI_Gather(localC, part, MPI_INT, result, part, MPI_INT, 0, MPI_COMM_WORLD);

	delete[] localA;
	delete[] localB;
	delete[] localC;

	end = MPI_Wtime();

	return end - start;
}

MultiplierMPI::MultiplierMPI()
{
	this->matricesSize = 0;
	this->resultFolder = "";
	this->validatorPath = "";
}

MultiplierMPI::MultiplierMPI(int size, const std::string& res_path, const std::string& validator_path, const std::string& stats_path, bool validating)
{
	this->matricesSize = size;
	this->resultFolder = res_path;
	_mkdir(resultFolder.c_str());
	this->statsPath = stats_path;
	this->validatorPath = validator_path;
	this->validating = validating;
}

MultiplierMPI::~MultiplierMPI()
{
    deleteMatrix(result);
    deleteMatrix(a);
    deleteMatrix(b);
}
