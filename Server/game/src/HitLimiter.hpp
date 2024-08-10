#ifdef __HIT_LIMITER_ENABLE__
namespace CHitLimiter
{
	bool RegisterHit(LPCHARACTER ch, LPCHARACTER enemy);

	extern DWORD HIT_LIMIT_BUFF;
	extern bool g_limiter_enable;
}
#endif

