#pragma once
#ifndef __DTREE_HPP__
#define __DTREE_HPP__

#include <vector>
#include <set>
#include <map>
#include <functional>
#include <cmath>

#include "feature.hpp"

namespace FawLearn {
	//分支结构
	struct Decision_Tree_Branch;
	typedef Decision_Tree_Branch *pDecision_Tree_Branch;
	struct Decision_Tree_Branch {
		ptrdiff_t feature_index;
		union {
			std::map<double, pDecision_Tree_Branch> *child;
			std::string *label;
		};
	};

	class DTree : public Feature_Object_Collection<std::string> {
	public:
		DTree (std::initializer_list<Feature_Object<std::string>> list) : Feature_Object_Collection (list) {}

		DTree (Feature_Object_Collection& o) : Feature_Object_Collection (o) {}

		DTree (const DTree &o) : Feature_Object_Collection (o) {}

		~DTree () {
			release_branch (m_root);
		}

		//计算
		std::string calc (Feature_Data dat) {
			if (!m_root) update_tree ();

			//应该进入哪个分支？
			std::function<std::string (pDecision_Tree_Branch)> calc_tree = [&calc_tree, &dat] (pDecision_Tree_Branch p) {
				if (p->feature_index != -1) {
					decltype(p->child->begin ()) min_match = p->child->begin ();
					double tmp = dat [p->feature_index];
					double min_err = ::fabs (min_match->first - tmp);
					for (auto i = min_match; i != p->child->end (); ++i) {
						if (::fabs ((i->first - tmp)) < min_err) {
							min_err = ::fabs (i->first - tmp);
							min_match = i;
						}
					}
					return calc_tree (min_match->second);
				} else {
					return *(p->label);
				}
			};

			return calc_tree (m_root);
		}

		//构建决策树
		void update_tree () {
			if (m_root) release_branch (m_root);
			ptrdiff_t axis_num = data [0].get_size ();

			//计算香农熵
			auto calc_shannon_entropy = [] (std::vector<Feature_Object<std::string>*> &dataset) {
				std::map<std::string, ptrdiff_t> m;
				for (Feature_Object<std::string> *&fo : dataset) ++m [fo->get_label ()];
				double prob, se = 0.0;
				for (auto i = m.begin (); i != m.end (); ++i) {
					prob = i->second / (double) dataset.size ();
					se -= prob * ::log2 (prob);
				}
				return se;
			};

			//切割数据集
			auto split_dataset = [] (std::vector<Feature_Object<std::string>*> &dataset, std::vector<Feature_Object<std::string>*> &after, ptrdiff_t axis, double value) {
				after.clear ();
				for (Feature_Object<std::string>* &fo : dataset) {
					if (double_is_equal ((*fo) [axis], value)) after.push_back (fo);
				}
			};

			//一串由0和1组成的字符串，长度等于特征个数，1表示已被决策树所使用，0表示未使用 (～￣▽￣)～
			std::string axis_sign (axis_num, '0');

			//选择最好的数据划分特征 <(￣ˇ￣)/
			auto choose_best_feature = [&axis_sign, &calc_shannon_entropy, &axis_num, &split_dataset] (std::vector<Feature_Object<std::string>*> &dataset) {
				std::set<double> s;
				double base_entropy = calc_shannon_entropy (dataset), prob, new_entropy, best_info_gain = -1, info_gain;
				std::vector<Feature_Object<std::string>*> after;
				ptrdiff_t best_feature = -1;
				for (ptrdiff_t axis = 0, g, size = dataset.size (); axis < axis_num; ++axis) {
					if (axis_sign[axis] == '1') continue;
					s.clear ();
					for (g = 0; g < size; ++g) s.insert ((*dataset [g]) [axis]);
					new_entropy = 0.0;
					for (double d : s) {
						split_dataset (dataset, after, axis, d);
						prob = after.size () / size;
						new_entropy += prob * calc_shannon_entropy (after);
					}
					info_gain = base_entropy - new_entropy;
					if (info_gain > best_info_gain || best_info_gain == -1) {
						best_info_gain = info_gain;
						best_feature = axis;
					}
				}
				if (best_feature != -1) axis_sign [best_feature] = '1';
				return best_feature;
			};

			//创建决策树分支
			std::function<pDecision_Tree_Branch (std::vector<Feature_Object<std::string>*> &, ptrdiff_t)> create_branch = [&create_branch, &choose_best_feature] (std::vector<Feature_Object<std::string>*> &dataset, ptrdiff_t feature_num) {
				pDecision_Tree_Branch ret = new Decision_Tree_Branch;

				ptrdiff_t i, size = dataset.size ();
				for (i = 1; i < size; ++i) {
					if (dataset [i]->get_label() != dataset[0]->get_label()) break;
				}
				if (i == dataset.size ()) {
					ret->feature_index = -1;
					ret->label = new std::string (dataset [0]->get_label ());
				} else {
					if (feature_num == 1) {
						std::map<std::string, ptrdiff_t> m;
						for (Feature_Object<std::string> *&fo : dataset) ++m [fo->get_label ()];
						i = -1;
						ret->label = new std::string ("");
						for (auto &t : m) {
							if (t.second > i) {
								*ret->label = t.first;
								i = t.second;
							}
						}
						ret->feature_index = -1;
					} else {
						ret->feature_index = choose_best_feature (dataset);
						std::map<double, std::vector<Feature_Object<std::string>*>> p;
						std::map<double, std::vector<Feature_Object<std::string>*>>::iterator j;
						for (i = 0; i < size; ++i) {
							for (j = p.begin (); j != p.end (); ++j) {
								if (double_is_equal (j->first, (*dataset [i]) [ret->feature_index])) break;
							}
							if (j == p.end ()) {
								p.insert ({ (*dataset [i]) [ret->feature_index], { dataset [i] } });
							} else {
								j->second.push_back (dataset [i]);
							}
						}
						ret->child = new std::map<double, pDecision_Tree_Branch> ();
						for (j = p.begin (); j != p.end (); ++j) {
							ret->child->insert ({ j->first, create_branch (j->second, feature_num - 1) });
						}
					}
				}
				return ret;
			};

			std::vector<Feature_Object<std::string>*> dat;
			for (auto i = data.begin (); i != data.end (); ++i) dat.push_back (&*i);
			m_root = create_branch (dat, axis_num);
		}

		void add_feature (Feature_Object<std::string> &o) override {
			if (m_root) {
				release_branch (m_root);
				m_root = nullptr;
			}
			return Feature_Object_Collection::add_feature (o);
		}

		ptrdiff_t del_feature (Feature_Object<std::string> &o) override {
			if (m_root) {
				release_branch (m_root);
				m_root = nullptr;
			}
			return Feature_Object_Collection::del_feature (o);
		}

	protected:
		//释放分支
		static void release_branch (pDecision_Tree_Branch p) {
			if (p) {
				if (p->feature_index != -1) {
					for (auto i = p->child->begin (); i != p->child->end (); ++i)
						release_branch (i->second);
					delete p->child;
				} else {
					delete p->label;
				}
				delete p;
			}
		}

		pDecision_Tree_Branch m_root = nullptr;
	};
}

#endif //__DTREE_HPP__
