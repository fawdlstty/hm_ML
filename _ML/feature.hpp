#pragma once
#ifndef __FEATURE_OBJ_HPP__
#define __FEATURE_OBJ_HPP__

#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <string>
#include <sstream>
#include <initializer_list>

namespace FawLearn {

//#ifdef UNICODE
//	typedef std::string string_t;
//#else
//	typedef std::wstring string_t;
//#endif

	inline bool double_is_equal (double a, double b) {
		static const double double_error = 0.000001;
		if (a == 0 || b == 0) return fabs (a + b) < double_error;
		return fabs (fabs (a) < fabs (b) ? ((b - a) / a) : ((a - b) / b)) < double_error;
	}

	//特征类，一个类对象代表单个数据的特征集
	class Feature_Data {
	public:
		Feature_Data (std::initializer_list<double> dat) {
			data.assign (dat.begin (), dat.end ());
		}

		Feature_Data (std::vector<double> dat) {
			data.swap (dat);
		}

		Feature_Data () {}

		ptrdiff_t get_size () {
			return data.size ();
		}

		double operator[](ptrdiff_t index) {
			return data [index];
		}

		Feature_Data* operator<<(double d) {
			data.push_back (d);
			return this;
		}

		void push_front (double d) {
			data.insert (data.begin(), d);
		}

		friend void operator>>(std::istringstream &iss, Feature_Data &fd);

	private:
		std::vector<double> data;
	};

	inline void operator>>(std::istringstream &iss, Feature_Data &fd) {
		double d;
		std::string s;
		while (!iss.eof ()) {
			iss >> d;
			fd << d;
		}
	}

	//数据类，一个类对象代表一个特征集与一个标签
	template<typename T>
	class Feature_Object : public Feature_Data {
	public:
		Feature_Object (T _label, std::initializer_list<double> dat) : Feature_Data (dat) {
			label = _label;
		}

		Feature_Object (T _label) : Feature_Data () {
			label = _label;
		}

		T get_label () {
			return label;
		}

	private:
		T label;
	};

	//数据组类，一个类对象代表一组数据类类对象
	template<typename T>
	class Feature_Object_Collection {
	public:
		Feature_Object_Collection (std::initializer_list<Feature_Object<T>> list) {
			data.assign (list.begin (), list.end ());
		}

		Feature_Object_Collection (const Feature_Object_Collection &o) {
			data.assign (o.data.begin (), o.data.end ());
		}

		Feature_Object_Collection () {}

		virtual void add_feature (Feature_Object<T> &o) {
			data.push_back (o);
		}

		virtual ptrdiff_t del_feature (Feature_Object<T> &o) {
			ptrdiff_t ret = 0, size = o.get_size ();
			for (ptrdiff_t i = 0, j; i < (ptrdiff_t) data.size (); ++i) {
				for (j = 0; j < size; ++j)
					if (double_is_equal (o [j], data [i] [j])) break;
				if (j == size && o.get_label () == data [i].get_label ()) {
					data.erase (data.begin () + i);
					++ret;
				}
			}
			return ret;
		}

		ptrdiff_t get_size () {
			return data.size ();
		}

		Feature_Object<T>& operator[](ptrdiff_t index) {
			return data [index];
		}

	protected:
		std::vector<Feature_Object<T>> data;
	};

	//typedef Feature_Data *pFeature_Data;
	//template<typename T> typedef Feature_Object<T> *pFeature_Object<T>;
	//template<typename T> typedef Feature_Object_Collection<T> *pFeature_Object_Collection<T>;
}

#endif //__FEATURE_OBJ_HPP__
