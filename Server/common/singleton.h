#pragma once
template <typename T> class singleton
{
public:
	static T* instance_;

	singleton()
	{
	}

	virtual ~singleton()
	{
		instance_ = nullptr;
	}

	static T & instance()
	{
		return (*get_instance());
	}

	static T & Instance()
	{
		return (*get_instance());
	}

	static T* get_instance()
	{
		if (!instance_)
		{
			instance_ = new T;
		}

		return instance_;
	}

	static void destroy_instance()
	{
		delete instance_;
		instance_ = nullptr;
	}
};

template <typename T> T* singleton<T>::instance_ = nullptr;
