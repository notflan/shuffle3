
#include <utility>
#include <stdexcept>

#include <cstdlib>

extern "C" {
	struct ErrorPayload {};
}

struct FatalError {
	FatalError(void* payload)
		: payload(payload) {}
	FatalError(FatalError&& m)
		: payload(std::exchange(m.payload, nullptr)) {}

	FatalError& operator=(FatalError&& m)
	{
		if(this != &m) {
			if(payload) destroy();
			payload = std::exchange(m.payload, nullptr);
		} return *this;
	}
	virtual ~FatalError() noexcept {
		destroy();
	}
protected:
	virtual void destroy() noexcept
	{
		/*if(auto* p = dynamic_cast<ErrorPayload*>(payload))
			delete p;*/
	}
public:
	void* payload;
};

extern "C" {
	[[noreturn, gnu::weak]]
	void _panic__start_unwind_cxx(void* payload)
	{
#if __EXCEPTIONS
		throw FatalError(payload);
#else
		::abort();
#endif
	}
}
