#pragma once
#ifndef __KNN_HPP__
#define __KNN_HPP__

#include <map>
#include <cmath>

#include "feature.hpp"

namespace FawLearn {
	//K-近邻算法实现
	class KNN : public Feature_Object_Collection <std::string> {
	public:
		KNN (std::initializer_list<Feature_Object<std::string>> list) : Feature_Object_Collection (list) {}

		KNN (Feature_Object_Collection& o) : Feature_Object_Collection (o) {}

		KNN (const KNN &o) : Feature_Object_Collection (o) {}

		std::string calc (Feature_Data dat, ptrdiff_t k) {
			Feature_Object<std::string> **o = new Feature_Object<std::string>* [k];
			double *sqc = new double [k];

			ptrdiff_t i, j, used, count = data.size (), size = dat.get_size ();
			double squared = 0;
			for (i = used = 0; i < count; ++i, squared = 0) {
				for (j = 0; j < size; ++j)
					squared += (pow (data [i] [j] - dat [j], 2));
				squared = ::pow (squared, 0.5);

				if (used < k) {
					++used;
					j = used - 1;
				} else if (sqc [k - 1] <= squared) {
					j = -1;
				} else {
					j = used - 1;
				}
				for (; j > 0; o [j] = o [j - 1], sqc [j] = sqc [j - 1], --j) {
					if (sqc [j - 1] < squared) {
						sqc [j] = squared;
						o [j] = &data [i];
						break;
					}
				}
				if (j == 0) {
					o [0] = &data [i];
					sqc [0] = squared;
				}
			}

			std::map<std::string, ptrdiff_t> m;
			std::string s;
			for (i = 0; i < used; ++i) ++m [o [i]->get_label ()];
			for (i = 0; i < used; ++i) {
				if (i == 0 || j < m [o [i]->get_label ()]) {
					j = m [s = o [i]->get_label ()];
				}
			}

			delete [] sqc;
			delete [] o;
			return s;
		}
	};
}

#endif //__KNN_HPP__
