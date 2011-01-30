#ifndef OPENP2P_WRAPPEDHANDLER_HPP
#define OPENP2P_WRAPPEDHANDLER_HPP

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace OpenP2P{

	typedef boost::function<void (boost::function<void ()>)> WrappedDispatcher;

	template <typename Handler>
	class WrappedHandler{
		public:
			WrappedHandler(WrappedDispatcher dispatcher, Handler handler) : dispatcher_(dispatcher), handler_(handler) { }

			void operator()() {
				dispatcher_(handler_);
			}

			void operator()() const {
				dispatcher_(handler_);
			}

			template <typename Arg1>
			void operator()(const Arg1& arg1) {
				dispatcher_(boost::bind(handler_, arg1));
			}

			template <typename Arg1>
			void operator()(const Arg1& arg1) const {
				dispatcher_(boost::bind(handler_, arg1));
			}

			template <typename Arg1, typename Arg2>
			void operator()(const Arg1& arg1, const Arg2& arg2) {
				dispatcher_(boost::bind(handler_, arg1, arg2));
			}

			template <typename Arg1, typename Arg2>
			void operator()(const Arg1& arg1, const Arg2& arg2) const {
				dispatcher_(boost::bind(handler_, arg1, arg2));
			}

			template <typename Arg1, typename Arg2, typename Arg3>
			void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3) {
				dispatcher_(boost::bind(handler_, arg1, arg2, arg3));
			}

			template <typename Arg1, typename Arg2, typename Arg3>
			void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3) const {
				dispatcher_(boost::bind(handler_, arg1, arg2, arg3));
			}

			template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
			void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4) {
				dispatcher_(boost::bind(handler_, arg1, arg2, arg3, arg4));
			}

			template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
			void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4) const {
				dispatcher_(boost::bind(handler_, arg1, arg2, arg3, arg4));
			}

			template < typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5 >
			void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, const Arg5& arg5) {
				dispatcher_(boost::bind(handler_, arg1, arg2, arg3, arg4, arg5));
			}

			template < typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5 >
			void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, const Arg5& arg5) const {
				dispatcher_(boost::bind(handler_, arg1, arg2, arg3, arg4, arg5));
			}

		private:
			WrappedDispatcher dispatcher_;
			Handler handler_;
	};

	template <typename Handler>
	WrappedHandler<Handler> Wrap(WrappedDispatcher dispatcher, Handler handler){
		return WrappedHandler<Handler>(dispatcher, handler);
	}

}

#endif
