#pragma once

#ifndef __cplusplus
#error "C++ header only"
#endif

namespace pr {

	namespace details [[gnu::visibility("inernal")]]{
		template<typename T>
		T* take(T*& ptr) noexcept { return std::exchange(ptr, nullptr); }
	}

	struct Bar {
		Bar() noexcept = default;
		Bar(const Bar&) noexcept = default;
		Bar(Bar&&) noexcept = default;
		Bar& operator=(const Bar&) noexcept = default;
		Bar& operator=(Bar&&) noexcept = default;
		virtual ~Bar() = default;

		virtual void spin(int) =0;
	};

	enum bar_kind {
		BRK_UNTRACKED,
		BRK_TRACKED,
	};

	template<bar_kind = BRK_UNTRACKED>
	struct progress;

	template<>
	struct progress<BRK_UNTRACKED>
		: public virtual Bar {
		
		progress(FILE* to);
		progress(progress const&);
		inline progress(progress&& mv) noexcept
			: Bar()
			, _perc(mv._perc)
			, _output(details::take(mv._output)) {}

		virtual ~progress();	

		void spin(int) override;
		
		//TODO: this
	protected:
		union {
			double _perc;
			size_t _num; // not used here
		};
		FILE* _output;
	};

	template<>
	class progress<BRK_TRACKED>
		: public progress<BRK_UNTRACKED> {
		using base_t = progress<BRK_UNTRACKED>;
	public:
		inline progress(FILE* to, size_t max) : base_t(to), _num(0), _max(max) {}
		progress(const progress&) = default;
		progress(progress&& m) : base_t(std::move(m)), _num(std::exchange(m._num, 0)), _max(m._max) {}

		void spin(int) override;
		//TODO: this

		virtual ~progress() {}

	protected:
		using base_t::_output;
	private:
		using base_t::_num;
		size_t _max;
	};
}
