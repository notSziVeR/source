
#ifndef __INC_METIN_II_UTILS_H__
#define __INC_METIN_II_UTILS_H__

#include <math.h>
#include <sstream>
#include <iostream>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <random>

#define IS_SET(flag, bit)		((flag) & (bit))
#define SET_BIT(var, bit)		((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) = (var) ^ (bit))

/*----- check int function -----*/
inline bool chr_is_number(char in)
{
	return in >= '0' && in <= '9';
}

inline bool str_is_number(const char* in)
{
	if (0 == in || 0 == in[0])	{ return false; }

	int len = strlen(in);
	for (int i = 0; i < len; ++i)
	{
		if ((in[i] < '0' || in[i] > '9') && (i > 0 || in[i] != '-'))
		{
			return false;
		}
	}

	return true;
}

inline float DISTANCE_SQRT(long dx, long dy)
{
	return ::sqrt((float)dx * dx + (float)dy * dy);
}

inline int DISTANCE_APPROX(int dx, int dy)
{
	int min, max;

	if (dx < 0)
	{
		dx = -dx;
	}

	if (dy < 0)
	{
		dy = -dy;
	}

	if (dx < dy)
	{
		min = dx;
		max = dy;
	}
	else
	{
		min = dy;
		max = dx;
	}

	// coefficients equivalent to ( 123/128 * max ) and ( 51/128 * min )
	return ((( max << 8 ) + ( max << 3 ) - ( max << 4 ) - ( max << 1 ) +
			 ( min << 7 ) - ( min << 5 ) + ( min << 3 ) - ( min << 1 )) >> 8 );
}

#ifndef __WIN32__
inline WORD MAKEWORD(BYTE a, BYTE b)
{
	return static_cast<WORD>(a) | (static_cast<WORD>(b) << 8);
}
#endif

extern void set_global_time(time_t t);
extern time_t get_global_time();

#include <string>
std::string mysql_hash_password(const char* tmp_pwd);

extern int	dice(int number, int size);
extern size_t str_lower(const char * src, char * dest, size_t dest_size);

extern void	skip_spaces(char **string);

extern const char *	one_argument(const char *argument, char *first_arg, size_t first_size);
extern const char *	two_arguments(const char *argument, char *first_arg, size_t first_size, char *second_arg, size_t second_size);
extern const char *	first_cmd(const char *argument, char *first_arg, size_t first_arg_size, size_t *first_arg_len_result);

extern int CalculateDuration(int iSpd, int iDur);

extern float gauss_random(float avg = 0, float sigma = 1);

extern int parse_time_str(const char* str);

extern bool WildCaseCmp(const char *w, const char *s);

#ifdef __EVENT_MANAGER_ENABLE__
	extern std::string GetFullDateFromTime(time_t&& end_time_sec, bool bWhiteSpace = true);
	extern std::string GetFullDateFromTime(const time_t& end_time_sec, bool bWhiteSpace = true);
#endif

template<class T>
inline std::string pretty_number(T value)
{
	struct custom_numpunct : std::numpunct < char >
	{
	protected:
		virtual char do_thousands_sep() const { return '.'; }
		virtual std::string do_grouping() const { return "\03"; }
	};

	std::stringstream ss;
	ss.imbue(std::locale(std::cout.getloc(), new custom_numpunct));
	ss << std::fixed << value;
	return ss.str();
}

namespace m2
{
template<typename T>
T minmax(T min, T value, T max)
{
	T tv;

	tv = (min > value ? min : value);
	return (max < tv) ? max : tv;
}
}

template <typename CharArray>
size_t copy_stoca(const std::string& src, CharArray& dst)
{
	auto size = src.size();
	auto end = src.end();

	if (size >= dst.size())
	{
		size = dst.size() - 1;
		end = src.begin() + size;
	}

	std::fill(dst.begin(), dst.end(), '\0');
	std::copy(src.begin(), end, dst.begin());

	return size;
}

extern std::vector<std::string> split_string(std::string str, const char delimiter = ' ');

extern bool str_is_int(const std::string& str);

#endif /* __INC_METIN_II_UTILS_H__ */

