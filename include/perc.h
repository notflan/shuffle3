#pragma once

#include <string>
#include <memory>
#include <utility>
#include <array>
#include <utility>

#include <cstdio>

#ifndef __cplusplus
#error "C++ header only"
#endif

namespace pr {

	namespace details [[gnu::visibility("internal")]] {
/*
		template<template<typename...> typename P>
		struct generic_valid { template<typename... Args> constexpr static inline bool value = requires{ typename P<Args...>; };  };
		template<template<typename...> class P>
		struct generic : std::conditional_t<generic_valid<P>::value,
					std::true_type,
					std::false_type>
		{ template<typename... Args> using type = P<Args...>; };

		template<template<typename> typename T, typename... Args> 
		concept Generic = generic_valid<T>::template value<Args...>;

		template<Generic T, typename... Args>
		using generic_t = generic<T>::template type<Args...>;

		template<template<typename> template T>
		concept AnyGeneric = requires{ typename generic_valid<T>; };
*/
		template<typename T>
		T* take(T*&& ptr) noexcept { return std::exchange(ptr, nullptr); }

/*
		template<template<typename U> Inst, typename T>
		struct is_inst_of { constexpr static inline bool value = std::is_same_v<T, Inst<U>>; };

		template<typename T, typename I>
		constexpr inline bool is_inst_of_v = is_inst_of<I, T>::value;
*/

		template<typename U, typename T>
		constexpr std::unique_ptr<U> static_ucast(std::unique_ptr<T>&& from) noexcept
		{ return std::unique_ptr<U>{static_cast<U*>(std::move(from).release())}; }

		template<typename U, typename T>
		constexpr std::shared_ptr<U> static_ucast(std::shared_ptr<T>&& from) noexcept
		{ return std::static_pointer_cast<U>(std::move(from)); }


		template<typename Ptr, typename From> requires(requires(From&& p) {
			{ p.release() };
			requires(std::is_constructible_v<Ptr, decltype(p.release())>);
		})
		constexpr Ptr static_pcast(From&& p) noexcept { return Ptr{p.release()}; }

	}

	using increment_t = long double;

	template<typename T>
	concept Bar = requires(std::remove_const_t<std::remove_reference_t<T>>& v) {
		{ v.spin(increment_t(0)) };
		{ std::as_const(v).aux() } -> std::convertible_to<std::string_view>;
		//{ v.aux() } -> std::assignable_from<std::string_view>;
		{ (v.aux() = std::string{}) } -> std::same_as<std::add_lvalue_reference_t<decltype(v.aux())>>;
		//requires(std::assignable_from<std::add_lvalue_reference_t<decltype(v.aux())>, std::string_view>);
		{ v.aux() += std::string_view{} } -> std::convertible_to<decltype(v.aux())>;
		{ v.aux(std::declval<std::string&&>()) } -> std::convertible_to<std::string_view>;
	};

	template<Bar>
	struct Carrier;

	struct Dynamic {
		template<Bar> friend class Carrier;

		constexpr auto make(Bar auto&& bar) noexcept(std::is_nothrow_move_constructible_v<std::remove_reference_t<decltype(bar)>>);

		constexpr virtual void spin(increment_t) =0;
		constexpr virtual std::string& aux() =0;
		constexpr virtual std::string const& aux() const noexcept =0;
		constexpr virtual std::string aux(std::string&& s) { return std::exchange(aux(), std::move(s)); }

		constexpr virtual ~Dynamic() {}
	private:
		constexpr Dynamic() noexcept {}
	};
	template<Bar B>
	struct Carrier : public virtual Dynamic {
		using held_type = B;

		template<typename... Args> requires(std::constructible_from<B, Args...>)
		constexpr Carrier(Args&&... args) noexcept(std::is_nothrow_constructible_v<B, Args...>)
			: Dynamic(), member_(std::forward<decltype(args)>(args)...) {}
		constexpr Carrier(Carrier&& m)  noexcept(std::is_nothrow_move_constructible_v<B>)
						requires(std::is_move_constructible_v<B>)
			: Dynamic(), member_(std::move(m.member_)) {}
		constexpr Carrier(Carrier const& c)	noexcept(std::is_nothrow_copy_constructible_v<B>)
							requires(std::is_copy_constructible_v<B>)
			: Dynamic(), member_(c.member_) {}

		constexpr Carrier& operator=(Carrier&& m) noexcept(std::is_nothrow_move_assignable_v<B>) requires(std::is_move_assignable_v<B>)
		{ if(this != &m) member_ = std::move(m.member_); return *this; }

		constexpr Carrier& operator=(Carrier const& c) noexcept(std::is_nothrow_copy_assignable_v<B>) requires(std::is_copy_assignable_v<B>)
		{ if(this != &c) member_ = c.member_; return *this; }

		constexpr void spin(increment_t a)  override { member_.spin(a); }

		constexpr std::string& aux() override { return member_.aux(); }
		constexpr std::string const& aux() const noexcept override  { return member_.aux(); }
		constexpr std::string aux(std::string&& s) override { return member_.aux(std::move(s)); }

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

	template<Bar T, typename Ptr = std::unique_ptr<Dynamic>>
	constexpr Ptr make_dyn(T&& bar) noexcept(std::is_nothrow_move_constructible_v<T>)
	{
		using namespace details;
		if constexpr(requires{
			typename T::held_type;
			requires(std::is_same_v<Carrier<typename T::held_type>, T>);
		}) return static_pcast<Ptr>(static_ucast<Dynamic>(std::make_unique<T>(std::move(bar))));
		else return static_pcast<Ptr>(static_ucast<Dynamic>(std::make_unique<Carrier<T>>(std::move(bar))));
	}

	template<Bar T, typename Ptr = std::unique_ptr<Dynamic>>
	constexpr Ptr make_dyn(T const& bar) noexcept(std::is_nothrow_move_constructible_v<T>)
	{
		if constexpr(std::is_copy_constructible_v<T>) {
			T nbar{bar};
			return make_dyn<T, Ptr>(std::move(nbar));
		} else {
			struct unsafe_ref {
				constexpr unsafe_ref(const T& ba) noexcept : b(std::addressof(ba)) {}
				constexpr unsafe_ref(const unsafe_ref&) noexcept = default;
				constexpr ~unsafe_ref() noexcept = default;
				constexpr unsafe_ref(unsafe_ref&& b) noexcept : b(std::exchange(b.b, nullptr)) {}

				constexpr unsafe_ref& operator=(unsafe_ref const&) noexcept = default;
				constexpr unsafe_ref& operator=(unsafe_ref&& m) noexcept {
					if(this != &m)
						b = std::exchange(m.b, nullptr);
					return *this;
				}

				const T* b;

				constexpr operator T const&() const noexcept { return *b; }
			};
			unsafe_ref re{bar};
			return make_dyn<unsafe_ref, Ptr>(std::move(re));
		}
	}

	template<typename Ptr> requires(requires(std::unique_ptr<Dynamic>&& p) { details::static_pcast<Ptr>(std::move(p)); })
	constexpr Ptr make_dyn_for(Bar auto&& bar) noexcept(std::is_nothrow_move_constructible_v<std::remove_reference_t<decltype(bar)>>)
	{ return make_dyn<decltype(bar), Ptr>(std::move(bar)); }

	constexpr void spin(Bar auto& bar, std::convertible_to<increment_t> auto&& a) { return bar.spin(increment_t(a)); }

	struct None {
		constexpr None() noexcept = default;
		constexpr ~None() noexcept = default;

		constexpr void spin(increment_t) const noexcept {}
		constexpr auto aux()  noexcept { return not_string(); }
		inline std::string const& aux() const noexcept { return none_; }
		constexpr std::string aux(std::convertible_to<std::string> auto&& s) const noexcept { return {}; } //{ return std::exchange(none_, std::string(std::move(s))); }
	private:
		struct not_string {
			constexpr not_string() noexcept= default;
			constexpr ~not_string() noexcept= default;

			[[gnu::const]]
			constexpr not_string& operator+=(std::convertible_to<std::string_view> auto&&) { return *this; }
			[[gnu::const]]
			constexpr not_string& operator=(std::convertible_to<std::string> auto&&) { return *this; }

			inline operator std::string&() noexcept { return none_; }
			inline operator std::string const&() const noexcept { return none_; }

		};
		/*constinit*/ thread_local static inline std::string none_;
	};
	constexpr inline None disable{};

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

		inline std::string& aux() noexcept { return tag(); }
		inline std::string const& aux() const noexcept { return tag(); }
		inline std::string aux(std::convertible_to<std::string> auto&& s)
		{ return std::exchange(tag(), std::string(std::move(s))); }
protected:
		std::string& tag() noexcept;
		const std::string& tag() const noexcept;
public:
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
				operator fract_t() const noexcept { return static_cast<const Progress*>(p_)->percentage(); }	
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


#ifdef SPINNER
	class Spinner {
		consteval static auto _GENERATE_MAP(auto const& map, size_t size) noexcept
				-> std::array<size_t, 256>
		{
			static_assert(sizeof(map) == size, "Bad map size");
			std::array<size_t, 256> out{};
			while( size --> 0 ) out[int(map[size])] = size;
			return out;
		}
		constexpr static increment_t range(increment_t by)
		{
			if(by < -1.0l) return -1.0l;
			else if(by > 1.0l) return 1.0l;
			return by;
		}
	public:
		constexpr static inline auto ROUTINE = "|/-\\|/-\\|";
		constexpr static inline auto ROUTINE_SIZE = sizeof(ROUTINE);
		constexpr static inline auto REVERSE_MAP = _GENERATE_MAP(ROUTINE, ROUTINE_SIZE); 
		static_assert(ROUTINE_SIZE != sizeof(char*), "Invalid routine size");

		constexpr ssize_t range(int sz) noexcept
		{
			/*if(__builtin_expect(sz < 0, false)) {
				
				std::terminate(); // TODO: How to handle wrapping negatives?? Ugh.
			}*/
			return ssize_t(sz) % ssize_t(ROUTINE_SIZE);
		}
		constexpr Spinner(size_t n) noexcept
			: cur_(ROUTINE[range(n)]) {}
		constexpr Spinner(char c = ROUTINE[0]) noexcept
			: cur_(range(REVERSE_MAP[int(c)])) {}

		constexpr Spinner(Spinner const&) noexcept = default;
		constexpr Spinner(Spinner &&) noexcept = default;
		~Spinner();

		inline void spin(int by) noexcept
		{
			operator+=(by);
			render();
		}
		inline void spin(increment_by by) noexcept
		{
			spin(int(range(by) * increment_t(ROUTINE_SIZE)));
		}

		inline Spinner& operator+=(int by) noexcept 
		{
			cur_ = ROUTINE[size_t(ssize_t(REVERSE_MAP[int(cur_)]) + range(by))];
			return *this;
		}

		inline Spinner& operator++() noexcept { spin(1); return *this; }
		inline Spinner operator++(int) noexcept { Spinner s = *this; ++s; return *this; }
		constexpr Spinner& operator=(Spinner&&) noexcept = default;
		constexpr Spinner& operator=(Spinner const&) noexcept = default;

		constexpr char& character() noexcept { return cur_; }
		constexpr char character() const noexcept { return cur_; }

		void render(bool flush=true);
	private:
		char cur_ = '|';
	};
	//TODO: Spinny bar thing (unbounded)
#endif

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
