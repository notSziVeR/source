#ifdef __ENABLE_BEGINNER_MANAGER__
#include "Parser.hpp"

class BeginnerHelper : public singleton<BeginnerHelper>
{
	public:
		BeginnerHelper() = default;
		virtual ~BeginnerHelper() = default;

	public:
		void Create();
		void Clear();
		bool Load(const std::string& fileName);

	public:
		void Broadcast(LPCHARACTER ch);
		void RecvChoice(LPCHARACTER ch, uint8_t iChoice);
		void Collect(LPCHARACTER ch);

	protected:
		std::map<uint32_t, BeginnerCollection::BeginnerSet> sets_;
};
#endif
