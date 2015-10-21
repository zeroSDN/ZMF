#ifndef ZMF_UT_TESTMODULEA_H
#define ZMF_UT_TESTMODULEA_H

#include <AbstractModule.hpp>

/**
 * @brief Simple test module class for testing
 * @author Jonas Grunert
 * @date created on 7/9/15.
*/
class TestModuleA : public zmf::AbstractModule {

public:
    TestModuleA(const zmf::data::ModuleUniqueId& moduleId, uint16_t version) :
            AbstractModule(moduleId, version, "TestModuleA", std::vector<zmf::ModuleDependency>()) { }

protected:
    virtual bool enable();

    virtual void disable();
};


#endif //ZMF_UT_TESTMODULEA_H
