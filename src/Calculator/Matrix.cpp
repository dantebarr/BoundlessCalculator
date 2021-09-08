#include "Matrix.h"

std::map<std::string, Eigen::MatrixXd> matrixMap;

//TO DO: should we prealloate entries in map and do error checking for names;
void storeMatrixValue(std::string matrixName, Eigen::MatrixXd matrixValue) {
    matrixMap[matrixName] = matrixValue;
}

void deleteMatrixValue(std::string matrixName) {
    matrixMap.erase(matrixName);
}

Eigen::MatrixXd getMatrixValue(std::string matrixName) {
    if (matrixMap.find(matrixName) != matrixMap.end()) {
        return matrixMap[matrixName];
    }

    throw Lepton::Exception("Error: Undefined matrix value");
}