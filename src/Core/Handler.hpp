#ifndef OPENP2P_CORE_HANDLER_HPP
#define OPENP2P_CORE_HANDLER_HPP

#include <boost/bind.hpp>

namespace openp2p{

	namespace Core{

		inline void NullFunction(){ }

		typedef boost::function<void (boost::function<void ()>)> WrappedDispatcher;

		template <typename Handler>
		class WrappedHandler{
			public:
				WrappedHandler(WrappedDispatcher dispatcher, Handler handler) : mDispatcher(dispatcher), mHandler(handler) { }

				void operator()() {
					mDispatcher(mHandler);
				}

				void operator()() const {
					mDispatcher(mHandler);
				}

				template <typename Arg1>
				void operator()(const Arg1& arg1) {
					mDispatcher(boost::bind(mHandler, arg1));
				}

				template <typename Arg1>
				void operator()(const Arg1& arg1) const {
					mDispatcher(boost::bind(mHandler, arg1));
				}

				template <typename Arg1, typename Arg2>
				void operator()(const Arg1& arg1, const Arg2& arg2) {
					mDispatcher(boost::bind(mHandler, arg1, arg2));
				}

				template <typename Arg1, typename Arg2>
				void operator()(const Arg1& arg1, const Arg2& arg2) const {
					mDispatcher(boost::bind(mHandler, arg1, arg2));
				}

				template <typename Arg1, typename Arg2, typename Arg3>
				void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3) {
					mDispatcher(boost::bind(mHandler, arg1, arg2, arg3));
				}

				template <typename Arg1, typename Arg2, typename Arg3>
				void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3) const {
					mDispatcher(boost::bind(mHandler, arg1, arg2, arg3));
				}

				template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
				void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4) {
					mDispatcher(boost::bind(mHandler, arg1, arg2, arg3, arg4));
				}

				template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
				void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4) const {
					mDispatcher(boost::bind(mHandler, arg1, arg2, arg3, arg4));
				}

				template < typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5 >
				void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, const Arg5& arg5) {
					mDispatcher(boost::bind(mHandler, arg1, arg2, arg3, arg4, arg5));
				}

				template < typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5 >
				void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, const Arg5& arg5) const {
					mDispatcher(boost::bind(mHandler, arg1, arg2, arg3, arg4, arg5));
				}

			private:
				WrappedDispatcher mDispatcher;
				Handler mHandler;
		};

		template <typename Handler>
		WrappedHandler<Handler> Wrap(WrappedDispatcher dispatcher, Handler handler){
			return WrappedHandler<Handler>(dispatcher, handler);
		}

	}

}

#endif
