#ifndef OPENP2P_CORE_WRAPPER_HPP
#define OPENP2P_CORE_WRAPPER_HPP

namespace openp2p{

	namespace Core{

		template <class T>
		void Destroy(T * t){
			delete t;
		}

		template <class T>
		boost::function<void()> WrapDestructor(T * t){
			return boost::bind(Destroy<T>, t);
		}

		template <class T>
		class Wrapper{
			public:
				Wrapper(T * t): mT(t){ }

				Wrapper(T& t) : mT(&t){ }

				~Wrapper(){
					mT->Destroy();
				}

				T& operator*() const{
					return *mT;
				}

				T * operator->() const{
					return mT;
				}

			private:
				T * mT;

		};

	}

}

#endif
