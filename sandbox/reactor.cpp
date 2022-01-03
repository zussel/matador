#include "matador/net/leader_follower_thread_pool.hpp"

#include "matador/utils/thread_helper.hpp"

#include "matador/logger/log_manager.hpp"

#include <iostream>
#include <random>

class taxi_station
{
public:
  taxi_station(int customers, std::size_t threads)
    : pool_(threads, [this]() { wait_for_customer(); })
    , customers_(customers)
    , log_(matador::create_logger("TaxiStation"))
    , eng_{std::random_device{}()}
  {}

  void run()
  {
    log_.info("start processing all %d customers", customers_.operator int());
    pool_.start();

    std::unique_lock<std::mutex> l(mutex_);
    no_customer_.wait(l, [this]() {
      return handled_customers_ >= customers_;
    });
    log_.info("all customers processed %d", customers_.operator int());
    pool_.shutdown();
  }

private:
  void wait_for_customer()
  {
    if (handled_customers_ == 10) {
      log_.info("handled customers == customers (%d == %d)",handled_customers_.operator int(), customers_.operator int());
      no_customer_.notify_one();
    }
    log_.info("waiting for customer %d", handled_customers_.operator int());
    std::uniform_int_distribution<> dist{1, 3};
    auto delay = std::chrono::milliseconds {dist(eng_) * 100};
    log_.info("delay %d millis", delay.count());
    std::this_thread::sleep_for(delay);

    pool_.promote_new_leader();

    handle_customer(handled_customers_++);

  }

  void handle_customer(int customer)
  {
    log_.info("handle customer %d start", customer);
    std::uniform_int_distribution<> dist{1, 3};
    auto delay = std::chrono::seconds{dist(eng_)};
    log_.info("delay %d secs", delay.count());
    std::this_thread::sleep_for(delay);
    log_.info("finished handling customer %d", customer);
  }

private:
  matador::leader_follower_thread_pool pool_;

  std::atomic_int customers_ {};
  std::atomic_int handled_customers_ {};

  std::thread::id nullid {0};

  std::mutex mutex_;
  std::condition_variable no_customer_;

  matador::logger log_;

  std::mt19937_64 eng_;
};


int main()
{

  //int num_threads = std::thread::hardware_concurrency();

  matador::add_log_sink(matador::create_stdout_sink());

  taxi_station t1(10, 4);

  t1.run();

  return 0;
}

#include "Reactor.hpp"

#include <unistd.h>
#include "EventHandler.hpp"
#include "EventHandlerArrayItem.hpp"
#include "ThreadPool.hpp"
#include "MutexGuard.hpp"
//========================================================================
// Reactor::RegisterHandler
//========================================================================
void Reactor::RegisterHandler(EventHandler& aEventHandler,
                              EventType aEventType )
{
  // protect internal data, mutex is locked from constructor
  // and unlocked from destructor
  const std::lock_guard<std::mutex> l(mutex_);
  EventHandlerArrayItem* temp = nullptr;
  int index = -1;
  index = FindHandler( aEventHandler );
  if( index < 0 ) {
    temp = new EventHandlerArrayItem( aEventHandler, aEventType );
    iItemArray.push_back( temp );
  } else {
    temp = iItemArray.at( index );
    temp->SetEventType( aEventType );
  }
  Notify();
}
//========================================================================
// Reactor::RemoveHandler
//========================================================================
void Reactor::RemoveHandler( EventHandler& aEventHandler,
                             EventType aEventType )
{
  // protect internal data, mutex is locked from constructor
  // and unlocked from destructor
  const std::lock_guard<std::mutex> l(mutex_);
  EventHandlerArrayItem* temp = nullptr;
  int index = -1;
  index = FindHandler( aEventHandler );
  if( index >= 0 )
  {
    temp = iItemArray.at( index );
    // if RemoveEventType return true, any event isn't left for
    // listening, so it can remove
    if( temp->RemoveEventType( aEventType ) )
    {
      iItemArray.erase( iItemArray.begin() + index );
    }
  }
  Notify();
}
//========================================================================
// Reactor::RemoveHandler
//========================================================================

void Reactor::RemoveHandler( EventHandler& aEventHandler )
{
// protect internal data, mutex is locked from constructor
// and unlocked from destructor
  const std::lock_guard<std::mutex> l(mutex_);
  int index = -1;
  index = FindHandler( aEventHandler );
  if( index >= 0 ) {
    iItemArray.erase(iItemArray.begin() + index );
  }
  Notify();
}
//========================================================================
// Reactor::HandleEvents
// Method is heart of reactor pattern. It uses select method to
// demultiplexing events. When event occurs first search correct
// EventHandler and then notify that for event
//========================================================================
void Reactor::HandleEvents( )
{
  int numberOfEvents = 0;
  int highestHandleNumber = 0;
  // handles
  fd_set readSet;
  fd_set writeSet;
  fd_set exceptionSet;
  // handle sets need to be zeroed before those are set
  FD_ZERO( &readSet );
  FD_ZERO( &writeSet );
  FD_ZERO( &exceptionSet );
  // add all event handlers to those handle set,
  // where they are registered
  highestHandleNumber = ConvertToFdSets(&readSet,
                                        &writeSet,
                                        &exceptionSet );
  numberOfEvents = select( highestHandleNumber + 1,
                           &readSet,
                           &writeSet,
                           &exceptionSet,
                           nullptr );
  // if notifier, no need to go through all eventhandlers any more
  // IsNotifier promotes new leader
  if( IsNotifier( &readSet ) )
  {
    return;
  }
  // if error occurs
  if( numberOfEvents <= 0 )
  {
    // Handle timeout
    // Handle errors
    if( iThreadPool )
    {
      iThreadPool->PromoteNewLeader();
    }
    return;
  }
  // find event handler and occurred event type
  EventType occurredEvent;
  EventHandler* eventHandler = ResolveEventHandler(occurredEvent,
                                                   &readSet,
                                                   &writeSet,
                                                   &exceptionSet );
  if( eventHandler )
  {
    // Deactivate event from Handle

    // Can't trust that event handler exists when HandleEvent
    // returns
    Handle handle = *(eventHandler->GetHandle());
    DeActivateHandle(handle, occurredEvent );
    if( iThreadPool )
    {
      // next new leader call handle events
      iThreadPool->PromoteNewLeader();
    }
    // old leader handle event of current event handler
    eventHandler->HandleEvent(occurredEvent );
    ActivateHandle(handle, occurredEvent );
  }
  else // unknown event happened, promote new leader and return
  {
    if( iThreadPool )
    {
      iThreadPool->PromoteNewLeader();
    }
  }
}
//========================================================================
// Reactor::SetThreadPool
//========================================================================
void Reactor::SetThreadPool( ThreadPool* aPool )
{
  iThreadPool = aPool;
}
//========================================================================
// Reactor::Instance
//========================================================================
Reactor* Reactor::iInstance = nullptr;
Reactor* Reactor::Instance()
{
  if( !iInstance )
  {
    iInstance = new Reactor();
  }
  return iInstance;
}
//========================================================================
// Reactor::~Reactor
//========================================================================
Reactor::~Reactor()
{
  if( iThreadPool )
  {
    iThreadPool->Stop();
  }
  while (!iItemArray.empty()) {
    auto i = iItemArray.back();
    iItemArray.pop_back();
    delete i;
  }
  if( iNotifierFd > 0 )
  {
    close( iNotifierFd );
  }
  if( iNotifiedFd > 0 )
  {
    close( iNotifiedFd );
  }

  if( iThreadPool )
  {
    delete iThreadPool;
    iThreadPool = nullptr;
  }
}
//========================================================================
// Reactor::Reactor
//========================================================================
Reactor::Reactor()
  : iNotifierFd( 0 ),
    iNotifiedFd( 0 ),
    iThreadPool( nullptr )
{
  // For notifying select function to return if reactor
  // event handler data has changed
  int fd[2];
  if( pipe( fd ) == 0 )
  {
    iNotifiedFd = fd[0]; // read pipe
    iNotifierFd = fd[1]; // write pipe
  }
}
//========================================================================
// Reactor::ConvertToFdSets
//========================================================================
int Reactor::ConvertToFdSets( fd_set* aReadSet,
                              fd_set* aWriteSet,
                              fd_set* aExceptionSet )
{
// protect internal data, mutex is locked from constructor
// and unlocked from destructor
  const std::lock_guard<std::mutex> l(mutex_);
  int highestDescriptor = -1;
  EventHandlerArrayItem* temp = nullptr;
  int fd_temp = 0;
  for (auto & i : iItemArray) {
    temp = i;
    fd_temp = temp->Handler()->GetHandle()->GetHandle();
    if( fd_temp > highestDescriptor )
    {
      highestDescriptor = fd_temp;
    }
    /*
     * If read event is set or accept event, add it to read
     * descriptor set. In select point of view these are same thing
     */
    if(temp->IsEvent( EReadEvent )||temp->IsEvent( EAcceptEvent )) {
      FD_SET( fd_temp , aReadSet );
    }
    /*
     * if write event is set, add it to write descriptor set
     */
    if( temp->IsEvent( EWriteEvent ) ) {
      FD_SET( fd_temp, aWriteSet );
    }
    /*
     * if exception event is set, add it to exception descriptor
     * set
     */
    if( temp->IsEvent( EExceptionEvent ) ) {
      FD_SET( fd_temp, aExceptionSet );
    }
  }
  // finally, add notify descriptor
  if( iNotifiedFd > 0 ) {
    FD_SET( iNotifiedFd, aReadSet );
    if( iNotifiedFd > highestDescriptor ) {
      highestDescriptor = iNotifiedFd;
    }
  }
  return highestDescriptor;
}
//========================================================================
// Reactor::FindHandler
//========================================================================
int Reactor::FindHandler( EventHandler& aHandler )
{
  return FindHandler( *(aHandler.GetHandle()) );
}
//========================================================================
// Reactor::FindHandler
//========================================================================
int Reactor::FindHandler( Handle& aHandle )
{
  for( unsigned int i = 0; i < iItemArray.size(); i++ ) {
    if( iItemArray.at( i )->Compare( aHandle ) ) {
      return i;
    }
  }
  return -1;
}
//========================================================================
// Reactor::Notify
//========================================================================
void Reactor::Notify()
{
  if( iNotifierFd > 0 ) {
    write( iNotifierFd, "1", sizeof(char));
  }
}
//========================================================================
// Reactor::IsNotifier
//========================================================================
bool Reactor::IsNotifier( fd_set* aReadSet )
{
  const std::lock_guard<std::mutex> l(mutex_);
  if( FD_ISSET( iNotifiedFd, aReadSet) )
  {
    char buf;
    read( iNotifiedFd, &buf, sizeof(char) );
    iThreadPool->PromoteNewLeader();
    return true;
  }
  else
  {
    return false;
  }
}

//========================================================================
// Reactor::ResolveEventHandler
//========================================================================
EventHandler* Reactor::ResolveEventHandler( EventType& aOccuredEvent,
                                            fd_set* aReadSet,
                                            fd_set* aWriteSet,
                                            fd_set* aExceptionSet )
{
  const std::lock_guard<std::mutex> l(mutex_);
  EventHandlerArrayItem* temp = nullptr;
//now resolve handles which cause event
  for(auto & i : iItemArray) {
    temp = i;
    if( CheckEvent( *temp, aReadSet, EReadEvent ) ) {
      aOccuredEvent = EReadEvent;
      return temp->Handler();
    } else if( CheckEvent( *temp, aWriteSet, EWriteEvent ) ) {
      aOccuredEvent = EWriteEvent;
      return temp->Handler();
    } else if( CheckEvent( *temp, aExceptionSet, EExceptionEvent ) ) {
      aOccuredEvent = EExceptionEvent;
      return temp->Handler();
    }
  }
//Unknow reason
  return nullptr;
}
//========================================================================
// Reactor::DeActivateHandle
//========================================================================
void Reactor::DeActivateHandle( Handle aHandler, EventType aEvent )
{
  const std::lock_guard<std::mutex> l(mutex_);
  int index = FindHandler( aHandler );
  if( index >= 0 )
  {
    iItemArray.at( index )->DeActivateEvent( aEvent );
    // no need to notify, Deactivation is done before leader
    // promotion
  }
}
//========================================================================
// Reactor::ActivateHandle
//========================================================================
void Reactor::ActivateHandle( Handle aHandler, EventType aEvent )
{
  const std::lock_guard<std::mutex> l(mutex_);
  int index = FindHandler( aHandler );
  if( index >= 0 )
  {
    iItemArray.at( index )->ActivateEvent( aEvent );
    Notify();
  }
}
//========================================================================
// Reactor::CheckEvent
//========================================================================
bool Reactor::CheckEvent( EventHandlerArrayItem& aEntity,
                          fd_set* aSet,
                          EventType aEvent )
{
  bool retVal = false;
  /*
   * fd_set need to check before, event check. This way
   * all event will be handled in somehow
   */
  if( FD_ISSET( aEntity.Handler()->GetHandle()->GetHandle(), aSet ) )
  {
// ensure that event handler wait occurred event
    if( aEntity.IsEvent( aEvent ) )
    {
      retVal = true;
    }
  }
  return retVal;
}
// End of file
