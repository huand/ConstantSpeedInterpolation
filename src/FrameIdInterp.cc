// Copyright 2019 Hubert Andre alstom

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
struct Data {
  // input values
  std::vector<double> id;  // TODO(hubert): to change after interp can accept
                           // int
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> z;
  double length = 1828;
  // computed values
  std::vector<double> d;
};

struct InterpData {
  std::vector<double> x;
  std::vector<double> id;
  double step;
};

double interp(std::vector<double> x, std::vector<double> y, double xq) {
  // TODO(hubert): check if x is stricly growing, x.size()==y.size()
  // TODO(hubert): check bounds
  // TODO(hubert): template for other types
  int id0;
  for (size_t i = 0; i < x.size(); i++) {
    if (xq - x[i] >= 0 && xq - x[i + 1] <= 0) {
      id0 = i;
      break;
    }
  }
  double x0, x1, y0, y1, yq;
  x0 = x[id0];
  x1 = x[id0 + 1];
  y0 = y[id0];
  y1 = y[id0 + 1];

  yq = y0 + (y1 - y0) / (x1 - x0) * (xq - x0);

  return yq;
}

int main(int argc, char const *argv[]) {
  std::ifstream infile("trajectory/CEF_trajectory.txt");
  if (!infile)
    std::cout << "Couldn't Open the trajectory text file.." << std::endl;
  int id;
  double x, y, z;
  char c;
  Data data;
  int k = 0;
  while (infile >> id >> c >> x >> c >> y >> c >> z) {
    data.id.push_back(id);
    data.x.push_back(x);
    data.y.push_back(y);
    data.z.push_back(z);
    if (k == 0) {
      data.d.push_back(0);
    } else {
      double dx = pow(pow(data.x[k] - data.x[k - 1], 2) +
                          pow(data.y[k] - data.y[k - 1], 2) +
                          pow(data.z[k] - data.z[k - 1], 2),
                      0.5);
      data.d.push_back(data.d[k - 1] + dx);
    }
    k++;
  }

  for (auto &&i : data.d) {
    i = i * data.length / data.d.back();
  }
  InterpData idata;
  idata.step = 0.05;
  idata.x.push_back(0);
  idata.id.push_back(0);
  k = 0;
  while (idata.x.back() < data.length) {
    k++;
    double xq = k * idata.step;
    idata.x.push_back(xq);
    idata.id.push_back(interp(data.d, data.id, xq));
  }
  // TODO(hubert): check if last x is <length

  std::ofstream outfile("id.txt");

  for (auto &&i : idata.id) {
    outfile << i << '\n';
  }
  outfile.close();
  return 0;
}
