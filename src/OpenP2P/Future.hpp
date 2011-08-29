#ifndef OPENP2P_FUTURE_HPP
#define OPENP2P_FUTURE_HPP

#include <utility>

#include <boost/function.hpp>

#include <OpenP2P/Promise.hpp>
#include <OpenP2P/Signal.hpp>

#include <OpenP2P/Notification/ComposeLayer.hpp>
#include <OpenP2P/Notification/Handler.hpp>
#include <OpenP2P/Notification/LateJoinLayer.hpp>
#include <OpenP2P/Notification/LayerPtr.hpp>
#include <OpenP2P/Notification/LayerWrapper.hpp>
#include <OpenP2P/Notification/ValueLayer.hpp>

namespace OpenP2P{
	
	template <typename T>
	class Future: private Notification::Handler, public Notification::LayerWrapper<T>{
		public:
			Future()
				: layer_(new Notification::ValueLayer<T>())
				{
					signal_.activate();
				}
			
			Future(const T& value)
				: layer_(new Notification::ValueLayer<T>(value))
				{
					signal_.activate();
				}
				
			Future(const Notification::LayerWrapper<T>& wrapper)
				: layer_(wrapper.getLayer())
				{
					layer_->addHandler(this);
				}
				
			Future(const Notification::LayerPtr<T>& layer)
				: layer_(layer)
				{
					layer_->addHandler(this);
				}
				
			Future(const Future<T>& future)
				: layer_(future.getLayer())
				{
					layer_->addHandler(this);
				}
				
			Future<T>& operator=(const Future<T>& future){
				if(layer_.get() != future.getLayer().get()){
					layer_->removeHandler(this);
				
					layer_ = future.getLayer();
					signal_.reset();
				
					layer_->addHandler(this);
				}
				return *this;
			}
				
			~Future(){
				layer_->removeHandler(this);
			}
			
			template <typename Out>
			Future<Out> compose(boost::function<Out (T)> transform){
				return Future<Out>(Notification::LayerPtr<Out>(new Notification::ComposeLayer<T, Out>(layer_, transform)));
			}
			
			template <typename S>
			Future< std::pair<T, S> > lateJoin(const Future<S>& future){
				typedef std::pair<T, S> OutType;
				return Future<OutType>(Notification::LayerPtr<OutType>(new Notification::LateJoinLayer<T, S>(layer_, future.getLayer())));
			}
			
			template <typename A, typename B>
			static Future< std::pair<A, B> > LateJoin(const Future<A>& future0, const Future<B>& future1){
				return future0.lateJoin(future1);
			}
			
			Notification::LayerPtr<T> getLayer() const{
				return layer_;
			}
			
			bool isReady(){
				return signal_.isActivated();
			}
			
			T get(){
				signal_.wait();
				return layer_->getValue();
			}
			
			void wait(){
				signal_.wait();
			}
		
			bool timedWait(double secs){
				return signal_.timedWait(secs);
			}
		
		private:		
			void notify(){
				signal_.activate();
			}
			
			Signal signal_;
			Notification::LayerPtr<T> layer_;
	
	};

}

#endif
