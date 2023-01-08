#pragma once

#include <string>
#include <memory>
#include <cstdio>

#ifndef __cplusplus
#error "C++ header only"
#endif

namespace pr {

	namespace details [[gnu::visibility("inernal")]]{
		template<typename T>
		T* take(T*& ptr) noexcept { return std::exchange(ptr, nullptr); }
	}

	using increment_t = long double;

	template<typename T>
	concept Bar = requires(T& v) {
		{ v.spin(increment_t(0)) };
		
	};

	template<Bar>
	struct Carrier;

	struct Dynamic {
		template<Bar B> friend class Carrier<B>;

		constexpr auto make(Bar auto&& bar) noexcept(std::is_nothrow_move_constructible_v<std::remove_reference_t<decltype(bar)>>);

		constexpr virtual void spin(increment_t) =0;

		constexpr virtual ~Dynamic() {}
	private:
		constexpr Dynamic() noexcept {}
	};
	template<Bar B>
	struct Carrier : public virtual Dynamic {
		using held_type = B;

		template<typename... Args> requires(std::constructible_from<B, Args...>)
		constexpr Carrier(Args&&... args) noexcept(std::is_nothrow_constructible_v<B, Args...>)
			: Dynamic(), member_(std::forward<decltype(args)>(args)) {}
		constexpr Carrier(Carrier&& m)  noexcept(std::is_nothrow_move_constructible_v<B>)
						requires(std::is_move_constructible_v<B>)
			: Dynamic(), member_(std::move(m.member_)) {}
		constexpr Carrier(Carrier const& c)	noexcept(std::is_nothrow_copy_constructible_v<B>)
							requires(std::is_copy_constructible_v<B>)
			: Dynamic(), member_(c.member_) {}

		constexpr Carrier& operator=(Carrier&& m) noexcept(std::is_nothrow_move_assignable_v<B>) requires(std::is_move_assignable_v<B>)
		{ if(this != &m) member_ = std::move(m.member_); return *this; }

		constexpr Carrier& operator=(Carrier const& c) noexcept(std::is_nothrow_copy_assignable_v<B>) requires(std::is_copy_assignable_v<B>)
		{ if(this != &m) member_ = c.member_; return *this; }

		constexpr void spin(increment_t a)  override { member_.spin(a); }
		template<typename... Args> requires(std::is_invocable_v<B::spin, B&, increment_t, Args...>)
		constexpr void spin(increment_t a, Args&&... args) noexcept(std::is_nothrow_invocable_v<B::spin, B&, increment_t, decltype(args)...>)
		{ return member_.spin(a, std::forward<decltype(args)>(args)...); }

		constexpr virtual ~Carrier() {}
	private:
		B member_;
	};
	template<Bar T>
	Carrier(T) -> Carrier<T>;
	template<Bar T>
	Carrier(T const&) -> Carrier<T>;
	template<Bar T>
	Carrier(T&&) -> Carrier<T>;

	constexpr auto Dynamic::make(Bar auto&& bar) noexcept(std::is_nothrow_move_constructible_v<std::remove_reference_t<decltype(bar)>>) { return Carrier(std::move(bar)); }

	template<Bar T>
	constexpr std::unique_ptr<Dynamic> make_dyn(T&& bar) noexcept(std::is_nothrow_move_constructible_v<T>)
	{
		template<typename T, template<typename U> Inst>
		constexpr bool is_inst_of = requires{
			requires(std::is_same_v<T, Inst<U>>);
		};
		template<typename U, typename T, template<typename...> Ptr = std::unique_ptr>
		constexpr Ptr<U> static_ucast(Ptr<T>&& from) noexcept
		{ return Ptr<U>{static_cast<U*>(std::move(from).release())}; }

		template<typename U, typename T>
		constexpr std::shared_ptr<U> static_ucast<U, T, std::shared_ptr>(std::shared_ptr<T>&& from) noexcept
		{ return std::static_pointer_cast<U>(std::move(from)); }

		if constexpr(is_inst_of<T, Carrier>) return static_ucast<Dynamic>(std::make_unique<T>(std::move(bar)));
		else return static_ucast<Dynamic>(std::make_unique<Carrier<T>>(std::move(bar)));
	}

	constexpr void spin(Bar auto& bar, std::convertible_to<increment_t> auto&& a) { return bar.spin(increment_t(a)); }

	struct None {
		constexpr None() noexcept = default;
		constexpr ~None() noexcept = default;

		constexpr void spin(increment_t) noexcept {}
	};

	// A bounded progress-bar
	struct Progress {
		using fract_t = increment_t;
		constexpr static inline bool DEFAULT_WIDTH = 50;
		constexpr static inline fract_t DEFAULT_TERM_FRACT=1.0l/3.0l;

		[[gnu::nonnull(1)]]
		Progress(FILE* p = stdout) noexcept;

		Progress(const Progress& p) noexcept;
		Progress(Progress&& p) noexcept;

		Progress& operator=(Progress const& c) noexcept;
		Progress& operator=(Progress&& m) noexcept;
		
		virtual ~Progress();

		std::string& tag() const noexcept;
		const std::string& tag() const noexcept;

		FILE* output() const noexcept;
		FILE*& output() noexcept;

		size_t& width() noexcept;
		size_t  width() const noexcept;

		fract_t& fraction() noexcept;
		fract_t  fraction() const noexcept;

		fract_t percentage() const noexcept;
		inline auto percentage() noexcept {
			struct v final {
				Progress* p_;
				v(Progress* p) noexcept : p_(p) {}

				v(v&&) noexcept = default;
				v(v const&) noexcept = default;
				v& operator=(v const&) noexcept = default;
				v& operator=(v&&) noexcept = default;
				~v() noexcept = default;

				const v& operator=(fract_t f) const noexcept { p_->percentage(f); return *this; }
				operator fract_t() const noexcept { return p_->percentage(); }	
			};
			return v(this);
		}

		void spin(increment_t by, bool render=true, bool flush=true) noexcept;
		void render(bool flush = true) const noexcept;
	private:
		void percentage(fract_t fract) noexcept;

		struct _impl; 
		std::shared_ptr<_impl> inner_;
	};

	//TODO: Spinny bar thing (unbounded)

/*
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
*/
}
