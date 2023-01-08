
#include <string>
#include <vector>

#include <fmt/format.h>

#include <sys/ioctl.h>

#include <perc.h>

namespace {
	size_t twidth(int fd = STDIN_FILENO, size_t or = Progress::DEFAULT_WIDTH) noexcept
	{
		struct winsize w;
		if(ioctl(fd, TIOCGWINSZ, &w) == -1) return or;
		return size_t(w.ws_col);
	}

	size_t def_bar_width(size_t tw) noexcept
	{
		return size_t(std::round(DEFAULT_TERM_FRACT * fract_t(tw)));
	}
}

namespace pr {

	struct Progress::_impl {
		Progress::fract_t fract; // 0..=1
		FILE* output;
		size_t width;
		std::string aux;
	};

	size_t& Progress::width() noexcept { return inner_->width; }
	size_t  Progress::width() const noexcept { return inner_->width; }

	fract_t& Progress::fraction() noexcept { return inner_->fract; }
	fract_t  Progress::fraction() const noexcept { return inner_->fract; }

	fract_t Progress::percentage() const noexcept { return inner_->fract * 100.0l; }
	void Progress::percentage(fract_t fract) noexcept { inner_->fract = fract / 100.0l; }

	std::string& Progress::tag() noexcept { return inner_->aux; }
	const std::string& Progress::tag() const noexcept { return inner_->aux; }

	FILE* Progress::output() const noexcept { return inner_->output; }
	FILE*& Progress::output() noexcept { return inner_->output; }

	void Progress::render(bool flush) const noexcept
	{
		constinit thread_local static std::vector<char> buffer{};

		const auto& inner = *inner_;
		FILE* out = inner.output;
		auto wf = fract_t(inner.width) * inner.fract;
		auto perc = percentage();
		size_t cf = size_t(std::floor(wf));

		
		auto print = []<typename F> (F f, auto&&... args) {
			return fmt::format_to(std::back_inserter(buffer), std::forward<F>(f), std::forward<decltype(args)>(args)...);
		};
	
		buffer.clear();
		// Render bar
		buffer.push_back('[');
		for(size_t i=0;i<inner.width;i++)
			if(i<=cf) buffer.push_back( '#' );
			else buffer.push_back( ' ' );
		buffer.push_back( ']' );
		print(": {:00.2}%", perc);
		if(inner.aux.size())
			print(" ({})", std::string_view(inner.aux));
		// Print bar
		fprintf(out, "\r%.*s", int(buffer.size() & INT_MAX), static_cast<const char*>(buffer.data()));
		// Flush output stream
		if(flush) fflush(out); 
	}
	void Progress::spin(increment_t by, bool r, bool f) noexcept
	{
		auto& inner = *inner_;
		inner.fract+=by;
		if(inner.fract > 1.0l)
			inner.fract=1.0l;
		if(r) this->render(f);
	}

	[[gnu::nonnull(1)]]
	Progress::Progress(FILE* p) noexcept
		: inner_(std::make_unique<_impl>(0, p, def_bar_width(twidth(fileno(p)))  )) {}

	Progress::Progress(const Progress& p) noexcept
		: inner_(p.inner_ ? std::make_unique<_impl>(*p.inner_) : nullptr) {}

	Progress::Progress(Progress&& p) noexcept
		: inner_(std::move(p.inner_)) {}

	Progress& Progress::operator=(Progress&& p) noexcept
	{	
		if(this != &p) inner_ = std::move(p.inner_);
		return *this;
	}

	Progress& Progress::operator=(Progress const& p) noexcept
	{
		if(this != &p) inner_ = p.inner_;
		return *this;
	}

	Progress::~Progress() {
		if(inner_)
			fputc('\n', inner_->output);
	}
}
