#include <queue>
#include <sys/time.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "Timer.hpp"
#include "Strand.hpp"

namespace openp2p{

	namespace Core{

		namespace{

			struct TimerTask{
				double time;
				boost::function<void ()> handler;
			};

			class compare{
				public:
					bool operator()(const TimerTask& a, const TimerTask& b) const{
						return a.time > b.time;
					}
			};

			double getMilliseconds(){
				struct timeval time;
				gettimeofday(&time, NULL);
				return (double(time.tv_sec) * 1000.0 + double(time.tv_usec)/1000.0);
			}

			class InternalTimer{
				public:
					InternalTimer() : mTimer(GetGlobalIOService()), mQueue(compare()), mCounter(0){
						mIsActive = false;
						mIsDestroying = false;
					}

					void Add(double seconds, boost::function<void ()> handler){
						++mCounter;
						mStrand.Schedule(boost::bind(&InternalTimer::DoAdd, this, seconds, handler));
					}

					void Destroy(){
						mStrand.Schedule(boost::bind(&InternalTimer::DoDestroy, this));
					}

				private:
					~InternalTimer(){ }

					void DoDestroy(){
						mIsDestroying = true;
						mTimer.cancel();
						if(mCounter == 0){
							delete this;
						}
					}

					void DoTask(TimerTask task){
						mCurrentTask = task;
						mIsActive = true;

						double milliseconds = (task.time - getMilliseconds());
						milliseconds = (milliseconds < 0.0) ? 0.0 : milliseconds;
						mTimer.cancel();
						mTimer.expires_from_now(boost::posix_time::milliseconds((unsigned int) (milliseconds + 0.5)));
						mTimer.async_wait(mStrand.Wrap(boost::bind(&InternalTimer::HandleTimer, this, boost::asio::placeholders::error, task)));
						++mCounter;
					}

					void DoAdd(double seconds, boost::function<void ()> handler){
						--mCounter;
						if(mIsDestroying){
							if(mCounter == 0){
								delete this;
							}
							return;
						}

						TimerTask task;
						task.time = getMilliseconds() + seconds * 1000.0;
						task.handler = handler;
						if(mIsActive){
							if(task.time < mCurrentTask.time){
								mQueue.push(mCurrentTask);
								DoTask(task);
							}else{
								mQueue.push(task);
							}
						}else{
							DoTask(task);
						}
					}

					void HandleTimer(const boost::system::error_code& error, TimerTask task){
						--mCounter;
						if(mIsDestroying){
							if(mCounter == 0){
								delete this;
							}
							return;
						}

						if(error == boost::asio::error::operation_aborted){
							return;
						}

						mStrand.Schedule(mCurrentTask.handler);

						if(mQueue.empty()){
							mIsActive = false;
						}else{
							DoTask(mQueue.top());
							mQueue.pop();
						}
					}

					boost::asio::deadline_timer mTimer;

					TimerTask mCurrentTask;

					bool mIsActive;
					bool mIsDestroying;

					std::priority_queue<TimerTask, std::vector<TimerTask>, compare> mQueue;

					boost::detail::atomic_count mCounter;

					Core::Strand mStrand;

			};

		}

		Timer::Timer(){
			mInternal = (void *) new InternalTimer();
		}

		Timer::~Timer(){
			((InternalTimer *) mInternal)->Destroy();
		}

		void Timer::Add(double seconds, boost::function<void ()> handler){
			((InternalTimer *) mInternal)->Add(seconds, handler);
		}

	}

}

