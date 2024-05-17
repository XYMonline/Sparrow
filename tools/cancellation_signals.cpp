#include "cancellation_signals.hpp"

namespace leo {
;
void cancellation_signals::emit(net::cancellation_type ct) {
	std::lock_guard<std::mutex> _(mtx);

	for (auto& sig : sigs)
		sig.emit(ct);
}

net::cancellation_slot cancellation_signals::slot() {
	std::lock_guard<std::mutex> _(mtx);

	auto itr = std::find_if(sigs.begin(), sigs.end(),
		[](net::cancellation_signal& sig)
		{
			return !sig.slot().has_handler();
		});

	if (itr != sigs.end())
		return itr->slot();
	else
		return sigs.emplace_back().slot();
}

}
