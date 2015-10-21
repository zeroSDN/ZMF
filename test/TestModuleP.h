#ifndef DEMO_MODULE_DEMOMODULE_H
#define DEMO_MODULE_DEMOMODULE_H

#include <AbstractModule.hpp>
#include <stdint.h>

/**
     * @brief Module-class of the unit-test for the PeerDiscoveryService
     * @details Dummy-module that only provides functionality that is necessary for testing the PeerDiscoveryService.
     * @author Matthias Blohm
     * @date created on 7/6/15.
    */
class TestModuleP : public zmf::AbstractModule {

public:
    /** Default constructor sets class attributes such as moduleId, name, version and dependencies*/
    TestModuleP(uint64_t instanceId, uint16_t instanceVersion);


    /** Sets the additional state of the module
     * @param state The new additional state the module should be set to
     */
    void setAdditionalState(std::vector<uint8_t> state);


protected:
    /** Dummy-method that enables the module
   * @return returns success of enabling module (always true)
   */
    virtual bool enable();

    /** Dummy-method that is called when the module is disabled (empty) */
    virtual void disable();


private:
    /// TestModuleP is always of type 1
    static const uint16_t MODULE_TYPE = 1;
};


#endif //DEMO_MODULE_DEMOMODULE_H
