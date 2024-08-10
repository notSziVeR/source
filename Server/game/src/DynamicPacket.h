#pragma once

#include <cstdint>

class CDynamicPacket
{
private:
	const char* data_;
	uint32_t size_;
	uint32_t extraSize_;

public:
	CDynamicPacket(const char* data)
		: data_(data), size_(UINT32_MAX), extraSize_(0)
	{}

	CDynamicPacket(const char* data, uint32_t size)
		: data_(data), size_(size), extraSize_(0)
	{}

	~CDynamicPacket() = default;

	template <typename T>
	const T* Get(bool isExtraSize = true)
	{
		if (sizeof(T) > size_)
		{
			return nullptr;
		}

		auto ret = reinterpret_cast<const T*>(data_);

		data_ += sizeof(T);
		size_ -= sizeof(T);

		if (isExtraSize)
		{
			extraSize_ += sizeof(T);
		}

		return ret;
	}

	size_t GetLeftSize() const
	{
		return size_;
	}

	size_t GetExtraSize() const
	{
		return extraSize_;
	}
};
