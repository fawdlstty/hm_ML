#pragma once
#ifndef __LOGISTIC_HPP__
#define __LOGISTIC_HPP__

#include <vector>
#include <cmath>

#include "feature.hpp"

//�ع�ѭ������
#define RUN_CYCLE_NUM 50000
//�ع��������
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

		void calc (std::vector<double> &theta/*��*/) {
			//��Ծ����
			auto sigmoid = [] (double d) { return 1. / (1. + ::pow (2.7182818284590451, 0 - d)); };

			//����theta������theta[0]�����0�� theta[1]�����1���Դ�����
			theta.clear ();
			ptrdiff_t i, j, k;
			theta.reserve (data [0].get_size ());
			for (i = 0; i < data [0].get_size (); ++i) theta.push_back (1);

			//��������е���ʱ����
			std::vector<double> tmp;
			tmp.resize (get_size ());
			double d;

			//Logistic�ع���ѭ��
			for (i = 0; i < RUN_CYCLE_NUM; ++i) {
				for (j = 0; j < get_size (); ++j) {
					//Hypothesis�� = sigmoid(dataMatrix*weights)
					tmp [j] = 0;
					for (k = 0; k < data [0].get_size (); ++k) tmp [j] += data [j] [k] * theta [k];
					tmp [j] = sigmoid (tmp [j]);

					//error = (Hypothesis�� - labelMat)
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
		//��ÿ�����ݵ�����ǰ�Ӹ�1�������0
		void init_theta0 () {
			for (Feature_Object<double> &d : data) {
				d.push_front (1);
			}
		}
	};
}

#endif //__LOGISTIC_HPP__
