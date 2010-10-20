#ifndef OPENP2P_CORE_QUEUE_HPP
#define OPENP2P_CORE_QUEUE_HPP

#include <queue>
#include <boost/bind.hpp>
#include "Strand.hpp"

namespace openp2p{

	namespace Core{

		const unsigned int QUEUE_SIZE = 1024;

		//Multi-producer, single consumer queue
		template <typename T>
		class Queue{
			public:
				Queue(){
					mReadPos = mWritePos = 0;
					for(unsigned int i = 0; i < QUEUE_SIZE; i++){
						mData[i] = 0;
					}
				}

				void Push(T * t){
					mStrand.Schedule(boost::bind(&Queue::DoWriteW, this, t));
				}

				T * Top(){
					return mData[mReadPos];
				}

				void Pop(){
					T * data = mData[mReadPos];
					if(data != 0){
						mData[mReadPos] = 0;
						mReadPos++;
						if(mReadPos >= QUEUE_SIZE){
							mReadPos = 0;
						}
						mStrand.Schedule(boost::bind(&Queue::FlushOverflow, this));
					}
				}

				bool IsEmpty(){
					return Top() == 0;
				}

			private:
				void DoWriteW(T * t){
					bool b = DoWrite(t);
					if(!b){
						//Need to overflow
						mOverflow.push(t);
					}
				}

				bool DoWrite(T * t){
					if(mData[mWritePos] == 0){
						//There is some space in the queue
						mData[mWritePos] = t;
						mWritePos++;
						if(mWritePos >= QUEUE_SIZE){
							mWritePos = 0;
						}
						return true;
					}else{
						//No space remaining
						return false;
					}
				}

				void FlushOverflow(){
					T * t = mOverflow.top();
					while(DoWrite(t)){
						mOverflow.pop();
						t = mOverflow.top();
					}
				}

				T * mData[QUEUE_SIZE];
				std::queue<T *> mOverflow;
				Strand mStrand;
				unsigned int mReadPos, mWritePos;

		};

	}

}

#endif
