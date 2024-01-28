#include "SharedGlobalState.h"
#include <memory> 

namespace sgs {
// when using with C-compatible functions version
SharedGlobalState& sharedGlobalState = SharedGlobalState::getInstance();

// std::shared_ptr<SharedGlobalState> SharedGlobalState::getInstance() {
//     static std::shared_ptr<SharedGlobalState> instance = std::make_shared<SharedGlobalState>();
//     return instance;
// }

}

// Implementation of C-compatible functions

extern "C" {

void* sharedGlobalStateGetInstance() {
    return reinterpret_cast<void*>(&sgs::SharedGlobalState::getInstance());
}

int sharedGlobalStateGetUptime(void* instance) {
    return static_cast<sgs::SharedGlobalState*>(instance)->getUptime();
}

void sharedGlobalStateTickUptimeMs(void* instance, int ms) {
    static_cast<sgs::SharedGlobalState*>(instance)->tickUptimeMs(ms);
}


} // extern "C"