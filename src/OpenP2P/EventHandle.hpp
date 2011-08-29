#ifndef OPENP2P_EVENTHANDLE_HPP
#define OPENP2P_EVENTHANDLE_HPP

#include <boost/utility.hpp>

#include <OpenP2P/Signal.hpp>
#include <OpenP2P/Notification/AndLayer.hpp>
#include <OpenP2P/Notification/Handler.hpp>
#include <OpenP2P/Notification/LayerPtr.hpp>
#include <OpenP2P/Notification/LayerWrapper.hpp>
#include <OpenP2P/Notification/OrLayer.hpp>
#include <OpenP2P/Notification/ValueLayer.hpp>

namespace OpenP2P{

	class EventHandle: public Notification::Handler, public Notification::LayerWrapper<bool>{
		public:
			inline EventHandle(bool value)
				: layer_(new Notification::ValueLayer<bool>(value))
				{
					layer_->addHandler(this);
				}
				
			inline EventHandle(const Notification::LayerWrapper<bool>& wrapper)
				: layer_(wrapper.getLayer())
				{
					layer_->addHandler(this);
				}
				
			inline EventHandle(const Notification::LayerPtr<bool>& ptr)
				: layer_(ptr)
				{
					layer_->addHandler(this);
				}
			
			inline EventHandle(const EventHandle& eventHandle)
				: layer_(eventHandle.layer_)
				{
					layer_->addHandler(this);
				}
				
			inline ~EventHandle(){
				layer_->removeHandler(this);
			}
				
			inline static EventHandle True(){
				return EventHandle(true);
			}
			
			inline static EventHandle False(){
				return EventHandle(false);
			}
			
			inline static EventHandle And(const EventHandle& handle0, const EventHandle& handle1){
				return EventHandle(Notification::LayerPtr<bool>(new Notification::AndLayer(handle0.getLayer(), handle1.getLayer())));
			}
			
			inline static EventHandle Or(const EventHandle& handle0, const EventHandle& handle1){
				return EventHandle(Notification::LayerPtr<bool>(new Notification::OrLayer(handle0.getLayer(), handle1.getLayer())));
			}
			
			inline Notification::LayerPtr<bool> getLayer() const{
				return layer_;
			}

			inline bool isActive(){
				return layer_->getValue();
			}

			inline void wait(){
				signal_.wait();
			}
			
			inline bool timedWait(double secs){
				return signal_.timedWait(secs);
			}

		private:
			inline void notify(){
				if(layer_->getValue()){
					signal_.activate();
				}else{
					signal_.reset();
				}
			}
		
			Signal signal_;
			Notification::LayerPtr<bool> layer_;

	};

}

#endif
