#pragma once


#include <functional>
#include <utility>
#include <memory>
#include <uv.h>
#include "event.hpp"
#include "request.hpp"
#include "loop.hpp"


namespace uvw {


/**
 * @brief WorkEvent event.
 *
 * It will be emitted by WorkReq according with its functionalities.
 */
struct WorkEvent: Event<WorkEvent> {};


/**
 * @brief The WorkReq request.
 *
 * It runs user code using a thread from the threadpool and gets notified in the
 * loop thread by means of an event.
 *
 * To create a `WorkReq` through a `Loop`, arguments follow:
 *
 * * A valid instance of a `Task`, that is of type `std::function<void(void)>`.
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/threadpool.html)
 * for further details.
 */
class WorkReq final: public Request<WorkReq, uv_work_t> {
    using InternalTask = std::function<void(void)>;

    static void workCallback(uv_work_t *req) {
        static_cast<WorkReq*>(req->data)->task();
    }

public:
    using Task = InternalTask;

    explicit WorkReq(ConstructorAccess ca, std::shared_ptr<Loop> ref, InternalTask t)
        : Request{std::move(ca), std::move(ref)}, task{t}
    {}

    /**
     * @brief Runs the given task in a separate thread.
     *
     * A WorkEvent event will be emitted on the loop thread when the task is
     * finished.<br/>
     * This request can be cancelled with `cancel()`.
     */
    void queue() {
        invoke(&uv_queue_work, parent(), get(), &workCallback, &defaultCallback<WorkEvent>);
    }

private:
    Task task{};
};


}
