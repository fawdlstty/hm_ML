#pragma once
#ifndef __BAYES_HPP__
#define __BAYES_HPP__

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

#include "feature.hpp"
#include "hTools.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "friso/friso.h"

#ifdef __cplusplus
}
#endif
#pragma comment(lib, "friso/friso.lib")

namespace FawLearn {
	class Bayes : public Feature_Object_Collection<std::string> {
	public:
		Bayes (std::initializer_list<Feature_Object<std::string>> list) : Feature_Object_Collection (list) {}

		Bayes (Feature_Object_Collection& o) : Feature_Object_Collection (o) {}

		Bayes (const Bayes &o) : Feature_Object_Collection (o) {}

		Bayes () : Feature_Object_Collection () {
			std::ifstream ifs ("words/words_gbk.csv");
			std::string tmp_str;
			std::vector<std::string> v;
			std::getline (ifs, tmp_str);
			while (std::getline (ifs, tmp_str)) {
				if (tmp_str.length () > 0) {
					v.clear ();
					hTools::str::split (tmp_str, v, ',');
					Feature_Object<std::string> fo (v [0]);
					auto p = m_map.lower_bound (v [4]);
					if (p != m_map.upper_bound (v [4])) {
						fo << p->second;
						fo << std::stoi (v [5]);
						add_feature (fo);
					}
				}
			}
			ifs.close ();
		}

		std::string calc (std::string &doc) {
			ptrdiff_t feel_size = m_map2.size ();
			ptrdiff_t *feel = new ptrdiff_t [feel_size];
			::memset (feel, 0, sizeof (ptrdiff_t)*feel_size);

			friso_t friso = ::friso_new ();
			friso_config_t config = ::friso_new_config ();
			if (1 != ::friso_init_from_ifile (friso, config, "friso/friso.ini")) {
				return "加载friso配置文件失败！";
			}
			friso_task_t task = ::friso_new_task ();
			::friso_set_text (task, &doc [0]);
			ptrdiff_t size = get_size (), index;
			std::string word;
			while (::friso_next (friso, config, task) != NULL) {
				word = task->hits->word;
				for (index = 0; index < size; ++index)
					if (data [index].get_label () == word) break;
				if (index != size)
					feel [(ptrdiff_t) (data [index]) [0]] += (ptrdiff_t) (data [index]) [1];
			}
			::friso_free_task (task);
			//friso_free_config (config);
			::friso_free (friso);

			for (index = 0, size = 0; index < feel_size; ++index)
				if (feel [index] > feel [size]) size = index;
			if (0 == feel [size]) {
				word = "未知";
			} else {
				word = m_map2 [size];
			}

			delete[] feel;
			return word;
		}

	protected:
		static std::map<std::string, int> m_map;
		static std::map<int, std::string> m_map2;
	};
}

#endif //__BAYES_HPP__
