#ifndef FUSE_ERROREXCEPTION_HPP
#define FUSE_ERROREXCEPTION_HPP

namespace FUSE {

	/**
	 * \brief Error Exception.
	 * 
	 * Instances of this class should be thrown
	 * to indicate file system errors (using
	 * values from errno.h); these will be caught
	 * and returned as errors to FUSE.
	 */
	class ErrorException {
		public:
			inline ErrorException(int e) :
				error_(e) { }
			
			inline int error() const {
				return error_;
			}
			
		private:
			int error_;
		
	};
	
}

#endif
