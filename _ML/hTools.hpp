//
// file:        hTools.hpp
// author:      Fawdlstty<fawdlstty@qq.com>
// description: something other functions
//

#pragma once

#ifndef __TOOLS_HPP_
#define __TOOLS_HPP_

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <sstream>
#include <cstring>
#include <cstdarg>

namespace hTools {
//字符串相关处理
class str {
public:
	//清除字符串开始部分空格
	template <typename charT>
	static void trimLeft (std::basic_string<charT> &str) {
		str.erase (0, str.find_first_not_of (' '));
	}

	//清除字符串结束部分空格
	template <typename charT>
	static void trimRight (std::basic_string<charT> &str) {
		str.erase (str.find_last_not_of (' ') + 1);
	}

	//清楚两端空格
	template <typename charT>
	static void trim (std::basic_string<charT> &str) {
		str.erase (0, str.find_first_not_of (' '));
		str.erase (str.find_last_not_of (' ') + 1);
	}

	//删除字符串中指定字符
	template <typename charT>
	static void erase (std::basic_string<charT> &str, const charT &charactor) {
		str.erase (remove_if (str.begin (), str.end (), bind2nd (std::equal_to<charT> (), charactor)), str.end ());
	}

	//替换字符串中指定字符串
	template <typename charT>
	static int replace (std::basic_string<charT> &str, const std::basic_string<charT> &strObj, const std::basic_string<charT> &strDest) {
		int ret = 0;
		charT pos = str.find (strObj);
		while (pos != std::basic_string<charT>::npos) {
			ret++;
			str.replace (pos, strObj.size (), strDest);
			pos = str.find (strObj);
		}
		return ret;
	}

	//一行中的字符串截断，并可转为其他类型
	template <typename T, typename charT>
	static int split_aLine_conv (const std::basic_string<charT> &str, std::vector<T> &seq, charT separator) {
		if (str.empty ()) return 0;
		int count = 0;
		std::basic_stringstream<charT> bs (str);
		for (std::basic_string<charT> s; std::getline (bs, s, separator); count++) {
			typename T val;
			std::basic_stringstream<charT> bss (s);
			bss >> val;
			seq.push_back (val);
		}
		return count;
	}

	//字符串截断
	template <typename charT>
	static void split (std::basic_string<charT> s, std::vector<std::basic_string<charT> >& v, char ch = ' ') {
		ptrdiff_t start = 0, p, len = s.length ();
		do {
			p = s.find (ch, start);
			if (p == -1) p = len;
			s [p] = '\0';
			if (s [start] != '\0') v.push_back (&s [start]);
			start = p + 1;
		} while (start < len);
	}

	//字符串格式化
	template <typename charT>
	static std::basic_string<charT> format (std::basic_string<charT> fmt_str, ...) {
		//来源：http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
		ptrdiff_t final_n, n = ((ptrdiff_t) fmt_str.size ()) * 2;
		std::basic_string<charT> str;
		std::unique_ptr<charT []> formatted;
		va_list ap;
		while (1) {
			formatted.reset (new charT [n]);
			//strcpy_s (&formatted [0], fmt_str.size (), fmt_str.c_str ());
			va_start (ap, fmt_str);
			final_n = vsnprintf_s (&formatted [0], n, _TRUNCATE, fmt_str.c_str (), ap);
			va_end (ap);
			if (final_n < 0 || final_n >= n)
				n += abs (final_n - n + 1);
			else
				break;
		}
		return std::basic_string<charT> (formatted.get ());
	}
};

//比较字符串大小，用于hash_map<string, int, hash_compare<string, string_less> > var;
template <typename charT>
class string_less : public std::binary_function<const std::basic_string<charT>, const std::basic_string<charT>, bool> {
public:
	result_type operator()(const first_argument_type& left, const second_argument_type& right) const {
		return left.compare (right) < 0;
	}
};

//base64 加密、解密
#pragma region base64加密解密
template <typename charT>
class base64 {
public:
	//base64 加密
	static bool encode (std::basic_string<charT>& _old, std::basic_string<charT>& _new) {
		static charT _code [] = {
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
			'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
			'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',   'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
			'w', 'x', 'y', 'z', '0', '1', '2', '3',   '4', '5', '6', '7', '8', '9', '+', '/',
		};
		ptrdiff_t i, len = _old.length ();
		_new.resize (((len % 3 ? 1 : 0) + len / 3) * 4);
		//主编码循环
		for (i = 0; i < len / 3; ++i) {
			_new [i * 4] = _code [_old [i * 3] >> 2];
			_new [i * 4 + 1] = _code [((_old [i * 3] & 0x03) << 4) | (_old [i * 3 + 1] >> 4)];
			_new [i * 4 + 2] = _code [((_old [i * 3 + 1] & 0x0F) << 2) | (_old [i * 3 + 2]) >> 6];
			_new [i * 4 + 3] = _code [_old [i * 3 + 2] & 0x3F];
		}
		//长度不为3的倍数
		if (len % 3) {
			//第一字节肯定不会溢出，故不检查边界
			_new [i * 4] = _code [_old [i * 3] >> 2];
			_new [i * 4 + 1] = _code [((_old [i * 3] & 0x03) << 4) | (i * 3 + 1 < len ? _old [i * 3 + 1] >> 4 : 0)];
			//倒数第二位可能为'='
			if (1 == len % 3)
				_new [i * 4 + 2] = '=';
			else
				_new [i * 4 + 2] = _code [(i * 3 + 1 < len ? (_old [i * 3 + 1] & 0x0F) << 2 : 0)];
			//末位肯定为'='
			_new [i * 4 + 3] = '=';
		}
		return true;
	}

	//base64 解密
	static bool decode (std::string& _old, std::string& _new) {
		static charT _ucode [] = {
			0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0,
			0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  62, 0,  0,  0,  63,
			52, 53, 54, 55, 56, 57, 58, 59,   60, 61, 0,  0,  0,  64, 0,  0,
			0,  0,  1,  2,  3,  4,  5,  6,    7,  8,  9,  10, 11, 12, 13, 14,
			15, 16, 17, 18, 19, 20, 21, 22,   23, 24, 25, 0,  0,  0,  0,  0,
			0,  26, 27, 28, 29, 30, 31, 32,   33, 34, 35, 36, 37, 38, 39, 40,
			41, 42, 43, 44, 45, 46, 47, 48,   49, 50, 51, 0,  0,  0,  0,  0,
		};
		ptrdiff_t i, len = _old.length ();
		ptrdiff_t len2 = len / 4 * 3;
		//判断末位是否有等号、有几个等号
		if ('=' == _old [len - 2])
			len2 -= 2;
		else if ('=' == _old [len - 1])
			len2--;
		else if (len % 4)
			len2 += (len % 4 - 1);
		_new.resize (len2);
		//主解密循环
		for (i = 0; i < len2 / 3; ++i) {
			_new [i * 3] = (_ucode [_old [i * 4]] << 2) | (_ucode [_old [i * 4 + 1]] >> 4);
			_new [i * 3 + 1] = ((_ucode [_old [i * 4 + 1]] & 0x0F) << 4) | (_ucode [_old [i * 4 + 2]] >> 2);
			_new [i * 3 + 2] = ((_ucode [_old [i * 4 + 2]] & 0x03) << 6) | (_ucode [_old [i * 4 + 3]]);
		}
		//末位为一个或两个等号的情况，这时候不能通过主解密循环进行解密
		if (len2 % 3) {
			_new [i * 3] = (_ucode [_old [i * 4]] << 2) | (_ucode [_old [i * 4 + 1]] >> 4);
			if (2 == len % 3)
				_new [i * 3 + 1] = ((_ucode [_old [i * 4 + 1]] & 0x0F) << 4) | (_ucode [_old [i * 4 + 2]] >> 2);
		}
		return true;
	}
};
#pragma endregion
}

#endif //__TOOLS_HPP_
