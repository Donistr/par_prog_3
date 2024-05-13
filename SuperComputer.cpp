#include "mpi.h"
#include <iostream>
#include <cstdlib>

int procsCount, procsRank;

class MultiplierMPI {
protected:
    int* a;
    int* b;
    int* result;

    int matricesSize;

    void defineMatrices() {
        for (size_t i = 0; i < matricesSize * matricesSize; ++i) {
            result[i] = 0;
            a[i] = rand() % 100;
            b[i] = rand() % 100;
        }
    }

    void delete_matrices() {
        if (procsRank == 0) {
            delete[] result;
            delete[] a;
            delete[] b;
        }
    }

    double multiplyMatrices() {
        int partSize, part;
        double start, end;
        
        start = MPI_Wtime();

        partSize = matricesSize / procsCount;
        part = partSize * matricesSize;

        MPI_Status stat;
        int* localA = new int[part], * localB = new int[part], * localC = new int[part];
        int tmp, ind;
        int next_proc, prev_proc;

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
                for (size_t k = 0; k < partSize; ++k)
                    tmp += localA[i * matricesSize + k] * localB[k * matricesSize + j];
                localC[i * matricesSize + j + partSize * procsRank] = tmp;
            }
        }

        for (int p = 1; p < procsCount; ++p) {
            next_proc = procsRank + 1;
            if (procsRank == procsCount - 1) {
                next_proc = 0;
            }
            prev_proc = procsRank - 1;
            if (procsRank == 0) {
                prev_proc = procsCount - 1;
            }

            MPI_Sendrecv_replace(localB, part, MPI_INT, next_proc, 0, prev_proc, 0, MPI_COMM_WORLD, &stat);

            for (int i = 0; i < partSize; ++i) {
                for (int j = 0; j < partSize; ++j) {
                    tmp = 0;
                    for (int k = 0; k < partSize; ++k) {
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
public:

    MultiplierMPI() {
        this->matricesSize = 0;
        result = NULL;
        a = NULL;
        b = NULL;
    }

    MultiplierMPI(int size) {
        this->matricesSize = size;
        result = new int[matricesSize * matricesSize];
        a = new int[matricesSize * matricesSize];
        b = new int[matricesSize * matricesSize];
    }

    ~MultiplierMPI() {
        delete_matrices();
    }


    double makeExperiment() {
        defineMatrices();
        double time = multiplyMatrices();

        return time;
    }
};

void write_statistics(size_t* matrix_size_array, double* time_arr, double* avg_time_array, size_t avg_array_len, size_t time_arr_len) {
    std::cout << "times: ";
    for (size_t i = 0; i < time_arr_len; ++i) {
        std::cout << time_arr[i] << ", ";
    }
    std::cout << std::endl;

    std::cout << "avg_time: ";
    for (size_t i = 0; i < avg_array_len; ++i) {
        std::cout << avg_time_array[i] << ", ";
    }
    std::cout << std::endl;

    std::cout << "matrix size: ";
    for (size_t i = 0; i < avg_array_len; ++i) {
        std::cout << matrix_size_array[i] << ", ";
    }
}

int main(int argc, char** argv) {
    srand(time(0));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &procsRank);
    MPI_Comm_size(MPI_COMM_WORLD, &procsCount);

    size_t const arr_len = 6;
    size_t const multiplying_count = 10;
    size_t arr[] = { 10, 50, 100, 250, 500, 1000 };

    double time_arr[multiplying_count], avg_time_arr[arr_len], time;
    for (size_t i = 0; i < arr_len; ++i) {
        avg_time_arr[i] = 0;
        MultiplierMPI multiplier(arr[i]);

        for (size_t j = 0; j < multiplying_count; ++j) {
            time = multiplier.makeExperiment();
            avg_time_arr[i] += time;

            if (i == arr_len - 1) {
                time_arr[j] = time;
            }
        }
        avg_time_arr[i] /= multiplying_count;

        if (procsRank == 0 && i == arr_len - 1) {
            write_statistics(arr, time_arr, avg_time_arr, arr_len, multiplying_count);
        }
    }
    
    MPI_Finalize();

    return 0;
}
