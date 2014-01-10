#ifndef OPENP2P_TIMEOUT_HPP
#define OPENP2P_TIMEOUT_HPP

#include <limits>

namespace OpenP2P {

	inline double InfiniteDoubleValue() {
		using namespace std;
		return numeric_limits<double>::has_infinity
			   ? numeric_limits<double>::infinity()
			   : numeric_limits<double>::max();
	}
	
	class Timeout {
		public:
			inline static Timeout Zero() {
				return Timeout(0.0, false);
			}
			
			inline static Timeout Infinite() {
				return Timeout(InfiniteDoubleValue(), true);
			}
			
			inline static Timeout Seconds(double seconds) {
				return Timeout(seconds, false);
			}
			
			inline static Timeout Milliseconds(double milliseconds) {
				return Timeout(milliseconds / 1000.0, false);
			}
			
			inline Timeout(const Timeout& timeout)
				: timeout_(timeout.seconds()),
				  isInfinite_(timeout.isInfinite()) { }
				  
			inline Timeout& operator=(const Timeout& timeout) {
				timeout_ = timeout.seconds();
				return *this;
			}
			
			inline bool isInfinite() const {
				return isInfinite_;
			}
			
			inline double seconds() const {
				return timeout_;
			}
			
			inline double milliseconds() const {
				return timeout_ * 1000.0;
			}
			
			inline bool hasExpired() const {
				return isInfinite_ ? false : timeout_ <= 0.0;
			}
			
		private:
			inline Timeout(double pSeconds, bool pIsInfinite)
				: timeout_(pSeconds),
				  isInfinite_(pIsInfinite) { }
				  
			double timeout_;
			bool isInfinite_;
			
	};
	
}

#endif
