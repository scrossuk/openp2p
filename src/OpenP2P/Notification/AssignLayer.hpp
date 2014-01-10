#ifndef OPENP2P_NOTIFICATION_ASSIGNLAYER_HPP
#define OPENP2P_NOTIFICATION_ASSIGNLAYER_HPP

#include <boost/utility.hpp>

#include <OpenP2P/Notification/Handler.hpp>
#include <OpenP2P/Notification/Layer.hpp>
#include <OpenP2P/Notification/Notifier.hpp>

namespace OpenP2P {

	namespace Notification {
	
		template <typename T>
		class AssignLayer: public Notification::Layer<T>, public boost::noncopyable {
			public:
				AssignLayer()
					: value_() { }
					
				AssignLayer(const T& initialValue)
					: value_(initialValue) { }
					
				bool hasValue() {
					return notifier_.isComplete();
				}
				
				void setValue(const T& value) {
					value_ = value;
					notifier_.notify();
				}
				
				T getValue() {
					return value_;
				}
				
				void addHandler(Notification::Handler* handler) {
					notifier_.addHandler(handler);
				}
				
				void removeHandler(Notification::Handler* handler) {
					notifier_.removeHandler(handler);
				}
				
			private:
				Notification::Notifier notifier_;
				T value_;
				
		};
		
	}
	
}

#endif
