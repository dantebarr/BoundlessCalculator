#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>
#include <map>
#include <string>
#include "lepton/Exception.h"

extern std::map<std::string, Eigen::MatrixXd> matrixMap;

void storeMatrixValue(std::string matrixName, Eigen::MatrixXd matrixValue);
void deleteMatrixValue(std::string matrixName);
Eigen::MatrixXd getMatrixValue(std::string matrixName);

