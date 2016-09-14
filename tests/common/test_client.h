#ifndef __TEST_CLIENT_H_
#define __TEST_CLIENT_H_

#include <service/actor.h>
#include <tools/timed.h>
#include <tools/defines.h>

#include <iostream>
#include <fstream>

BEGIN_ENUM_DECL(TestResult) {
    Success, //!< Test or Request met requirement
    Failure, //!< Test or request failed requirement
    Partial, //! Test met only part of required results.
    None, //! Initial state
};
END_ENUM_DECL(TestResult, None, "None");


//! tell when the test should end
void setGlobalTimeout(double ms);

SHARED_PTR(TestClient);

/**
 TestClient does simple things. Mostly it's job is to dispatch requests and expect reply.
 Use expect() to set what kind of reply you're expecting.
 you can also schedule request to be published using schedule() with a delay
 When you schedule() request you must not forget to startScheduler() ;) 
 
 Note, you're expected to implement prepareTest(). This function allows you to create your workflow(s) and prepare your test queries.
 */
class TestClient: public Actor {
    std::map<boost::uuids::uuid, ETestResult> expected_results;
    std::map<boost::uuids::uuid, boost::function<void(ActorPtr,RequestPtr,ETestResult&)> > additionnal_data;
    std::map<double, std::vector<RequestPtr> > scheduled_requests;
    Timed begin;
    Timed nextStop;
    ETestResult global_result;
public:
    TestClient();
    virtual ~TestClient();
    
protected:
    //! ActiveObject callback prepare things for you.
    void started() override;
    
    //! to implement, this function will get called when the client is ready to be used.
    //! here you should create your workflow and prepare your test queries.
    void prepareTest();
    
    //! Client callback,
    void newRequestReceived() override;
    
    //! tell what result we expect for a given request.
    void expect(RequestPtr, ETestResult);
    //! tell what result to expect but also need a control done by provided function
    void expect(RequestPtr, ETestResult, boost::function<void(ActorPtr,RequestPtr,ETestResult&)>);
    
    //! store request and schedule it to be executed later. Note, you may schedule something for 0, it will be executed right on start. Negative will be concidered as 0 as well.
    void schedule(RequestPtr, double ms_timeout);
    
    //! start request scheduler.
    void startScheduler();
    
    //! Abort the test :) ahah
    void abortTest();
private:
    //! called by timer, will perform all next scheduled requests.
    void startNextBatch();
    void scheduleNextBatch();
};

#endif // __TEST_CLIENT_H_