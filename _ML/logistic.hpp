#pragma once
#ifndef __LOGISTIC_HPP__
#define __LOGISTIC_HPP__

#include <vector>
#include <cmath>

#include "feature.hpp"

//回归循环次数
#define RUN_CYCLE_NUM 50000
//回归递增步长
#define ALPHA_NUM 0.001

namespace FawLearn {
	class Logistic : public Feature_Object_Collection <double> {
	public:
		Logistic (std::initializer_list<Feature_Object<double>> list) : Feature_Object_Collection (list) {
			init_theta0 ();
		}

		Logistic (Feature_Object_Collection& o) : Feature_Object_Collection (o) {
			init_theta0 ();
		}

		Logistic (const Logistic &o) : Feature_Object_Collection (o) {}

		void calc (std::vector<double> &theta/*θ*/) {
			//阶跃函数
			auto sigmoid = [] (double d) { return 1. / (1. + ::pow (2.7182818284590451, 0 - d)); };

			//构建theta，其中theta[0]代表θ0， theta[1]代表θ1，以此类推
			theta.clear ();
			ptrdiff_t i, j, k;
			theta.reserve (data [0].get_size ());
			for (i = 0; i < data [0].get_size (); ++i) theta.push_back (1);

			//矩阵计算中的临时变量
			std::vector<double> tmp;
			tmp.resize (get_size ());
			double d;

			//Logistic回归主循环
			for (i = 0; i < RUN_CYCLE_NUM; ++i) {
				for (j = 0; j < get_size (); ++j) {
					//Hypothesisθ = sigmoid(dataMatrix*weights)
					tmp [j] = 0;
					for (k = 0; k < data [0].get_size (); ++k) tmp [j] += data [j] [k] * theta [k];
					tmp [j] = sigmoid (tmp [j]);

					//error = (Hypothesisθ - labelMat)
					tmp [j] -= data [j].get_label ();
				}

				//weights = weights - alpha * dataMatrix.transpose () * error
				for (j = 0; j < data [0].get_size (); ++j) {
					d = 0;
					for (k = 0; k < get_size (); ++k) d += data [k] [j] * tmp [k];
					d *= ALPHA_NUM;
					theta [j] -= d;
				}
			}
		}

		void add_feature (Feature_Object<double> &o) override {
			Feature_Object<double> tmp (o);
			tmp.push_front (1);
			return Feature_Object_Collection::add_feature (tmp);
		}

		ptrdiff_t del_feature (Feature_Object<double> &o) override {
			Feature_Object<double> tmp (o);
			tmp.push_front (1);
			return Feature_Object_Collection::del_feature (tmp);
		}

	private:
		//在每个数据的特征前加个1，代表θ0
		void init_theta0 () {
			for (Feature_Object<double> &d : data) {
				d.push_front (1);
			}
		}
	};
}

#endif //__LOGISTIC_HPP__
